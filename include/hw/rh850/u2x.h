/*
 * SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright (c) 2026 Linumiz
 */

#ifndef HW_RH850_U2X_H
#define HW_RH850_U2X_H

#include "hw/core/boards.h"
#include "hw/intc/rh850_intc.h"
#include "hw/timer/renesas_ostm.h"
#include "hw/char/renesas_rlin3.h"
#include "hw/rh850/u2c_sysctl.h"
#include "hw/rh850/u2b_intsel.h"
#include "hw/core/split-irq.h"
#include "qemu/units.h"
#include "target/rh850/cpu.h"

#define TYPE_RH850_U2X_MACHINE MACHINE_TYPE_NAME("rh850-u2x")
#define TYPE_RH850_U2A_MACHINE MACHINE_TYPE_NAME("rh850-u2a")
#define TYPE_RH850_U2B_MACHINE MACHINE_TYPE_NAME("rh850-u2b")
OBJECT_DECLARE_TYPE(RH850U2XMachineState, RH850U2XMachineClass,
                    RH850_U2X_MACHINE)

#define RH850_U2X_CFLASH_BASE       0x00000000
#define RH850_U2X_TEST_BASE         0x40000000
#define RH850_U2X_LRAM0_BASE        0xfdc00000
#define RH850_U2X_LRAM_SELF_BASE    0xfde00000
#define RH850_U2X_LRAM_SIZE         (64 * KiB)
#define RH850_U2X_PBUS_BASE         0xff000000
#define RH850_U2X_PBUS_SIZE         0xfb8000
#define RH850_U2X_IBUS_BASE         0xfffb8000
#define RH850_U2X_IBUS_SIZE         0x8000
#define RH850_U2X_CPUPERI_BASE      0xfffc0000

#define RH850_U2X_INTC1_SELF_BASE   0xfffc0000
#define RH850_U2X_INTC1_PE0_BASE    0xfffc4000
#define RH850_U2X_INTC2_BASE        0xfff80000
#define RH850_U2X_FENC_BASE         0xff9a3a00
#define RH850_U2X_FEINC_PE0_BASE    0xff9a3b00
#define RH850_U2X_SYSCTL_BASE       0xff980000

#define RH850_U2X_NCRAM             4
#define RH850_U2X_NOSTM             2
#define RH850_U2X_NRLIN3            24
#define RH850_U2X_CONSOLE_RLIN3     0
#define RH850_U2X_CLK_HSB_HZ        80000000

typedef struct RH850U2XRegion {
    hwaddr base;
    uint64_t size;
} RH850U2XRegion;

typedef struct RH850U2XChip {
    const char *desc;
    const char *sysctl_chip;
    uint32_t pid;
    uint64_t cflash_size;
    uint64_t cpuperi_size;
    RH850U2XRegion cram[RH850_U2X_NCRAM];
    uint32_t intc_channels;
    const uint32_t (*level)[2];
    unsigned nlevel;
    hwaddr ostm_base[RH850_U2X_NOSTM];
    uint16_t ostm_irq[RH850_U2X_NOSTM];
    hwaddr rlin3_base[RH850_U2X_NRLIN3];
    uint16_t rlin3_irq[RH850_U2X_NRLIN3];
    const uint16_t *intsel_irq;
} RH850U2XChip;

struct RH850U2XMachineClass {
    MachineClass parent_class;

    const RH850U2XChip *chip;
};

struct RH850U2XMachineState {
    MachineState parent_obj;

    RH850CPU cpu;
    RH850INTCState intc;
    MemoryRegion intc1_self;
    RenesasOSTMState ostm[RH850_U2X_NOSTM];
    RenesasRLIN3State rlin3[RH850_U2X_NRLIN3];
    RH850U2CSysctlState sysctl;
    RH850U2BIntselState intsel;
    SplitIRQ rlin3_split[RH850_U2X_NRLIN3][RENESAS_RLIN3_NIRQ];
    Clock *clk_hsb;
    uint32_t clk_hsb_hz;
    bool test_exit;
    MemoryRegion cflash;
    MemoryRegion lram0;
    MemoryRegion lram_self;
    MemoryRegion cram[RH850_U2X_NCRAM];
};

#endif
