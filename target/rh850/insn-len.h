/*
 * SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright (c) 2026 Linumiz
 */

#ifndef RH850_INSN_LEN_H
#define RH850_INSN_LEN_H

#define RH850_HW_REG2(hw0)  (((hw0) >> 11) & 0x1f)
#define RH850_HW_OP6(hw0)   (((hw0) >> 5) & 0x3f)

static inline bool rh850_insn_is_16bit(uint16_t hw0)
{
    if (((hw0 >> 9) & 3) == 3) {
        return false;
    }
    return !(RH850_HW_OP6(hw0) == 0x17 && RH850_HW_REG2(hw0) == 0);
}

static inline int rh850_insn_len(uint16_t hw0, uint16_t hw1)
{
    unsigned op6 = RH850_HW_OP6(hw0);

    if (RH850_HW_REG2(hw0) != 0) {
        return 4;
    }
    switch (op6) {
    case 0x17:
    case 0x31:
        return 6;
    case 0x37:
        return (hw1 & 1) ? 4 : 6;
    case 0x3c:
    case 0x3d:
        if (!(hw1 & 1)) {
            return 4;
        }
        if ((hw1 & 0x1f) == 0x01) {
            return 4;
        }
        if ((hw1 & 7) == 3) {
            static const int prepare_len[4] = { 4, 6, 6, 8 };
            return prepare_len[(hw1 >> 3) & 3];
        }
        return 6;
    default:
        return 4;
    }
}

#endif
