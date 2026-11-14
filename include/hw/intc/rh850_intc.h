/*
 * SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright (c) 2026 Linumiz
 */

#ifndef HW_INTC_RH850_INTC_H
#define HW_INTC_RH850_INTC_H

#include "hw/core/sysbus.h"
#include "qom/object.h"

#define TYPE_RH850_INTC "rh850-intc"
OBJECT_DECLARE_SIMPLE_TYPE(RH850INTCState, RH850_INTC)

#define RH850_INTC_NCH          630
#define RH850_INTC_MAX_NCH      1024
#define RH850_INTC1_NCH         32
#define RH850_INTC_NFEINT       3

#define RH850_INTC1_SIZE        0x280
#define RH850_INTC2_SIZE        0x8000
#define RH850_INTC_FE_SIZE      0x10

#define RH850_INTC_REQ_VALID    (1 << 24)
#define RH850_INTC_REQ_TB       22
#define RH850_INTC_REQ_PRIO     16
#define RH850_INTC_REQ_CH_MASK  0x3ff

enum {
    RH850_INTC_OUT_EIINT,
    RH850_INTC_OUT_FEINT,
    RH850_INTC_OUT_FENMI,
    RH850_INTC_OUT_NUM,
};

enum {
    RH850_INTC_ACK_EIINT,
    RH850_INTC_ACK_FENMI,
    RH850_INTC_ACK_EOI,
    RH850_INTC_ACK_NUM,
};

struct RH850INTCState {
    SysBusDevice parent_obj;

    MemoryRegion intc1;
    MemoryRegion intc2;
    MemoryRegion fenc;
    MemoryRegion feinc;
    qemu_irq out[RH850_INTC_OUT_NUM];

    uint32_t nch;
    uint8_t prio_bits;
    bool u2c_level_table;
    uint32_t nlevel_ranges;
    uint32_t *level_ranges;

    uint8_t *eirf;
    uint8_t *eimk;
    uint8_t *eitb;
    uint8_t *eiov;
    uint8_t *eip;
    uint32_t *eibd;
    uint8_t *in;
    uint8_t *level;

    uint32_t fenmif;
    uint32_t fenmi_req;
    uint32_t fenmi_in;
    uint32_t feintf;
    uint32_t feintmsk;
    uint32_t feint_in;
};

#endif
