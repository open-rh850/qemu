# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright (c) 2026 Linumiz

    .include "macros.inc"
    .text
    mov 0x12345678, r1
    movhi 0x1234, r0, r2
    movea 0x5678, r2, r2
    cmp r1, r2
    be 1f
    FAIL 1
1:  mov 0x89abcdef, r3
    movhi 0x89ac, r0, r4
    movea -0x3211, r4, r4
    cmp r3, r4
    be 1f
    FAIL 2
1:
    mov -16, r5
    CHECK 5, 0xfffffff0, 3
    mov 15, r6
    CHECK 6, 0x0000000f, 4
    mov -1, r7
    CHECK 7, 0xffffffff, 5
    mov 0, r8
    CHECK 8, 0, 6
    mov r1, r9
    CHECK 9, 0x12345678, 7
    CHECK 1, 0x12345678, 8
    mov r0, r10
    CHECK 10, 0, 9
    LI32 0, 0x5a5a5a5a
    CHECK 0, 0, 10
    PASS
