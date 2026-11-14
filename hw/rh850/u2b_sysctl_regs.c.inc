/*
 * SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright (c) 2026 Linumiz
 */

static const RH850U2CSysctlReg rh850_u2b_sysctl_regs[] = {
    { 0x0140, 0x00000000, 0x00070007, SC_RW, SC_KEY_CLKKCPROT1, 1 },
    { 0x0148, 0x00000000, 0x00070007, SC_RO, SC_KEY_NONE, -1 },
    { 0x0700, 0x00000000, 0xffffffff, SC_KEY, SC_KEY_NONE, -1 },
    { 0x0964, 0x00000000, 0x00000000, SC_RO, SC_KEY_NONE, -1 },
    { 0x0a84, 0x00000000, 0x00000000, SC_RO, SC_KEY_NONE, -1 },
    { 0x1060, 0x00ffffff, 0x80ffffff, SC_RW, SC_KEY_MSRKCPROT, -1 },
    { 0x1180, 0x000003ff, 0x000003ff, SC_RW, SC_KEY_MSRKCPROT, -1 },
    { 0x1710, 0x00000000, 0xffffffff, SC_KEY, SC_KEY_NONE, -1 },
};
