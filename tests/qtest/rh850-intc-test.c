/*
 * SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright (c) 2026 Linumiz
 */

#include "qemu/osdep.h"
#include "libqtest.h"

#define INTC1_PE0   0xfffc4000
#define INTC2       0xfff80000
#define EIC_EIRF    0x1000
#define EIC_EICT    0x8000

typedef struct {
    const char *machine;
    int level_ch;
    int edge_ch;
} IntcCase;

static uint64_t eic_addr(int ch)
{
    return (ch < 32 ? INTC1_PE0 : INTC2) + 2 * ch;
}

static uint16_t eic(QTestState *qts, int ch)
{
    return qtest_readw(qts, eic_addr(ch));
}

static void test_intc_channels(const void *data)
{
    const IntcCase *c = data;
    QTestState *qts = qtest_initf("-M %s", c->machine);

    g_assert_cmphex(eic(qts, c->level_ch), ==, EIC_EICT | 0x8f);
    qtest_set_irq_in(qts, "/machine/intc", NULL, c->level_ch, 1);
    g_assert_cmphex(eic(qts, c->level_ch) & EIC_EIRF, ==, EIC_EIRF);
    qtest_writew(qts, eic_addr(c->level_ch), 0x008f);
    g_assert_cmphex(eic(qts, c->level_ch) & EIC_EIRF, ==, EIC_EIRF);
    qtest_set_irq_in(qts, "/machine/intc", NULL, c->level_ch, 0);
    g_assert_cmphex(eic(qts, c->level_ch) & EIC_EIRF, ==, 0);
    qtest_writew(qts, eic_addr(c->level_ch), EIC_EIRF | 0x8f);
    g_assert_cmphex(eic(qts, c->level_ch) & EIC_EIRF, ==, 0);

    g_assert_cmphex(eic(qts, c->edge_ch), ==, 0x8f);
    qtest_set_irq_in(qts, "/machine/intc", NULL, c->edge_ch, 1);
    qtest_set_irq_in(qts, "/machine/intc", NULL, c->edge_ch, 0);
    g_assert_cmphex(eic(qts, c->edge_ch) & EIC_EIRF, ==, EIC_EIRF);
    qtest_writew(qts, eic_addr(c->edge_ch), 0x008f);
    g_assert_cmphex(eic(qts, c->edge_ch) & EIC_EIRF, ==, 0);

    qtest_quit(qts);
}

static const IntcCase cases[] = {
    { "rh850-u2c", 37, 63 },
    { "rh850-u2a", 39, 199 },
    { "rh850-u2b", 576, 360 },
};

int main(int argc, char **argv)
{
    int i;

    g_test_init(&argc, &argv, NULL);
    for (i = 0; i < ARRAY_SIZE(cases); i++) {
        g_autofree char *name = g_strdup_printf("rh850/intc/%s",
                                                cases[i].machine);

        qtest_add_data_func(name, &cases[i], test_intc_channels);
    }
    return g_test_run();
}
