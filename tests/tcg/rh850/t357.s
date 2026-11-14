# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright (c) 2026 Linumiz

    .include "macros.inc"
    .text
    VECTORS 0
    RECCLR
    SETPSW 0x00030020
    LA 20, d1
    jarl fxset, lp
i1: .hword 0x17e1, 0x1d80
r1: movea 1, r0, r12
    jarl fxchk, lp
    LA 20, d2
    jarl fxset, lp
i2: .hword 0x17e1, 0x1d82
r2: movea 2, r0, r12
    jarl fxchk, lp
    LA 20, d3
    jarl fxset, lp
i3: .hword 0x17e1, 0x1d84
r3: movea 3, r0, r12
    jarl fxchk, lp
    LA 20, d4
    jarl fxset, lp
i4: .hword 0x17e1, 0x1d86
r4: movea 4, r0, r12
    jarl fxchk, lp
    LA 20, d5
    jarl fxset, lp
i5: .hword 0x17e1, 0x1d88
r5: movea 5, r0, r12
    jarl fxchk, lp
    LA 20, d6
    jarl fxset, lp
i6: .hword 0x17e1, 0x1d8a
r6: movea 6, r0, r12
    jarl fxchk, lp
    LA 20, d7
    jarl fxset, lp
i7: .hword 0x17e1, 0x1d8c
r7: movea 7, r0, r12
    jarl fxchk, lp
    LA 20, d8
    jarl fxset, lp
i8: .hword 0x17e1, 0x1d8e
r8: movea 8, r0, r12
    jarl fxchk, lp
    LA 20, d9
    jarl fxset, lp
i9: .hword 0x17e1, 0x1d90
r9: movea 9, r0, r12
    jarl fxchk, lp
    LA 20, d10
    jarl fxset, lp
i10: .hword 0x17e1, 0x1d92
r10: movea 10, r0, r12
    jarl fxchk, lp
    LA 20, d11
    jarl fxset, lp
i11: .hword 0x17e1, 0x1d94
r11: movea 11, r0, r12
    jarl fxchk, lp
    LA 20, d12
    jarl fxset, lp
i12: .hword 0x17e1, 0x1d96
r12: movea 12, r0, r12
    jarl fxchk, lp
    LA 20, d13
    jarl fxset, lp
i13: .hword 0x17e1, 0x1d98
r13: movea 13, r0, r12
    jarl fxchk, lp
    LA 20, d14
    jarl fxset, lp
i14: .hword 0x17e1, 0x1d9a
r14: movea 14, r0, r12
    jarl fxchk, lp
    LA 20, d15
    jarl fxset, lp
i15: .hword 0x17e1, 0x1d9c
r15: movea 15, r0, r12
    jarl fxchk, lp
    LA 20, d16
    jarl fxset, lp
i16: .hword 0x17e1, 0x1d9e
r16: movea 16, r0, r12
    jarl fxchk, lp
    LA 20, d17
    jarl fxset, lp
i17: .hword 0x17e1, 0x1d84
r17: movea 17, r0, r12
    jarl fxchk, lp
    LA 20, d18
    jarl fxset, lp
    RESUME r18
i18: .hword 0x17e1, 0x1d84
r18: movea 18, r0, r12
    jarl fxchk, lp
    LA 20, d19
    jarl fxset, lp
i19: .hword 0x17f0, 0x1da0
r19: movea 19, r0, r12
    jarl fxchk, lp
    LA 20, d20
    jarl fxset, lp
i20: .hword 0x17f1, 0x1da0
r20: movea 20, r0, r12
    jarl fxchk, lp
    LA 20, d21
    jarl fxset, lp
i21: .hword 0x17fe, 0x1da0
r21: movea 21, r0, r12
    jarl fxchk, lp
    LA 20, d22
    jarl fxset, lp
i22: .hword 0x17f8, 0x1da0
r22: movea 22, r0, r12
    jarl fxchk, lp
    LA 20, d23
    jarl fxset, lp
i23: .hword 0x17f9, 0x1da0
r23: movea 23, r0, r12
    jarl fxchk, lp
    LA 20, d24
    jarl fxset, lp
i24: .hword 0x17fa, 0x1da0
r24: movea 24, r0, r12
    jarl fxchk, lp
    LA 20, d25
    jarl fxset, lp
i25: .hword 0x17fb, 0x1da0
r25: movea 25, r0, r12
    jarl fxchk, lp
    LA 20, d26
    jarl fxset, lp
i26: .hword 0x07a1, 0x1e9d, 0x1000
r26: movea 26, r0, r12
    jarl fxchk, lp
    LA 20, d27
    jarl fxset, lp
i27: .hword 0x07a1, 0x1ebd, 0x17ff
r27: movea 27, r0, r12
    jarl fxchk, lp
    LA 20, d28
    jarl fxset, lp
i28: .hword 0x07a1, 0x1ebd, 0x1253
r28: movea 28, r0, r12
    jarl fxchk, lp
    LA 20, d29
    jarl fxset, lp
i29: .hword 0x07a1, 0x1e9d, 0x12c5
r29: movea 29, r0, r12
    jarl fxchk, lp
    LA 20, d30
    jarl fxset, lp
i30: .hword 0x07a1, 0x1e9d, 0x16ba
r30: movea 30, r0, r12
    jarl fxchk, lp
    LA 20, d31
    jarl fxset, lp
i31: .hword 0x07a1, 0x1e1d, 0x1003
r31: movea 31, r0, r12
    jarl fxchk, lp
    LA 20, d32
    .hword 0x07b4, 0x129d, 0x0000
    .hword 0x17ff, 0x05a0
    stsr 5, r10, 0
    andi 1, r10, r10
    CHECK 10, 0x00000000, 32
    .hword 0x17ff, 0x0da0
    stsr 5, r10, 0
    andi 1, r10, r10
    CHECK 10, 0x00000001, 32
    .hword 0x17ff, 0x15a0
    stsr 5, r10, 0
    andi 1, r10, r10
    CHECK 10, 0x00000000, 32
    .hword 0x17ff, 0x1da0
    stsr 5, r10, 0
    andi 1, r10, r10
    CHECK 10, 0x00000000, 32
    .hword 0x17ff, 0x25a0
    stsr 5, r10, 0
    andi 1, r10, r10
    CHECK 10, 0x00000000, 32
    .hword 0x17ff, 0x2da0
    stsr 5, r10, 0
    andi 1, r10, r10
    CHECK 10, 0x00000001, 32
    LA 20, d33
    .hword 0x07b4, 0x129d, 0x0000
    .hword 0x17ff, 0x05a0
    stsr 5, r10, 0
    andi 1, r10, r10
    CHECK 10, 0x00000001, 33
    .hword 0x17ff, 0x0da0
    stsr 5, r10, 0
    andi 1, r10, r10
    CHECK 10, 0x00000000, 33
    .hword 0x17ff, 0x15a0
    stsr 5, r10, 0
    andi 1, r10, r10
    CHECK 10, 0x00000001, 33
    .hword 0x17ff, 0x1da0
    stsr 5, r10, 0
    andi 1, r10, r10
    CHECK 10, 0x00000001, 33
    .hword 0x17ff, 0x25a0
    stsr 5, r10, 0
    andi 1, r10, r10
    CHECK 10, 0x00000000, 33
    .hword 0x17ff, 0x2da0
    stsr 5, r10, 0
    andi 1, r10, r10
    CHECK 10, 0x00000001, 33
    LA 20, d34
    .hword 0x07b4, 0x129d, 0x0000
    .hword 0x17ff, 0x05a0
    stsr 5, r10, 0
    andi 1, r10, r10
    CHECK 10, 0x00000001, 34
    .hword 0x17ff, 0x0da0
    stsr 5, r10, 0
    andi 1, r10, r10
    CHECK 10, 0x00000001, 34
    .hword 0x17ff, 0x15a0
    stsr 5, r10, 0
    andi 1, r10, r10
    CHECK 10, 0x00000001, 34
    .hword 0x17ff, 0x1da0
    stsr 5, r10, 0
    andi 1, r10, r10
    CHECK 10, 0x00000001, 34
    .hword 0x17ff, 0x25a0
    stsr 5, r10, 0
    andi 1, r10, r10
    CHECK 10, 0x00000001, 34
    .hword 0x17ff, 0x2da0
    stsr 5, r10, 0
    andi 1, r10, r10
    CHECK 10, 0x00000001, 34
    LA 20, d35
    .hword 0x07b4, 0x129d, 0x0000
    .hword 0x17ff, 0x05a0
    stsr 5, r10, 0
    andi 1, r10, r10
    CHECK 10, 0x00000000, 35
    .hword 0x17ff, 0x0da0
    stsr 5, r10, 0
    andi 1, r10, r10
    CHECK 10, 0x00000000, 35
    .hword 0x17ff, 0x15a0
    stsr 5, r10, 0
    andi 1, r10, r10
    CHECK 10, 0x00000000, 35
    .hword 0x17ff, 0x1da0
    stsr 5, r10, 0
    andi 1, r10, r10
    CHECK 10, 0x00000000, 35
    .hword 0x17ff, 0x25a0
    stsr 5, r10, 0
    andi 1, r10, r10
    CHECK 10, 0x00000000, 35
    .hword 0x17ff, 0x2da0
    stsr 5, r10, 0
    andi 1, r10, r10
    CHECK 10, 0x00000000, 35
    PASS
fxset:
    ld.w 88[r20], r10
    ldsr r10, 6, 10
    ldsr r0, 12, 10
    ldsr r0, 13, 10
    .hword 0x07b4, 0x0a9d, 0x0000
    .hword 0x07b4, 0x129d, 0x0010
    .hword 0x07b4, 0x1a9d, 0x0020
    jmp [lp]
fxchk:
    LI32 21, 0xfe010000
    .hword 0x07b5, 0x1abd, 0x0000
    ld.w 0[r21], r10
    ld.w 48[r20], r11
    cmp r10, r11
    bne 9f
    ld.w 4[r21], r10
    ld.w 52[r20], r11
    cmp r10, r11
    bne 9f
    ld.w 8[r21], r10
    ld.w 56[r20], r11
    cmp r10, r11
    bne 9f
    ld.w 12[r21], r10
    ld.w 60[r20], r11
    cmp r10, r11
    bne 9f
    stsr 6, r10, 10
    ld.w 64[r20], r11
    cmp r10, r11
    bne 9f
    stsr 13, r10, 10
    ld.w 68[r20], r11
    cmp r10, r11
    bne 9f
    stsr 12, r10, 10
    ld.w 72[r20], r11
    cmp r10, r11
    bne 9f
    LI32 17, REC
    ld.w REC_CNT[r17], r10
    ld.w 76[r20], r11
    cmp r10, r11
    bne 9f
    ld.w 80[r20], r11
    cmp r0, r11
    be 1f
    ld.w REC_EIIC[r17], r10
    movea 0x75, r0, r11
    cmp r10, r11
    bne 9f
    ld.w REC_OFF[r17], r10
    movea 0x70, r0, r11
    cmp r10, r11
    bne 9f
    ld.w REC_EIPC[r17], r10
    ld.w 84[r20], r11
    cmp r10, r11
    bne 9f
1:  jmp [lp]
9:  shl 16, r12
    movea 0x3333, r12, r29
    movhi 0x4000, r0, r30
    st.w r29, 0[r30]
8:  br 8b
    .balign 16
d1:
    .word 0x40000000, 0x40000000, 0x40000000, 0x3f800000
    .word 0x3f800000, 0x40000000, 0x40400000, 0x7fc00000
    .word 0xc11c6610, 0x3fa44f15, 0xc1076e2d, 0xbe7d2d4e
    .word 0x00000000, 0x00000000, 0x00000000, 0x00000000
    .word 0x00220000, 0x00000000, 0x00000000, 0x00000000
    .word 0x00000000, i1, 0x00220000
    .balign 16
d2:
    .word 0x40000000, 0x40000000, 0x40000000, 0x3f800000
    .word 0x3f800000, 0x40000000, 0x40400000, 0x7fc00000
    .word 0xc0460fb1, 0xbff5c0c3, 0x3fd49d16, 0xc05edd97
    .word 0x00000000, 0x00000000, 0x00000000, 0xffffffff
    .word 0x00220000, 0x00000000, 0x00000000, 0x00000000
    .word 0x00000000, i2, 0x00220000
    .balign 16
d3:
    .word 0x40000000, 0x40000000, 0x40000000, 0x3f800000
    .word 0x3f800000, 0x40000000, 0x40400000, 0x7fc00000
    .word 0x3e046f12, 0x3eada0cb, 0xbd8fe21c, 0xbeaced0e
    .word 0x00000000, 0xffffffff, 0x00000000, 0x00000000
    .word 0x00220000, 0x00000000, 0x00000000, 0x00000000
    .word 0x00000000, i3, 0x00220000
    .balign 16
d4:
    .word 0x40000000, 0x40000000, 0x40000000, 0x3f800000
    .word 0x3f800000, 0x40000000, 0x40400000, 0x7fc00000
    .word 0x3fc50034, 0x407bbc9a, 0xc04974ce, 0x3dd1b000
    .word 0x00000000, 0xffffffff, 0x00000000, 0xffffffff
    .word 0x00220000, 0x00000000, 0x00000000, 0x00000000
    .word 0x00000000, i4, 0x00220000
    .balign 16
d5:
    .word 0x40000000, 0x40000000, 0x40000000, 0x3f800000
    .word 0x3f800000, 0x40000000, 0x40400000, 0x7fc00000
    .word 0xbede9959, 0xc0fda406, 0x3de44b04, 0xc004665f
    .word 0xffffffff, 0x00000000, 0x00000000, 0x00000000
    .word 0x00220000, 0x00000000, 0x00000000, 0x00000000
    .word 0x00000000, i5, 0x00220000
    .balign 16
d6:
    .word 0x40000000, 0x40000000, 0x40000000, 0x3f800000
    .word 0x3f800000, 0x40000000, 0x40400000, 0x7fc00000
    .word 0xbf243665, 0xc163b71f, 0x3e07dbb2, 0x40ca16e0
    .word 0xffffffff, 0x00000000, 0x00000000, 0xffffffff
    .word 0x00220000, 0x00000000, 0x00000000, 0x00000000
    .word 0x00000000, i6, 0x00220000
    .balign 16
d7:
    .word 0x40000000, 0x40000000, 0x40000000, 0x3f800000
    .word 0x3f800000, 0x40000000, 0x40400000, 0x7fc00000
    .word 0x3ee99229, 0xc11aa977, 0xbeaa2167, 0x3dc18961
    .word 0xffffffff, 0xffffffff, 0x00000000, 0x00000000
    .word 0x00220000, 0x00000000, 0x00000000, 0x00000000
    .word 0x00000000, i7, 0x00220000
    .balign 16
d8:
    .word 0x40000000, 0x40000000, 0x40000000, 0x3f800000
    .word 0x3f800000, 0x40000000, 0x40400000, 0x7fc00000
    .word 0xbfc490dd, 0x4075f007, 0x3ebb35b9, 0xc016e8df
    .word 0xffffffff, 0xffffffff, 0x00000000, 0xffffffff
    .word 0x00220000, 0x00000000, 0x00000000, 0x00000000
    .word 0x00000000, i8, 0x00220000
    .balign 16
d9:
    .word 0x40000000, 0x40000000, 0x40000000, 0x3f800000
    .word 0x3f800000, 0x40000000, 0x40400000, 0x7fc00000
    .word 0x411ab4f4, 0x41997427, 0xc1690fc3, 0xc11364f2
    .word 0x00000000, 0x00000000, 0x00000000, 0x00000000
    .word 0x00220010, 0x10000000, 0x00000000, 0x00000000
    .word 0x00000000, i9, 0x00220000
    .balign 16
d10:
    .word 0x40000000, 0x40000000, 0x40000000, 0x3f800000
    .word 0x3f800000, 0x40000000, 0x40400000, 0x7fc00000
    .word 0xc02912d3, 0xc0cc08ec, 0xc0842453, 0xbf24606d
    .word 0x00000000, 0x00000000, 0x00000000, 0xffffffff
    .word 0x00220010, 0x10000000, 0x00000000, 0x00000000
    .word 0x00000000, i10, 0x00220000
    .balign 16
d11:
    .word 0x40000000, 0x40000000, 0x40000000, 0x3f800000
    .word 0x3f800000, 0x40000000, 0x40400000, 0x7fc00000
    .word 0x3f5f266a, 0xbe9c1ecf, 0xbfc8a922, 0x3ea68cf1
    .word 0x00000000, 0xffffffff, 0x00000000, 0x00000000
    .word 0x00220010, 0x10000000, 0x00000000, 0x00000000
    .word 0x00000000, i11, 0x00220000
    .balign 16
d12:
    .word 0x40000000, 0x40000000, 0x40000000, 0x3f800000
    .word 0x3f800000, 0x40000000, 0x40400000, 0x7fc00000
    .word 0xc0697247, 0xbd8bb210, 0x40064ef4, 0x405384d8
    .word 0x00000000, 0xffffffff, 0x00000000, 0xffffffff
    .word 0x00220010, 0x10000000, 0x00000000, 0x00000000
    .word 0x00000000, i12, 0x00220000
    .balign 16
d13:
    .word 0x40000000, 0x40000000, 0x40000000, 0x3f800000
    .word 0x3f800000, 0x40000000, 0x40400000, 0x7fc00000
    .word 0x40fda5c8, 0x40126a89, 0x3fade83f, 0x3d88b67f
    .word 0xffffffff, 0x00000000, 0x00000000, 0x00000000
    .word 0x00220010, 0x10000000, 0x00000000, 0x00000000
    .word 0x00000000, i13, 0x00220000
    .balign 16
d14:
    .word 0x40000000, 0x40000000, 0x40000000, 0x3f800000
    .word 0x3f800000, 0x40000000, 0x40400000, 0x7fc00000
    .word 0x3e76346e, 0xbd9d4cf4, 0xbfbfefd8, 0x41ef3e56
    .word 0xffffffff, 0x00000000, 0x00000000, 0xffffffff
    .word 0x00220010, 0x10000000, 0x00000000, 0x00000000
    .word 0x00000000, i14, 0x00220000
    .balign 16
d15:
    .word 0x40000000, 0x40000000, 0x40000000, 0x3f800000
    .word 0x3f800000, 0x40000000, 0x40400000, 0x7fc00000
    .word 0xc0b8b8d3, 0xc00070c1, 0xc17c4a02, 0x40571bce
    .word 0xffffffff, 0xffffffff, 0x00000000, 0x00000000
    .word 0x00220010, 0x10000000, 0x00000000, 0x00000000
    .word 0x00000000, i15, 0x00220000
    .balign 16
d16:
    .word 0x40000000, 0x40000000, 0x40000000, 0x3f800000
    .word 0x3f800000, 0x40000000, 0x40400000, 0x7fc00000
    .word 0xbf4d8a85, 0xc0b48879, 0xbfd25608, 0xc1d8fefc
    .word 0xffffffff, 0xffffffff, 0x00000000, 0xffffffff
    .word 0x00220010, 0x10000000, 0x00000000, 0x00000000
    .word 0x00000000, i16, 0x00220000
    .balign 16
d17:
    .word 0x7f800001, 0x00000000, 0x3f800000, 0xffc00000
    .word 0x00000000, 0xff800000, 0x7fc00000, 0x80000000
    .word 0x410a942b, 0x41e7dedc, 0x4130592f, 0x3fffc45f
    .word 0x00000000, 0x00000000, 0x00000000, 0x00000000
    .word 0x00220010, 0x00000010, 0x00000000, 0x00000000
    .word 0x00000000, i17, 0x00220000
    .balign 16
d18:
    .word 0x7f800001, 0x00000000, 0x3f800000, 0xffc00000
    .word 0x00000000, 0xff800000, 0x7fc00000, 0x80000000
    .word 0x3f6f3db4, 0xc06806e0, 0xbd8cd7c3, 0xc1755d3b
    .word 0x3f6f3db4, 0xc06806e0, 0xbd8cd7c3, 0xc1755d3b
    .word 0x00224200, 0x00000000, 0x00000010, 0x00000001
    .word 0x00000001, i18, 0x00220200
    .balign 16
d19:
    .word 0x7f800000, 0xfffcc638, 0x401da8cc, 0xbf800000
    .word 0x7f800001, 0xff800000, 0xff800002, 0x7fad3549
    .word 0x3dce7c7b, 0x3d8d49c1, 0xbe037f92, 0x3f06a86d
    .word 0x7f800001, 0x7f800000, 0x7f800002, 0x7fad3549
    .word 0x00220000, 0x00000000, 0x00000000, 0x00000001
    .word 0x00000000, i19, 0x00220000
    .balign 16
d20:
    .word 0xff800008, 0x00000000, 0xffec89b6, 0x3f800000
    .word 0x7f800001, 0x7fce3e6d, 0x7fa88466, 0x7fcd57c6
    .word 0xc0bb9177, 0x3fa9e684, 0x4081e947, 0xbe4a7481
    .word 0xff800001, 0xffce3e6d, 0xffa88466, 0xffcd57c6
    .word 0x00220000, 0x00000000, 0x00000000, 0x00000001
    .word 0x00000000, i20, 0x00220000
    .balign 16
d21:
    .word 0x7fc74366, 0xff800001, 0x7fade006, 0xc149da5e
    .word 0x7f800001, 0x7f800000, 0x00000000, 0x3f800000
    .word 0xc02bc1eb, 0xc0c5dd9c, 0xbe40d696, 0xc1cd7e92
    .word 0x7f800001, 0x7f800000, 0x00000000, 0x3f800000
    .word 0x00220000, 0x00000000, 0x00000000, 0x00000001
    .word 0x00000000, i21, 0x00220000
    .balign 16
d22:
    .word 0xbedee6e3, 0xc100e616, 0x40bd9e72, 0x40da05bc
    .word 0x3fc87b16, 0xc048dec4, 0x40712b41, 0x3dc96377
    .word 0xbfa8d7dc, 0x3f1283f7, 0x3e24b9e6, 0x3f228478
    .word 0xbfc87b16, 0xc048dec4, 0xc0712b41, 0x3dc96377
    .word 0x00220000, 0x00000000, 0x00000000, 0x00000001
    .word 0x00000000, i22, 0x00220000
    .balign 16
d23:
    .word 0xc1f7da58, 0x3f354c0f, 0x3d9466a5, 0xc124e1d9
    .word 0x40b562db, 0xc1b50bbc, 0x3fd4a93c, 0x3ea5bda7
    .word 0xbfc1db32, 0x4123cd42, 0xc0589ba9, 0xc0018366
    .word 0x41b50bbc, 0x40b562db, 0xbea5bda7, 0x3fd4a93c
    .word 0x00220000, 0x00000000, 0x00000000, 0x00000001
    .word 0x00000000, i23, 0x00220000
    .balign 16
d24:
    .word 0x3dc4737b, 0xc0f48b0f, 0xbfb870a7, 0x41038895
    .word 0xbf794ffd, 0xbf71b35d, 0x3e5a36f2, 0x3fd3cfdc
    .word 0xc0b6d1ac, 0x40a98d4e, 0xbde47eb1, 0x3e65f626
    .word 0xbf794ffd, 0x3f71b35d, 0x3e5a36f2, 0xbfd3cfdc
    .word 0x00220000, 0x00000000, 0x00000000, 0x00000001
    .word 0x00000000, i24, 0x00220000
    .balign 16
d25:
    .word 0xbecfc79a, 0xbeda00bc, 0x403435a3, 0xc04c2ecf
    .word 0xc0185e02, 0x406545bf, 0x3ec8ab3d, 0x40690fef
    .word 0x3d933a44, 0xbf71b9ab, 0x3f59e878, 0x3f243024
    .word 0x406545bf, 0x40185e02, 0x40690fef, 0xbec8ab3d
    .word 0x00220000, 0x00000000, 0x00000000, 0x00000001
    .word 0x00000000, i25, 0x00220000
    .balign 16
d26:
    .word 0xbe26430b, 0xbf5a0be9, 0x40813887, 0xbe9d8c6f
    .word 0xbd827c51, 0x40149bd9, 0x3fa0a34d, 0xc15f3a31
    .word 0xbebee317, 0x3f915750, 0x3fe99b1a, 0x3da90fc3
    .word 0xbe26430b, 0xbe26430b, 0xbe26430b, 0xbe26430b
    .word 0x00220000, 0x00000000, 0x00000000, 0x00000001
    .word 0x00000000, i26, 0x00220000
    .balign 16
d27:
    .word 0xbe787f2c, 0xbe3443fd, 0x3ec50ff5, 0xbe868251
    .word 0x3f5434f7, 0xc125b423, 0xbf77b312, 0xbde535b8
    .word 0x3e119092, 0x40c3cca9, 0x40b9f1fd, 0x3db063a3
    .word 0xbde535b8, 0xbde535b8, 0xbde535b8, 0xbde535b8
    .word 0x00220000, 0x00000000, 0x00000000, 0x00000001
    .word 0x00000000, i27, 0x00220000
    .balign 16
d28:
    .word 0x3e4bab8c, 0x418de24c, 0x4030f3e9, 0xbeda5159
    .word 0x3f2a7525, 0x40220231, 0x3e6a0ff2, 0x41c81c00
    .word 0x3e1a4357, 0x3f78f852, 0x3f718827, 0x4118c170
    .word 0xbeda5159, 0x4030f3e9, 0x418de24c, 0x40220231
    .word 0x00220000, 0x00000000, 0x00000000, 0x00000001
    .word 0x00000000, i28, 0x00220000
    .balign 16
d29:
    .word 0x3da8ef3d, 0xc1a1e58f, 0x3dbe68c0, 0x3f1c4f79
    .word 0x4121b56f, 0xbfec58b0, 0xc135d963, 0xc18362c4
    .word 0x404ac73f, 0xbe6db057, 0x401954f6, 0x3e903093
    .word 0xbfec58b0, 0x3da8ef3d, 0x3f1c4f79, 0xc1a1e58f
    .word 0x00220000, 0x00000000, 0x00000000, 0x00000001
    .word 0x00000000, i29, 0x00220000
    .balign 16
d30:
    .word 0x414b65c0, 0x3e746c4d, 0x3feab10c, 0xbdd87387
    .word 0xc1fb5cdf, 0xbf1c60ee, 0x3dcf7683, 0xc020693e
    .word 0x409a8edc, 0x3da2f0b0, 0x3f85ca5c, 0xc1d57f38
    .word 0x3feab10c, 0xc020693e, 0x3feab10c, 0xbdd87387
    .word 0x00220000, 0x00000000, 0x00000000, 0x00000001
    .word 0x00000000, i30, 0x00220000
    .balign 16
d31:
    .word 0xbfca5c81, 0xbd9a7566, 0xc0bf2ad4, 0x417de3d5
    .word 0xbf497019, 0xbec468bf, 0xbf004285, 0x3e08b4c2
    .word 0x00000000, 0x80000000, 0x00000001, 0x00000000
    .word 0xbf497019, 0xbd9a7566, 0xc0bf2ad4, 0x3e08b4c2
    .word 0x00220000, 0x00000000, 0x00000000, 0x00000001
    .word 0x00000000, i31, 0x00220000
    .balign 16
d32:
    .word 0x00000000, 0x00000001, 0x00000000, 0x00000000
    .balign 16
d33:
    .word 0x00000005, 0x00000000, 0x80000000, 0x00000007
    .balign 16
d34:
    .word 0x00000001, 0x00000001, 0x00000001, 0x00000001
    .balign 16
d35:
    .word 0x00000000, 0x00000000, 0x00000000, 0x00000000
