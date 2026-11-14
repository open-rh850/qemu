# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright (c) 2026 Linumiz

    .include "macros.inc"
    .equ INTSEL, 0xff090800
    .equ RL36, 0xffc7c600
    .equ RL38, 0xffc7c800
    .text
    VECTORS 0
    RECCLR
    LI32 11, INTSEL
    ld.w 0[r11], r12
    CHECK 12, 0, 1
    LI32 12, 0xffffffff
    st.w r12, 20[r11]
    ld.w 20[r11], r12
    CHECK 12, 0x001f7fff, 2
    st.w r0, 20[r11]
    movea 3 << 5, r0, r12
    st.w r12, 0[r11]
    EICW 329, 0x0002
    LI32 10, RL38
    RLUART 1
    di
    movea 0x41, r0, r11
    st.b r11, RL_LUTDR[r10]
    SETPSW 0x0
n1: CHECKR REC_CNT, 1, 3
    CHECKEI 0x120, 0x1149, n1, 0, 0x20, 4
    LI32 11, INTSEL
    st.b r0, 0[r11]
    st.b r0, 1[r11]
    di
    movea 0x42, r0, r11
    st.b r11, RL_LUTDR[r10]
    SETPSW 0x0
    nop
    CHECKR REC_CNT, 1, 10
    EICR 12, 329
    CHECK 12, 0x0002, 11
    LI32 11, INTSEL
    mov 1, r12
    st.b r12, 0[r11]
    ld.w 0[r11], r12
    CHECK 12, 1, 12
    EICW 321, 0x0002
    EICW 993, 0x0003
    LI32 10, RL36
    RLUART 1
    di
    movea 0x43, r0, r11
    st.b r11, RL_LUTDR[r10]
    SETPSW 0x0
    nop
    nop
    CHECKR REC_CNT, 3, 20
    CHECKLOG 1, 0x120, 0x1141, 21
    CHECKLOG 2, 0x130, 0x13e1, 23
    PASS
