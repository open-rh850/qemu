<!--
SPDX-License-Identifier: GPL-2.0-or-later
-->

# QEMU for Renesas RH850

QEMU master with the RH850 target (qemu-system-rh850, machines
rh850-u2a, rh850-u2b and rh850-u2c). Used by the Zephyr RH850 port for
CI and as a Zephyr SDK host tool.

Branch dev/rh850/qemu-downstream: dev/rh850/qemu-upstream-next, the
RH850 target, devices, machines, tests and documentation
(docs/system/target-rh850.rst) on QEMU master, plus this README and the
release script.

Build:

    ./configure --target-list=rh850-softmmu
    make

Release archive (qemu-rh850-linux-x86_64.tar.gz with bin/qemu-system-rh850,
unpacked to /opt/qemu-rh850):

    scripts/rh850-release.sh <build-dir> <output-dir>

Run a Zephyr image:

    qemu-system-rh850 -M rh850-u2c -nographic -kernel zephyr.elf
