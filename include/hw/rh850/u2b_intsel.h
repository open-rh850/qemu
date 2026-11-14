/*
 * SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright (c) 2026 Linumiz
 */

#ifndef HW_RH850_U2B_INTSEL_H
#define HW_RH850_U2B_INTSEL_H

#include "hw/core/sysbus.h"
#include "qom/object.h"

#define TYPE_RH850_U2B_INTSEL "rh850-u2b-intsel"
OBJECT_DECLARE_SIMPLE_TYPE(RH850U2BIntselState, RH850_U2B_INTSEL)

#define RH850_U2B_INTSEL_BASE       0xff090800
#define RH850_U2B_INTSEL_SIZE       0x18
#define RH850_U2B_INTSEL_NREG       6
#define RH850_U2B_INTSEL_FIRST      6
#define RH850_U2B_INTSEL_NUNIT      17
#define RH850_U2B_INTSEL_NMUX       34
#define RH850_U2B_INTSEL_NLINE      4

struct RH850U2BIntselState {
    SysBusDevice parent_obj;

    MemoryRegion iomem;
    qemu_irq out[RH850_U2B_INTSEL_NMUX * RH850_U2B_INTSEL_NLINE];
    uint32_t sel[RH850_U2B_INTSEL_NREG];
    uint8_t in[RH850_U2B_INTSEL_NUNIT * RH850_U2B_INTSEL_NLINE];
};

#endif
