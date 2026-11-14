# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright (c) 2026 Linumiz

    .include "macros.inc"
    .text
    VECTORS 0
    RECCLR
    SETPSW 0x2b
    RESUME d1
i1: .hword 0x07e0 | 5, 0x0110
d1: CHECKR REC_CNT, 1, 30
    CHECKFE 0x60, 0x60, i1, 0x2b, 0xeb, 31
    SETPSW 0x2b
    RESUME d2
i2: .hword 0x07e0 | 1, 0x9960
d2: CHECKR REC_CNT, 2, 40
    CHECKFE 0x60, 0x60, i2, 0x2b, 0xeb, 41
    SETPSW 0x2b
    RESUME d3
i3: .hword 0x07e0 | 1, 0x9160
d3: CHECKR REC_CNT, 3, 50
    CHECKFE 0x60, 0x60, i3, 0x2b, 0xeb, 51
    PASS
