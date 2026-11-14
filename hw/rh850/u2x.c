/*
 * SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright (c) 2026 Linumiz
 */

#include "qemu/osdep.h"
#include "qemu/error-report.h"
#include "qemu/log.h"
#include "qapi/error.h"
#include "elf.h"
#include "hw/core/loader.h"
#include "hw/core/qdev-clock.h"
#include "hw/core/qdev-properties.h"
#include "hw/core/qdev-properties-system.h"
#include "hw/misc/sifive_test.h"
#include "hw/misc/unimp.h"
#include "hw/rh850/u2x.h"
#include "qobject/qlist.h"
#include "system/reset.h"
#include "system/system.h"
#include "system/address-spaces.h"

static uint64_t rh850_u2x_cflash_read(void *opaque, hwaddr addr,
                                      unsigned size)
{
    return 0;
}

static void rh850_u2x_cflash_write(void *opaque, hwaddr addr, uint64_t val,
                                   unsigned size)
{
    qemu_log_mask(LOG_GUEST_ERROR, "rh850: write of %u bytes to code flash "
                  "at 0x%08" HWADDR_PRIx " ignored\n", size, addr);
}

static const MemoryRegionOps rh850_u2x_cflash_ops = {
    .read = rh850_u2x_cflash_read,
    .write = rh850_u2x_cflash_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
};

static void rh850_u2x_load_kernel(const char *filename, uint64_t flash_size)
{
    ssize_t size;

    size = load_elf(filename, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                    ELFDATA2LSB, EM_V800, 0, 0);
    if (size == ELF_LOAD_NOT_ELF) {
        size = load_image_targphys(filename, RH850_U2X_CFLASH_BASE,
                                   flash_size, NULL);
        if (size < 0) {
            error_report("could not load kernel '%s'", filename);
            exit(1);
        }
    } else if (size < 0) {
        error_report("could not load kernel '%s': %s (expected ELF "
                     "machine %d, little endian)", filename,
                     load_elf_strerror(size), EM_V800);
        exit(1);
    }
}

static void rh850_u2x_intc_init(RH850U2XMachineState *s,
                                const RH850U2XChip *chip)
{
    SysBusDevice *sbd = SYS_BUS_DEVICE(&s->intc);
    DeviceState *cpu = DEVICE(&s->cpu);
    QList *level = qlist_new();
    unsigned i;

    for (i = 0; i < chip->nlevel; i++) {
        qlist_append_int(level, chip->level[i][0]);
        qlist_append_int(level, chip->level[i][1]);
    }
    qdev_prop_set_uint32(DEVICE(sbd), "num-channels", chip->intc_channels);
    qdev_prop_set_uint8(DEVICE(sbd), "prio-bits", 6);
    qdev_prop_set_bit(DEVICE(sbd), "u2c-level-table", false);
    qdev_prop_set_array(DEVICE(sbd), "level-ranges", level);
    sysbus_realize(sbd, &error_fatal);
    sysbus_mmio_map(sbd, 0, RH850_U2X_INTC1_PE0_BASE);
    sysbus_mmio_map(sbd, 1, RH850_U2X_INTC2_BASE);
    sysbus_mmio_map(sbd, 2, RH850_U2X_FENC_BASE);
    sysbus_mmio_map(sbd, 3, RH850_U2X_FEINC_PE0_BASE);
    memory_region_init_alias(&s->intc1_self, OBJECT(s), "rh850.intc1-self",
                             sysbus_mmio_get_region(sbd, 0), 0,
                             RH850_INTC1_SIZE);
    memory_region_add_subregion(get_system_memory(),
                                RH850_U2X_INTC1_SELF_BASE, &s->intc1_self);
    for (i = 0; i < RH850_INTC_OUT_NUM; i++) {
        sysbus_connect_irq(sbd, i, qdev_get_gpio_in(cpu, i));
    }
    for (i = 0; i < RH850_INTC_ACK_NUM; i++) {
        qdev_connect_gpio_out_named(cpu, "intc-ack", i,
                                    qdev_get_gpio_in_named(DEVICE(sbd),
                                                           "ack", i));
    }
}

static void rh850_u2x_ostm_init(RH850U2XMachineState *s,
                                const RH850U2XChip *chip)
{
    int i;

    s->clk_hsb = clock_new(OBJECT(s), "clkc-hsb");
    clock_set_hz(s->clk_hsb, s->clk_hsb_hz);
    for (i = 0; i < RH850_U2X_NOSTM; i++) {
        g_autofree char *name = g_strdup_printf("ostm%d", i);
        SysBusDevice *sbd;

        object_initialize_child(OBJECT(s), name, &s->ostm[i],
                                TYPE_RENESAS_OSTM);
        sbd = SYS_BUS_DEVICE(&s->ostm[i]);
        qdev_connect_clock_in(DEVICE(sbd), "clk", s->clk_hsb);
        sysbus_realize(sbd, &error_fatal);
        sysbus_mmio_map(sbd, 0, chip->ostm_base[i]);
        sysbus_connect_irq(sbd, 0, qdev_get_gpio_in(DEVICE(&s->intc),
                                                    chip->ostm_irq[i]));
    }
}

static void rh850_u2x_intsel_init(RH850U2XMachineState *s,
                                  const RH850U2XChip *chip)
{
    SysBusDevice *sbd;
    int m, k;

    object_initialize_child(OBJECT(s), "intsel", &s->intsel,
                            TYPE_RH850_U2B_INTSEL);
    sbd = SYS_BUS_DEVICE(&s->intsel);
    sysbus_realize(sbd, &error_fatal);
    sysbus_mmio_map(sbd, 0, RH850_U2B_INTSEL_BASE);
    for (m = 0; m < RH850_U2B_INTSEL_NMUX; m++) {
        for (k = 0; k < RH850_U2B_INTSEL_NLINE; k++) {
            sysbus_connect_irq(sbd, m * RH850_U2B_INTSEL_NLINE + k,
                               qdev_get_gpio_in(DEVICE(&s->intc),
                                                chip->intsel_irq[m] + k));
        }
    }
}

static qemu_irq rh850_u2x_rlin3_route(RH850U2XMachineState *s,
                                      const RH850U2XChip *chip, int n, int k)
{
    qemu_irq sel = qdev_get_gpio_in(DEVICE(&s->intsel),
                                    (n - RH850_U2B_INTSEL_FIRST) *
                                    RH850_U2B_INTSEL_NLINE + k);
    g_autofree char *name = NULL;
    DeviceState *split;

    if (!chip->rlin3_irq[n]) {
        return sel;
    }
    name = g_strdup_printf("rlin3%d-split%d", n, k);
    object_initialize_child(OBJECT(s), name, &s->rlin3_split[n][k],
                            TYPE_SPLIT_IRQ);
    split = DEVICE(&s->rlin3_split[n][k]);
    qdev_prop_set_uint16(split, "num-lines", 2);
    qdev_realize(split, NULL, &error_fatal);
    qdev_connect_gpio_out(split, 0, qdev_get_gpio_in(DEVICE(&s->intc),
                                                     chip->rlin3_irq[n] + k));
    qdev_connect_gpio_out(split, 1, sel);
    return qdev_get_gpio_in(split, 0);
}

static void rh850_u2x_rlin3_init(RH850U2XMachineState *s,
                                 const RH850U2XChip *chip)
{
    int n, k, hd = 1;

    for (n = 0; n < RH850_U2X_NRLIN3; n++) {
        g_autofree char *name = g_strdup_printf("rlin3%d", n);
        DeviceState *dev;
        Chardev *chr;

        object_initialize_child(OBJECT(s), name, &s->rlin3[n],
                                TYPE_RENESAS_RLIN3);
        dev = DEVICE(&s->rlin3[n]);
        chr = serial_hd(n == RH850_U2X_CONSOLE_RLIN3 ? 0 : hd++);
        qdev_prop_set_chr(dev, "chardev", chr);
        sysbus_realize(SYS_BUS_DEVICE(dev), &error_fatal);
        sysbus_mmio_map(SYS_BUS_DEVICE(dev), 0, chip->rlin3_base[n]);
        if (chip->intsel_irq && n >= RH850_U2B_INTSEL_FIRST &&
            n < RH850_U2B_INTSEL_FIRST + RH850_U2B_INTSEL_NUNIT) {
            for (k = 0; k < RENESAS_RLIN3_NIRQ; k++) {
                sysbus_connect_irq(SYS_BUS_DEVICE(dev), k,
                                   rh850_u2x_rlin3_route(s, chip, n, k));
            }
            continue;
        }
        if (!chip->rlin3_irq[n]) {
            continue;
        }
        for (k = 0; k < RENESAS_RLIN3_NIRQ; k++) {
            sysbus_connect_irq(SYS_BUS_DEVICE(dev), k,
                               qdev_get_gpio_in(DEVICE(&s->intc),
                                                chip->rlin3_irq[n] + k));
        }
    }
}

static void rh850_u2x_init(MachineState *machine)
{
    RH850U2XMachineState *s = RH850_U2X_MACHINE(machine);
    const RH850U2XChip *chip = RH850_U2X_MACHINE_GET_CLASS(machine)->chip;
    MemoryRegion *sysmem = get_system_memory();
    int i;

    object_initialize_child(OBJECT(machine), "cpu", &s->cpu,
                            machine->cpu_type);
    qdev_prop_set_uint32(DEVICE(&s->cpu), "pid", chip->pid);
    qdev_realize(DEVICE(&s->cpu), NULL, &error_fatal);
    qemu_register_resettable(OBJECT(&s->cpu));
    object_initialize_child(OBJECT(machine), "intc", &s->intc,
                            TYPE_RH850_INTC);
    rh850_u2x_intc_init(s, chip);
    rh850_u2x_ostm_init(s, chip);
    if (chip->intsel_irq) {
        rh850_u2x_intsel_init(s, chip);
    }
    rh850_u2x_rlin3_init(s, chip);
    object_initialize_child(OBJECT(machine), "sysctl", &s->sysctl,
                            TYPE_RH850_U2C_SYSCTL);
    qdev_prop_set_string(DEVICE(&s->sysctl), "chip", chip->sysctl_chip);
    sysbus_realize(SYS_BUS_DEVICE(&s->sysctl), &error_fatal);
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->sysctl), 0, RH850_U2X_SYSCTL_BASE);

    memory_region_init_rom_device(&s->cflash, NULL, &rh850_u2x_cflash_ops,
                                  s, "rh850.cflash", chip->cflash_size,
                                  &error_fatal);
    memory_region_add_subregion(sysmem, RH850_U2X_CFLASH_BASE, &s->cflash);

    memory_region_init_ram(&s->lram0, NULL, "rh850.lram0",
                           RH850_U2X_LRAM_SIZE, &error_fatal);
    memory_region_add_subregion(sysmem, RH850_U2X_LRAM0_BASE, &s->lram0);
    memory_region_init_alias(&s->lram_self, NULL, "rh850.lram-self",
                             &s->lram0, 0, RH850_U2X_LRAM_SIZE);
    memory_region_add_subregion(sysmem, RH850_U2X_LRAM_SELF_BASE,
                                &s->lram_self);

    for (i = 0; i < RH850_U2X_NCRAM && chip->cram[i].size; i++) {
        g_autofree char *name = g_strdup_printf("rh850.cram%d", i);

        memory_region_init_ram(&s->cram[i], NULL, name, chip->cram[i].size,
                               &error_fatal);
        memory_region_add_subregion(sysmem, chip->cram[i].base, &s->cram[i]);
    }

    create_unimplemented_device("rh850.pbus", RH850_U2X_PBUS_BASE,
                                RH850_U2X_PBUS_SIZE);
    create_unimplemented_device("rh850.ibus", RH850_U2X_IBUS_BASE,
                                RH850_U2X_IBUS_SIZE);
    create_unimplemented_device("rh850.cpu-peri", RH850_U2X_CPUPERI_BASE,
                                chip->cpuperi_size);

    if (s->test_exit) {
        sifive_test_create(RH850_U2X_TEST_BASE);
    }

    if (machine->kernel_filename) {
        rh850_u2x_load_kernel(machine->kernel_filename, chip->cflash_size);
    }
}

static bool rh850_u2x_get_test_exit(Object *obj, Error **errp)
{
    return RH850_U2X_MACHINE(obj)->test_exit;
}

static void rh850_u2x_set_test_exit(Object *obj, bool value, Error **errp)
{
    RH850_U2X_MACHINE(obj)->test_exit = value;
}

static void rh850_u2x_instance_init(Object *obj)
{
    RH850U2XMachineState *s = RH850_U2X_MACHINE(obj);

    s->clk_hsb_hz = RH850_U2X_CLK_HSB_HZ;
    object_property_add_uint32_ptr(obj, "clk-hsb", &s->clk_hsb_hz,
                                   OBJ_PROP_FLAG_READWRITE);
    object_property_set_description(obj, "clk-hsb",
                                    "CLK_HSB frequency in Hz (OSTM clock)");
}

static void rh850_u2x_class_init(ObjectClass *oc, const void *data)
{
    MachineClass *mc = MACHINE_CLASS(oc);
    static const char * const valid_cpu_types[] = {
        TYPE_RH850_G4MH2_CPU,
        NULL
    };

    mc->init = rh850_u2x_init;
    mc->default_cpu_type = TYPE_RH850_G4MH2_CPU;
    mc->valid_cpu_types = valid_cpu_types;

    object_class_property_add_bool(oc, "test-exit", rh850_u2x_get_test_exit,
                                   rh850_u2x_set_test_exit);
    object_class_property_set_description(oc, "test-exit",
                                          "Exit device at 0x40000000 for "
                                          "test programs (default off)");
}

#include "u2a16_level.c.inc"
#include "u2b24_level.c.inc"

static const RH850U2XChip rh850_u2a_chip = {
    .desc = "Renesas RH850/U2A16 (G4MH2, CPU0 only)",
    .sysctl_chip = "u2a",
    .pid = 0x07840700,
    .cflash_size = 16 * MiB,
    .cpuperi_size = 0x14000,
    .cram = {
        { 0xfe000000, 512 * KiB },
        { 0xfe100000, 512 * KiB },
        { 0xfe400000, 2 * MiB },
        { 0xfe800000, 256 * KiB },
    },
    .intc_channels = 768,
    .level = rh850_u2a16_level,
    .nlevel = ARRAY_SIZE(rh850_u2a16_level),
    .ostm_base = { 0xffbf0000, 0xffbf0100 },
    .ostm_irq = { 199, 200 },
    .rlin3_base = {
        0xffd28000, 0xffc7c000, 0xffd28200, 0xffc7c080,
        0xffd28400, 0xffc7c100, 0xffd28600, 0xffc7c180,
        0xffd28800, 0xffc7c200, 0xffd28a00, 0xffc7c280,
        0xffd28c00, 0xffc7c300, 0xffd28e00, 0xffc7c380,
        0xffd29000, 0xffc7c400, 0xffd29200, 0xffc7c480,
        0xffd29400, 0xffc7c500, 0xffd29600, 0xffc7c580,
    },
    .rlin3_irq = {
        416, 420, 424, 428, 432, 436, 440, 444,
        448, 452, 456, 460, 464, 468, 472, 476,
        480, 484, 488, 492, 496, 500, 504, 508,
    },
};

static void rh850_u2a_class_init(ObjectClass *oc, const void *data)
{
    MachineClass *mc = MACHINE_CLASS(oc);
    RH850U2XMachineClass *uc = RH850_U2X_MACHINE_CLASS(oc);

    uc->chip = &rh850_u2a_chip;
    mc->desc = uc->chip->desc;
}

static const uint16_t rh850_u2b_intsel_irq[RH850_U2B_INTSEL_NMUX] = {
    320, 328, 332, 344, 348, 387, 391, 521, 525, 529, 545, 549,
    553, 557, 564, 568, 572, 733, 737, 765, 769, 773, 777, 781,
    785, 789, 882, 890, 894, 898, 902, 906, 951, 955,
};

static const RH850U2XChip rh850_u2b_chip = {
    .desc = "Renesas RH850/U2B24 (G4MH2, CPU0 only)",
    .sysctl_chip = "u2b",
    .pid = 0x07840f01,
    .cflash_size = 24 * MiB,
    .cpuperi_size = 0x1c000,
    .cram = {
        { 0xfe000000, 512 * KiB },
        { 0xfe080000, 512 * KiB },
        { 0xfe100000, 512 * KiB },
        { 0xfe800000, 0x220000 },
    },
    .intc_channels = 1024,
    .level = rh850_u2b24_level,
    .nlevel = ARRAY_SIZE(rh850_u2b24_level),
    .ostm_base = { 0xffbf0000, 0xff89a100 },
    .ostm_irq = { 360, 361 },
    .rlin3_base = {
        0xffc7c000, 0xff89b100, 0xffc7c200, 0xff89b300,
        0xffc7c400, 0xff89b500, 0xffc7c600, 0xff89b700,
        0xffc7c800, 0xff89b900, 0xffc7ca00, 0xff89bb00,
        0xffc7cc00, 0xff89bd00, 0xffc7ce00, 0xff89bf00,
        0xffc7d000, 0xff89c100, 0xffc7d200, 0xff89c300,
        0xffc7d400, 0xff89c500, 0xffc7d600, 0xff978000,
    },
    .rlin3_irq = {
        620, 625, 630, 635, 640, 644, 992, 996,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 648,
    },
    .intsel_irq = rh850_u2b_intsel_irq,
};

static void rh850_u2b_class_init(ObjectClass *oc, const void *data)
{
    MachineClass *mc = MACHINE_CLASS(oc);
    RH850U2XMachineClass *uc = RH850_U2X_MACHINE_CLASS(oc);

    uc->chip = &rh850_u2b_chip;
    mc->desc = uc->chip->desc;
}

static const TypeInfo rh850_u2x_types[] = {
    {
        .name           = TYPE_RH850_U2X_MACHINE,
        .parent         = TYPE_MACHINE,
        .abstract       = true,
        .instance_size  = sizeof(RH850U2XMachineState),
        .instance_init  = rh850_u2x_instance_init,
        .class_size     = sizeof(RH850U2XMachineClass),
        .class_init     = rh850_u2x_class_init,
    }, {
        .name           = TYPE_RH850_U2A_MACHINE,
        .parent         = TYPE_RH850_U2X_MACHINE,
        .class_init     = rh850_u2a_class_init,
    }, {
        .name           = TYPE_RH850_U2B_MACHINE,
        .parent         = TYPE_RH850_U2X_MACHINE,
        .class_init     = rh850_u2b_class_init,
    },
};

DEFINE_TYPES(rh850_u2x_types)
