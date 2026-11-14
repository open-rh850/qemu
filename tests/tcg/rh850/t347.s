# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright (c) 2026 Linumiz

    .include "macros.inc"
    .text
    VECTORS 0
    RECCLR
    stsr 6, r10, 1
    CHECK 10, 0x07840f01, 1
    EEICR 10, 1023
    CHECK 10, 0x0080000f, 2
    EEICW 1023, 0x0000003e
    EEICR 10, 1023
    CHECK 10, 0x0000003e, 3
    EICR 10, 1023
    CHECK 10, 0x000e, 4
    LI32 11, 0x3f
    ldsr r11, 14, 2
    LI32 11, 0x000f0002
    ldsr r11, 13, 2
    EEICW 1023, 0x1000003e
    SETPSW 0x03f00000
n1: CHECKR REC_CNT, 1, 5
    CHECKEI 0x1f0, 0x13ff, n1, 0x03f00000, 0x03e00020, 6
    PASS
