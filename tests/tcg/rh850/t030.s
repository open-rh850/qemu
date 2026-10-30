# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright (c) 2026 Linumiz

    .include "macros.inc"
    .text
    mov 0, r10
    jr 1f
    FAIL 1
1:
    mov 1, r11
2:  movea 1, r10, r10
    cmp 2, r10
    be 3f
    jr 2b
3:  CHECK 10, 2, 2
    jr far
    FAIL 3
back:
    LA 5, 4f
    jmp [r5]
    FAIL 4
4:
    LA 6, 5f + 1
    jmp [r6]
    FAIL 5
5:
    LA 7, 5b + 1
    cmp r7, r6
    be 6f
    FAIL 6
6:
    PASS

    .space 0x30000, 0
far:
    jr back
    FAIL 7
