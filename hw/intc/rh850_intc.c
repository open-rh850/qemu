/*
 * SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright (c) 2026 Linumiz
 */

#include "qemu/osdep.h"
#include "qemu/log.h"
#include "qemu/module.h"
#include "qapi/error.h"
#include "hw/core/irq.h"
#include "hw/core/qdev-properties.h"
#include "hw/intc/rh850_intc.h"
#include "migration/vmstate.h"

#include "rh850_u2c8_level.c.inc"

#define INTC1_IMR0      0x0f0
#define INTC1_EIBD      0x100
#define INTC1_FIBD      0x1c0
#define INTC1_EEIC      0x200
#define INTC2_IMR       0x1000
#define INTC2_EIBD      0x2000
#define INTC2_EEIC      0x4000

#define FENC_FENMIF     0x0
#define FENC_FENMIC     0x8
#define FEINC_FEINTF    0x0
#define FEINC_FEINTMSK  0x4
#define FEINC_FEINTC    0x8
#define FEINT_BITS      0x7

#define EIBD_CST        (1u << 31)
#define EIBD_WMASK      0x03000007u
#define EIBD_PEID       0x7u

static bool rh850_intc_to_pe0(RH850INTCState *s, int ch)
{
    return ch < RH850_INTC1_NCH || !(s->eibd[ch] & EIBD_PEID);
}

static void rh850_intc_update(RH850INTCState *s)
{
    int ch, best = -1;
    uint32_t req = 0;

    for (ch = 0; ch < s->nch; ch++) {
        if (s->eirf[ch] && !s->eimk[ch] && rh850_intc_to_pe0(s, ch) &&
            (best < 0 || s->eip[ch] < s->eip[best])) {
            best = ch;
        }
    }
    if (best >= 0) {
        req = RH850_INTC_REQ_VALID | s->eitb[best] << RH850_INTC_REQ_TB |
              s->eip[best] << RH850_INTC_REQ_PRIO | best;
    }
    qemu_set_irq(s->out[RH850_INTC_OUT_EIINT], req);
    qemu_set_irq(s->out[RH850_INTC_OUT_FEINT],
                 !!(s->feintf & ~s->feintmsk & FEINT_BITS));
    qemu_set_irq(s->out[RH850_INTC_OUT_FENMI], s->fenmi_req);
}

static void rh850_intc_set_irq(void *opaque, int ch, int level)
{
    RH850INTCState *s = opaque;

    level = !!level;
    if (s->level[ch]) {
        s->eirf[ch] = level;
    } else if (level && !s->in[ch]) {
        if (s->eirf[ch]) {
            s->eiov[ch] = 1;
        }
        s->eirf[ch] = 1;
    }
    s->in[ch] = level;
    rh850_intc_update(s);
}

static void rh850_intc_set_fenmi(void *opaque, int n, int level)
{
    RH850INTCState *s = opaque;

    if (level && !s->fenmi_in) {
        s->fenmif = 1;
        s->fenmi_req = 1;
    }
    s->fenmi_in = !!level;
    rh850_intc_update(s);
}

static void rh850_intc_set_feint(void *opaque, int n, int level)
{
    RH850INTCState *s = opaque;

    s->feint_in = deposit32(s->feint_in, n, 1, !!level);
    s->feintf |= s->feint_in;
    rh850_intc_update(s);
}

static void rh850_intc_ack(void *opaque, int n, int level)
{
    RH850INTCState *s = opaque;

    switch (n) {
    case RH850_INTC_ACK_EIINT:
        if (level >= 0 && level < s->nch && !s->level[level]) {
            s->eirf[level] = 0;
        }
        break;
    case RH850_INTC_ACK_FENMI:
        s->fenmi_req = 0;
        break;
    default:
        return;
    }
    rh850_intc_update(s);
}

static uint32_t rh850_eic_read(RH850INTCState *s, int ch)
{
    return s->level[ch] << 15 | s->eirf[ch] << 12 | s->eimk[ch] << 7 |
           s->eitb[ch] << 6 | s->eiov[ch] << 5 | extract32(s->eip[ch], 0, 4);
}

static uint32_t rh850_eeic_read(RH850INTCState *s, int ch)
{
    return (uint32_t)s->level[ch] << 31 | s->eirf[ch] << 28 |
           s->eimk[ch] << 23 | s->eitb[ch] << 22 | s->eiov[ch] << 15 |
           s->eip[ch];
}

static void rh850_eic_write(RH850INTCState *s, int ch, int lane, uint8_t v)
{
    if (lane == 0) {
        s->eimk[ch] = extract32(v, 7, 1);
        s->eitb[ch] = extract32(v, 6, 1);
        s->eiov[ch] = extract32(v, 5, 1);
        s->eip[ch] = deposit32(s->eip[ch], 0, 4, v);
    } else if (!s->level[ch]) {
        s->eirf[ch] = extract32(v, 4, 1);
    }
}

static void rh850_eeic_write(RH850INTCState *s, int ch, int lane, uint8_t v)
{
    switch (lane) {
    case 0:
        s->eip[ch] = extract32(v, 0, s->prio_bits);
        break;
    case 1:
        s->eiov[ch] = extract32(v, 7, 1);
        break;
    case 2:
        s->eimk[ch] = extract32(v, 7, 1);
        s->eitb[ch] = extract32(v, 6, 1);
        break;
    default:
        if (!s->level[ch]) {
            s->eirf[ch] = extract32(v, 4, 1);
        }
        break;
    }
}

static uint32_t rh850_imr_read(RH850INTCState *s, int m)
{
    uint32_t v = 0;
    int k, ch;

    for (k = 0; k < 32; k++) {
        ch = m * 32 + k;
        v |= (uint32_t)(ch < s->nch ? s->eimk[ch] : 1) << k;
    }
    return v;
}

static void rh850_imr_write(RH850INTCState *s, int m, int lane, uint8_t v)
{
    int k, ch;

    for (k = 0; k < 8; k++) {
        ch = m * 32 + lane * 8 + k;
        if (ch < s->nch && rh850_intc_to_pe0(s, ch)) {
            s->eimk[ch] = extract32(v, k, 1);
        }
    }
}

static uint32_t rh850_eibd_wmask(RH850INTCState *s, int ch)
{
    return EIBD_WMASK | (s->level[ch] ? 0 : EIBD_CST);
}

enum { R_NONE, R_EIC, R_EEIC, R_IMR, R_EIBD, R_RO0 };

static int rh850_intc_decode(RH850INTCState *s, hwaddr addr, bool intc2,
                             int *idx)
{
    int lo = intc2 ? RH850_INTC1_NCH : 0;
    int hi = intc2 ? s->nch : RH850_INTC1_NCH;
    hwaddr eibd = intc2 ? INTC2_EIBD : INTC1_EIBD;
    hwaddr eeic = intc2 ? INTC2_EEIC : INTC1_EEIC;

    if (addr >= 2 * lo && addr < 2 * hi) {
        *idx = addr / 2;
        return R_EIC;
    }
    if (addr >= eeic + 4 * lo && addr < eeic + 4 * hi) {
        *idx = (addr - eeic) / 4;
        return R_EEIC;
    }
    if (addr >= eibd + 4 * lo && addr < eibd + 4 * hi) {
        *idx = (addr - eibd) / 4;
        return intc2 ? R_EIBD : R_RO0;
    }
    if (!intc2 && (addr & ~3) == INTC1_IMR0) {
        *idx = 0;
        return R_IMR;
    }
    if (!intc2 && (addr & ~3) == INTC1_FIBD) {
        return R_RO0;
    }
    if (intc2 && addr >= INTC2_IMR + 4 &&
        addr < INTC2_IMR + 4 * DIV_ROUND_UP(s->nch, 32)) {
        *idx = (addr - INTC2_IMR) / 4;
        return R_IMR;
    }
    return R_NONE;
}

static uint64_t rh850_intc_read(RH850INTCState *s, hwaddr addr, bool intc2)
{
    int idx = 0;
    uint32_t v;

    switch (rh850_intc_decode(s, addr, intc2, &idx)) {
    case R_EIC:
        return extract32(rh850_eic_read(s, idx), (addr & 1) * 8, 8);
    case R_EEIC:
        v = rh850_eeic_read(s, idx);
        break;
    case R_IMR:
        v = rh850_imr_read(s, idx);
        break;
    case R_EIBD:
        v = s->eibd[idx];
        break;
    case R_RO0:
        return 0;
    default:
        qemu_log_mask(LOG_GUEST_ERROR, "rh850_intc: read of reserved "
                      "INTC%d offset 0x%" HWADDR_PRIx "\n", intc2 + 1, addr);
        return 0;
    }
    return extract32(v, (addr & 3) * 8, 8);
}

static void rh850_intc_write(RH850INTCState *s, hwaddr addr, uint8_t v,
                             bool intc2)
{
    int idx = 0;
    int lane = addr & 3;
    uint32_t m;

    switch (rh850_intc_decode(s, addr, intc2, &idx)) {
    case R_EIC:
        rh850_eic_write(s, idx, addr & 1, v);
        break;
    case R_EEIC:
        rh850_eeic_write(s, idx, lane, v);
        break;
    case R_IMR:
        rh850_imr_write(s, idx, lane, v);
        break;
    case R_EIBD:
        m = rh850_eibd_wmask(s, idx) & (0xffu << (lane * 8));
        s->eibd[idx] = (s->eibd[idx] & ~m) | (((uint32_t)v << lane * 8) & m);
        break;
    case R_RO0:
        return;
    default:
        qemu_log_mask(LOG_GUEST_ERROR, "rh850_intc: write of reserved "
                      "INTC%d offset 0x%" HWADDR_PRIx "\n", intc2 + 1, addr);
        return;
    }
    rh850_intc_update(s);
}

static uint64_t rh850_intc1_read(void *opaque, hwaddr addr, unsigned size)
{
    return rh850_intc_read(opaque, addr, false);
}

static void rh850_intc1_write(void *opaque, hwaddr addr, uint64_t val,
                              unsigned size)
{
    rh850_intc_write(opaque, addr, val, false);
}

static uint64_t rh850_intc2_read(void *opaque, hwaddr addr, unsigned size)
{
    return rh850_intc_read(opaque, addr, true);
}

static void rh850_intc2_write(void *opaque, hwaddr addr, uint64_t val,
                              unsigned size)
{
    rh850_intc_write(opaque, addr, val, true);
}

static const MemoryRegionOps rh850_intc1_ops = {
    .read = rh850_intc1_read,
    .write = rh850_intc1_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .valid = { .min_access_size = 1, .max_access_size = 4 },
    .impl = { .min_access_size = 1, .max_access_size = 1 },
};

static const MemoryRegionOps rh850_intc2_ops = {
    .read = rh850_intc2_read,
    .write = rh850_intc2_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .valid = { .min_access_size = 1, .max_access_size = 4 },
    .impl = { .min_access_size = 1, .max_access_size = 1 },
};

static uint64_t rh850_fenc_read(void *opaque, hwaddr addr, unsigned size)
{
    RH850INTCState *s = opaque;

    return addr == FENC_FENMIF ? s->fenmif : 0;
}

static void rh850_fenc_write(void *opaque, hwaddr addr, uint64_t val,
                             unsigned size)
{
    RH850INTCState *s = opaque;

    if (addr == FENC_FENMIC && (val & 1)) {
        s->fenmif = 0;
    }
}

static uint64_t rh850_feinc_read(void *opaque, hwaddr addr, unsigned size)
{
    RH850INTCState *s = opaque;

    switch (addr) {
    case FEINC_FEINTF:
        return s->feintf;
    case FEINC_FEINTMSK:
        return s->feintmsk;
    default:
        return 0;
    }
}

static void rh850_feinc_write(void *opaque, hwaddr addr, uint64_t val,
                              unsigned size)
{
    RH850INTCState *s = opaque;

    switch (addr) {
    case FEINC_FEINTMSK:
        s->feintmsk = val & FEINT_BITS;
        break;
    case FEINC_FEINTC:
        s->feintf = (s->feintf & ~val) | s->feint_in;
        break;
    default:
        return;
    }
    rh850_intc_update(s);
}

static const MemoryRegionOps rh850_fenc_ops = {
    .read = rh850_fenc_read,
    .write = rh850_fenc_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .valid = { .min_access_size = 4, .max_access_size = 4 },
};

static const MemoryRegionOps rh850_feinc_ops = {
    .read = rh850_feinc_read,
    .write = rh850_feinc_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .valid = { .min_access_size = 4, .max_access_size = 4 },
};

static void rh850_intc_reset_hold(Object *obj, ResetType type)
{
    RH850INTCState *s = RH850_INTC(obj);
    int ch;

    for (ch = 0; ch < s->nch; ch++) {
        s->eirf[ch] = s->level[ch] ? s->in[ch] : 0;
        s->eimk[ch] = 1;
        s->eitb[ch] = 0;
        s->eiov[ch] = 0;
        s->eip[ch] = 0xf;
        s->eibd[ch] = 0;
    }
    s->fenmif = 0;
    s->fenmi_req = 0;
    s->feintf = s->feint_in;
    s->feintmsk = FEINT_BITS;
}

static void rh850_intc_reset_exit(Object *obj, ResetType type)
{
    rh850_intc_update(RH850_INTC(obj));
}

static void rh850_intc_init(Object *obj)
{
    RH850INTCState *s = RH850_INTC(obj);
    SysBusDevice *d = SYS_BUS_DEVICE(obj);
    size_t i;

    memory_region_init_io(&s->intc1, obj, &rh850_intc1_ops, s,
                          "rh850-intc1", RH850_INTC1_SIZE);
    memory_region_init_io(&s->intc2, obj, &rh850_intc2_ops, s,
                          "rh850-intc2", RH850_INTC2_SIZE);
    memory_region_init_io(&s->fenc, obj, &rh850_fenc_ops, s,
                          "rh850-fenc", RH850_INTC_FE_SIZE);
    memory_region_init_io(&s->feinc, obj, &rh850_feinc_ops, s,
                          "rh850-feinc", RH850_INTC_FE_SIZE);
    sysbus_init_mmio(d, &s->intc1);
    sysbus_init_mmio(d, &s->intc2);
    sysbus_init_mmio(d, &s->fenc);
    sysbus_init_mmio(d, &s->feinc);
    for (i = 0; i < RH850_INTC_OUT_NUM; i++) {
        sysbus_init_irq(d, &s->out[i]);
    }
    qdev_init_gpio_in_named(DEVICE(obj), rh850_intc_set_fenmi, "fenmi", 1);
    qdev_init_gpio_in_named(DEVICE(obj), rh850_intc_set_feint, "feint",
                            RH850_INTC_NFEINT);
    qdev_init_gpio_in_named(DEVICE(obj), rh850_intc_ack, "ack",
                            RH850_INTC_ACK_NUM);
}

static void rh850_intc_set_level(RH850INTCState *s, uint32_t lo, uint32_t hi,
                                 Error **errp)
{
    if (lo > hi || hi >= s->nch) {
        error_setg(errp, "level range %" PRIu32 "-%" PRIu32
                   " is outside 0-%" PRIu32, lo, hi, s->nch - 1);
        return;
    }
    while (lo <= hi) {
        s->level[lo++] = 1;
    }
}

static void rh850_intc_realize(DeviceState *dev, Error **errp)
{
    RH850INTCState *s = RH850_INTC(dev);
    Error *err = NULL;
    size_t i;

    if (s->nch <= RH850_INTC1_NCH || s->nch > RH850_INTC_MAX_NCH) {
        error_setg(errp, "num-channels %" PRIu32 " is outside %d-%d",
                   s->nch, RH850_INTC1_NCH + 1, RH850_INTC_MAX_NCH);
        return;
    }
    if (s->prio_bits != 4 && s->prio_bits != 6) {
        error_setg(errp, "prio-bits must be 4 or 6");
        return;
    }
    if (s->nlevel_ranges & 1) {
        error_setg(errp, "level-ranges needs pairs of channels");
        return;
    }
    s->eirf = g_new0(uint8_t, s->nch);
    s->eimk = g_new0(uint8_t, s->nch);
    s->eitb = g_new0(uint8_t, s->nch);
    s->eiov = g_new0(uint8_t, s->nch);
    s->eip = g_new0(uint8_t, s->nch);
    s->eibd = g_new0(uint32_t, s->nch);
    s->in = g_new0(uint8_t, s->nch);
    s->level = g_new0(uint8_t, s->nch);
    if (s->nlevel_ranges) {
        for (i = 0; i < s->nlevel_ranges && !err; i += 2) {
            rh850_intc_set_level(s, s->level_ranges[i],
                                 s->level_ranges[i + 1], &err);
        }
    } else if (s->u2c_level_table) {
        for (i = 0; i < ARRAY_SIZE(rh850_u2c8_level) && !err; i++) {
            rh850_intc_set_level(s, rh850_u2c8_level[i][0],
                                 rh850_u2c8_level[i][1], &err);
        }
    }
    if (err) {
        error_propagate(errp, err);
        return;
    }
    qdev_init_gpio_in(dev, rh850_intc_set_irq, s->nch);
}

static void rh850_intc_unrealize(DeviceState *dev)
{
    RH850INTCState *s = RH850_INTC(dev);

    g_free(s->eirf);
    g_free(s->eimk);
    g_free(s->eitb);
    g_free(s->eiov);
    g_free(s->eip);
    g_free(s->eibd);
    g_free(s->in);
    g_free(s->level);
}

static const Property rh850_intc_properties[] = {
    DEFINE_PROP_UINT32("num-channels", RH850INTCState, nch, RH850_INTC_NCH),
    DEFINE_PROP_UINT8("prio-bits", RH850INTCState, prio_bits, 4),
    DEFINE_PROP_BOOL("u2c-level-table", RH850INTCState, u2c_level_table, true),
    DEFINE_PROP_ARRAY("level-ranges", RH850INTCState, nlevel_ranges,
                      level_ranges, qdev_prop_uint32, uint32_t),
};

static const VMStateDescription vmstate_rh850_intc = {
    .name = TYPE_RH850_INTC,
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (const VMStateField[]) {
        VMSTATE_VARRAY_UINT32(eirf, RH850INTCState, nch, 0,
                              vmstate_info_uint8, uint8_t),
        VMSTATE_VARRAY_UINT32(eimk, RH850INTCState, nch, 0,
                              vmstate_info_uint8, uint8_t),
        VMSTATE_VARRAY_UINT32(eitb, RH850INTCState, nch, 0,
                              vmstate_info_uint8, uint8_t),
        VMSTATE_VARRAY_UINT32(eiov, RH850INTCState, nch, 0,
                              vmstate_info_uint8, uint8_t),
        VMSTATE_VARRAY_UINT32(eip, RH850INTCState, nch, 0,
                              vmstate_info_uint8, uint8_t),
        VMSTATE_VARRAY_UINT32(eibd, RH850INTCState, nch, 0,
                              vmstate_info_uint32, uint32_t),
        VMSTATE_VARRAY_UINT32(in, RH850INTCState, nch, 0,
                              vmstate_info_uint8, uint8_t),
        VMSTATE_UINT32(fenmif, RH850INTCState),
        VMSTATE_UINT32(fenmi_req, RH850INTCState),
        VMSTATE_UINT32(fenmi_in, RH850INTCState),
        VMSTATE_UINT32(feintf, RH850INTCState),
        VMSTATE_UINT32(feintmsk, RH850INTCState),
        VMSTATE_UINT32(feint_in, RH850INTCState),
        VMSTATE_END_OF_LIST()
    }
};

static void rh850_intc_class_init(ObjectClass *klass, const void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    ResettableClass *rc = RESETTABLE_CLASS(klass);

    dc->realize = rh850_intc_realize;
    dc->unrealize = rh850_intc_unrealize;
    dc->vmsd = &vmstate_rh850_intc;
    device_class_set_props(dc, rh850_intc_properties);
    rc->phases.hold = rh850_intc_reset_hold;
    rc->phases.exit = rh850_intc_reset_exit;
}

static const TypeInfo rh850_intc_types[] = {
    {
        .name           = TYPE_RH850_INTC,
        .parent         = TYPE_SYS_BUS_DEVICE,
        .instance_size  = sizeof(RH850INTCState),
        .instance_init  = rh850_intc_init,
        .class_init     = rh850_intc_class_init,
    },
};

DEFINE_TYPES(rh850_intc_types)
