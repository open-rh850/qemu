# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright (c) 2026 Linumiz

    .include "macros.inc"
    .text
j1: jarl f1, r31
j1n:
    FAIL 1
f1: LA 10, j1n
    cmp r10, r31
    be 1f
    FAIL 2
1:
j2: jr f2
    FAIL 3
    .skip 0x1ffffe - (. - j2), 0
f2:
    jr32 j3s
j3t: jr32 j3e
    .skip 0x200000 - (. - j3t), 0
j3s: jr j3t
    FAIL 4
j3e:
    jr j4s
j4t: LA 10, j4n
    cmp r10, r29
    be j4e
    FAIL 5
j4s: jarl j4t, r29
j4n: FAIL 6
j4e:
    jr32 ram1
    FAIL 7
back1:
    jarl32 ram2, r28
j5n:
    LA 10, j5n
    cmp r10, r28
    be 1f
    FAIL 8
1:  CHECK 27, 0x5a5a, 9
    LA 12, f6
    jarl [r12], r12
j6n:
    FAIL 10
f6: LA 10, j6n
    cmp r10, r12
    be 1f
    FAIL 11
1:
    LA 13, f7 + 1
    jarl [r13], r14
j7n:
    FAIL 12
f7: LA 10, j7n
    cmp r10, r14
    be 1f
    FAIL 13
1:
    LA 15, f8 + 74566 + 1
    jmp -74566[r15]
    FAIL 14
f8:
    LA 16, f9 + 1
    jmp [r16]
    FAIL 15
f9: PASS

    .data
ram1:
    jr32 back1
ram2:
    movea 0x5a5a, r0, r27
    jmp [r28]
