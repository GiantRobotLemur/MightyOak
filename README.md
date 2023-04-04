# Mighty Oak : An Acorn Archimedes and RiscPC Emulator

The Archimedes was a British computer popular in schools in the 1990s which was the first ARM-based desktop machine. ARM originally stood for Acorn RISC Machine, but was later changed to Advanced RISC Machine when the processor architecture outgrew the Archimedes and its successors.

## Aims

The aim of the project is to create a program which can emulate various models of the Archimedes and RiscPC lines with configurable hardware in an easy to use package. Part of that is to preserve the system and another to explore its capabilities.

The intention is to create a high performance interpreting emulator which will run on any platform. I will resort to dynamic recompilation if I can't get the performance I need from interpretation, but other emulators exist which can do this, so it must be possible.

One of the final aims is to integrate hardware accelerated graphical rendering with the guest operating system. RISC OS had numerous hooks which allowed system drawing subroutines to be replaced. I'd like to replace them with calls to the host machine running GPU accelerated OpenGL (or the like) to produce fast anti-aliased graphics from an emulated machine running at a clock speed as low as 8 MHz. This goal is mostly just for the hell of it (like strapping a rocket engine to a steam train), but would also attest to what a flexible system the Archimedes was.

## Motivation

Why create an Archimedes emulator? There are a number of reasons:
* It was an impressive machine that deserves to be preserved as original hardware and supporting devices age (PAL TV, low resolution analogue displays, floppy disks, etc.).
* To explore the capabilities of the hardware and RISC OS.
* To play my copyright protected games on an emulator which will accept disk images containing appropriate metadata captured from the original media.
* It's fun!

## Existing Components

At the current time, the project consists of the following major components written in C++.
* Ag Core - A platform agnostic runtime library build base on the STL and C Runtime on which to build the system. Some unit test coverage.
* Asm Tools - A library which can assemble and disassemble 32-bit ARM assembly language up to the ARMv4 architecture. This includes a fully fledged assembler akin to the one integrated into BBC BASIC. Plenty of unit test coverage.
* Arm Emu - A library which emulates the core of an ARM processor, but without MMU. Some unit test coverage.

## Dependencies

The project is currently dependent on the following up-stream components:
* Google Test 1.10+ [Required]
* MASM [Optional]
* Doxygen [Optional]
* SDL 2 [Required, but not yet] - for later GUI application

## License

This code is provided under the LGPL version 3 licensing conditions as described in the LICENSE file at the root of the repository.

## TODO

There is a long shopping list of elements which need to be developed. In the short term the following are desirable:
* Create a CLI interface to the assembler in Asm Tools.
* Finish the Linux port of the runtime library.
* Splitting processor core components into template classes for reuse/recombination.
* Emulation of MEMC-based memory management.
* Emulation of ARMv2 (ARM 2/250)
* Emulation of the rest of the A-Series chipset (IOC, VIDC 10, 82c710/11, CMOS RAM).
* Implement a GUI application in SDL2+ which allows configuration and interactive use.
* Implement ARM 3 (ARMv2 with cache)
* Implement FPA support in both Asm Tools and Arm Emu.
* Implement ARM 3 + FPA.

