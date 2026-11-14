/*
 * SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright (c) 2026 Linumiz
 */

static bool fxu_enabled(DisasContext *ctx)
{
    if (!ctx->cu1) {
        gen_raise(ctx, RH850_EXCP_UCPOP, RH850_CAUSE_UCPOP + 1, ctx->pc);
        return false;
    }
    return true;
}

static int wr_ofs(int r, int n)
{
    return offsetof(CPURH850State, wr) + (r * 4 + n) * 4;
}

static void gen_ld_wr(TCGv_i32 *v, int r)
{
    int n;

    for (n = 0; n < 4; n++) {
        v[n] = tcg_temp_new_i32();
        tcg_gen_ld_i32(v[n], tcg_env, wr_ofs(r, n));
    }
}

static void gen_st_wr(int r, TCGv_i32 *v)
{
    int n;

    for (n = 0; n < 4; n++) {
        tcg_gen_st_i32(v[n], tcg_env, wr_ofs(r, n));
    }
}

static TCGv_i32 fxu_regs(int r1, int r2, int r3)
{
    return tcg_constant_i32(r1 | r2 << 5 | r3 << 10);
}

static bool gen_fxu_arith(DisasContext *ctx, arg_fxr *a, int id)
{
    if (!ctx->fxu) {
        return false;
    }
    if (fxu_enabled(ctx)) {
        gen_helper_fxu_arith(tcg_env, tcg_constant_i32(id),
                             fxu_regs(a->r1, a->r2, a->r3));
    }
    return true;
}

static bool trans_CMPF_S4(DisasContext *ctx, arg_fxi *a)
{
    if (!ctx->fxu) {
        return false;
    }
    if (fxu_enabled(ctx)) {
        gen_helper_fxu_cmpf(tcg_env, tcg_constant_i32(a->imm),
                            fxu_regs(a->r1, a->r2, a->r3));
    }
    return true;
}

#define TRANS_FXA(NAME)                                                 \
    static bool trans_##NAME(DisasContext *ctx, arg_fxr *a)             \
    {                                                                   \
        return gen_fxu_arith(ctx, a, FXI_##NAME);                       \
    }

TRANS_FXA(ADDF_S4)
TRANS_FXA(ADDRF_S4)
TRANS_FXA(ADDSUBF_S4)
TRANS_FXA(ADDSUBNF_S4)
TRANS_FXA(ADDSUBNXF_S4)
TRANS_FXA(ADDSUBXF_S4)
TRANS_FXA(ADDXF_S4)
TRANS_FXA(DIVF_S4)
TRANS_FXA(FMAF_S4)
TRANS_FXA(FMSF_S4)
TRANS_FXA(FNMAF_S4)
TRANS_FXA(FNMSF_S4)
TRANS_FXA(MAXF_S4)
TRANS_FXA(MAXRF_S4)
TRANS_FXA(MINF_S4)
TRANS_FXA(MINRF_S4)
TRANS_FXA(MULF_S4)
TRANS_FXA(MULRF_S4)
TRANS_FXA(MULXF_S4)
TRANS_FXA(RECIPF_S4)
TRANS_FXA(RSQRTF_S4)
TRANS_FXA(SQRTF_S4)
TRANS_FXA(SUBADDF_S4)
TRANS_FXA(SUBADDNF_S4)
TRANS_FXA(SUBADDNXF_S4)
TRANS_FXA(SUBADDXF_S4)
TRANS_FXA(SUBF_S4)
TRANS_FXA(SUBRF_S4)
TRANS_FXA(SUBXF_S4)

static bool gen_fxu_cvt(DisasContext *ctx, arg_fxr *a, int ctl)
{
    if (!ctx->fxu) {
        return false;
    }
    if (fxu_enabled(ctx)) {
        gen_helper_fxu_cvt(tcg_env, tcg_constant_i32(ctl),
                           fxu_regs(0, a->r2, a->r3));
    }
    return true;
}

#define TRANS_FXC(NAME, KIND, CTL)                                      \
    static bool trans_##NAME(DisasContext *ctx, arg_fxr *a)             \
    {                                                                   \
        return gen_fxu_cvt(ctx, a, (KIND) << 4 | (CTL));                \
    }

TRANS_FXC(CEILF_SUW4, FXCVT_F2I, CVT_RP | CVT_U)
TRANS_FXC(CEILF_SW4, FXCVT_F2I, CVT_RP)
TRANS_FXC(CVTF_SUW4, FXCVT_F2I, CVT_RM | CVT_U)
TRANS_FXC(CVTF_SW4, FXCVT_F2I, CVT_RM)
TRANS_FXC(FLOORF_SUW4, FXCVT_F2I, CVT_RMI | CVT_U)
TRANS_FXC(FLOORF_SW4, FXCVT_F2I, CVT_RMI)
TRANS_FXC(ROUNDF_SUW4, FXCVT_F2I, CVT_RN | CVT_U)
TRANS_FXC(ROUNDF_SW4, FXCVT_F2I, CVT_RN)
TRANS_FXC(TRNCF_SUW4, FXCVT_F2I, CVT_RZ | CVT_U)
TRANS_FXC(TRNCF_SW4, FXCVT_F2I, CVT_RZ)
TRANS_FXC(CVTF_UWS4, FXCVT_I2F, CVT_U)
TRANS_FXC(CVTF_WS4, FXCVT_I2F, 0)
TRANS_FXC(CVTF_SH4, FXCVT_S2H, 0)
TRANS_FXC(CVTF_HS4, FXCVT_H2S, 0)

static bool gen_fxu_sign(DisasContext *ctx, arg_fxr *a, bool neg)
{
    TCGv_i32 v[4];
    int n;

    if (!ctx->fxu) {
        return false;
    }
    if (fxu_enabled(ctx)) {
        gen_ld_wr(v, a->r2);
        for (n = 0; n < 4; n++) {
            if (neg) {
                tcg_gen_xori_i32(v[n], v[n], 0x80000000u);
            } else {
                tcg_gen_andi_i32(v[n], v[n], 0x7fffffffu);
            }
        }
        gen_st_wr(a->r3, v);
    }
    return true;
}

static bool trans_ABSF_S4(DisasContext *ctx, arg_fxr *a)
{
    return gen_fxu_sign(ctx, a, false);
}

static bool trans_NEGF_S4(DisasContext *ctx, arg_fxr *a)
{
    return gen_fxu_sign(ctx, a, true);
}

static bool trans_MOVV_W4(DisasContext *ctx, arg_fxr *a)
{
    TCGv_i32 v[4];

    if (!ctx->fxu) {
        return false;
    }
    if (fxu_enabled(ctx)) {
        gen_ld_wr(v, a->r2);
        gen_st_wr(a->r3, v);
    }
    return true;
}

static bool trans_FLPV_S4(DisasContext *ctx, arg_fxi *a)
{
    TCGv_i32 v[4], d[4];
    int n;

    if (!ctx->fxu) {
        return false;
    }
    if (fxu_enabled(ctx)) {
        gen_ld_wr(v, a->r2);
        for (n = 0; n < 4; n++) {
            d[n] = v[a->imm & 1 ? n ^ 1 : n];
            if ((n & 1) == (a->imm >> 1)) {
                d[n] = tcg_temp_new_i32();
                tcg_gen_xori_i32(d[n], v[a->imm & 1 ? n ^ 1 : n],
                                 0x80000000u);
            }
        }
        gen_st_wr(a->r3, d);
    }
    return true;
}

static bool trans_TRFSRV_W4(DisasContext *ctx, arg_fxi *a)
{
    TCGv_i32 v[4], t;
    int n;

    if (!ctx->fxu) {
        return false;
    }
    if (!fxu_enabled(ctx)) {
        return true;
    }
    gen_ld_wr(v, a->r2);
    switch (a->imm) {
    case 0 ... 3:
        tcg_gen_setcondi_i32(TCG_COND_NE, cpu_psw_z, v[a->imm], 0);
        break;
    case 4:
        t = tcg_temp_new_i32();
        tcg_gen_movi_i32(cpu_psw_z, 1);
        for (n = 0; n < 4; n++) {
            tcg_gen_setcondi_i32(TCG_COND_NE, t, v[n], 0);
            tcg_gen_and_i32(cpu_psw_z, cpu_psw_z, t);
        }
        break;
    case 5:
        t = tcg_temp_new_i32();
        tcg_gen_or_i32(t, v[0], v[1]);
        tcg_gen_or_i32(t, t, v[2]);
        tcg_gen_or_i32(t, t, v[3]);
        tcg_gen_setcondi_i32(TCG_COND_NE, cpu_psw_z, t, 0);
        break;
    default:
        gen_log_undefined(ctx, "TRFSRV.W4 imm3 6/7");
        break;
    }
    return true;
}

static bool trans_CMOVF_W4(DisasContext *ctx, arg_fxm *a)
{
    TCGv_i32 v1[4], v2[4], v4[4];
    uint32_t hw;
    int n;

    if (!ctx->fxu) {
        return false;
    }
    if (!fxu_enabled(ctx)) {
        return true;
    }
    hw = next_hw(ctx);
    gen_ld_wr(v1, a->r1);
    gen_ld_wr(v2, extract32(hw, 11, 5));
    gen_ld_wr(v4, extract32(hw, 0, 5));
    for (n = 0; n < 4; n++) {
        tcg_gen_movcond_i32(TCG_COND_NE, v1[n], v4[n], tcg_constant_i32(0),
                            v1[n], v2[n]);
    }
    gen_st_wr(a->r3, v1);
    return true;
}

static bool trans_SHFLV_W4(DisasContext *ctx, arg_fxm *a)
{
    TCGv_i32 v1[4], v2[4], d[4];
    uint32_t hw, imm;
    int n, sel;

    if (!ctx->fxu) {
        return false;
    }
    if (!fxu_enabled(ctx)) {
        return true;
    }
    hw = next_hw(ctx);
    imm = a->imm << 11 | extract32(hw, 0, 11);
    gen_ld_wr(v1, a->r1);
    gen_ld_wr(v2, extract32(hw, 11, 5));
    for (n = 0; n < 4; n++) {
        sel = extract32(imm, n * 3, 3);
        d[n] = sel & 4 ? v2[sel & 3] : v1[sel & 3];
    }
    gen_st_wr(a->r3, d);
    return true;
}

static TCGv_i32 gen_fxu_addr(DisasContext *ctx, arg_fxm *a, int itype,
                             MemOp mop, bool st, int mask)
{
    TCGv_i32 addr = gen_addr(get_gpr(a->r1), (int16_t)next_hw(ctx));
    uint32_t mei = gen_mei(ctx, itype, a->r3, mop, st);

    if (mask) {
        gen_mae_if_unaligned(ctx, addr, mask, mei);
    }
    return addr;
}

static void gen_wr_from_i64(int r, int n, TCGv_i64 val)
{
    TCGv_i32 lo = tcg_temp_new_i32();
    TCGv_i32 hi = tcg_temp_new_i32();

    tcg_gen_extr_i64_i32(lo, hi, val);
    tcg_gen_st_i32(lo, tcg_env, wr_ofs(r, n));
    tcg_gen_st_i32(hi, tcg_env, wr_ofs(r, n + 1));
}

static TCGv_i64 gen_wr_to_i64(int r, int n)
{
    TCGv_i64 val = tcg_temp_new_i64();

    tcg_gen_ld_i64(val, tcg_env, wr_ofs(r, n));
    return val;
}

static bool trans_LDV_W(DisasContext *ctx, arg_fxm *a)
{
    TCGv_i32 addr, val;
    int n;

    if (!ctx->fxu) {
        return false;
    }
    if (fxu_enabled(ctx)) {
        addr = gen_fxu_addr(ctx, a, MEI_FXV, MO_32, false, 0);
        val = tcg_temp_new_i32();
        tcg_gen_qemu_ld_i32(val, addr, 0, MO_LEUL | ctx->align);
        for (n = 0; n < 4; n++) {
            if (a->imm & (1 << n)) {
                tcg_gen_st_i32(val, tcg_env, wr_ofs(a->r3, n));
            }
        }
    }
    return true;
}

static bool trans_LDV_DW(DisasContext *ctx, arg_fxm *a)
{
    TCGv_i32 addr;
    TCGv_i64 val;

    if (!ctx->fxu) {
        return false;
    }
    if (fxu_enabled(ctx)) {
        addr = gen_fxu_addr(ctx, a, MEI_FXV, MO_64, false, 3);
        val = tcg_temp_new_i64();
        tcg_gen_qemu_ld_i64(val, addr, 0, MO_LEUQ);
        if (a->imm & 1) {
            gen_wr_from_i64(a->r3, 0, val);
        }
        if (a->imm & 2) {
            gen_wr_from_i64(a->r3, 2, val);
        }
    }
    return true;
}

static bool trans_LDV_QW(DisasContext *ctx, arg_fxm *a)
{
    TCGv_i32 addr;
    TCGv_i128 val;
    TCGv_i64 lo, hi;

    if (!ctx->fxu) {
        return false;
    }
    if (fxu_enabled(ctx)) {
        addr = gen_fxu_addr(ctx, a, MEI_FXV, MO_128, false, 15);
        val = tcg_temp_new_i128();
        lo = tcg_temp_new_i64();
        hi = tcg_temp_new_i64();
        tcg_gen_qemu_ld_i128(val, addr, 0, MO_LE | MO_128);
        tcg_gen_extr_i128_i64(lo, hi, val);
        gen_wr_from_i64(a->r3, 0, lo);
        gen_wr_from_i64(a->r3, 2, hi);
    }
    return true;
}

static bool trans_LDVZ_H4(DisasContext *ctx, arg_fxm *a)
{
    TCGv_i32 addr, t;
    TCGv_i64 val, h;
    int n;

    if (!ctx->fxu) {
        return false;
    }
    if (fxu_enabled(ctx)) {
        addr = gen_fxu_addr(ctx, a, MEI_FXVZ, MO_64, false, 3);
        val = tcg_temp_new_i64();
        h = tcg_temp_new_i64();
        t = tcg_temp_new_i32();
        tcg_gen_qemu_ld_i64(val, addr, 0, MO_LEUQ);
        for (n = 0; n < 4; n++) {
            tcg_gen_extract_i64(h, val, n * 16, 16);
            tcg_gen_extrl_i64_i32(t, h);
            tcg_gen_st_i32(t, tcg_env, wr_ofs(a->r3, n));
        }
    }
    return true;
}

static bool trans_STV_W(DisasContext *ctx, arg_fxm *a)
{
    TCGv_i32 addr, val;

    if (!ctx->fxu) {
        return false;
    }
    if (fxu_enabled(ctx)) {
        addr = gen_fxu_addr(ctx, a, MEI_FXV, MO_32, true, 0);
        val = tcg_temp_new_i32();
        tcg_gen_ld_i32(val, tcg_env, wr_ofs(a->r3, a->imm));
        tcg_gen_qemu_st_i32(val, addr, 0, MO_LEUL | ctx->align);
        gen_link_kill(addr, 4);
    }
    return true;
}

static bool trans_STV_DW(DisasContext *ctx, arg_fxm *a)
{
    TCGv_i32 addr;

    if (!ctx->fxu) {
        return false;
    }
    if (fxu_enabled(ctx)) {
        addr = gen_fxu_addr(ctx, a, MEI_FXV, MO_64, true, 3);
        tcg_gen_qemu_st_i64(gen_wr_to_i64(a->r3, a->imm * 2), addr, 0,
                            MO_LEUQ);
        gen_link_kill(addr, 8);
    }
    return true;
}

static bool trans_STV_QW(DisasContext *ctx, arg_fxm *a)
{
    TCGv_i32 addr;
    TCGv_i128 val;

    if (!ctx->fxu) {
        return false;
    }
    if (fxu_enabled(ctx)) {
        addr = gen_fxu_addr(ctx, a, MEI_FXV, MO_128, true, 15);
        val = tcg_temp_new_i128();
        tcg_gen_concat_i64_i128(val, gen_wr_to_i64(a->r3, 0),
                                gen_wr_to_i64(a->r3, 2));
        tcg_gen_qemu_st_i128(val, addr, 0, MO_LE | MO_128);
        gen_link_kill(addr, 16);
    }
    return true;
}

static bool trans_STVZ_H4(DisasContext *ctx, arg_fxm *a)
{
    TCGv_i32 addr;
    TCGv_i64 val, h;
    int n;

    if (!ctx->fxu) {
        return false;
    }
    if (fxu_enabled(ctx)) {
        addr = gen_fxu_addr(ctx, a, MEI_FXVZ, MO_64, true, 3);
        val = tcg_temp_new_i64();
        h = tcg_temp_new_i64();
        tcg_gen_movi_i64(val, 0);
        for (n = 0; n < 4; n++) {
            tcg_gen_ld32u_i64(h, tcg_env, wr_ofs(a->r3, n));
            tcg_gen_deposit_i64(val, val, h, n * 16, 16);
        }
        tcg_gen_qemu_st_i64(val, addr, 0, MO_LEUQ);
        gen_link_kill(addr, 8);
    }
    return true;
}
