# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright (c) 2026 Linumiz

    .include "macros.inc"
    .text
    VECTORS 0
    RECCLR
    SETPSW 0x00030020
    LA 20, m1
    LI32 21, 0xfe010000
    LI32 10, 0xa0a0a0a0
    st.w r10, 256[r21]
    LI32 10, 0xb1b1b1b1
    st.w r10, 260[r21]
    LI32 10, 0xc2c2c2c2
    st.w r10, 264[r21]
    LI32 10, 0xd3d3d3d3
    st.w r10, 268[r21]
    .hword 0x07b5, 0x1a9d, 0x0100
    .hword 0x07b4, 0x183d, 0x0004
    .hword 0x07b5, 0x1abd, 0x0200
    ld.w 512[r21], r10
    CHECK 10, 0x22222222, 1
    ld.w 516[r21], r10
    CHECK 10, 0xb1b1b1b1, 1
    ld.w 520[r21], r10
    CHECK 10, 0xc2c2c2c2, 1
    ld.w 524[r21], r10
    CHECK 10, 0xd3d3d3d3, 1
    LI32 10, 0xa0a0a0a0
    st.w r10, 256[r21]
    LI32 10, 0xb1b1b1b1
    st.w r10, 260[r21]
    LI32 10, 0xc2c2c2c2
    st.w r10, 264[r21]
    LI32 10, 0xd3d3d3d3
    st.w r10, 268[r21]
    .hword 0x07b5, 0x1a9d, 0x0100
    .hword 0x07b4, 0x18dd, 0x0004
    .hword 0x07b5, 0x1abd, 0x0200
    ld.w 512[r21], r10
    CHECK 10, 0xa0a0a0a0, 2
    ld.w 516[r21], r10
    CHECK 10, 0x22222222, 2
    ld.w 520[r21], r10
    CHECK 10, 0x22222222, 2
    ld.w 524[r21], r10
    CHECK 10, 0xd3d3d3d3, 2
    LI32 10, 0xa0a0a0a0
    st.w r10, 256[r21]
    LI32 10, 0xb1b1b1b1
    st.w r10, 260[r21]
    LI32 10, 0xc2c2c2c2
    st.w r10, 264[r21]
    LI32 10, 0xd3d3d3d3
    st.w r10, 268[r21]
    .hword 0x07b5, 0x1a9d, 0x0100
    .hword 0x07b4, 0x193d, 0x0004
    .hword 0x07b5, 0x1abd, 0x0200
    ld.w 512[r21], r10
    CHECK 10, 0x22222222, 3
    ld.w 516[r21], r10
    CHECK 10, 0xb1b1b1b1, 3
    ld.w 520[r21], r10
    CHECK 10, 0xc2c2c2c2, 3
    ld.w 524[r21], r10
    CHECK 10, 0x22222222, 3
    LI32 10, 0xa0a0a0a0
    st.w r10, 256[r21]
    LI32 10, 0xb1b1b1b1
    st.w r10, 260[r21]
    LI32 10, 0xc2c2c2c2
    st.w r10, 264[r21]
    LI32 10, 0xd3d3d3d3
    st.w r10, 268[r21]
    .hword 0x07b5, 0x1a9d, 0x0100
    .hword 0x07b4, 0x19fd, 0x0004
    .hword 0x07b5, 0x1abd, 0x0200
    ld.w 512[r21], r10
    CHECK 10, 0x22222222, 4
    ld.w 516[r21], r10
    CHECK 10, 0x22222222, 4
    ld.w 520[r21], r10
    CHECK 10, 0x22222222, 4
    ld.w 524[r21], r10
    CHECK 10, 0x22222222, 4
    LI32 10, 0xa0a0a0a0
    st.w r10, 256[r21]
    LI32 10, 0xb1b1b1b1
    st.w r10, 260[r21]
    LI32 10, 0xc2c2c2c2
    st.w r10, 264[r21]
    LI32 10, 0xd3d3d3d3
    st.w r10, 268[r21]
    .hword 0x07b5, 0x1a9d, 0x0100
    .hword 0x07b4, 0x1b3d, 0x0008
    .hword 0x07b5, 0x1abd, 0x0200
    ld.w 512[r21], r10
    CHECK 10, 0x33333333, 5
    ld.w 516[r21], r10
    CHECK 10, 0x44444444, 5
    ld.w 520[r21], r10
    CHECK 10, 0xc2c2c2c2, 5
    ld.w 524[r21], r10
    CHECK 10, 0xd3d3d3d3, 5
    LI32 10, 0xa0a0a0a0
    st.w r10, 256[r21]
    LI32 10, 0xb1b1b1b1
    st.w r10, 260[r21]
    LI32 10, 0xc2c2c2c2
    st.w r10, 264[r21]
    LI32 10, 0xd3d3d3d3
    st.w r10, 268[r21]
    .hword 0x07b5, 0x1a9d, 0x0100
    .hword 0x07b4, 0x1b5d, 0x0008
    .hword 0x07b5, 0x1abd, 0x0200
    ld.w 512[r21], r10
    CHECK 10, 0xa0a0a0a0, 6
    ld.w 516[r21], r10
    CHECK 10, 0xb1b1b1b1, 6
    ld.w 520[r21], r10
    CHECK 10, 0x33333333, 6
    ld.w 524[r21], r10
    CHECK 10, 0x44444444, 6
    LI32 10, 0xa0a0a0a0
    st.w r10, 256[r21]
    LI32 10, 0xb1b1b1b1
    st.w r10, 260[r21]
    LI32 10, 0xc2c2c2c2
    st.w r10, 264[r21]
    LI32 10, 0xd3d3d3d3
    st.w r10, 268[r21]
    .hword 0x07b5, 0x1a9d, 0x0100
    .hword 0x07b4, 0x1b7d, 0x0008
    .hword 0x07b5, 0x1abd, 0x0200
    ld.w 512[r21], r10
    CHECK 10, 0x33333333, 7
    ld.w 516[r21], r10
    CHECK 10, 0x44444444, 7
    ld.w 520[r21], r10
    CHECK 10, 0x33333333, 7
    ld.w 524[r21], r10
    CHECK 10, 0x44444444, 7
    LI32 10, 0xa0a0a0a0
    st.w r10, 256[r21]
    LI32 10, 0xb1b1b1b1
    st.w r10, 260[r21]
    LI32 10, 0xc2c2c2c2
    st.w r10, 264[r21]
    LI32 10, 0xd3d3d3d3
    st.w r10, 268[r21]
    .hword 0x07b5, 0x1a9d, 0x0100
    .hword 0x07b4, 0x1a9d, 0x0010
    .hword 0x07b5, 0x1abd, 0x0200
    ld.w 512[r21], r10
    CHECK 10, 0x55555555, 8
    ld.w 516[r21], r10
    CHECK 10, 0x66666666, 8
    ld.w 520[r21], r10
    CHECK 10, 0x77777777, 8
    ld.w 524[r21], r10
    CHECK 10, 0x88888888, 8
    LI32 10, 0xa0a0a0a0
    st.w r10, 256[r21]
    LI32 10, 0xb1b1b1b1
    st.w r10, 260[r21]
    LI32 10, 0xc2c2c2c2
    st.w r10, 264[r21]
    LI32 10, 0xd3d3d3d3
    st.w r10, 268[r21]
    .hword 0x07b5, 0x1a9d, 0x0100
    .hword 0x07b4, 0x1bdd, 0x0008
    .hword 0x07b5, 0x1abd, 0x0200
    ld.w 512[r21], r10
    CHECK 10, 0x00003333, 9
    ld.w 516[r21], r10
    CHECK 10, 0x00003333, 9
    ld.w 520[r21], r10
    CHECK 10, 0x00004444, 9
    ld.w 524[r21], r10
    CHECK 10, 0x00004444, 9
    .hword 0x07b4, 0x1a9d, 0x0000
    LI32 10, 0xeeeeeeee
    st.w r10, 768[r21]
    LI32 10, 0xeeeeeeee
    st.w r10, 772[r21]
    LI32 10, 0xeeeeeeee
    st.w r10, 776[r21]
    LI32 10, 0xeeeeeeee
    st.w r10, 780[r21]
    .hword 0x07b5, 0x1a1d, 0x030c
    .hword 0x07b5, 0x1a3d, 0x0308
    .hword 0x07b5, 0x1a5d, 0x0304
    .hword 0x07b5, 0x1a7d, 0x0300
    ld.w 768[r21], r10
    CHECK 10, 0x44444444, 10
    ld.w 772[r21], r10
    CHECK 10, 0x33333333, 10
    ld.w 776[r21], r10
    CHECK 10, 0x22222222, 10
    ld.w 780[r21], r10
    CHECK 10, 0x11111111, 10
    .hword 0x07b5, 0x1b9d, 0x0318
    .hword 0x07b5, 0x1bbd, 0x0310
    ld.w 784[r21], r10
    CHECK 10, 0x33333333, 11
    ld.w 788[r21], r10
    CHECK 10, 0x44444444, 11
    ld.w 792[r21], r10
    CHECK 10, 0x11111111, 11
    ld.w 796[r21], r10
    CHECK 10, 0x22222222, 11
    .hword 0x07b5, 0x1bfd, 0x0320
    ld.w 800[r21], r10
    CHECK 10, 0x22221111, 12
    ld.w 804[r21], r10
    CHECK 10, 0x44443333, 12
    movea 4, r20, r22
    .hword 0x07b6, 0x237d, 0x0000
    .hword 0x07b5, 0x22bd, 0x0330
    ld.w 816[r21], r10
    CHECK 10, 0x22222222, 13
    ld.w 820[r21], r10
    CHECK 10, 0x33333333, 13
    ld.w 824[r21], r10
    CHECK 10, 0x22222222, 13
    ld.w 828[r21], r10
    CHECK 10, 0x33333333, 13
    movea 4, r20, r22
    SETPSW 0x00030020
    RESUME a14
e14: .hword 0x07b6, 0x329d, 0x0000
a14: CHECKFE 0xc0, 0xc0, e14, 0x00030020, 0x000300e0, 14
    CHECKR REC_MEI, 0x6006083a, 14
    stsr 6, r10, 2
    movea 4, r20, r11
    cmp r11, r10
    be 8f
    FAIL 14
8:
    movea 8, r20, r22
    SETPSW 0x00030020
    RESUME a15
e15: .hword 0x07b6, 0x329d, 0x0000
a15: CHECKFE 0xc0, 0xc0, e15, 0x00030020, 0x000300e0, 15
    CHECKR REC_MEI, 0x6006083a, 15
    stsr 6, r10, 2
    movea 8, r20, r11
    cmp r11, r10
    be 8f
    FAIL 15
8:
    movea 1032, r21, r22
    SETPSW 0x00030020
    RESUME a16
e16: .hword 0x07b6, 0x32bd, 0x0000
a16: CHECKFE 0xc0, 0xc0, e16, 0x00030020, 0x000300e0, 16
    CHECKR REC_MEI, 0x6006083b, 16
    stsr 6, r10, 2
    movea 1032, r21, r11
    cmp r11, r10
    be 8f
    FAIL 16
8:
    movea 2, r20, r22
    SETPSW 0x00030020
    RESUME a17
e17: .hword 0x07b6, 0x337d, 0x0000
a17: CHECKFE 0xc0, 0xc0, e17, 0x00030020, 0x000300e0, 17
    CHECKR REC_MEI, 0x6006063a, 17
    stsr 6, r10, 2
    movea 2, r20, r11
    cmp r11, r10
    be 8f
    FAIL 17
8:
    movea 1026, r21, r22
    SETPSW 0x00030020
    RESUME a18
e18: .hword 0x07b6, 0x33fd, 0x0000
a18: CHECKFE 0xc0, 0xc0, e18, 0x00030020, 0x000300e0, 18
    CHECKR REC_MEI, 0x6006063f, 18
    stsr 6, r10, 2
    movea 1026, r21, r11
    cmp r11, r10
    be 8f
    FAIL 18
8:
    SETPSW 0x00010020
    RESUME u20
e20: .hword 0x17e1, 0x1da4
u20: CHECKFE 0x80, 0x81, e20, 0x00010020, 0x000100e0, 20
    SETPSW 0x00010020
    RESUME u21
e21: .hword 0x07a0, 0x1a9d, 0x0000
u21: CHECKFE 0x80, 0x81, e21, 0x00010020, 0x000100e0, 21
    SETPSW 0x00010020
    RESUME u22
e22: .hword 0x17e8, 0x1da0
u22: CHECKFE 0x80, 0x81, e22, 0x00010020, 0x000100e0, 22
    SETPSW 0x00010020
    RESUME u23
e23: .hword 0x07a1, 0x1e1d, 0x1004
u23: CHECKFE 0x80, 0x81, e23, 0x00010020, 0x000100e0, 23
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
    .balign 16
m1:
    .word 0x11111111, 0x22222222, 0x33333333, 0x44444444
    .word 0x55555555, 0x66666666, 0x77777777, 0x88888888
