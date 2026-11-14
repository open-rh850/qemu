# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright (c) 2026 Linumiz

    .include "macros.inc"
    .equ TBL, 0xfe020000
    .equ RBIP, 0xfe030000
    .text
    VECTORS 0
    RECCLR
    LI32 11, 0x3f
    ldsr r11, 14, 2
    LI32 11, 0x00010002
    ldsr r11, 13, 2
    LI32 11, TBL
    ldsr r11, 4, 1
    LA 12, th
    st.w r12, 400[r11]
    LI32 11, 0x8000
    ldsr r11, 15, 2
    LI32 11, RBIP
    ldsr r11, 18, 2
    ldsr r0, 17, 2
    EEICW 100, 0x00400028
    EEICW 100, 0x10400028
    SETPSW 0x03f00000
n1: CHECKR REC_CNT, 1, 1
    CHECKEI 0x300, 0x1064, n1, 0x03f00000, 0x02800020, 2
    stsr 17, r10, 2
    CHECK 10, 1, 7
    CHECKRL REC_EIPC, n1, 8
    LI32 25, RBIP - 4
    ld.w 0[r25], r24
    LA 28, n1
    cmp r28, r24
    be 1f
    FAIL 9
1:  CHECKM RBIP - 12, 0x1064, 10
    di
    EEICW 100, 0x10400028
    SETPSW 0x03f00000
n2: CHECKR REC_CNT, 2, 11
    stsr 17, r10, 2
    CHECK 10, 2, 12
    di
    RESUME d3
    HWR INTC2 + 0x4000 + 400, 0x00c00028
    EEICW 100, 0x10400028
    SETPSW 0x03f00000
    nop
d3: CHECKR REC_CNT, 3, 20
    CHECKR REC_OFF, 0x10, 21
    CHECKR REC_FEIC, 0x1c, 22
    stsr 17, r10, 2
    CHECK 10, 2, 23
    EEICR 10, 100
    CHECK 10, 0x00c00028, 24
    di
    LI32 11, 0x003f0002
    ldsr r11, 13, 2
    LI32 11, 63
    ldsr r11, 17, 2
    stsr 17, r10, 2
    CHECK 10, 63, 30
    RESUME d4
    HWR INTC2 + 0x4000 + 400, 0x00c00028
    EEICW 100, 0x10400028
    SETPSW 0x03f00000
    nop
d4: CHECKR REC_CNT, 4, 31
    CHECKR REC_FEIC, 0x1c, 32
    LI32 11, 62
    ldsr r11, 17, 2
    di
    EEICW 100, 0x10400028
    SETPSW 0x03f00000
n5: CHECKR REC_CNT, 5, 40
    CHECKR REC_OFF, 0x300, 41
    stsr 17, r10, 2
    CHECK 10, 63, 42
    PASS
th: movea 0x300, r0, r19
    jr exc_record
