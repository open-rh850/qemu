.. SPDX-License-Identifier: GPL-2.0-or-later
.. Copyright (c) 2026 Linumiz

.. _RH850-System-emulator:

RH850 System emulator
---------------------

Use the executable ``qemu-system-rh850`` to emulate the Renesas RH850/U2C
microcontroller (RH850/U2C8, R7F702600AFABB) with its G4KH CPU core. Product
information: https://www.renesas.com/en/products/rh850-u2c

The ``rh850-u2c`` machine emulates:

-  G4KH CPU (CPU0 only): basic and cache instructions, system registers,
   EI/FE level exceptions, the register bank and the single and double
   precision FPU
-  Code flash (8 MiB at 0x00000000), local RAM (128 KiB at 0xfdc00000,
   with its self view at 0xfde00000), cluster RAM (0xfe000000) and retention
   RAM (0x12000000)
-  Interrupt controller (INTC1 of CPU0, INTC2, FENMI and FEINT)
-  OS timers OSTM0 and OSTM1
-  RLIN3 UART units (UART mode)
-  A clock controller and standby controller stub, with the software system
   reset (SWSRESA) of the reset controller

Not emulated: other peripherals (their address ranges are unimplemented
devices), the second CPU (PE1), MPU protection (its registers exist, accesses
are not checked), caches and cycle-accurate timing.

Running
=======

The machine loads an ELF image with ``-kernel``; execution starts at the
reset vector. The first serial port is RLIN311::

  qemu-system-rh850 -M rh850-u2c -nographic -kernel <elf>

Machine properties:

``clk-hsb=<Hz>``
  CLKC_HSB frequency, the OSTM clock (default 20000000).

``test-exit=on|off``
  Map an exit device at 0x40000000 for test programs (default off). The
  address is reserved on silicon.

RH850/U2A and RH850/U2B
=======================

The ``rh850-u2a`` (RH850/U2A16) and ``rh850-u2b`` (RH850/U2B24) machines use
the G4MH2 CPU core (``-cpu g4mh2``). They emulate:

-  G4MH2 CPU (CPU0 only): the G4KH instructions plus LDM.MP and STM.MP,
   system registers with selection IDs up to 15, 64 interrupt priority levels
   (INTCFG.EPL), 32 MPU areas; the virtualization support is off
   (HVCFG.HVE = 0), so its registers are undefined and HVTRAP, LDM.GSR and
   STM.GSR raise a reserved instruction exception; no FXU
-  Code flash (16 MiB on U2A, 24 MiB on U2B, at 0x00000000), local RAM of
   CPU0 (64 KiB at 0xfdc00000, with its self view at 0xfde00000) and the
   cluster RAM
-  Interrupt controller (INTC1 of CPU0, INTC2, FENMI and FEINT) with 768
   (U2A) or 1024 (U2B) channels, edge or level detected as on the U2A16 and
   U2B24
-  OS timers OSTM0 and OSTM1
-  RLIN3 UART units (UART mode); on U2B, the interrupts of RLIN36 to RLIN322
   go through the RLIN3INTSEL0 to RLIN3INTSEL5 selection registers
-  The clock controller, standby controller and reset controller registers
   used to start the RLIN3 and OSTM units, and the software system reset

The first serial port is RLIN30::

  qemu-system-rh850 -M rh850-u2a -nographic -kernel <elf>
  qemu-system-rh850 -M rh850-u2b -nographic -kernel <elf>

They have the same ``test-exit`` property, and ``clk-hsb`` (CLK_HSB on U2A,
CLKC_HSB on U2B) with a default of 80000000.
