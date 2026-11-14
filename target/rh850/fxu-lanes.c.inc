/*
 * SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright (c) 2026 Linumiz
 */

static const RH850FxuLane rh850_fxu_lanes[FXI_NUM][4] = {
    [FXI_ADDF_S4] = {
        { FOP_ADD, 0, 2, 0, 1, 0, 0, 0 },
        { FOP_ADD, 0, 2, 1, 1, 1, 0, 0 },
        { FOP_ADD, 0, 2, 2, 1, 2, 0, 0 },
        { FOP_ADD, 0, 2, 3, 1, 3, 0, 0 },
    },
    [FXI_ADDRF_S4] = {
        { FOP_ADD, 0, 1, 1, 1, 0, 0, 0 },
        { FOP_ADD, 0, 2, 1, 2, 0, 0, 0 },
        { FOP_ADD, 0, 1, 3, 1, 2, 0, 0 },
        { FOP_ADD, 0, 2, 3, 2, 2, 0, 0 },
    },
    [FXI_ADDSUBF_S4] = {
        { FOP_SUB, 0, 2, 0, 1, 0, 0, 0 },
        { FOP_ADD, 0, 2, 1, 1, 1, 0, 0 },
        { FOP_SUB, 0, 2, 2, 1, 2, 0, 0 },
        { FOP_ADD, 0, 2, 3, 1, 3, 0, 0 },
    },
    [FXI_ADDSUBNF_S4] = {
        { FOP_SUB, 1, 2, 0, 1, 0, 0, 0 },
        { FOP_ADD, 1, 2, 1, 1, 1, 0, 0 },
        { FOP_SUB, 1, 2, 2, 1, 2, 0, 0 },
        { FOP_ADD, 1, 2, 3, 1, 3, 0, 0 },
    },
    [FXI_ADDSUBNXF_S4] = {
        { FOP_SUB, 1, 2, 0, 1, 1, 0, 0 },
        { FOP_ADD, 1, 2, 1, 1, 0, 0, 0 },
        { FOP_SUB, 1, 2, 2, 1, 3, 0, 0 },
        { FOP_ADD, 1, 2, 3, 1, 2, 0, 0 },
    },
    [FXI_ADDSUBXF_S4] = {
        { FOP_SUB, 0, 2, 0, 1, 1, 0, 0 },
        { FOP_ADD, 0, 2, 1, 1, 0, 0, 0 },
        { FOP_SUB, 0, 2, 2, 1, 3, 0, 0 },
        { FOP_ADD, 0, 2, 3, 1, 2, 0, 0 },
    },
    [FXI_ADDXF_S4] = {
        { FOP_ADD, 0, 2, 0, 1, 1, 0, 0 },
        { FOP_ADD, 0, 2, 1, 1, 0, 0, 0 },
        { FOP_ADD, 0, 2, 2, 1, 3, 0, 0 },
        { FOP_ADD, 0, 2, 3, 1, 2, 0, 0 },
    },
    [FXI_DIVF_S4] = {
        { FOP_DIV, 0, 2, 0, 1, 0, 0, 0 },
        { FOP_DIV, 0, 2, 1, 1, 1, 0, 0 },
        { FOP_DIV, 0, 2, 2, 1, 2, 0, 0 },
        { FOP_DIV, 0, 2, 3, 1, 3, 0, 0 },
    },
    [FXI_FMAF_S4] = {
        { FOP_FMA, 0, 2, 0, 1, 0, 3, 0 },
        { FOP_FMA, 0, 2, 1, 1, 1, 3, 1 },
        { FOP_FMA, 0, 2, 2, 1, 2, 3, 2 },
        { FOP_FMA, 0, 2, 3, 1, 3, 3, 3 },
    },
    [FXI_FMSF_S4] = {
        { FOP_FMS, 0, 2, 0, 1, 0, 3, 0 },
        { FOP_FMS, 0, 2, 1, 1, 1, 3, 1 },
        { FOP_FMS, 0, 2, 2, 1, 2, 3, 2 },
        { FOP_FMS, 0, 2, 3, 1, 3, 3, 3 },
    },
    [FXI_FNMAF_S4] = {
        { FOP_FMA, 1, 2, 0, 1, 0, 3, 0 },
        { FOP_FMA, 1, 2, 1, 1, 1, 3, 1 },
        { FOP_FMA, 1, 2, 2, 1, 2, 3, 2 },
        { FOP_FMA, 1, 2, 3, 1, 3, 3, 3 },
    },
    [FXI_FNMSF_S4] = {
        { FOP_FMS, 1, 2, 0, 1, 0, 3, 0 },
        { FOP_FMS, 1, 2, 1, 1, 1, 3, 1 },
        { FOP_FMS, 1, 2, 2, 1, 2, 3, 2 },
        { FOP_FMS, 1, 2, 3, 1, 3, 3, 3 },
    },
    [FXI_MAXF_S4] = {
        { FOP_MAX, 0, 2, 0, 1, 0, 0, 0 },
        { FOP_MAX, 0, 2, 1, 1, 1, 0, 0 },
        { FOP_MAX, 0, 2, 2, 1, 2, 0, 0 },
        { FOP_MAX, 0, 2, 3, 1, 3, 0, 0 },
    },
    [FXI_MAXRF_S4] = {
        { FOP_MAX, 0, 1, 1, 1, 0, 0, 0 },
        { FOP_MAX, 0, 2, 1, 2, 0, 0, 0 },
        { FOP_MAX, 0, 1, 3, 1, 2, 0, 0 },
        { FOP_MAX, 0, 2, 3, 2, 2, 0, 0 },
    },
    [FXI_MINF_S4] = {
        { FOP_MIN, 0, 2, 0, 1, 0, 0, 0 },
        { FOP_MIN, 0, 2, 1, 1, 1, 0, 0 },
        { FOP_MIN, 0, 2, 2, 1, 2, 0, 0 },
        { FOP_MIN, 0, 2, 3, 1, 3, 0, 0 },
    },
    [FXI_MINRF_S4] = {
        { FOP_MIN, 0, 1, 1, 1, 0, 0, 0 },
        { FOP_MIN, 0, 2, 1, 2, 0, 0, 0 },
        { FOP_MIN, 0, 1, 3, 1, 2, 0, 0 },
        { FOP_MIN, 0, 2, 3, 2, 2, 0, 0 },
    },
    [FXI_MULF_S4] = {
        { FOP_MUL, 0, 2, 0, 1, 0, 0, 0 },
        { FOP_MUL, 0, 2, 1, 1, 1, 0, 0 },
        { FOP_MUL, 0, 2, 2, 1, 2, 0, 0 },
        { FOP_MUL, 0, 2, 3, 1, 3, 0, 0 },
    },
    [FXI_MULRF_S4] = {
        { FOP_MUL, 0, 1, 1, 1, 0, 0, 0 },
        { FOP_MUL, 0, 2, 1, 2, 0, 0, 0 },
        { FOP_MUL, 0, 1, 3, 1, 2, 0, 0 },
        { FOP_MUL, 0, 2, 3, 2, 2, 0, 0 },
    },
    [FXI_MULXF_S4] = {
        { FOP_MUL, 0, 2, 0, 1, 1, 0, 0 },
        { FOP_MUL, 0, 2, 1, 1, 0, 0, 0 },
        { FOP_MUL, 0, 2, 2, 1, 3, 0, 0 },
        { FOP_MUL, 0, 2, 3, 1, 2, 0, 0 },
    },
    [FXI_RECIPF_S4] = {
        { FOP_RECIP, 0, 2, 0, 0, 0, 0, 0 },
        { FOP_RECIP, 0, 2, 1, 0, 0, 0, 0 },
        { FOP_RECIP, 0, 2, 2, 0, 0, 0, 0 },
        { FOP_RECIP, 0, 2, 3, 0, 0, 0, 0 },
    },
    [FXI_RSQRTF_S4] = {
        { FOP_RSQRT, 0, 2, 0, 0, 0, 0, 0 },
        { FOP_RSQRT, 0, 2, 1, 0, 0, 0, 0 },
        { FOP_RSQRT, 0, 2, 2, 0, 0, 0, 0 },
        { FOP_RSQRT, 0, 2, 3, 0, 0, 0, 0 },
    },
    [FXI_SQRTF_S4] = {
        { FOP_SQRT, 0, 2, 0, 0, 0, 0, 0 },
        { FOP_SQRT, 0, 2, 1, 0, 0, 0, 0 },
        { FOP_SQRT, 0, 2, 2, 0, 0, 0, 0 },
        { FOP_SQRT, 0, 2, 3, 0, 0, 0, 0 },
    },
    [FXI_SUBADDF_S4] = {
        { FOP_ADD, 0, 2, 0, 1, 0, 0, 0 },
        { FOP_SUB, 0, 2, 1, 1, 1, 0, 0 },
        { FOP_ADD, 0, 2, 2, 1, 2, 0, 0 },
        { FOP_SUB, 0, 2, 3, 1, 3, 0, 0 },
    },
    [FXI_SUBADDNF_S4] = {
        { FOP_ADD, 1, 2, 0, 1, 0, 0, 0 },
        { FOP_SUB, 1, 2, 1, 1, 1, 0, 0 },
        { FOP_ADD, 1, 2, 2, 1, 2, 0, 0 },
        { FOP_SUB, 1, 2, 3, 1, 3, 0, 0 },
    },
    [FXI_SUBADDNXF_S4] = {
        { FOP_ADD, 1, 2, 0, 1, 1, 0, 0 },
        { FOP_SUB, 1, 2, 1, 1, 0, 0, 0 },
        { FOP_ADD, 1, 2, 2, 1, 3, 0, 0 },
        { FOP_SUB, 1, 2, 3, 1, 2, 0, 0 },
    },
    [FXI_SUBADDXF_S4] = {
        { FOP_ADD, 0, 2, 0, 1, 1, 0, 0 },
        { FOP_SUB, 0, 2, 1, 1, 0, 0, 0 },
        { FOP_ADD, 0, 2, 2, 1, 3, 0, 0 },
        { FOP_SUB, 0, 2, 3, 1, 2, 0, 0 },
    },
    [FXI_SUBF_S4] = {
        { FOP_SUB, 0, 2, 0, 1, 0, 0, 0 },
        { FOP_SUB, 0, 2, 1, 1, 1, 0, 0 },
        { FOP_SUB, 0, 2, 2, 1, 2, 0, 0 },
        { FOP_SUB, 0, 2, 3, 1, 3, 0, 0 },
    },
    [FXI_SUBRF_S4] = {
        { FOP_SUB, 0, 1, 1, 1, 0, 0, 0 },
        { FOP_SUB, 0, 2, 1, 2, 0, 0, 0 },
        { FOP_SUB, 0, 1, 3, 1, 2, 0, 0 },
        { FOP_SUB, 0, 2, 3, 2, 2, 0, 0 },
    },
    [FXI_SUBXF_S4] = {
        { FOP_SUB, 0, 2, 0, 1, 1, 0, 0 },
        { FOP_SUB, 0, 2, 1, 1, 0, 0, 0 },
        { FOP_SUB, 0, 2, 2, 1, 3, 0, 0 },
        { FOP_SUB, 0, 2, 3, 1, 2, 0, 0 },
    },
};
