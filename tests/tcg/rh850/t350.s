# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright (c) 2026 Linumiz

    .include "macros.inc"
    .text
    VECTORS 0
    RECCLR
    stsr 6, r10, 1
    CHECK 10, 0x07840700, 1
    SETPSW 0x00030020
    stsr 5, r10, 0
    CHECK 10, 0x00010020, 2
    SETPSW 0x00030020
    RESUME d1
i1: stsr 6, r10, 10
d1: CHECKFE 0x80, 0x81, i1, 0x00010020, 0x000100e0, 3
    CHECKR REC_CNT, 1, 10
    PASS
