# ArmEmu - Documentation

This component represents the initial core of the emulation logic. It can
currently emulate the entire ARMv2 instructions set. Note that does not
include the additions provided by the ARM3 which are referred to in the
code as ARMv2a (i.e. SWP, and generic co-processor instructions). While
emulation for the system co-processor does not exist, the processor can
theoretically execute the SWP, LDR/STR of signed bytes and half-words and
the long multiply instructions.

A primitive test system can be emulated containing 32KB ROM, 32KB RAM and
an ARMv2 core. Various unit tests exist to validate the existing functionality.
These are implemented in the google test app ArmEmu_Tests.

An additional application exists to test the performance of various emulated
configurations: EmuPerfTest. This runs a hand assembled implementation of
the old Dhrystone 2.1 benchmark which is assembled at build time using AAsm
and embedded in the executable. The results of the benchmark themselves aren't
important, but the processing is measured against the actual passage of time
to determine the maximum simulated processor clock speed for a given
configuration.

Currently only the ARMv2 test system configuration is supported and no
performance profiling has been performed as a result of testing. Running on
two control machines, A) an 11th Gen i7 on Windows 11, and B) a 1st Gen i7
running x64 Linux, we get the following results:

A) ~60 MHz
B) ~30 MHz

After refactoring the code to use templates and less branching:

A) ~180 MHz
B) ~60 MHz

The refactoring will allow the emulator components to be combined in various
different ways without having to resort to virtual function
calls, therefore making it possible to support lots of different processor
configurations without a performance penalty. These will be used to expand
the EmuPerfTest tool.