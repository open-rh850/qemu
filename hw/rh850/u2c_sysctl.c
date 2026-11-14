/*
 * SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright (c) 2026 Linumiz
 */

#include "qemu/osdep.h"
#include "qemu/log.h"
#include "qemu/module.h"
#include "qapi/error.h"
#include "hw/core/qdev-properties.h"
#include "system/runstate.h"
#include "hw/rh850/u2c_sysctl.h"
#include "migration/vmstate.h"

#include "u2c_sysctl_regs.c.inc"
#include "u2a_sysctl_regs.c.inc"
#include "u2b_sysctl_regs.c.inc"

QEMU_BUILD_BUG_ON(ARRAY_SIZE(rh850_u2c_sysctl_regs) !=
                  RH850_U2C_SYSCTL_NREGS);

#define KCPROT_ENABLE   0xa5a5a501u
#define KCPROT_DISABLE  0xa5a5a500u

#define RES_RESFC       0x0c00
#define RES_RESKCPROT0  0x0f00
#define RES_SWSRESA     0x840c
#define RES_RESF        0x8500
#define RESF_POR        0x00000005u
#define RESF_SRES2F0    0x00000010u
#define RESFC_MASK      0x0000f5ddu

static const uint16_t sysctl_key_offset[SC_NKEYS] = {
    [SC_KEY_CLKKCPROT1] = 0x0700,
    [SC_KEY_CLKKCPROT4] = 0x0730,
    [SC_KEY_MSRKCPROT] = 0x1710,
    [SC_KEY_STBCKCPROT] = 0x1700,
};

static int sysctl_find(RH850U2CSysctlState *s, hwaddr addr)
{
    int i;

    for (i = 0; i < s->nregs; i++) {
        if (s->regs[i].offset == addr) {
            return i;
        }
    }
    return -1;
}

static bool sysctl_reset_read(RH850U2CSysctlState *s, hwaddr addr,
                              uint64_t *val)
{
    switch (addr) {
    case RES_RESKCPROT0:
        *val = s->reskce;
        return true;
    case RES_RESF:
        *val = s->resf;
        return true;
    case RES_RESFC:
    case RES_SWSRESA:
        *val = 0;
        return true;
    }
    return false;
}

static bool sysctl_reset_write(RH850U2CSysctlState *s, hwaddr addr,
                               uint64_t val)
{
    switch (addr) {
    case RES_RESKCPROT0:
        if (val == KCPROT_ENABLE || val == KCPROT_DISABLE) {
            s->reskce = val & 1;
        } else {
            qemu_log_mask(LOG_GUEST_ERROR, "rh850_u2c_sysctl: key register "
                          "0x%" HWADDR_PRIx " = 0x%" PRIx64 " ignored\n",
                          addr, val);
        }
        return true;
    case RES_RESFC:
        s->resf &= ~(val & RESFC_MASK);
        return true;
    case RES_SWSRESA:
        if (!s->reskce) {
            qemu_log_mask(LOG_GUEST_ERROR, "rh850_u2c_sysctl: protected "
                          "write to 0x%" HWADDR_PRIx " ignored\n", addr);
        } else if (val & 1) {
            s->resf |= RESF_SRES2F0;
            qemu_system_reset_request(SHUTDOWN_CAUSE_GUEST_RESET);
        }
        return true;
    case RES_RESF:
        qemu_log_mask(LOG_GUEST_ERROR, "rh850_u2c_sysctl: write to read-only "
                      "0x%" HWADDR_PRIx "\n", addr);
        return true;
    }
    return false;
}

static bool sysctl_unlocked(RH850U2CSysctlState *s, int key)
{
    int i = sysctl_find(s, sysctl_key_offset[key]);

    return key == SC_KEY_NONE || (i >= 0 && s->val[i]);
}

static uint64_t sysctl_read(void *opaque, hwaddr addr, unsigned size)
{
    RH850U2CSysctlState *s = opaque;
    int i = sysctl_find(s, addr);
    uint64_t val;

    if (size == 4 && sysctl_reset_read(s, addr, &val)) {
        return val;
    }
    if (i < 0) {
        qemu_log_mask(LOG_UNIMP, "rh850_u2c_sysctl: read of 0x%" HWADDR_PRIx
                      "\n", addr);
        return 0;
    }
    if (size != 4) {
        qemu_log_mask(LOG_GUEST_ERROR, "rh850_u2c_sysctl: %u-byte read at "
                      "0x%" HWADDR_PRIx "\n", size, addr);
        return 0;
    }
    return s->regs[i].kind == SC_TRIG ? 0 : s->val[i];
}

static void sysctl_write(void *opaque, hwaddr addr, uint64_t val,
                         unsigned size)
{
    RH850U2CSysctlState *s = opaque;
    int i = sysctl_find(s, addr);
    const RH850U2CSysctlReg *r;

    if (size == 4 && sysctl_reset_write(s, addr, val)) {
        return;
    }
    if (i < 0) {
        qemu_log_mask(LOG_UNIMP, "rh850_u2c_sysctl: write of 0x%" HWADDR_PRIx
                      " = 0x%" PRIx64 "\n", addr, val);
        return;
    }
    r = &s->regs[i];
    if (size != 4) {
        qemu_log_mask(LOG_GUEST_ERROR, "rh850_u2c_sysctl: %u-byte write at "
                      "0x%" HWADDR_PRIx "\n", size, addr);
        return;
    }
    switch (r->kind) {
    case SC_KEY:
        if (val == KCPROT_ENABLE || val == KCPROT_DISABLE) {
            s->val[i] = val & 1;
        } else {
            qemu_log_mask(LOG_GUEST_ERROR, "rh850_u2c_sysctl: key register "
                          "0x%" HWADDR_PRIx " = 0x%" PRIx64 " ignored\n",
                          addr, val);
        }
        return;
    case SC_RO:
        qemu_log_mask(LOG_GUEST_ERROR, "rh850_u2c_sysctl: write to read-only "
                      "0x%" HWADDR_PRIx "\n", addr);
        return;
    }
    if (!sysctl_unlocked(s, r->key)) {
        qemu_log_mask(LOG_GUEST_ERROR, "rh850_u2c_sysctl: protected write to "
                      "0x%" HWADDR_PRIx " ignored\n", addr);
        return;
    }
    if (r->kind == SC_TRIG) {
        if ((val & 3) == 1) {
            s->val[r->mirror] = 3;
        } else if ((val & 3) == 2) {
            s->val[r->mirror] = 0;
        }
        return;
    }
    s->val[i] = (s->val[i] & ~r->wmask) | (val & r->wmask);
    if (r->mirror >= 0) {
        s->val[r->mirror] = s->val[i];
    }
}

static const MemoryRegionOps sysctl_ops = {
    .read = sysctl_read,
    .write = sysctl_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .valid = { .min_access_size = 1, .max_access_size = 4 },
    .impl = { .min_access_size = 1, .max_access_size = 4 },
};

static void sysctl_reset_hold(Object *obj, ResetType type)
{
    RH850U2CSysctlState *s = RH850_U2C_SYSCTL(obj);
    int i;

    for (i = 0; i < s->nregs; i++) {
        s->val[i] = s->regs[i].reset;
    }
    s->reskce = 0;
}

static void sysctl_init(Object *obj)
{
    RH850U2CSysctlState *s = RH850_U2C_SYSCTL(obj);

    memory_region_init_io(&s->iomem, obj, &sysctl_ops, s,
                          "rh850-u2c-sysctl", RH850_U2C_SYSCTL_SIZE);
    sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->iomem);
    s->resf = RESF_POR;
}

static void sysctl_realize(DeviceState *dev, Error **errp)
{
    RH850U2CSysctlState *s = RH850_U2C_SYSCTL(dev);

    if (!s->chip || !strcmp(s->chip, "u2c")) {
        s->regs = rh850_u2c_sysctl_regs;
        s->nregs = ARRAY_SIZE(rh850_u2c_sysctl_regs);
    } else if (!strcmp(s->chip, "u2a")) {
        s->regs = rh850_u2a_sysctl_regs;
        s->nregs = ARRAY_SIZE(rh850_u2a_sysctl_regs);
    } else if (!strcmp(s->chip, "u2b")) {
        s->regs = rh850_u2b_sysctl_regs;
        s->nregs = ARRAY_SIZE(rh850_u2b_sysctl_regs);
    } else {
        error_setg(errp, "chip '%s' is not u2c, u2a or u2b", s->chip);
    }
}

static const Property sysctl_properties[] = {
    DEFINE_PROP_STRING("chip", RH850U2CSysctlState, chip),
};

static const VMStateDescription vmstate_sysctl = {
    .name = TYPE_RH850_U2C_SYSCTL,
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (const VMStateField[]) {
        VMSTATE_UINT32_ARRAY(val, RH850U2CSysctlState,
                             RH850_U2C_SYSCTL_NREGS),
        VMSTATE_UINT32(reskce, RH850U2CSysctlState),
        VMSTATE_UINT32(resf, RH850U2CSysctlState),
        VMSTATE_END_OF_LIST()
    }
};

static void sysctl_class_init(ObjectClass *klass, const void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    ResettableClass *rc = RESETTABLE_CLASS(klass);

    dc->realize = sysctl_realize;
    dc->vmsd = &vmstate_sysctl;
    device_class_set_props(dc, sysctl_properties);
    rc->phases.hold = sysctl_reset_hold;
}

static const TypeInfo sysctl_types[] = {
    {
        .name           = TYPE_RH850_U2C_SYSCTL,
        .parent         = TYPE_SYS_BUS_DEVICE,
        .instance_size  = sizeof(RH850U2CSysctlState),
        .instance_init  = sysctl_init,
        .class_init     = sysctl_class_init,
    },
};

DEFINE_TYPES(sysctl_types)
