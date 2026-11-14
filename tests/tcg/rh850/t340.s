# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright (c) 2026 Linumiz

    .include "macros.inc"
    .text
    VECTORS 0
    RECCLR
    LI32 1, 0xfe000100
    SETPSW 0x2b
    RESUME d1
i1: .hword (30 << 11) | 0x07e0 | 1, (31 << 11) | 0x0166
d1: CHECKR REC_CNT, 1, 1
    CHECKFE 0x60, 0x60, i1, 0x2b, 0xeb, 2
    SETPSW 0x2b
    RESUME d2
i2: .hword (30 << 11) | 0x07e0 | 1, (31 << 11) | 0x0164
d2: CHECKR REC_CNT, 2, 10
    CHECKFE 0x60, 0x60, i2, 0x2b, 0xeb, 11
    CHECKM 0xfe000100, 0, 20
    PASS
