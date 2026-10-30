/*
 * SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright (c) 2026 Linumiz
 */

#include "qemu/osdep.h"
#include "qemu/log.h"
#include "cpu.h"
#include "sysreg.h"
#include "exec/helper-proto.h"
#include "accel/tcg/cpu-loop.h"

#include "sysreg-table.c.inc"

static const RH850SysRegInfo *rh850_sysreg_find(unsigned sel, unsigned id)
{
    size_t i;

    for (i = 0; i < ARRAY_SIZE(rh850_sysregs); i++) {
        if (rh850_sysregs[i].sel == sel && rh850_sysregs[i].id == id) {
            return &rh850_sysregs[i];
        }
    }
    return NULL;
}

void rh850_sysreg_reset(CPURH850State *env)
{
    size_t i;

    for (i = 0; i < ARRAY_SIZE(rh850_sysregs); i++) {
        const RH850SysRegInfo *r = &rh850_sysregs[i];

        if (!(r->flags & (RH850_SRF_PSW | RH850_SRF_MPIDX))) {
            env->sr[r->sel][r->id] = r->reset;
        }
    }
}

uint32_t rh850_sysreg_wmask(unsigned n)
{
    return rh850_sysreg_find(n >> 5, n & 31)->wmask;
}

static void rh850_sysreg_check(CPURH850State *env, const RH850SysRegInfo *r,
                               unsigned id, bool write, uintptr_t ra)
{
    bool um = FIELD_EX32(env->psw_ctrl, PSW, UM);
    unsigned cu = FIELD_EX32(env->psw_ctrl, PSW, CU);

    if (!r) {
        if (um) {
            rh850_raise_exception(env, RH850_EXCP_PIE, RH850_CAUSE_PIE, ra);
        }
        return;
    }
    switch (r->access) {
    case RH850_SR_CU0_SV:
    case RH850_SR_CU0:
        if (!(cu & 1)) {
            rh850_raise_exception(env, RH850_EXCP_UCPOP, RH850_CAUSE_UCPOP,
                                  ra);
        }
        break;
    case RH850_SR_CU1_SV:
    case RH850_SR_CU1:
        if (!(cu & 2)) {
            rh850_raise_exception(env, RH850_EXCP_UCPOP,
                                  RH850_CAUSE_UCPOP + 1, ra);
        }
        break;
    }
    if (!um) {
        return;
    }
    switch (r->access) {
    case RH850_SR_SV:
    case RH850_SR_CU0_SV:
    case RH850_SR_CU1_SV:
        rh850_raise_exception(env, RH850_EXCP_PIE, RH850_CAUSE_PIE, ra);
    case RH850_SR_PMU:
        if (!(RH850_SR(env, RH850_SR_PMUMCTRL) & (1u << ((id - 16) / 2)))) {
            rh850_raise_exception(env, RH850_EXCP_PIE, RH850_CAUSE_PIE, ra);
        }
        break;
    case RH850_SR_UMR_SVW:
        if (write) {
            rh850_raise_exception(env, RH850_EXCP_PIE, RH850_CAUSE_PIE, ra);
        }
        break;
    }
}

static uint32_t *rh850_mpu_slot(CPURH850State *env, unsigned id)
{
    unsigned idx = RH850_SR(env, RH850_SR_MPIDX);
    unsigned nmpue = RH850_SR(env, RH850_SR_MPCFG) & 0x1f;

    if (idx > nmpue || idx >= RH850_MPU_ENTRIES) {
        return NULL;
    }
    switch (id) {
    case 20:
        return &env->mpla[idx];
    case 21:
        return &env->mpua[idx];
    default:
        return &env->mpat[idx];
    }
}

static const struct {
    uint16_t n;
    uint8_t vbit, fbit, len;
} rh850_fpview_map[] = {
    { RH850_SR_FPST, 8, 10, 6 },
    { RH850_SR_FPST, 5, 22, 1 },
    { RH850_SR_FPST, 0, 0, 5 },
    { RH850_SR_FPCC, 0, 24, 8 },
    { RH850_SR_FPCFG, 8, 18, 2 },
    { RH850_SR_FPCFG, 0, 5, 5 },
};

static bool rh850_fpview(CPURH850State *env, unsigned n, bool write,
                         uint32_t *val)
{
    uint32_t *fpsr = &RH850_SR(env, RH850_SR_FPSR);
    uint32_t v = 0;
    bool hit = false;
    size_t i;

    for (i = 0; i < ARRAY_SIZE(rh850_fpview_map); i++) {
        unsigned vb = rh850_fpview_map[i].vbit;
        unsigned fb = rh850_fpview_map[i].fbit;
        unsigned len = rh850_fpview_map[i].len;

        if (rh850_fpview_map[i].n != n) {
            continue;
        }
        hit = true;
        if (write) {
            *fpsr = deposit32(*fpsr, fb, len, extract32(*val, vb, len));
        } else {
            v = deposit32(v, vb, len, extract32(*fpsr, fb, len));
        }
    }
    if (hit && !write) {
        *val = v;
    }
    return hit;
}

bool rh850_sysreg_get(CPURH850State *env, unsigned sel, unsigned id,
                      uint32_t *val)
{
    const RH850SysRegInfo *r = rh850_sysreg_find(sel, id);
    uint32_t *slot;

    if (!r) {
        return false;
    }
    if (r->flags & RH850_SRF_PSW) {
        *val = rh850_cpu_pack_psw(env);
    } else if (r->flags & RH850_SRF_MPIDX) {
        slot = rh850_mpu_slot(env, id);
        *val = slot ? *slot : 0;
    } else if (rh850_fpview(env, sel << 5 | id, false, val)) {
    } else if ((sel << 5 | id) == RH850_SR_ICSR) {
        *val = rh850_cpu_icsr(env);
    } else {
        *val = env->sr[sel][id];
    }
    return true;
}

uint32_t HELPER(stsr)(CPURH850State *env, uint32_t sel, uint32_t id)
{
    uint32_t val;

    rh850_sysreg_check(env, rh850_sysreg_find(sel, id), id, false, GETPC());
    return rh850_sysreg_get(env, sel, id, &val) ? val : 0;
}

bool rh850_sysreg_set(CPURH850State *env, unsigned sel, unsigned id,
                      uint32_t val)
{
    const RH850SysRegInfo *r = rh850_sysreg_find(sel, id);
    uint32_t *slot;
    uint32_t mask;

    if (!r) {
        return false;
    }
    if (r->flags & RH850_SRF_RO) {
        return true;
    }
    if ((r->flags & RH850_SRF_SVLOCK) && (RH850_SR(env, RH850_SR_SVLOCK) & 1)) {
        return true;
    }
    if (r->flags & RH850_SRF_PSW) {
        mask = FIELD_EX32(env->psw_ctrl, PSW, UM) ?
               RH850_PSW_FLAGS_MASK : r->wmask;
        rh850_cpu_unpack_psw(env, (rh850_cpu_pack_psw(env) & ~mask) |
                                  (val & mask));
        return true;
    }
    if (r->flags & RH850_SRF_ISPR) {
        if (RH850_SR(env, RH850_SR_INTCFG) & 1) {
            env->sr[sel][id] = val & 0xffff;
        }
        return true;
    }
    if (r->flags & RH850_SRF_MPIDX) {
        slot = rh850_mpu_slot(env, id);
        if (slot) {
            *slot = val & r->wmask;
        }
        return true;
    }
    if (rh850_fpview(env, sel << 5 | id, true, &val)) {
        rh850_fpu_sync(env);
        return true;
    }
    if ((sel << 5 | id) == RH850_SR_FPEC) {
        env->sr[sel][id] &= val;
        if (!env->sr[sel][id]) {
            cpu_reset_interrupt(env_cpu(env), RH850_INTERRUPT_FPI);
        }
        return true;
    }
    env->sr[sel][id] = (env->sr[sel][id] & ~r->wmask) | (val & r->wmask);
    if ((sel << 5 | id) == RH850_SR_FPSR) {
        rh850_fpu_sync(env);
    }
    if ((sel << 5 | id) == RH850_SR_RBNR && env->sr[sel][id] > 16) {
        env->sr[sel][id] = 16;
    }
    return true;
}

void HELPER(ldsr)(CPURH850State *env, uint32_t sel, uint32_t id,
                  uint32_t val)
{
    rh850_sysreg_check(env, rh850_sysreg_find(sel, id), id, true, GETPC());
    rh850_sysreg_set(env, sel, id, val);
}
