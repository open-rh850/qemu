# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright (c) 2026 Linumiz

    .include "macros.inc"
    .text
    VECTORS 0
    RECCLR
    stsr 6, r10, 1
    CHECK 10, 0x07840f01, 1
    SETPSW 0x00030020
    stsr 5, r10, 0
    CHECK 10, 0x00030020, 2
    stsr 6, r10, 10
    CHECK 10, 0x00220000, 3
    stsr 9, r10, 10
    CHECK 10, 0x00000003, 4
    mov -1, r11
    ldsr r11, 6, 10
    stsr 6, r10, 10
    CHECK 10, 0x00eeffff, 5
    stsr 8, r10, 10
    CHECK 10, 0x00003f3f, 6
    stsr 10, r10, 10
    CHECK 10, 0x0000031f, 7
    ldsr r0, 8, 10
    stsr 6, r10, 10
    CHECK 10, 0x00ae03e0, 8
    ldsr r0, 10, 10
    stsr 6, r10, 10
    CHECK 10, 0x00a20000, 9
    ldsr r11, 12, 10
    stsr 12, r10, 10
    CHECK 10, 0x3f3f3f3f, 10
    ldsr r11, 13, 10
    stsr 13, r10, 10
    CHECK 10, 0x1f1f1f1f, 11
    SETPSW 0x00010020
    RESUME d1
i1: stsr 6, r10, 10
d1: CHECKFE 0x80, 0x81, i1, 0x00010020, 0x000100e0, 12
    SETPSW 0x00010020
    RESUME d2
i2: ldsr r0, 6, 10
d2: CHECKFE 0x80, 0x81, i2, 0x00010020, 0x000100e0, 20
    CHECKR REC_CNT, 2, 30
    PASS
