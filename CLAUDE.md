# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

MightyOak is a cross-platform emulator for the Acorn Archimedes and RiscPC — the first ARM-based desktop computers. It is an interpreter-based ARM emulator written in C++ with optional GPU-accelerated graphics rendering. The project is licensed under LGPL v3.

## Build Commands

```bash
# Clone with submodules
git clone --recurse-submodules https://github.com/GiantRobotLemur/MightyOak.git

# Configure (Visual Studio 2022 x64)
cmake -S MightyOak -B MightyOakBuild -G "Visual Studio 17 2022" -A x64

# Configure (gcc 11)
cmake -S MightyOak -B MightyOakBuild -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release

# Optional: enable Qt6 debugger UI
cmake -S MightyOak -B MightyOakBuild -DCMAKE_PREFIX_PATH=<path-to-Qt6>

# Build
cmake --build MightyOakBuild --config Release

# Run all tests
ctest --test-dir MightyOakBuild --output-on-failure

# Run individual test suites (from build dir)
./bin/AsmTools_Tests                          # Assembler/disassembler tests
./bin/ArmEmu_Tests                            # ARM emulator tests
./bin/AsmTools_Tests --gtest_filter=TestName   # Single test

# Run performance benchmark
./bin/EmuPerfTest

# Build Doxygen documentation (requires Doxygen 1.8+)
cmake --build MightyOakBuild --target Documentation
```

Supported platforms: Visual Studio 2022 x64 (Windows), gcc 11 x64 (Linux).

## Architecture

### Namespaces
- `Mo::Asm` — ARM assembler/disassembler (AsmTools library)
- `Mo::Arm` — ARM processor emulation (ArmEmu library)
- `Ag` — Silver runtime library (git submodule at `Source/Ag/`)

### Major Components

**AsmTools** (`Source/AsmTools/`) — Standalone ARM assembly/disassembly library supporting ARMv2–ARMv5 and partial FPA. Contains a full lexer, parser, expression evaluator, symbol table, and object code builder. Produces two CLI tools: `AAsm` (assembler) and `ADisasm` (disassembler). Public API: `Source/Include/AsmTools.hpp`.

**ArmEmu** (`Source/ArmEmu/`) — ARM processor and hardware emulator. Emulates ARM CPU core plus Archimedes hardware: MEMC (memory controller), VIDC10 (video), IOC (I/O), and keyboard controller. Public API: `Source/Include/ArmEmu.hpp`.

**ArmDebugger** (`Source/ArmDebugger/`) — Optional Qt 6.5 GUI debugger with memory viewer, register display, and breakpoints. Only built when Qt6 is found by CMake.

**Ag** (`Source/Ag/`) — Reusable cross-platform runtime (git submodule). Provides CMake helper functions (`ag_add_library`, `ag_add_test_app`, `ag_add_cli_app`, `ag_add_gui_app`) used by all build targets.

### Performance-Critical Design

The emulator's CPU pipeline uses **C++ templates instead of virtual functions** in performance-critical paths. The `.inl` files in `Source/ArmEmu/` contain template implementations for:
- `InstructionPipeline.inl` — fetch/decode/execute pipeline
- `ExecutionUnit.inl` — instruction execution
- `RegisterFile.inl` / `ARMv2CoreRegisterFile.inl` — register file management
- `AluInstructions.inl` — ALU operations
- `DataTransferInstructions.inl` — LDR/STR/LDM/STM
- `InstructionDecoder.inl` / `ARMv2InstructionDecoder.inl` — opcode decoding
- `ArmSystem.inl` / `Hardware.inl` — system-level template composition

Optional x64 MASM assembly optimizations exist in `AluOperations_Win32_x64.asm` with a C++ fallback in `AluOperations_NoArch.cpp` (controlled by the `USE_ASM` CMake option).

### Build System Notes

- CMake 3.21+ required, C++17 standard
- Dependencies (Google Test, Moody Camel queue, GLM, SDL3) are auto-fetched via CMake FetchContent
- The `EmuPerfTest` target assembles ARM source at build time using `AAsm`, then embeds the binary via `ag_add_static_data()`
- `AG_STATIC_RUNTIME` is forced ON; `AG_CONFIG_SDL3` and `AG_CONFIG_OPEN_GL` are forced ON

## Coding Conventions

- Avoid C++ virtual functions in performance-critical emulation code — use templates instead
- Use Doxygen comments (`//!`, `@file`, `@brief`, `@author`, `@date`, `@copyright`) for documentation
- Namespaces: lowercase (`Mo`, `Ag`). Classes/enums: PascalCase. Functions: camelCase
- Public headers go in `Source/Include/AsmTools/` or `Source/Include/ArmEmu/`
- Test files follow the pattern `Test_*.cpp` using Google Test
- Do not automatically commit changes
- Do not ask to commit changes
