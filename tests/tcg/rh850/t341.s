# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright (c) 2026 Linumiz

    .include "macros.inc"
    .text
    VECTORS 0
    RECCLR
    stsr 6, r10, 1
    CHECK 10, 0x07840700, 1
    stsr 2, r10, 5
    CHECK 10, 0x0002001f, 2
    SETPSW 0x00010020
    stsr 6, r10, 0
    CHECK 10, 0x00220000, 3
    LI32 11, 0xffffffff
    ldsr r11, 6, 0
    stsr 6, r10, 0
    CHECK 10, 0xffeeffff, 4
    ldsr r0, 6, 0
    stsr 6, r10, 0
    CHECK 10, 0x00200000, 5
    SETPSW 0x20
    stsr 13, r10, 2
    CHECK 10, 0x000f0000, 6
    LI32 11, 0x00070003
    ldsr r11, 13, 2
    stsr 13, r10, 2
    CHECK 10, 0x00070002, 7
    ldsr r0, 13, 2
    LI32 11, 40
    ldsr r11, 17, 2
    stsr 17, r10, 2
    CHECK 10, 40, 8
    ldsr r0, 17, 2
    LI32 11, 0xffffffff
    ldsr r11, 14, 2
    stsr 14, r10, 2
    CHECK 10, 0x3f, 9
    LI32 11, 0x12345678
    ldsr r11, 0, 9
    stsr 0, r10, 9
    CHECK 10, 0, 10
    LI32 11, 0xffffffff
    ldsr r11, 0, 14
    stsr 0, r10, 14
    CHECK 10, 0x0000ff03, 11
    ldsr r11, 16, 14
    stsr 16, r10, 14
    CHECK 10, 0xffffffff, 12
    ldsr r11, 7, 15
    stsr 7, r10, 15
    CHECK 10, 0x00001fff, 13
    ldsr r11, 16, 1
    stsr 16, r10, 1
    CHECK 10, 0, 14
    ldsr r11, 15, 0
    stsr 15, r10, 0
    CHECK 10, 0, 15
    ldsr r11, 18, 0
    stsr 18, r10, 0
    CHECK 10, 0, 16
    stsr 0, r10, 3
    CHECK 10, 0, 23
    ldsr r11, 17, 1
    stsr 17, r10, 1
    CHECK 10, 0, 24
    ldsr r11, 9, 11
    stsr 9, r10, 11
    CHECK 10, 0, 25
    ldsr r11, 11, 0
    stsr 11, r10, 0
    CHECK 10, 0, 17
    stsr 5, r10, 1
    CHECK 10, 0, 18
    stsr 1, r10, 1
    CHECK 10, 0xffffffff, 19
    stsr 26, r10, 4
    CHECK 10, 0x00001044, 20
    stsr 24, r10, 4
    CHECK 10, 0x00010003, 21
    LI32 11, 31
    ldsr r11, 16, 5
    LI32 11, 0x00001234
    ldsr r11, 20, 5
    stsr 20, r10, 5
    CHECK 10, 0x00001234, 22
    SETPSW 0x2b
    RESUME d1
i1: .hword 0x07e0 | 5, 0x0110
d1: CHECKR REC_CNT, 1, 30
    CHECKFE 0x60, 0x60, i1, 0x2b, 0xeb, 31
    SETPSW 0x2b
    RESUME d2
i2: .hword 0x07e0 | 1, 0x9960
d2: CHECKR REC_CNT, 2, 40
    CHECKFE 0x60, 0x60, i2, 0x2b, 0xeb, 41
    SETPSW 0x2b
    RESUME d3
i3: .hword 0x07e0 | 1, 0x9160
d3: CHECKR REC_CNT, 3, 50
    CHECKFE 0x60, 0x60, i3, 0x2b, 0xeb, 51
    PASS
