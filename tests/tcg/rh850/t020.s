# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright (c) 2026 Linumiz

    .include "macros.inc"

    .macro FLAGS z, s, ov, cy, n
    .if \z
    bz 7f
    .else
    bnz 7f
    .endif
    FAIL \n
7:  .if \s
    bn 7f
    .else
    bp 7f
    .endif
    FAIL (\n + 1)
7:  .if \ov
    bv 7f
    .else
    bnv 7f
    .endif
    FAIL (\n + 2)
7:  .if \cy
    bc 7f
    .else
    bnc 7f
    .endif
    FAIL (\n + 3)
7:
    .endm

    .text
    mov 5, r1
    mov 5, r2
    cmp r1, r2
    FLAGS 1, 0, 0, 0, 10
    mov 1, r1
    mov -1, r2
    cmp r1, r2
    FLAGS 0, 1, 0, 0, 20
    mov 2, r1
    mov 1, r2
    cmp r1, r2
    FLAGS 0, 1, 0, 1, 30
    mov 1, r1
    mov 2, r2
    cmp r1, r2
    FLAGS 0, 0, 0, 0, 40
    LI32 2, 0x80000000
    mov 1, r1
    cmp r1, r2
    FLAGS 0, 0, 1, 0, 50
    LI32 2, 0x7fffffff
    mov -1, r1
    cmp r1, r2
    FLAGS 0, 1, 1, 1, 60
    mov -1, r2
    cmp -1, r2
    FLAGS 1, 0, 0, 0, 70
    mov 0, r2
    cmp 15, r2
    FLAGS 0, 1, 0, 1, 80
    LI32 2, 0x80000000
    cmp -16, r2
    FLAGS 0, 1, 0, 1, 90
    CHECK 2, 0x80000000, 100
    cmp r0, r0
    FLAGS 1, 0, 0, 0, 110
    PASS
