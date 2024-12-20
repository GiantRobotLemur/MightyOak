# Mighty Oak : An Acorn Archimedes and RiscPC Emulator

The Archimedes was a British computer popular in schools in the 1990s which
was the first ARM-based desktop machine. ARM originally stood for Acorn RISC
Machine, but was later changed to Advanced RISC Machine when the processor
architecture outgrew the Archimedes and its successors.

## Aims

The aim of the project is to create a program which can emulate various models
of the Archimedes and RiscPC lines with configurable hardware in an easy to use
package. Part of that is to preserve the system and another to explore its
capabilities.

The intention is to create a high performance interpreting emulator which will
run on any platform. I will resort to dynamic recompilation if I can't get the
performance I need from interpretation, but other emulators exist which can do
this, so it must be possible.

One of the final aims is to integrate hardware accelerated graphical rendering
with the guest operating system. RISC OS had numerous hooks which allowed
system drawing subroutines to be replaced. I'd like to replace them with calls
to the host machine running GPU accelerated OpenGL (or the like) to produce
fast anti-aliased graphics from an emulated machine running at a clock speed as
low as 8 MHz. This goal is mostly just for the hell of it (like strapping a
rocket engine to a steam train), but would also attest to what a flexible
system the Archimedes was.

## Motivation

Why create an Archimedes emulator? There are a number of reasons:
* It was an impressive machine that deserves to be preserved as the
availability of original hardware and supporting devices (i.e. PAL TV, low
resolution analogue displays, floppy disks, etc.) diminishes.
* To explore the capabilities of the hardware and RISC OS.
* To play copyright protected games on an emulator which will accept disk
images containing appropriate metadata captured from the original media.
* It's fun!

## Existing Components

At the current time, the project consists of the following major components
written in C++.
* [Ag Core](https://github.com/GiantRobotLemur/Ag) -
A platform agnostic runtime library build based on the STL and C Runtime
on which to build the system. Some unit test coverage. This has been ported
to a git sub-module so that it can be used in other projects.
* [Asm Tools](Doc/AsmTools.md) - A library which can assemble and disassemble
32-bit ARM assembly language up to the ARMv4 architecture. This includes a
fully fledged assembler akin to the one integrated into BBC BASIC. Plenty of
unit test coverage.
* [Arm Emu](Doc/ArmEmu.md) - A library which emulates the core of an ARM
processor, but not MMU or any other hardware yet. Some unit test coverage.

## Dependencies

The project is currently dependent on the following up-stream components:
* [Google Test 1.12+](https://github.com/google/googletest) [Required - but downloaded by CMake]
* [Moody Camel Reader/Writer Queue](https://github.com/cameron314/readerwriterqueue) [Required - but downloaded by CMake]
* MASM [Optional]
* [Doxygen](https://www.doxygen.nl/) [Optional]
* [Qt 6.5](https://www.qt.io/product/framework) [Optional] - For interactive ARM Debugger

## Build

You can clone the repository and submodules from here using:
```
git clone --recurse-submodule https://github.com/GiantRobotLemur/MightyOak.git
```

Configure with CMake using:
```
cmake -S MightyOak -B MightyOakBuild [-G <generator>] [-DCMAKE_PREFIX_PATH=<path to Qt>]
```

Currently the supported platforms are Visual Studio 2022 and gcc 11, both on x64.
While other platforms may work, they aren't officially supported.

## License

This code is provided under the LGPL version 3 licensing conditions as
described in the [LICENSE file](LICENSE) at the root of the repository.

## Status

The project is currently on ice. The Silver (Ag) library has been split off
into a sub-module for use in other projects. Hopefully interest will return
this way sometime soon.

## TODO

There is a long shopping list of elements which need to be developed. These will
be managed as GitHub issues.
