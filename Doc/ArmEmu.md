# ArmEmu - Documentation

This component represents the initial core of the emulation logic. It can
currently emulate the entire ARMv2 instructions set. Note that does not
include the additions provided by the ARM3 which are referred to in the
code  as ARMv2a (i.e. SWP, and generic co-processor instructions). While
emulation for the system control co-processor does not exist, the processor
can theoretically execute the SWP, LDR/STR of signed bytes and half-words and
the long multiply instructions.

A primitive test system can be emulated containing 32KB ROM, 32KB RAM and
an ARMv2 core. Various unit tests exist to validate the existing functionality.
These are implemented in the google test app ArmEmu_Tests.

An additional application exists to test the performance of various emulated
configurations: EmuPerfTest. This runs an implementation of the old Dhrystone
2.1 benchmark which has been hand translated from C into ARM assembly
language. The source code is assembled at build time using AAsm and embedded
in the executable as binary ARM machine code. The results of the benchmark are
used to determine the maximum simulated processor clock speed for a given
configuration.

Currently only the ARMv2 test system configuration is supported and no
performance profiling has been performed as a result of testing. Running on
two test machines, A) an 11th Gen i7 running Windows 11, and B) a 1st Gen i7
running x64 Linux, we get the following results:

| Milestone              |    1st Gen i7    |    11th Gen i7    |
| ---------------------- | ---------------- | ----------------- |
| Initial implementation |           30 MHz |            60 MHz |
| Template refactor      | 60 MHz / 33 MIPS | 180 MHz / 98 MIPS |

The refactoring allows the emulator components (when they are written) to be
combined in various different ways without having to resort to virtual function
calls, therefore making it possible to support lots of different processor
configurations without a performance penalty. These will be used to expand
the EmuPerfTest tool.

# MEMC-Based System Emulation

Recent changes have attempted to create an emulation of a MEMC/VIDC/IOC-based
system. ARM2 and ARM3-based systems can be instantiated, although there
is currently no support for the System Control Co-Processor implemented
by the ARM3 (making it an ARM250).

Development became bogged down developing the KART keyboard/mouse interface.
Some work has been done to develop a basis system ROM which can initialise
the interface. To more easily develop that system, an interactive debugger
was created in Qt. This is usable, but partially complete.

The dependency on Qt is optional. If it is not found, the debugger will not
be configured as part of the project.