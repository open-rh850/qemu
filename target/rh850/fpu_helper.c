/*
 * SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright (c) 2026 Linumiz
 */

#include "qemu/osdep.h"
#include "cpu.h"
#include "sysreg.h"
#include "fxu.h"
#include "exec/helper-proto.h"
#include "accel/tcg/cpu-loop.h"
#include "fpu/softfloat.h"
#include "fpu/softfloat-helpers.h"

#define FPSR_FN         (1u << 23)
#define FPSR_IF         (1u << 22)
#define FPSR_PEM        (1u << 21)
#define FPSR_RM_SHIFT   18
#define FPSR_FS         (1u << 17)
#define FPSR_XC_SHIFT   10
#define FPSR_XE_SHIFT   5
#define FPSR_CAUSES     0x1fu

enum {
    FPC_I = 1 << 0,
    FPC_U = 1 << 1,
    FPC_O = 1 << 2,
    FPC_Z = 1 << 3,
    FPC_V = 1 << 4,
    FPC_E = 1 << 5,
    FPC_IF = 1 << 6,
};

enum { FPRM_RN, FPRM_RZ, FPRM_RP, FPRM_RM };

static void fp_status_sync(float_status *s, uint32_t sr)
{
    static const FloatRoundMode rm[4] = {
        float_round_nearest_even, float_round_to_zero, float_round_up,
        float_round_down,
    };

    set_float_rounding_mode(rm[(sr >> FPSR_RM_SHIFT) & 3], s);
    set_flush_to_zero(false, s);
    set_flush_inputs_to_zero(sr & FPSR_FS, s);
    set_snan_rule(float_snan_bit_is_zero, s);
    set_default_nan_mode(false, s);
    set_float_default_nan_pattern(0x7f, s);
    set_float_2nan_prop_rule(float_2nan_prop_ab, s);
    set_float_3nan_prop_rule(float_3nan_prop_abc, s);
    set_float_infzeronan_rule(float_infzeronan_dnan_never |
                              float_infzeronan_suppress_invalid, s);
    set_float_detect_tininess(float_tininess_before_rounding, s);
    set_float_ftz_detection(float_ftz_before_rounding, s);
}

void rh850_fpu_sync(CPURH850State *env)
{
    fp_status_sync(&env->fp_status, RH850_SR(env, RH850_SR_FPSR));
}

void rh850_fxu_sync(CPURH850State *env)
{
    fp_status_sync(&env->fxu_status, RH850_SR(env, RH850_SR_FXSR));
}

bool rh850_fpu_flags(CPURH850State *env, uint32_t causes, bool unimpl,
                     uintptr_t retaddr)
{
    uint32_t *fpsr = &RH850_SR(env, RH850_SR_FPSR);
    uint32_t xe = (*fpsr >> FPSR_XE_SHIFT) & FPSR_CAUSES;
    CPUState *cs = env_cpu(env);

    if (unimpl || (causes & xe)) {
        *fpsr = deposit32(*fpsr, FPSR_XC_SHIFT, 6, unimpl << 5 | causes);
        RH850_SR(env, RH850_SR_FPEPC) = env->pc;
        if (*fpsr & FPSR_PEM) {
            rh850_raise_exception(env, RH850_EXCP_FPE, RH850_CAUSE_FPE,
                                  retaddr);
        }
        RH850_SR(env, RH850_SR_FPEC) = 1;
        cpu_set_interrupt(cs, RH850_INTERRUPT_FPI);
        env->pc += 4;
        cpu_loop_exit(cs);
    }
    *fpsr |= causes;
    return true;
}

enum {
    FOP_ADD, FOP_SUB, FOP_MUL, FOP_DIV,
    FOP_SQRT, FOP_RECIP, FOP_RSQRT,
    FOP_FMA, FOP_FMS, FOP_FNMA, FOP_FNMS,
    FOP_MAX, FOP_MIN,
};

static int fop_nops(int op)
{
    return op < FOP_SQRT ? 2 : op < FOP_FMA ? 1 : 3;
}

static void fpu_apply(CPURH850State *env, uint32_t c, uintptr_t ra)
{
    if (c & FPC_IF) {
        RH850_SR(env, RH850_SR_FPSR) |= FPSR_IF;
    }
    rh850_fpu_flags(env, c & FPSR_CAUSES, c & FPC_E, ra);
}

static float128 rsqrt128(float128 x, float_status *s)
{
    float_status t = *s;

    set_float_rounding_mode(float_round_nearest_even, &t);
    x = float128_div(int32_to_float128(1, &t), float128_sqrt(x, &t), &t);
    s->float_exception_flags |= t.float_exception_flags &
        (float_flag_invalid | float_flag_divbyzero);
    return x;
}

static int cvt_round(int ctl, float_status *s)
{
    static const FloatRoundMode rm[] = {
        float_round_nearest_even, float_round_nearest_even, float_round_up,
        float_round_down, float_round_to_zero,
    };

    return (ctl & 7) == CVT_RM ? get_float_rounding_mode(s) : rm[ctl & 7];
}

#define FPU_FOP(SUF, FT)                                                    \
static FT fop_##SUF(int op, FT a, FT b, FT c, float_status *s)              \
{                                                                           \
    switch (op) {                                                           \
    case FOP_ADD:                                                           \
        return SUF##_add(a, b, s);                                          \
    case FOP_SUB:                                                           \
        return SUF##_sub(a, b, s);                                          \
    case FOP_MUL:                                                           \
        return SUF##_mul(a, b, s);                                          \
    case FOP_DIV:                                                           \
        return SUF##_div(a, b, s);                                          \
    case FOP_SQRT:                                                          \
        return SUF##_sqrt(a, s);                                            \
    case FOP_RECIP:                                                         \
        return SUF##_div(SUF##_one, a, s);                                  \
    case FOP_RSQRT:                                                         \
        return float128_to_##SUF(rsqrt128(SUF##_to_float128(a, s), s), s); \
    case FOP_FMA:                                                           \
    case FOP_FNMA:                                                          \
        return SUF##_muladd(a, b, c, 0, s);                                 \
    default:                                                                \
        return SUF##_muladd(a, b, c, float_muladd_negate_c, s);             \
    }                                                                       \
}

#define FPU_FLUSH(SUF, FT, UT)                                              \
static FT fp_flush_##SUF(uint32_t sr, FT z, UT minn, UT half)               \
{                                                                           \
    bool neg = SUF##_is_neg(z);                                             \
    bool up;                                                                \
                                                                            \
    switch ((sr >> FPSR_RM_SHIFT) & 3) {                                    \
    case FPRM_RN:                                                           \
        up = (sr & FPSR_FN) && SUF##_val(SUF##_abs(z)) >= half;             \
        break;                                                              \
    case FPRM_RP:                                                           \
        up = !neg;                                                          \
        break;                                                              \
    case FPRM_RM:                                                           \
        up = neg;                                                           \
        break;                                                              \
    default:                                                                \
        up = false;                                                         \
        break;                                                              \
    }                                                                       \
    return SUF##_set_sign(make_##SUF(up ? minn : 0), neg);                  \
}

#define FPU_ARITH(SUF, FT, UT, MINN, HALF)                                  \
static UT fp_arith_##SUF(float_status *s, uint32_t sr, int op, bool neg,    \
                         UT a, UT b, UT c, uint32_t *cp)                    \
{                                                                           \
    float_status t;                                                         \
    FT v[3] = { a, b, c };                                                  \
    int i, n = fop_nops(op);                                                \
    uint32_t causes = 0, flushed = 0;                                       \
    FT r, z;                                                                \
    int fl;                                                                 \
                                                                            \
    for (i = 0; i < n; i++) {                                               \
        if (SUF##_is_signaling_nan(v[i], s)) {                              \
            *cp = FPC_V;                                                    \
            return SUF##_val(SUF##_default_nan(s));                         \
        }                                                                   \
    }                                                                       \
    for (i = 0; i < n; i++) {                                               \
        if (SUF##_is_quiet_nan(v[i], s)) {                                  \
            *cp = 0;                                                        \
            return SUF##_val(v[i]);                                         \
        }                                                                   \
    }                                                                       \
    for (i = 0; i < n && !(sr & FPSR_FS); i++) {                            \
        if (SUF##_is_denormal(v[i])) {                                      \
            *cp = FPC_E;                                                    \
            return a;                                                       \
        }                                                                   \
    }                                                                       \
    set_float_exception_flags(0, s);                                        \
    r = fop_##SUF(op, a, b, c, s);                                          \
    fl = get_float_exception_flags(s);                                      \
    if (fl & float_flag_input_denormal_flushed) {                           \
        flushed = FPC_IF;                                                   \
    }                                                                       \
    if (fl & float_flag_invalid) {                                          \
        causes = FPC_V;                                                     \
        r = SUF##_default_nan(s);                                           \
    } else if (fl & float_flag_divbyzero) {                                 \
        causes = FPC_Z;                                                     \
    } else if (SUF##_is_zero_or_denormal(r) ||                              \
               SUF##_val(SUF##_abs(r)) == (MINN)) {                         \
        t = *s;                                                             \
        set_float_rounding_mode(float_round_to_zero, &t);                   \
        set_float_exception_flags(0, &t);                                   \
        z = fop_##SUF(op, a, b, c, &t);                                     \
        if (SUF##_is_denormal(z) ||                                         \
            (SUF##_is_zero(z) &&                                            \
             (get_float_exception_flags(&t) & float_flag_inexact))) {       \
            if (!(sr & FPSR_FS)) {                                          \
                *cp = flushed | FPC_E;                                      \
                return a;                                                   \
            }                                                               \
            r = fp_flush_##SUF(sr, z, MINN, HALF);                          \
            fl = float_flag_underflow | float_flag_inexact;                 \
        }                                                                   \
    }                                                                       \
    if (!causes) {                                                          \
        causes = (fl & float_flag_overflow ? FPC_O : 0) |                   \
                 (fl & float_flag_underflow ? FPC_U : 0) |                  \
                 (fl & float_flag_inexact ? FPC_I : 0);                     \
    }                                                                       \
    if (flushed && !(causes & (FPC_V | FPC_Z))) {                           \
        causes |= FPC_I;                                                    \
    }                                                                       \
    if ((op == FOP_RECIP || op == FOP_RSQRT) && SUF##_is_normal(a) &&       \
        !(causes & FPC_V)) {                                                \
        causes |= FPC_I;                                                    \
    }                                                                       \
    if ((neg || op == FOP_FNMA || op == FOP_FNMS) && !SUF##_is_any_nan(r)) { \
        r = SUF##_chs(r);                                                   \
    }                                                                       \
    *cp = causes | flushed;                                                 \
    return SUF##_val(r);                                                    \
}                                                                           \
                                                                            \
static UT fpu_arith_##SUF(CPURH850State *env, int op, UT a, UT b, UT c,     \
                          uintptr_t ra)                                     \
{                                                                           \
    uint32_t causes;                                                        \
    UT r = fp_arith_##SUF(&env->fp_status, RH850_SR(env, RH850_SR_FPSR),    \
                          op, false, a, b, c, &causes);                     \
                                                                            \
    fpu_apply(env, causes, ra);                                             \
    return r;                                                               \
}

#define FPU_MINMAX(SUF, UT)                                                 \
static UT fp_minmax_##SUF(float_status *s, bool max, UT a, UT b,            \
                          uint32_t *cp)                                     \
{                                                                           \
    float_status t = *s;                                                    \
                                                                            \
    *cp = 0;                                                                \
    if (SUF##_is_signaling_nan(a, s) || SUF##_is_signaling_nan(b, s)) {     \
        *cp = FPC_V;                                                        \
        return SUF##_val(SUF##_default_nan(s));                             \
    }                                                                       \
    if (SUF##_is_quiet_nan(a, s)) {                                         \
        return SUF##_is_quiet_nan(b, s) ? a : b;                            \
    }                                                                       \
    if (SUF##_is_quiet_nan(b, s)) {                                         \
        return a;                                                           \
    }                                                                       \
    if (SUF##_is_zero(a) && SUF##_is_zero(b)) {                             \
        return max ? a : b;                                                 \
    }                                                                       \
    set_flush_inputs_to_zero(false, &t);                                    \
    return SUF##_lt_quiet(a, b, &t) == max ? b : a;                         \
}                                                                           \
                                                                            \
static UT fpu_minmax_##SUF(CPURH850State *env, bool max, UT a, UT b,        \
                           uintptr_t ra)                                    \
{                                                                           \
    uint32_t causes;                                                        \
    UT r = fp_minmax_##SUF(&env->fp_status, max, a, b, &causes);            \
                                                                            \
    fpu_apply(env, causes, ra);                                             \
    return r;                                                               \
}

FPU_FLUSH(float32, float32, uint32_t)
FPU_FLUSH(float64, float64, uint64_t)
FPU_FLUSH(float16, float16, uint16_t)
FPU_FOP(float32, float32)
FPU_FOP(float64, float64)
FPU_ARITH(float32, float32, uint32_t, 0x00800000u, 0x00400000u)
FPU_ARITH(float64, float64, uint64_t, 0x0010000000000000ull,
          0x0008000000000000ull)
FPU_MINMAX(float32, uint32_t)
FPU_MINMAX(float64, uint64_t)

#define FPU_HELPERS(NAME, OP)                                               \
uint32_t HELPER(NAME##_s)(CPURH850State *env, uint32_t a, uint32_t b)       \
{                                                                           \
    return fpu_arith_float32(env, OP, a, b, 0, GETPC());                    \
}                                                                           \
uint64_t HELPER(NAME##_d)(CPURH850State *env, uint64_t a, uint64_t b)       \
{                                                                           \
    return fpu_arith_float64(env, OP, a, b, 0, GETPC());                    \
}

FPU_HELPERS(addf, FOP_ADD)
FPU_HELPERS(subf, FOP_SUB)
FPU_HELPERS(mulf, FOP_MUL)
FPU_HELPERS(divf, FOP_DIV)
FPU_HELPERS(sqrtf, FOP_SQRT)
FPU_HELPERS(recipf, FOP_RECIP)
FPU_HELPERS(rsqrtf, FOP_RSQRT)

#define FPU_HELPER_FMA(NAME, OP)                                            \
uint32_t HELPER(NAME)(CPURH850State *env, uint32_t a, uint32_t b,           \
                      uint32_t c)                                           \
{                                                                           \
    return fpu_arith_float32(env, OP, a, b, c, GETPC());                    \
}

FPU_HELPER_FMA(fmaf_s, FOP_FMA)
FPU_HELPER_FMA(fmsf_s, FOP_FMS)
FPU_HELPER_FMA(fnmaf_s, FOP_FNMA)
FPU_HELPER_FMA(fnmsf_s, FOP_FNMS)

#define FPU_HELPER_MINMAX(NAME, MAX)                                        \
uint32_t HELPER(NAME##_s)(CPURH850State *env, uint32_t a, uint32_t b)       \
{                                                                           \
    return fpu_minmax_float32(env, MAX, a, b, GETPC());                     \
}                                                                           \
uint64_t HELPER(NAME##_d)(CPURH850State *env, uint64_t a, uint64_t b)       \
{                                                                           \
    return fpu_minmax_float64(env, MAX, a, b, GETPC());                     \
}

FPU_HELPER_MINMAX(maxf, true)
FPU_HELPER_MINMAX(minf, false)

#define FPU_F2I(SUF, UT, IT, W, IMIN, IMAX, UMAX)                           \
static IT fp_f2i_##SUF##_##W(float_status *s, uint32_t sr, UT a, int ctl,   \
                             uint32_t *cp)                                  \
{                                                                           \
    bool u = ctl & CVT_U;                                                   \
    bool neg = SUF##_is_neg(a);                                             \
    FloatRoundMode rm = cvt_round(ctl, s);                                  \
    IT r;                                                                   \
    int fl;                                                                 \
                                                                            \
    *cp = 0;                                                                \
    if (SUF##_is_any_nan(a)) {                                              \
        *cp = FPC_V;                                                        \
        return u ? 0 : (IMIN);                                              \
    }                                                                       \
    if (!(sr & FPSR_FS) && SUF##_is_zero_or_denormal(a) &&                  \
        !SUF##_is_zero(a)) {                                                \
        *cp = FPC_E;                                                        \
        return 0;                                                           \
    }                                                                       \
    set_float_exception_flags(0, s);                                        \
    if (u) {                                                                \
        r = SUF##_to_uint##W##_scalbn(a, rm, 0, s);                         \
    } else {                                                                \
        r = SUF##_to_int##W##_scalbn(a, rm, 0, s);                          \
    }                                                                       \
    fl = get_float_exception_flags(s);                                      \
    if (fl & float_flag_input_denormal_flushed) {                           \
        *cp = FPC_IF | FPC_I;                                               \
    } else if (fl & float_flag_invalid || (u && neg && !SUF##_is_zero(a))) { \
        *cp = FPC_V;                                                        \
        r = neg ? (u ? 0 : (IMIN)) : (u ? (UMAX) : (IMAX));                 \
    } else if (fl & float_flag_inexact) {                                   \
        *cp = FPC_I;                                                        \
    }                                                                       \
    return r;                                                               \
}                                                                           \
                                                                            \
static IT f2i_##SUF##_##W(CPURH850State *env, UT a, int ctl, uintptr_t ra)  \
{                                                                           \
    uint32_t causes;                                                        \
    IT r = fp_f2i_##SUF##_##W(&env->fp_status, RH850_SR(env, RH850_SR_FPSR), \
                              a, ctl, &causes);                             \
                                                                            \
    fpu_apply(env, causes, ra);                                             \
    return r;                                                               \
}

FPU_F2I(float32, uint32_t, uint32_t, 32, INT32_MIN, INT32_MAX, UINT32_MAX)
FPU_F2I(float32, uint32_t, uint64_t, 64, INT64_MIN, INT64_MAX, UINT64_MAX)
FPU_F2I(float64, uint64_t, uint32_t, 32, INT32_MIN, INT32_MAX, UINT32_MAX)
FPU_F2I(float64, uint64_t, uint64_t, 64, INT64_MIN, INT64_MAX, UINT64_MAX)

uint32_t HELPER(f2i_sw)(CPURH850State *env, uint32_t a, uint32_t ctl)
{
    return f2i_float32_32(env, a, ctl, GETPC());
}

uint64_t HELPER(f2i_sl)(CPURH850State *env, uint32_t a, uint32_t ctl)
{
    return f2i_float32_64(env, a, ctl, GETPC());
}

uint32_t HELPER(f2i_dw)(CPURH850State *env, uint64_t a, uint32_t ctl)
{
    return f2i_float64_32(env, a, ctl, GETPC());
}

uint64_t HELPER(f2i_dl)(CPURH850State *env, uint64_t a, uint32_t ctl)
{
    return f2i_float64_64(env, a, ctl, GETPC());
}

static uint64_t fp_i2f(float_status *s, uint64_t a, int ctl, bool l, bool d,
                       uint32_t *cp)
{
    int64_t v = l ? (int64_t)a : (int32_t)a;
    uint64_t uv = l ? a : (uint32_t)a;
    uint64_t r;

    set_float_exception_flags(0, s);
    if (ctl & CVT_U) {
        r = d ? float64_val(uint64_to_float64(uv, s)) :
                float32_val(uint64_to_float32(uv, s));
    } else {
        r = d ? float64_val(int64_to_float64(v, s)) :
                float32_val(int64_to_float32(v, s));
    }
    *cp = get_float_exception_flags(s) & float_flag_inexact ? FPC_I : 0;
    return r;
}

static uint64_t i2f(CPURH850State *env, uint64_t a, int ctl, bool l, bool d,
                    uintptr_t ra)
{
    uint32_t causes;
    uint64_t r = fp_i2f(&env->fp_status, a, ctl, l, d, &causes);

    fpu_apply(env, causes, ra);
    return r;
}

uint32_t HELPER(i2f_ws)(CPURH850State *env, uint32_t a, uint32_t ctl)
{
    return i2f(env, a, ctl, false, false, GETPC());
}

uint64_t HELPER(i2f_wd)(CPURH850State *env, uint32_t a, uint32_t ctl)
{
    return i2f(env, a, ctl, false, true, GETPC());
}

uint32_t HELPER(i2f_ls)(CPURH850State *env, uint64_t a, uint32_t ctl)
{
    return i2f(env, a, ctl, true, false, GETPC());
}

uint64_t HELPER(i2f_ld)(CPURH850State *env, uint64_t a, uint32_t ctl)
{
    return i2f(env, a, ctl, true, true, GETPC());
}

#define FPU_F2F(NAME, IS, IT, OS, OT, CONV, MINN, HALF)                     \
static OT fp_##NAME(float_status *s, uint32_t sr, IT a, uint32_t *cp)       \
{                                                                           \
    float_status t;                                                         \
    uint32_t causes, flushed = 0;                                           \
    OT r, z;                                                                \
    int fl;                                                                 \
                                                                            \
    if (IS##_is_signaling_nan(a, s)) {                                      \
        *cp = FPC_V;                                                        \
        return OS##_default_nan(s);                                         \
    }                                                                       \
    if (!(sr & FPSR_FS) && IS##_is_zero_or_denormal(a) &&                   \
        !IS##_is_zero(a)) {                                                 \
        *cp = FPC_E;                                                        \
        return OS##_zero;                                                   \
    }                                                                       \
    set_float_exception_flags(0, s);                                        \
    r = CONV(a, s);                                                         \
    fl = get_float_exception_flags(s);                                      \
    if (fl & float_flag_input_denormal_flushed) {                           \
        flushed = FPC_IF;                                                   \
    }                                                                       \
    if (!IS##_is_any_nan(a) && (OS##_is_zero_or_denormal(r) ||              \
                                OS##_val(OS##_abs(r)) == (MINN))) {         \
        t = *s;                                                             \
        set_float_rounding_mode(float_round_to_zero, &t);                   \
        set_float_exception_flags(0, &t);                                   \
        z = CONV(a, &t);                                                    \
        if ((OS##_is_zero_or_denormal(z) && !OS##_is_zero(z)) ||            \
            (OS##_is_zero(z) &&                                             \
             (get_float_exception_flags(&t) & float_flag_inexact))) {       \
            if (!(sr & FPSR_FS)) {                                          \
                *cp = flushed | FPC_E;                                      \
                return OS##_zero;                                           \
            }                                                               \
            r = fp_flush_##OS(sr, z, MINN, HALF);                           \
            fl = float_flag_underflow | float_flag_inexact;                 \
        }                                                                   \
    }                                                                       \
    causes = (fl & float_flag_overflow ? FPC_O : 0) |                       \
             (fl & float_flag_underflow ? FPC_U : 0) |                      \
             (fl & float_flag_inexact ? FPC_I : 0);                         \
    if (flushed) {                                                          \
        causes |= FPC_I;                                                    \
    }                                                                       \
    *cp = causes | flushed;                                                 \
    return r;                                                               \
}                                                                           \
                                                                            \
static OT NAME(CPURH850State *env, IT a, uintptr_t ra)                      \
{                                                                           \
    uint32_t causes;                                                        \
    OT r = fp_##NAME(&env->fp_status, RH850_SR(env, RH850_SR_FPSR), a,      \
                     &causes);                                              \
                                                                            \
    fpu_apply(env, causes, ra);                                             \
    return r;                                                               \
}

static float16 f32_to_f16(float32 a, float_status *s)
{
    return float32_to_float16(a, true, s);
}

FPU_F2F(f2f_sd, float32, float32, float64, float64, float32_to_float64,
        0x0010000000000000ull, 0x0008000000000000ull)
FPU_F2F(f2f_ds, float64, float64, float32, float32, float64_to_float32,
        0x00800000u, 0x00400000u)
FPU_F2F(f2f_sh, float32, float32, float16, float16, f32_to_f16,
        0x0400u, 0x0200u)

uint64_t HELPER(cvtf_sd)(CPURH850State *env, uint32_t a)
{
    return float64_val(f2f_sd(env, a, GETPC()));
}

uint32_t HELPER(cvtf_ds)(CPURH850State *env, uint64_t a)
{
    return float32_val(f2f_ds(env, a, GETPC()));
}

uint32_t HELPER(cvtf_sh)(CPURH850State *env, uint32_t a)
{
    return float16_val(f2f_sh(env, a, GETPC()));
}

static uint32_t fp_h2s(float_status *s, uint32_t a, uint32_t *cp)
{
    float_status t = *s;
    float16 h = make_float16(a & 0xffff);

    *cp = 0;
    if (float16_is_signaling_nan(h, &t)) {
        *cp = FPC_V;
        return float32_val(float32_default_nan(&t));
    }
    set_flush_inputs_to_zero(false, &t);
    return float32_val(float16_to_float32(h, true, &t));
}

uint32_t HELPER(cvtf_hs)(CPURH850State *env, uint32_t a)
{
    uint32_t causes;
    uint32_t r = fp_h2s(&env->fp_status, a, &causes);

    fpu_apply(env, causes, GETPC());
    return r;
}

#define FPU_CMPF(SUF, UT)                                                   \
static bool fp_cmpf_##SUF(float_status *s, UT a, UT b, uint32_t fcond,      \
                          uint32_t *cp)                                     \
{                                                                           \
    float_status t = *s;                                                    \
    bool less = false, eq = false, un = false;                              \
    FloatRelation rel;                                                      \
                                                                            \
    *cp = 0;                                                                \
    if (SUF##_is_any_nan(a) || SUF##_is_any_nan(b)) {                       \
        un = true;                                                          \
        if ((fcond & 8) || SUF##_is_signaling_nan(a, &t) ||                 \
            SUF##_is_signaling_nan(b, &t)) {                                \
            *cp = FPC_V;                                                    \
        }                                                                   \
    } else {                                                                \
        set_flush_inputs_to_zero(false, &t);                                \
        rel = SUF##_compare_quiet(a, b, &t);                                \
        less = rel == float_relation_less;                                  \
        eq = rel == float_relation_equal;                                   \
    }                                                                       \
    return ((fcond & 4) && less) || ((fcond & 2) && eq) ||                  \
           ((fcond & 1) && un);                                             \
}                                                                           \
                                                                            \
void HELPER(cmpf_##SUF)(CPURH850State *env, UT a, UT b, uint32_t ctl)       \
{                                                                           \
    uint32_t causes;                                                        \
    bool r = fp_cmpf_##SUF(&env->fp_status, a, b, ctl & 15, &causes);       \
                                                                            \
    fpu_apply(env, causes, GETPC());                                        \
    RH850_SR(env, RH850_SR_FPSR) =                                          \
        deposit32(RH850_SR(env, RH850_SR_FPSR), 24 + (ctl >> 4), 1, r);     \
}

FPU_CMPF(float32, uint32_t)
FPU_CMPF(float64, uint64_t)

typedef struct RH850FxuLane {
    uint8_t op, neg, a, al, b, bl, c, cl;
} RH850FxuLane;

#include "fxu-lanes.c.inc"

static void fxu_apply(CPURH850State *env, const uint32_t *val,
                      const uint32_t *c, int rd, uintptr_t ra)
{
    uint32_t *fxsr = &RH850_SR(env, RH850_SR_FXSR);
    uint32_t xe = ((*fxsr >> FPSR_XE_SHIFT) & FPSR_CAUSES) | FPC_E;
    uint32_t xc = 0, fxxc = 0, fxxp = 0;
    bool trap = false;
    int n;

    for (n = 0; n < 4; n++) {
        xc |= c[n];
        fxxc = deposit32(fxxc, n * 8, 6, c[n]);
        fxxp = deposit32(fxxp, n * 8, 5, c[n]);
        trap |= (c[n] & xe) != 0;
    }
    if (xc & FPC_IF) {
        *fxsr |= FPSR_IF;
    }
    if (trap) {
        *fxsr = deposit32(*fxsr, FPSR_XC_SHIFT, 6, xc);
        RH850_SR(env, RH850_SR_FXXC) = fxxc;
        rh850_raise_exception(env, RH850_EXCP_FXE, RH850_CAUSE_FXE, ra);
    }
    *fxsr |= xc & FPSR_CAUSES;
    RH850_SR(env, RH850_SR_FXXP) |= fxxp;
    memcpy(env->wr[rd], val, sizeof(env->wr[rd]));
}

void HELPER(fxu_arith)(CPURH850State *env, uint32_t id, uint32_t regs)
{
    float_status *s = &env->fxu_status;
    uint32_t sr = RH850_SR(env, RH850_SR_FXSR);
    int reg[4] = { 0, extract32(regs, 0, 5), extract32(regs, 5, 5),
                   extract32(regs, 10, 5) };
    uint32_t val[4], c[4];
    int n;

    for (n = 0; n < 4; n++) {
        const RH850FxuLane *l = &rh850_fxu_lanes[id][n];
        uint32_t a = env->wr[reg[l->a]][l->al];
        uint32_t b = env->wr[reg[l->b]][l->bl];
        uint32_t d = env->wr[reg[l->c]][l->cl];

        switch (l->op) {
        case FOP_MAX:
        case FOP_MIN:
            val[n] = fp_minmax_float32(s, l->op == FOP_MAX, a, b, &c[n]);
            break;
        case FOP_RECIP:
            val[n] = fp_arith_float32(s, sr, FOP_DIV, false,
                                      float32_val(float32_one), a, 0, &c[n]);
            break;
        default:
            val[n] = fp_arith_float32(s, sr, l->op, l->neg, a, b, d, &c[n]);
            break;
        }
    }
    fxu_apply(env, val, c, reg[3], GETPC());
}

void HELPER(fxu_cvt)(CPURH850State *env, uint32_t ctl, uint32_t regs)
{
    float_status *s = &env->fxu_status;
    uint32_t sr = RH850_SR(env, RH850_SR_FXSR);
    int r2 = extract32(regs, 5, 5), r3 = extract32(regs, 10, 5);
    uint32_t val[4], c[4];
    int n;

    for (n = 0; n < 4; n++) {
        uint32_t a = env->wr[r2][n];

        switch (ctl >> 4) {
        case FXCVT_F2I:
            val[n] = fp_f2i_float32_32(s, sr, a, ctl & 15, &c[n]);
            break;
        case FXCVT_I2F:
            val[n] = fp_i2f(s, a, ctl & 15, false, false, &c[n]);
            break;
        case FXCVT_S2H:
            val[n] = float16_val(fp_f2f_sh(s, sr, a, &c[n]));
            break;
        default:
            val[n] = fp_h2s(s, a, &c[n]);
            break;
        }
    }
    fxu_apply(env, val, c, r3, GETPC());
}

void HELPER(fxu_cmpf)(CPURH850State *env, uint32_t fcond, uint32_t regs)
{
    int r1 = extract32(regs, 0, 5), r2 = extract32(regs, 5, 5);
    uint32_t val[4], c[4];
    int n;

    for (n = 0; n < 4; n++) {
        val[n] = fp_cmpf_float32(&env->fxu_status, env->wr[r2][n],
                                 env->wr[r1][n], fcond, &c[n]) ?
                 UINT32_MAX : 0;
    }
    fxu_apply(env, val, c, extract32(regs, 10, 5), GETPC());
}
