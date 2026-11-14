# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright (c) 2026 Linumiz

    .include "macros.inc"
    .macro LDMMP r1, eh, et
    .hword (\eh << 11) | 0x07e0 | \r1, (\et << 11) | 0x0166
    .endm
    .macro STMMP eh, et, r1
    .hword (\eh << 11) | 0x07e0 | \r1, (\et << 11) | 0x0164
    .endm
    .text
    VECTORS 0
    RECCLR
    LI32 1, 0xfe010000
    LI32 2, 0x11111113
    st.w r2, 0[r1]
    LI32 2, 0x22222222
    st.w r2, 4[r1]
    LI32 2, 0xffffffff
    st.w r2, 8[r1]
    LI32 2, 0x44444447
    st.w r2, 12[r1]
    LI32 2, 0x55555555
    st.w r2, 16[r1]
    LI32 2, 0x00000012
    st.w r2, 20[r1]
    LI32 3, 0xfe010003
    LDMMP 3, 30, 31
    CHECK 3, 0xfe010003, 1
    LI32 4, 30
    ldsr r4, 16, 5
    stsr 20, r10, 5
    CHECK 10, 0x11111110, 2
    stsr 21, r10, 5
    CHECK 10, 0x22222220, 3
    stsr 22, r10, 5
    CHECK 10, 0xffffc0bf, 4
    LI32 4, 31
    ldsr r4, 16, 5
    stsr 20, r10, 5
    CHECK 10, 0x44444444, 5
    stsr 22, r10, 5
    CHECK 10, 0x00000012, 6
    LI32 5, 0xfe020002
    STMMP 30, 31, 5
    CHECKM 0xfe020000, 0x11111110, 7
    CHECKM 0xfe020004, 0x22222220, 8
    CHECKM 0xfe020008, 0xffffc0bf, 9
    CHECKM 0xfe02000c, 0x44444444, 10
    CHECKM 0xfe020010, 0x55555554, 11
    CHECKM 0xfe020014, 0x00000012, 12
    CHECKM 0xfe020018, 0, 13
    LDMMP 1, 1, 0
    LI32 4, 1
    ldsr r4, 16, 5
    stsr 20, r10, 5
    CHECK 10, 0, 14
    LDMMP 1, 0, 0
    stsr 20, r10, 5
    CHECK 10, 0, 15
    LI32 4, 0
    ldsr r4, 16, 5
    stsr 20, r10, 5
    CHECK 10, 0x11111110, 16
    LI32 4, 1
    ldsr r4, 8, 1
    LI32 1, 0xfe010000 + 12
    LDMMP 1, 0, 0
    LI32 4, 0
    ldsr r4, 8, 1
    stsr 20, r10, 5
    CHECK 10, 0x11111110, 17
    CHECKR REC_CNT, 0, 18
    SETPSW 0x4000002b
    RESUME d1
i1: LDMMP 1, 0, 0
d1: CHECKR REC_CNT, 1, 20
    CHECKFE 0xa0, 0xa0, i1, 0x4000002b, 0xeb, 21
    SETPSW 0x4000002b
    RESUME d2
i2: STMMP 0, 0, 5
d2: CHECKR REC_CNT, 2, 30
    CHECKFE 0xa0, 0xa0, i2, 0x4000002b, 0xeb, 31
    PASS
