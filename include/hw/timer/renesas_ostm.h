/*
 * SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright (c) 2026 Linumiz
 */

#ifndef HW_TIMER_RENESAS_OSTM_H
#define HW_TIMER_RENESAS_OSTM_H

#include "hw/core/sysbus.h"
#include "qemu/timer.h"
#include "qom/object.h"

#define TYPE_RENESAS_OSTM "renesas-ostm"
OBJECT_DECLARE_SIMPLE_TYPE(RenesasOSTMState, RENESAS_OSTM)

#define RENESAS_OSTM_SIZE 0x40

struct RenesasOSTMState {
    SysBusDevice parent_obj;

    MemoryRegion iomem;
    QEMUTimer timer;
    Clock *clk;
    qemu_irq tint;

    uint32_t cmp;
    uint32_t cnt;
    uint8_t to;
    uint8_t toe;
    uint8_t te;
    uint8_t ctl;
    int64_t base;
    uint8_t tint_held;
};

#endif
