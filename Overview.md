# MightyOak Architecture Overview

MightyOak is a cross-platform emulator for the Acorn Archimedes and (planned)
RiscPC — the first ARM-based desktop computers. It is an interpreter-based ARM
emulator written in C++17 with SDL3-based display and optional Qt6 debugger UI.
Licensed under LGPL v3.

## Directory Structure

```
MightyOak/
├── Source/
│   ├── Ag/                  Git submodule — "Silver" cross-platform runtime
│   ├── AsmTools/            ARM assembler & disassembler library
│   ├── ArmEmu/              ARM CPU + Archimedes hardware emulation
│   ├── ArmDebugger/         Optional Qt6 GUI debugger
│   ├── MightyOakLib/        Shared emulator application logic (SDL3)
│   ├── EmulatorApp/         GUI application entry point
│   └── Include/             All public headers
│       ├── AsmTools/
│       ├── ArmEmu/
│       └── MightyOakLib/
├── Tests/                   ARM assembly source for test ROMs
├── Doc/                     Doxygen configuration
└── cmake/                   CMake utility scripts
```

## Dependency Graph

```
MightyOak (SDL3 GUI app)
  └── MightyOakLib
        ├── ArmEmu
        │     ├── AsmTools     (used in tests + build-time ROM assembly)
        │     └── AgCore
        └── Sdl3Tools
              └── AgCore

ArmDebugger (optional Qt6 debugger)
  ├── ArmEmu
  ├── AsmTools
  └── AgCore + QtInterop
```

## Major Components

### Ag — Silver Runtime (git submodule)

A reusable cross-platform runtime providing:

- **AgCore** — `Ag::App` lifecycle, `Ag::String` (UTF-8), `Ag::Fs::Path`,
  `Ag::CLI::ProgramArguments`, `Ag::EnumInfo<T>` enum reflection,
  `Ag::MonotonicTicks` / `Ag::HighResMonotonicTimer`, cache-aligned allocation
  (`Ag::createUniqueAligned<T>`), `Ag::Bin::` bit manipulation utilities,
  exception hierarchy.
- **Sdl3Tools** — C++ wrappers for SDL3: `Window`, `EventProcessor`,
  `PeriodicEventProcessor`, GPU objects (`Device`, `Swapchain`, `Texture`).
- **QtInterop** — Qt6 integration (used only by ArmDebugger).
- **CMake helpers** — `ag_add_library()`, `ag_add_test_app()`,
  `ag_add_cli_app()`, `ag_add_gui_app()`, `ag_add_static_data()`,
  `ag_configure_build()`, `ag_configure_gtest()`, `ag_configure_sdl3()`.

### AsmTools — ARM Assembler & Disassembler

Namespace `Mo::Asm`. Supports ARMv2–ARMv5 with partial FPA. Classical
multi-pass assembler pipeline:

```
Source text → Lexer → Parser → Syntax Tree → Expression Evaluator → Object Code
```

**Lexer layer.** `InputSource` / `InputSet` manage source input and include
stacks. Multiple context-specific `LexicalAnalysers` (statement, expression,
register list, PSR) are selected by a `LexicalContext` enum. `InputContext`
combines source and lexer into a token stream.

**Parser layer.** `ParseContext` wraps `InputContext` with lookahead.
`parseSourceCode()` builds a tree of `ISyntaxNode` implementations:
`LabelStatement`, `DataDirectiveStatement`, `AssemblyDirectiveStatement`,
`IncludeStatement`, and instruction nodes (`AluInstructionNode`,
`DataTransferInstructionNode`, `MultiTransferInstructionNode`,
`AddressDirectiveNode`, `AtomicSwapInstructionNode`,
`CoProcInstructionStatements`).

**Expression system.** `Value` (typed value), `Expr` / `ExpressionNode`
(expression tree), `SymbolTable` (labels, equates, forward references resolved
in a second pass), `ConstantSet` (predefined register names, condition codes).

**Code generation.** `AssemblyState` tracks the current address and output
buffer. `ObjectCodeBuilder` accumulates machine words and patches forward
references. `assembleInstruction()` encodes up to 4 words per statement (for
multi-word pseudo-instructions like `ADR`). The result is an `ObjectCode`
object.

**Disassembler.** `Disassembly.cpp` decodes 32-bit words into
`InstructionInfo` objects. `FormatInstruction` renders them as human-readable
assembly text.

**CLI tools.** `AAsm` (assembler) and `ADisasm` (disassembler).

**Public API** (`Source/Include/AsmTools.hpp`):
```cpp
ObjectCode assembleFile(const Ag::String &filePath, const Options &opts, Messages &messages);
ObjectCode assembleText(const Ag::String &sourceCode, const Options &opts, Messages &messages);
```

### ArmEmu — ARM Emulation Core

Namespace `Mo::Arm`. Emulates the ARM CPU plus Archimedes system hardware.

#### Template-Based Execution (Zero Virtual Dispatch)

The hot path eliminates virtual function calls entirely through C++ template
composition. A traits struct selects concrete types at compile time:

```
ArmSystem<TSysTraits>
  ├── HardwareType           e.g. MemcHardware, TestBedHardware
  ├── RegisterFileType       e.g. ARMv2CoreRegisterFile<HardwareType>
  └── ExecutionUnitType      SingleModeExecutionUnit<HW, RF, Pipeline>
        └── InstructionPipeline<PipelineTraits>
              └── DecoderType  e.g. ARMv2InstructionDecoder<HW, RF>
```

Concrete traits are defined in `SystemConfigurations.inl`:

| Traits struct              | Hardware        | Register file    | Decoder               | Use case               |
|----------------------------|-----------------|------------------|-----------------------|------------------------|
| `ArmV2TestSystemTraits`    | TestBedHardware | ARMv2CoreRegFile | ARMv2InstructionDecoder | Unit tests            |
| `ArmV2aTestSystemTraits`   | TestBedHardware | ARMv2aCoreRegFile| ARMv2aInstructionDecoder| ARMv2a unit tests     |
| `ArmV2MemcSystemTraits`    | MemcHardware    | ARMv2CoreRegFile | ARMv2InstructionDecoder | Archimedes ARM2       |
| `ArmV2aMemcSystemTraits`   | MemcHardware    | ARMv2aCoreRegFile| ARMv2aInstructionDecoder| A-Series / ARM3       |

`ArmSystemBuilder::createSystem()` selects the traits at runtime based on
`SystemModel` + `ProcessorModel` and instantiates the correct
`ArmSystem<Traits>`.

#### Key `.inl` Files

| File                            | Purpose                                              |
|---------------------------------|------------------------------------------------------|
| `Hardware.inl`                  | `BasicIrqManagerHardware` base; IRQ mask/status logic |
| `TestBedHardware.inl`           | Minimal hardware for tests (32 KB ROM + 32 KB RAM)   |
| `ARMv2CoreRegisterFile.inl`     | 26-bit register file, banked registers, mode switching|
| `ARMv2InstructionDecoder.inl`   | 3-bit major opcode dispatch (bits 27:25)             |
| `InstructionPipeline.inl`       | Fetch, condition check, decode/execute, PC advance   |
| `ExecutionUnit.inl`             | IRQ priority loop; cycle-limited or unlimited run    |
| `ArmSystem.inl`                 | Concrete `IArmSystem`; device connection, run methods|
| `AluInstructions.inl`           | ALU data processing operations                       |
| `DataTransferInstructions.inl`  | LDR/STR/LDM/STM implementation                      |
| `SystemConfigurations.inl`      | Traits struct definitions                            |

#### Hardware Components

**MemcHardware** (`MemcHardware.hpp/cpp`) — MEMC1/MEMC1a memory controller.

- 64 MB logical address space: RAM (0–32 MB, page-translated), physical RAM
  (32–48 MB), IOC (48–52 MB), VIDC write-only (52–54 MB), MEMC CAM registers
  (54–56 MB), address translation space (56–64 MB).
- 512 page mappings with configurable page sizes (4/8/16/32 KB) and 4-level
  page protection (PPL bits).
- Owns IOC, VIDC10, and AcornKeyboardController as sub-devices.
- Video DMA addresses: Vinit, Vstart, Vend (framebuffer), Cinit (cursor).

**IOC** (`IOC.hpp/cpp`) — VL86C410 I/O controller.

- Interrupt subsystem: `IocIrqState` with `std::atomic` fields for thread-safe
  IRQ A/B (16-bit) and FIRQ (8-bit) registers, plus 6 control pins (C0–C5 for
  I2C, etc.).
- Four timers: 3 general-purpose `Counter` objects + 1 `KartCounter` for the
  keyboard serial clock, all scheduled via `GuestTask`.
- KART serial: lock-free `SynchronisedByteQueue` Rx/Tx queues (Moody Camel)
  for cross-thread keyboard protocol communication.
- Cache-line alignment on all shared state
  (`std::hardware_destructive_interference_size`).

**VIDC10** (`VIDC10.hpp/cpp`) — VL86C310 video controller.

- Write-only MMIO; register selected by bits 31:26 of the written word.
- 16-colour palette (13-bit physical colour), border colour, 3 cursor colours,
  8-channel stereo positions.
- Full H/V timing registers; control register (pixel rate, BPP, interlace).
- Schedules VSync via `GuestTask` at frame rate derived from timing registers.

**AcornKeyboardController** (`AcornKeyboardController.cpp`) — KART protocol
state machine.

- Protocol: Idle → WaitingForRAK1 → WaitingForRAK2 → Initialised → command
  processing (RQID, LEDS, RQMP, RQPD).

**Display** (`Display.hpp/cpp`) — read-side frame renderer.

- Reads framebuffer from MemcHardware RAM using VIDC10 DMA addresses.
- Per-BPP scanline renderers (1, 2, 4, 8 BPP).
- Converts 13-bit VIDC physical colour to ARGB32 via palette lookup.

#### SystemContext and Scheduling

`SystemContext` tracks emulated time:

- `_masterClock` — 64-bit tick counter (e.g. 32 MHz for Archimedes).
- `_cpuClockShift` — CPU clock = master clock >> shift.
- `GuestTask` linked list sorted by scheduled tick — devices insert tasks
  (IOC timers, VIDC VSync); `incrementCPUClock()` dispatches when due.
- `_fuzz[64]` — random noise for unmapped DRAM reads.
- `postMessageToHost()` → `GuestEventQueue` (lock-free SPSC to host thread).

#### AddressMap and Device Wiring

- `AddressMap` — sorted vector with binary-search `tryFindRegion()`. Separate
  read and write maps.
- Device hierarchy: `IHardwareDevice` → `IAddressRegion` → `IHostBlock`
  (host RAM/ROM) or `IMMIOBlock` (register dispatch).
- `ConnectionContext` — built during `ArmSystem::initialise()` for devices to
  discover and wire up to each other.

### MightyOakLib — Application Layer

Namespace `Mo`. SDL3-based application framework:

- `EmulatorApp` — extends `Ag::App`; implements `initialise()`, `run()`,
  `shutdown()`.
- `CliOptions` — command-line parsing into `Arm::Options`.
- `EmulatorSession` — owns an `Arm::Options` and (future) running
  `IArmSystem`.
- `AppState` / `AppContext` — state machine pattern for the application
  lifecycle.
- `SessionRunningState` — concrete state for active emulation (currently a
  stub).

### ArmDebugger — Qt6 GUI Debugger (Optional)

Built only when Qt6.5+ is found. Provides:

- `DebuggerWindow` — main window with memory viewer, register display.
- `EmulatorSession` — debugger's own session wrapper around `IArmSystem`.
- `UI/` — `MemoryBlockView`, `DataBlockView`, `RegisterViewWidget`.
- `Annotations/` — source annotation system for labeled memory regions.
- `Breakpoint` — breakpoint management.

## Coding Standards

### Naming

- Namespaces: lowercase (`Mo`, `Ag`, `Mo::Asm`, `Mo::Arm`).
- Classes and enums: `PascalCase`.
- Functions and methods: `camelCase`.
- Member variables: `_prefixedCamelCase`.
- Constants/enum values: `PascalCase`.

### Documentation

Doxygen comments throughout: `//!`, `@file`, `@brief`, `@author`, `@date`,
`@copyright`.

### File Organisation

- Public headers in `Source/Include/{AsmTools,ArmEmu,MightyOakLib}/`.
- Umbrella headers: `AsmTools.hpp`, `ArmEmu.hpp`, `MightyOakLib.hpp`.
- Template implementations in `.inl` files (listed as sources in CMake for IDE
  visibility but included via `#include` in `.cpp` files).
- Test files follow the pattern `Test_*.cpp` using Google Test.
- Source files grouped by component within each library's `CMakeLists.txt`
  via `source_group()`.

### Performance Conventions

- No virtual functions in the emulation hot path — templates and traits
  composition instead.
- Cache-line alignment on cross-thread shared state.
- Lock-free SPSC queues (Moody Camel `ReaderWriterQueue`) for emulator-to-host
  communication.
- Optional x64 MASM optimisations for ALU operations
  (`AluOperations_Win32_x64.asm`) with C++ fallback
  (`AluOperations_NoArch.cpp`), controlled by the `USE_ASM` CMake option.
- `GuestTask` scheduled callbacks avoid polling — devices set up timed events
  that fire as the CPU clock advances.

## Build System

CMake 3.21+, C++17. External dependencies fetched automatically:

| Dependency         | Version | Purpose                              |
|--------------------|---------|--------------------------------------|
| Google Test        | latest  | Unit testing                         |
| readerwriterqueue  | 1.0.6   | Lock-free SPSC queue (Moody Camel)   |
| SDL3               | latest  | Display, input, audio                |
| Qt6 (optional)     | 6.5+    | ArmDebugger GUI                      |
| Doxygen (optional) | 1.8+    | API documentation                    |

Build-time ROM assembly: `AAsm` assembles test ROM sources
(`Tests/ArmEmu/*.arm`) into binaries during the build, which are then embedded
as C arrays via `ag_add_static_data()`.

Supported platforms: Visual Studio 2022 x64 (Windows), gcc 11 x64 (Linux).

## Testing Patterns

- Google Test with `GTEST_TEST` macro.
- Fixture classes (e.g. `MemcHardwareTests : public ::testing::Test`).
- Direct template instantiation in tests — tests create concrete
  `ArmSystem<Traits>` types bypassing `IArmSystem` for deeper access.
- **BKPT-based test checkpoints** — embedded test ROMs insert `BKPT #N`
  instructions; the test harness reads back the breakpoint number via
  `InstructionInfo::disassemble()` to verify which checkpoint was reached.
- Shared test utilities: `TestTools`, `TestExecTools`, `TestConstraints`,
  `LoggerDevice`.
- 21 AsmTools test files covering each pipeline stage; 27 ArmEmu test files
  covering CPU, hardware, and integration.
- `EmuPerfTest` benchmark target for performance measurement.

## Technical Debt

### Blocking Issues (Preventing Boot to Desktop)

1. **`SessionRunningState::runInternal()` is a stub.** No emulator thread or
   SDL event loop. The `EmulatorApp::run()` state machine has no functional
   running state.

2. **System ROM loading not implemented.** `ArmSystemBuilder::createSystem()`
   has empty `// TODO: Load system ROM preset` stubs for both TestBed and
   Archimedes/A-Series paths. Systems start with blank ROM.

3. **Five keyboard input methods are stubs.** `keyDown()`, `keyUp()`,
   `mouseButtonDown()`, `mouseButtonUp()`, `mouseDelta()` — no scancode
   encoding. `tryMapMouseButton()` returns 0 for all buttons.

4. **No SDL keycode to Acorn keyboard matrix mapping.**

5. **I2C bus and CMOS RAM not implemented.** RISC OS may stall at I2C probe
   without these.

6. **Cursor rendering not implemented.** `Display::renderFrame()` handles the
   framebuffer but not the hardware cursor. The `Cinit` MEMC register write
   handler is empty.

### Minor Debt

- Co-processor load/store (opcode 0x06) raises undefined instruction — FPA
  LDC/STC not implemented.
- Multi-pipeline mode switching (ARM/Thumb interworking) not implemented.
- `makeProductionModel()` may not validate all 24 production model
  configurations.
- CPU clock speed CLI option limited to MHz granularity (no Hz).
- MASM detection hack — only checks for x64 Windows, needs adaptation for
  other assembler types.
- MEMC reset uses a non-standard 4 MB page to simplify ROM-at-zero mapping.
- IOC KART counter accessed as `_counters[3]` relying on memory layout of
  consecutive `Counter` and `KartCounter` objects.
- FPA co-processor disassembly partially implemented.
- Commented-out dead code in `ArmSystem.inl` (old device connection approach).
- ArmDebugger: step-into / step-over not implemented; dark theme icons missing;
  ARMv3+ register display not handled.
- RiscPC `SystemModel` not supported — falls through to an error message.
