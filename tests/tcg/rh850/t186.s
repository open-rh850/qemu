# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright (c) 2026 Linumiz

    .include "macros.inc"
    .text
    VECTORS 0
    LA 10, ctab
    ldsr r10, 20, 0
    SETPSW 0x3f
    callt 0
c0: stsr 5, r11, 0
    CHECK 11, 0x3f, 1
    CHECK 12, 0x100, 2
    LA 28, c0
    cmp r28, r13
    be 1f
    FAIL 3
1:  CHECK 14, 0x1f, 4
    SETPSW 0x2a
    callt 1
    stsr 5, r11, 0
    CHECK 11, 0x0a, 5
    CHECK 12, 0x101, 6
    SETPSW 0x20
    callt 5
c5: CHECK 12, 0x105, 7
    stsr 16, r11, 0
    LA 28, c5 + 1
    cmp r28, r11
    be 1f
    FAIL 8
1:  SETPSW 0x3f
    callt 63
    stsr 5, r11, 0
    CHECK 11, 0x20, 9
    CHECK 12, 0x13f, 10
    PASS

    .balign 2
ctab:
    .hword f0 - ctab, f1 - ctab
    .rept 3
    .hword bad - ctab
    .endr
    .hword f5 - ctab
    .rept 57
    .hword bad - ctab
    .endr
    .hword f63 - ctab

f0: mov 0x100, r12
    stsr 16, r13, 0
    stsr 17, r14, 0
    cmp r0, r12
    ctret
f1: mov 0x101, r12
    ei
    ctret
f5: mov 0x105, r12
    stsr 16, r11, 0
    add 1, r11
    ldsr r11, 16, 0
    ctret
f63: mov 0x13f, r12
    ldsr r0, 17, 0
    ctret
bad: FAIL 20
