/*
 * SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright (c) 2026 Linumiz
 */

#include "qemu/osdep.h"
#include "qemu/bitmap.h"
#include "qemu/log.h"
#include "cpu.h"
#include "sysreg.h"
#include "gdbstub/helpers.h"

enum {
    GDB_SELID0 = 32,
    GDB_PC = 64,
    GDB_FPU = 122,
    GDB_FPU_FIRST = GDB_FPU + 6,
    GDB_FPU_LAST = GDB_FPU + 11,
    GDB_SELID1 = 150,
    GDB_VR0 = 374,
};

QEMU_BUILD_BUG_ON(GDB_VR0 + 32 != RH850_GDB_NUM_REGS);
QEMU_BUILD_BUG_ON(GDB_SELID1 + 7 * 32 != GDB_VR0);

static bool rh850_gdb_sysreg(int n, unsigned *sel, unsigned *id)
{
    if (n >= GDB_SELID0 && n < GDB_PC) {
        *sel = 0;
        *id = n - GDB_SELID0;
        return true;
    }
    if (n >= GDB_FPU_FIRST && n <= GDB_FPU_LAST) {
        *sel = 0;
        *id = n - GDB_FPU;
        return true;
    }
    if (n >= GDB_SELID1 && n < GDB_VR0) {
        *sel = 1 + (n - GDB_SELID1) / 32;
        *id = (n - GDB_SELID1) % 32;
        return true;
    }
    return false;
}

static void rh850_gdb_unmapped(int n)
{
    static DECLARE_BITMAP(logged, RH850_GDB_NUM_REGS);

    if (!test_and_set_bit(n, logged)) {
        qemu_log_mask(LOG_UNIMP, "rh850: gdb register %d has no state, "
                      "write ignored\n", n);
    }
}

int rh850_cpu_gdb_read_register(CPUState *cs, GByteArray *mem_buf, int n)
{
    CPURH850State *env = cpu_env(cs);
    unsigned sel, id;
    uint32_t val;

    if (n < RH850_NUM_GPRS) {
        return gdb_get_reg32(mem_buf, env->gpr[n]);
    }
    if (n == GDB_PC) {
        return gdb_get_reg32(mem_buf, env->pc);
    }
    if (n >= GDB_VR0) {
        return gdb_get_reg64(mem_buf, 0);
    }
    if (rh850_gdb_sysreg(n, &sel, &id) &&
        rh850_sysreg_get(env, sel, id, &val)) {
        return gdb_get_reg32(mem_buf, val);
    }
    return gdb_get_reg32(mem_buf, 0);
}

int rh850_cpu_gdb_write_register(CPUState *cs, uint8_t *mem_buf, int n)
{
    CPURH850State *env = cpu_env(cs);
    uint32_t val = ldl_le_p(mem_buf);
    unsigned sel, id;

    if (n < RH850_NUM_GPRS) {
        if (n) {
            env->gpr[n] = val;
        }
        return 4;
    }
    if (n == GDB_PC) {
        env->pc = val & ~1u;
        return 4;
    }
    if (n >= GDB_VR0) {
        rh850_gdb_unmapped(n);
        return 8;
    }
    if (!rh850_gdb_sysreg(n, &sel, &id) ||
        !rh850_sysreg_set(env, sel, id, val)) {
        rh850_gdb_unmapped(n);
    }
    return 4;
}

static int rh850_gdb_get_sreg(CPUState *cs, GByteArray *buf, int n)
{
    RH850CPU *cpu = RH850_CPU(cs);
    uint32_t val = 0;

    if (n < cpu->gdb_nsreg) {
        rh850_sysreg_get(&cpu->env, cpu->gdb_sreg[n] >> 5,
                         cpu->gdb_sreg[n] & 31, &val);
    }
    return gdb_get_reg32(buf, val);
}

static int rh850_gdb_set_sreg(CPUState *cs, uint8_t *buf, int n)
{
    RH850CPU *cpu = RH850_CPU(cs);

    if (n < cpu->gdb_nsreg) {
        rh850_sysreg_set(&cpu->env, cpu->gdb_sreg[n] >> 5,
                         cpu->gdb_sreg[n] & 31, ldl_le_p(buf));
    }
    return 4;
}

static const struct {
    uint16_t id;
    const char *name;
} rh850_gdb_fxsr[] = {
    { RH850_SR_FXSR, "fxsr" }, { RH850_SR_FXST, "fxst" },
    { RH850_SR_FXINFO, "fxinfo" }, { RH850_SR_FXCFG, "fxcfg" },
    { RH850_SR_FXXC, "fxxc" }, { RH850_SR_FXXP, "fxxp" },
};

static int rh850_gdb_get_fxu(CPUState *cs, GByteArray *buf, int n)
{
    CPURH850State *env = cpu_env(cs);
    uint32_t *w, val = 0;

    if (n < 32) {
        w = env->wr[n];
        return gdb_get_reg128(buf, (uint64_t)w[3] << 32 | w[2],
                              (uint64_t)w[1] << 32 | w[0]);
    }
    if (n < 32 + ARRAY_SIZE(rh850_gdb_fxsr)) {
        rh850_sysreg_get(env, rh850_gdb_fxsr[n - 32].id >> 5,
                         rh850_gdb_fxsr[n - 32].id & 31, &val);
    }
    return gdb_get_reg32(buf, val);
}

static int rh850_gdb_set_fxu(CPUState *cs, uint8_t *buf, int n)
{
    CPURH850State *env = cpu_env(cs);
    int i;

    if (n < 32) {
        for (i = 0; i < 4; i++) {
            env->wr[n][i] = ldl_le_p(buf + 4 * i);
        }
        return 16;
    }
    if (n < 32 + ARRAY_SIZE(rh850_gdb_fxsr)) {
        rh850_sysreg_set(env, rh850_gdb_fxsr[n - 32].id >> 5,
                         rh850_gdb_fxsr[n - 32].id & 31, ldl_le_p(buf));
    }
    return 4;
}

static void rh850_register_gdb_fxu(CPUState *cs)
{
    RH850CPU *cpu = RH850_CPU(cs);
    GDBFeatureBuilder builder;
    unsigned i;

    gdb_feature_builder_init(&builder, &cpu->gdb_fxu_feature,
                             "org.gnu.gdb.v850.fxu", "rh850-fxu.xml",
                             cs->gdb_num_regs);
    gdb_feature_builder_append_tag(&builder,
        "<vector id=\"v4f\" type=\"ieee_single\" count=\"4\"/>");
    gdb_feature_builder_append_tag(&builder,
        "<vector id=\"v4u\" type=\"uint32\" count=\"4\"/>");
    gdb_feature_builder_append_tag(&builder, "<union id=\"fxvec\">");
    gdb_feature_builder_append_tag(&builder,
        "<field name=\"f\" type=\"v4f\"/>");
    gdb_feature_builder_append_tag(&builder,
        "<field name=\"u\" type=\"v4u\"/>");
    gdb_feature_builder_append_tag(&builder, "</union>");
    for (i = 0; i < 32; i++) {
        gdb_feature_builder_append_reg(&builder, g_strdup_printf("wr%u", i),
                                       128, i, "fxvec", NULL);
    }
    for (i = 0; i < ARRAY_SIZE(rh850_gdb_fxsr); i++) {
        gdb_feature_builder_append_reg(&builder, rh850_gdb_fxsr[i].name, 32,
                                       32 + i, "uint32", NULL);
    }
    gdb_feature_builder_end(&builder);
    gdb_register_coprocessor(cs, rh850_gdb_get_fxu, rh850_gdb_set_fxu,
                             &cpu->gdb_fxu_feature);
}

void rh850_cpu_register_gdb_regs(CPUState *cs)
{
    RH850CPU *cpu = RH850_CPU(cs);
    const RH850CPUDef *def = cpu->def;
    GDBFeatureBuilder builder;
    unsigned i;

    gdb_feature_builder_init(&builder, &cpu->gdb_sreg_feature,
                             "org.gnu.gdb.v850.sreg", "rh850-sreg.xml",
                             cs->gdb_num_regs);
    cpu->gdb_nsreg = 0;
    for (i = 0; i < def->nsysregs; i++) {
        const RH850SysRegInfo *r = &def->sysregs[i];

        if (r->sel < 8 || r->access == RH850_SR_CU1 ||
            r->access == RH850_SR_CU1_SV) {
            continue;
        }
        g_assert(cpu->gdb_nsreg < RH850_GDB_MAX_SREGS);
        gdb_feature_builder_append_reg(&builder, g_ascii_strdown(r->name, -1),
                                       32, cpu->gdb_nsreg, "uint32", NULL);
        cpu->gdb_sreg[cpu->gdb_nsreg++] = r->sel << 5 | r->id;
    }
    gdb_feature_builder_end(&builder);
    gdb_register_coprocessor(cs, rh850_gdb_get_sreg, rh850_gdb_set_sreg,
                             &cpu->gdb_sreg_feature);
    if (cpu->fxu) {
        rh850_register_gdb_fxu(cs);
    }
}
