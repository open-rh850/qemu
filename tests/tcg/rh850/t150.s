# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright (c) 2026 Linumiz

    .include "macros.inc"
    .text
    LI32 11, 0xfe000100
    LI32 1, 0x11111111
    st.w r1, 0[r11]
    ldl.w [r11], r12
    CHECK 12, 0x11111111, 1
    LI32 13, 0x22222222
    stc.w r13, [r11]
    CHECK 13, 1, 1
    CHECKM 0xfe000100, 0x22222222, 1
    LI32 13, 0x33333333
    stc.w r13, [r11]
    CHECK 13, 0, 2
    CHECKM 0xfe000100, 0x22222222, 2
    ldl.w [r11], r12
    CLL
    LI32 13, 0x44444444
    stc.w r13, [r11]
    CHECK 13, 0, 3
    CHECKM 0xfe000100, 0x22222222, 3
    LI32 1, 0x55555555
    st.w r1, 4[r11]
    ldl.w [r11], r12
    LI32 14, 0xfe000104
    LI32 13, 0x66666666
    stc.w r13, [r14]
    CHECK 13, 0, 4
    CHECKM 0xfe000104, 0x55555555, 4
    LI32 13, 0x66666666
    stc.w r13, [r11]
    CHECK 13, 0, 5
    CHECKM 0xfe000100, 0x22222222, 5
    ldl.w [r11], r12
    LI32 1, 0x77777777
    st.w r1, 0[r11]
    LI32 13, 0x88888888
    stc.w r13, [r11]
    CHECK 13, 0, 6
    CHECKM 0xfe000100, 0x77777777, 6
    ldl.w [r11], r12
    st.b r0, 31[r11]
    LI32 13, 0x99999999
    stc.w r13, [r11]
    CHECK 13, 0, 7
    ldl.w [r11], r12
    st.w r0, 32[r11]
    LI32 13, 0xaaaaaaaa
    stc.w r13, [r11]
    CHECK 13, 1, 8
    CHECKM 0xfe000100, 0xaaaaaaaa, 8
    ldl.w [r11], r12
    set1 0, 8[r11]
    LI32 13, 0xbbbbbbbb
    stc.w r13, [r11]
    CHECK 13, 0, 9
    ldl.w [r11], r12
    LI32 13, 0xcc
    STC_B 13, 11
    CHECK 13, 0, 10
    CHECKM 0xfe000100, 0xaaaaaaaa, 10
    LDL_BU 11, 12
    CHECK 12, 0xaa, 11
    LI32 13, 0x5a
    STC_B 13, 11
    CHECK 13, 1, 11
    CHECKM 0xfe000100, 0xaaaaaa5a, 11
    LDL_HU 11, 12
    CHECK 12, 0xaa5a, 12
    LI32 13, 0x1234
    STC_H 13, 11
    CHECK 13, 1, 12
    CHECKM 0xfe000100, 0xaaaa1234, 12
    LI32 15, 0xfe000200
    st.w r0, 0[r15]
    ldl.w [r11], r12
    ldl.w [r15], r12
    LI32 13, 0xdddddddd
    stc.w r13, [r11]
    CHECK 13, 0, 13
    ldl.w [r11], r12
    ldl.w [r15], r12
    LI32 13, 0xeeeeeeee
    stc.w r13, [r15]
    CHECK 13, 1, 14
    CHECKM 0xfe000200, 0xeeeeeeee, 14
    LI32 16, 0x50000000
    ldl.w [r16], r12
    CHECK 12, 0, 15
    LI32 13, 0x1
    stc.w r13, [r16]
    CHECK 13, 0, 15
    LI32 17, 0xfdc00040
    LI32 1, 0x0f0f0f0f
    st.w r1, 0[r17]
    ldl.w [r17], r12
    CHECK 12, 0x0f0f0f0f, 16
    LI32 13, 0xf0f0f0f0
    stc.w r13, [r17]
    CHECK 13, 1, 16
    CHECKM 0xfdc00040, 0xf0f0f0f0, 16
    LI32 26, 1
    LI32 27, 2
    cmp r27, r26
    ldl.w [r11], r12
    LI32 13, 7
    stc.w r13, [r11]
    setf 0x2, r20
    setf 0x4, r21
    setf 0x1, r23
    CHECK 20, 0, 17
    CHECK 21, 1, 17
    CHECK 23, 1, 17
    PASS
