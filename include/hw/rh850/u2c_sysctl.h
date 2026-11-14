/*
 * SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright (c) 2026 Linumiz
 */

#ifndef HW_RH850_U2C_SYSCTL_H
#define HW_RH850_U2C_SYSCTL_H

#include "hw/core/sysbus.h"
#include "qom/object.h"

#define TYPE_RH850_U2C_SYSCTL "rh850-u2c-sysctl"
OBJECT_DECLARE_SIMPLE_TYPE(RH850U2CSysctlState, RH850_U2C_SYSCTL)

#define RH850_U2C_SYSCTL_BASE   0xff980000
#define RH850_U2C_SYSCTL_SIZE   0x10000
#define RH850_U2C_SYSCTL_NREGS  132

enum {
    SC_RW,
    SC_RO,
    SC_TRIG,
    SC_KEY,
};

enum {
    SC_KEY_NONE,
    SC_KEY_CLKKCPROT1,
    SC_KEY_CLKKCPROT4,
    SC_KEY_MSRKCPROT,
    SC_KEY_STBCKCPROT,
    SC_NKEYS,
};

typedef struct RH850U2CSysctlReg {
    uint16_t offset;
    uint32_t reset;
    uint32_t wmask;
    uint8_t kind;
    uint8_t key;
    int16_t mirror;
} RH850U2CSysctlReg;

struct RH850U2CSysctlState {
    SysBusDevice parent_obj;

    MemoryRegion iomem;
    char *chip;
    const RH850U2CSysctlReg *regs;
    int nregs;
    uint32_t val[RH850_U2C_SYSCTL_NREGS];
    uint32_t reskce;
    uint32_t resf;
};

#endif
