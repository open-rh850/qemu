/*
 * SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright (c) 2026 Linumiz
 */

#include "qemu/osdep.h"
#include "qemu/log.h"
#include "hw/core/irq.h"
#include "hw/rh850/u2b_intsel.h"
#include "migration/vmstate.h"

static const uint8_t intsel_shift[6] = { 0, 5, 10, 16, 21, 26 };

static int intsel_unit(RH850U2BIntselState *s, int mux)
{
    unsigned v = extract32(s->sel[mux / 6], intsel_shift[mux % 6], 5);

    return v >= 1 && v <= RH850_U2B_INTSEL_NUNIT ? v - 1 : -1;
}

static void intsel_update(RH850U2BIntselState *s)
{
    int m, k, u;

    for (m = 0; m < RH850_U2B_INTSEL_NMUX; m++) {
        u = intsel_unit(s, m);
        for (k = 0; k < RH850_U2B_INTSEL_NLINE; k++) {
            qemu_set_irq(s->out[m * RH850_U2B_INTSEL_NLINE + k],
                         u >= 0 && s->in[u * RH850_U2B_INTSEL_NLINE + k]);
        }
    }
}

static void intsel_set_irq(void *opaque, int n, int level)
{
    RH850U2BIntselState *s = opaque;
    int u = n / RH850_U2B_INTSEL_NLINE, k = n % RH850_U2B_INTSEL_NLINE;
    int m;

    s->in[n] = !!level;
    for (m = 0; m < RH850_U2B_INTSEL_NMUX; m++) {
        if (intsel_unit(s, m) == u) {
            qemu_set_irq(s->out[m * RH850_U2B_INTSEL_NLINE + k], s->in[n]);
        }
    }
}

static uint32_t intsel_wmask(int reg)
{
    return reg == RH850_U2B_INTSEL_NREG - 1 ? 0x001f7fff : 0x7fff7fff;
}

static uint64_t intsel_read(void *opaque, hwaddr addr, unsigned size)
{
    RH850U2BIntselState *s = opaque;

    return extract32(s->sel[addr / 4], (addr & 3) * 8, 8);
}

static void intsel_write(void *opaque, hwaddr addr, uint64_t val,
                         unsigned size)
{
    RH850U2BIntselState *s = opaque;
    int reg = addr / 4, sh = (addr & 3) * 8;
    uint32_t m = intsel_wmask(reg) & (0xffu << sh);

    s->sel[reg] = (s->sel[reg] & ~m) | (((uint32_t)val << sh) & m);
    intsel_update(s);
}

static const MemoryRegionOps intsel_ops = {
    .read = intsel_read,
    .write = intsel_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .valid = { .min_access_size = 1, .max_access_size = 4 },
    .impl = { .min_access_size = 1, .max_access_size = 1 },
};

static void intsel_reset_hold(Object *obj, ResetType type)
{
    RH850U2BIntselState *s = RH850_U2B_INTSEL(obj);

    memset(s->sel, 0, sizeof(s->sel));
}

static void intsel_reset_exit(Object *obj, ResetType type)
{
    intsel_update(RH850_U2B_INTSEL(obj));
}

static void intsel_init(Object *obj)
{
    RH850U2BIntselState *s = RH850_U2B_INTSEL(obj);
    int i;

    memory_region_init_io(&s->iomem, obj, &intsel_ops, s, "rh850-u2b-intsel",
                          RH850_U2B_INTSEL_SIZE);
    sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->iomem);
    for (i = 0; i < ARRAY_SIZE(s->out); i++) {
        sysbus_init_irq(SYS_BUS_DEVICE(obj), &s->out[i]);
    }
    qdev_init_gpio_in(DEVICE(obj), intsel_set_irq, ARRAY_SIZE(s->in));
}

static const VMStateDescription vmstate_intsel = {
    .name = TYPE_RH850_U2B_INTSEL,
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (const VMStateField[]) {
        VMSTATE_UINT32_ARRAY(sel, RH850U2BIntselState, RH850_U2B_INTSEL_NREG),
        VMSTATE_UINT8_ARRAY(in, RH850U2BIntselState,
                            RH850_U2B_INTSEL_NUNIT * RH850_U2B_INTSEL_NLINE),
        VMSTATE_END_OF_LIST()
    }
};

static void intsel_class_init(ObjectClass *klass, const void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    ResettableClass *rc = RESETTABLE_CLASS(klass);

    dc->vmsd = &vmstate_intsel;
    rc->phases.hold = intsel_reset_hold;
    rc->phases.exit = intsel_reset_exit;
}

static const TypeInfo intsel_types[] = {
    {
        .name           = TYPE_RH850_U2B_INTSEL,
        .parent         = TYPE_SYS_BUS_DEVICE,
        .instance_size  = sizeof(RH850U2BIntselState),
        .instance_init  = intsel_init,
        .class_init     = intsel_class_init,
    },
};

DEFINE_TYPES(intsel_types)
