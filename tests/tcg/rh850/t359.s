# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright (c) 2026 Linumiz

    .include "macros.inc"
    .text
    VECTORS 0
    RECCLR
    SETPSW 0x00030020
    SETPSW 0x00010020
    RESUME u1
e1: .hword 0x17e1, 0x1da4
u1: CHECKFE 0x80, 0x81, e1, 0x00010020, 0x000100e0, 1
    SETPSW 0x00010020
    RESUME u2
e2: .hword 0x07a0, 0x1a9d, 0x0000
u2: CHECKFE 0x80, 0x81, e2, 0x00010020, 0x000100e0, 2
    SETPSW 0x00010020
    RESUME u3
e3: .hword 0x17e8, 0x1da0
u3: CHECKFE 0x80, 0x81, e3, 0x00010020, 0x000100e0, 3
    SETPSW 0x00010020
    RESUME u4
e4: .hword 0x07a1, 0x1e1d, 0x1004
u4: CHECKFE 0x80, 0x81, e4, 0x00010020, 0x000100e0, 4
    SETPSW 0x00030020
    PASS
fxset:
    ld.w 88[r20], r10
    ldsr r10, 6, 10
    ldsr r0, 12, 10
    ldsr r0, 13, 10
    .hword 0x07b4, 0x0a9d, 0x0000
    .hword 0x07b4, 0x129d, 0x0010
    .hword 0x07b4, 0x1a9d, 0x0020
    jmp [lp]
fxchk:
    LI32 21, 0xfe010000
    .hword 0x07b5, 0x1abd, 0x0000
    ld.w 0[r21], r10
    ld.w 48[r20], r11
    cmp r10, r11
    bne 9f
    ld.w 4[r21], r10
    ld.w 52[r20], r11
    cmp r10, r11
    bne 9f
    ld.w 8[r21], r10
    ld.w 56[r20], r11
    cmp r10, r11
    bne 9f
    ld.w 12[r21], r10
    ld.w 60[r20], r11
    cmp r10, r11
    bne 9f
    stsr 6, r10, 10
    ld.w 64[r20], r11
    cmp r10, r11
    bne 9f
    stsr 13, r10, 10
    ld.w 68[r20], r11
    cmp r10, r11
    bne 9f
    stsr 12, r10, 10
    ld.w 72[r20], r11
    cmp r10, r11
    bne 9f
    LI32 17, REC
    ld.w REC_CNT[r17], r10
    ld.w 76[r20], r11
    cmp r10, r11
    bne 9f
    ld.w 80[r20], r11
    cmp r0, r11
    be 1f
    ld.w REC_EIIC[r17], r10
    movea 0x75, r0, r11
    cmp r10, r11
    bne 9f
    ld.w REC_OFF[r17], r10
    movea 0x70, r0, r11
    cmp r10, r11
    bne 9f
    ld.w REC_EIPC[r17], r10
    ld.w 84[r20], r11
    cmp r10, r11
    bne 9f
1:  jmp [lp]
9:  shl 16, r12
    movea 0x3333, r12, r29
    movhi 0x4000, r0, r30
    st.w r29, 0[r30]
8:  br 8b
