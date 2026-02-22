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

Concrete traits are defined in `SystemConfigurations.inl`. Each traits struct
is templated on `bool TAllowDiagnostics = false`, a compile-time gate that
controls whether `if constexpr` diagnostic branches in `SingleModeExecutionUnit`
are compiled in or optimised out entirely:

| Traits struct              | Hardware        | Register file    | Decoder               | Use case               |
|----------------------------|-----------------|------------------|-----------------------|------------------------|
| `ArmV2TestSystemTraits<>`  | TestBedHardware | ARMv2CoreRegFile | ARMv2InstructionDecoder | Unit tests            |
| `ArmV2aTestSystemTraits<>` | TestBedHardware | ARMv2aCoreRegFile| ARMv2aInstructionDecoder| ARMv2a unit tests     |
| `ArmV2aSTestSystemTraits<>`| TestBedHardware | ARMv2aCoreRegFile| ARMv2aInstructionDecoder(ARM250)| ARM250 unit tests |
| `ArmV2MemcSystemTraits<>`  | MemcHardware    | ARMv2CoreRegFile | ARMv2InstructionDecoder | Archimedes ARM2       |
| `ArmV2aMemcSystemTraits<>` | MemcHardware    | ARMv2aCoreRegFile| ARMv2aInstructionDecoder| A-Series / ARM3       |
| `ArmV2aSMemcSystemTraits<>`| MemcHardware    | ARMv2aCoreRegFile| ARMv2aInstructionDecoder(ARM250)| A3010/A3020 ARM250|

`ArmSystemBuilder::createSystem()` selects the traits at runtime based on
`SystemModel` + `ProcessorModel` and instantiates the correct
`ArmSystem<Traits>`. `ArmSystemBuilder::setDiagnosticsEnabled(true)` causes
the `<true>` template variant to be instantiated instead, enabling diagnostic
hooks. ROM loading is handled by `MemcHardware`'s constructor which reads the
ROM file path from `Options::getRomPath()`. `Options::findRomImagePath()`
walks up a directory hierarchy to locate a `ROMs/` folder relative to the
executable or a configured base path.

#### Key `.inl` Files

| File                            | Purpose                                              |
|---------------------------------|------------------------------------------------------|
| `Hardware.inl`                  | `BasicIrqManagerHardware` base; IRQ mask/status logic |
| `TestBedHardware.inl`           | Minimal hardware for tests (32 KB ROM + 32 KB RAM)   |
| `ARMv2CoreRegisterFile.inl`     | 26-bit register file, banked registers, mode switching|
| `ARMv2InstructionDecoder.inl`   | 3-bit major opcode dispatch (bits 27:25)             |
| `InstructionPipeline.inl`       | Fetch, condition check, decode/execute, PC advance; records last PC/opcode/executed for diagnostics |
| `ExecutionUnit.inl`             | IRQ priority loop; cycle-limited or unlimited run; `TAllowDiagnostics` template parameter gates `if constexpr` diagnostic branches; `connect()` discovers `IDiagnosticSink` via device system |
| `ArmSystem.inl`                 | Concrete `IArmSystem`; `initialise()` calls `connectAllDevices()` then `_hardware.connect()` and `_execUnit.connect()` for post-init wiring; run methods |
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
- Owns IOC, VIDC10, AcornKeyboardController, I2CBus, and PCF8583 as sub-devices.
  `addIntegralHardware()` pushes IOC, VIDC10, and keyboard into the device
  collection so they participate in the `connectAllDevices()` lifecycle.
- Loads system ROM from file into high ROM (0x3800000) at construction;
  reset maps high ROM to logical address 0 via a 4 MB MEMC page.
- MEMC DMA register writes (Vinit, Vstart, Vend, Cinit) are forwarded to
  `VIDC10` via setter methods; MemcHardware no longer stores these addresses.

**IOC** (`IOC.hpp/cpp`) — VL86C410 I/O controller.

- Interrupt subsystem: `IocIrqState` with `std::atomic` fields for thread-safe
  IRQ A/B (16-bit) and FIRQ (8-bit) registers, plus 6 control pins (C0–C5).
- I2C integration: control register writes on C0 (SDA) and C1 (SCL) are
  forwarded to the `I2CBus` via `setI2CBus()`; SDA input state is fed back
  to the control pin input register.
- Four timers: 3 general-purpose `Counter` objects + 1 `KartCounter` for the
  keyboard serial clock, all scheduled via `GuestTask`.
- KART serial: lock-free `SynchronisedByteQueue` Rx/Tx queues (Moody Camel)
  for cross-thread keyboard protocol communication.
- Cache-line alignment on all shared state
  (`std::hardware_destructive_interference_size`).
- Optional `IDiagnosticSink` for MMIO read/write logging and interrupt state
  change notifications.

**VIDC10** (`VIDC10.hpp/cpp`) — VL86C310 video controller. Implements
`IVideoFrameProvider`.

- Write-only MMIO; register selected by bits 31:26 of the written word.
- 16-colour palette (13-bit physical colour), border colour, 3 cursor colours,
  8-channel stereo positions.
- Full H/V timing registers; control register (pixel rate, BPP, interlace).
- Schedules VSync via `GuestTask` at frame rate derived from timing registers.
- Owns DMA address fields (Vinit, Vstart, Vend, Cinit) received from MEMC
  register writes; `reset()` clears them and cancels any scheduled VSync.
- `getRawFrame()` copies raw indexed framebuffer bytes from physical RAM
  (handling DMA address wrapping), builds a 256-entry ARGB8888 palette
  (with 8-BPP green-channel override expansion), and returns `RawFrameInfo`.
- Registers under device alias `"DISPLAY"` for host-side lookup.

**AcornKeyboardController** (`AcornKeyboardController.cpp`) — KART protocol
state machine.

- Protocol: Idle → WaitingForRAK1 → WaitingForRAK2 → Initialised → command
  processing (RQID, LEDS, RQMP, RQPD).

**I2CBus** (`I2CBus.hpp/cpp`) — bit-banged I2C bus controller.

- Monitors SDA/SCL line transitions driven by IOC control register writes.
- Detects START/STOP conditions, clocks in address and data bytes.
- Dispatches to registered `II2CDevice` implementations (up to 4 devices).
- Drives SDA for ACK and read-data phases.

**PCF8583** (`PCF8583.hpp/cpp`) — PCF8583 RTC and CMOS RAM emulation.

- I2C device at address 0x50 (7-bit), implementing the `II2CDevice` interface.
- 256-byte register space: 0x00–0x0F control/clock, 0x10–0xFF general RAM.
- RISC OS CMOS settings area at offset 0x40 with sensible defaults
  (Mode 12, 640x256 16-colour desktop configuration).
- Auto-incrementing register pointer for sequential read/write access.

**IVideoFrameProvider** (`IVideoFrameProvider.hpp`) — public interface for
host-side frame capture, derived from `IMMIOBlock`.

- `getRawFrame()` returns raw indexed framebuffer bytes, a 256-entry ARGB8888
  palette, and a `RawFrameInfo` struct (Width, Height, BytesPerRow,
  BitsPerPixel, BorderColour). Designed for future GPU shader-based palette
  lookup rendering.

**Display** (`Display.hpp/cpp`) — read-side frame renderer.

- Reads framebuffer from MemcHardware RAM using VIDC10 DMA addresses
  (sourced from `VIDC10` rather than `MemcHardware`).
- Per-BPP scanline renderers (1, 2, 4, 8 BPP).
- Converts 13-bit VIDC physical colour to ARGB32 via palette lookup.

#### Diagnostic System

An observer-based diagnostic system provides instruction tracing, interrupt
logging, and MMIO access visibility for boot debugging and system analysis.

**IDiagnosticSink** (`IDiagnosticSink.hpp`) — abstract interface deriving from
`IHardwareDevice` with four event callbacks: `onInstruction()`,
`onMemoryAccess()`, `onInterruptChange()`, `onModeChange()`.

Hookup: Diagnostic sinks participate in the standard device lifecycle via
`ArmSystemBuilder::addDevice()`. The `IDiagnosticSink::registerDevice()`
override registers the sink under its own name and adds a common alias
`"DiagnosticSink"` so any concrete sink is discoverable under a single
well-known name. After `connectAllDevices()`, `ArmSystem::initialise()` calls
`_execUnit.connect()` which uses
`SystemContext::tryFindTypedDevice<IDiagnosticSink>("DiagnosticSink", ...)`
to discover and store the sink pointer. No diagnostic-specific APIs exist on
`IArmSystem` or `SystemContext` — the sink is purely discovered through the
device system.

Compile-time gating: All traits structs are templated on
`bool TAllowDiagnostics`, which is forwarded to `SingleModeExecutionUnit` as
a template parameter. Diagnostic hooks in `ExecutionUnit.inl` are wrapped in
`if constexpr (AllowDiagnostics)` so that when `TAllowDiagnostics=false`
(the default), the diagnostic branches are optimised out entirely — true
zero overhead. `ArmSystemBuilder::setDiagnosticsEnabled(true)` selects the
`<true>` template variant at system creation time.

Event sources:
- **Instruction trace** — `ExecutionUnit.inl` calls `onInstruction()` after
  each `_pipeline.executeNext()`, populating `InstructionTraceEntry` (PC,
  opcode, PSR, cycles, executed flag). `InstructionPipeline.inl` records
  `_lastPC`, `_lastInstruction`, `_lastWasExecuted` as part of its normal
  fetch/decode flow.
- **Interrupt events** — `ExecutionUnit.inl` calls `onInterruptChange()` after
  `handleFirq()` / `handleIrq()`. IOC calls `onInterruptChange()` from
  `raiseVSyncIrq()`, `setInterruptLow()`, and `setFastHighInterrupt()`.
- **MMIO access** — IOC `read()` and `write()` call `onMemoryAccess()` with
  the address, value, and direction.

Concrete implementations (public headers in `Source/Include/ArmEmu/`,
implementations in `Source/ArmEmu/`):
- **RingBufferTrace** — fixed-size circular buffers (default 4096 instructions,
  256 interrupts, 1024 MMIO entries) with `dumpToStream()` for text output.
- **BootProgressMonitor** — detects boot milestones by observing MMIO address
  patterns (MEMC 0x3600000, IOC 0x3200000, VIDC 0x3400000, I2C toggling on
  IOC control register). Tracks per-category access counts and a timestamped
  milestone list. `printSummary()` for text output.
- **BootStallDetector** — monitors the instruction stream for tight PC loops
  (the primary symptom of a boot stall). Maintains a circular buffer of recent
  PC values and periodically checks for low PC diversity. Records `StallInfo`
  entries with loop PCs and detection timestamps. Tracks high-water-mark PC
  (furthest into ROM the boot reached). `printReport()` outputs stall info
  with disassembled instructions via `Asm::InstructionInfo`.
- **CompositeDiagnosticSink** — header-only forwarder to multiple child sinks.

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
  (host RAM/ROM) or `IMMIOBlock` (register dispatch). `IDiagnosticSink` also
  derives from `IHardwareDevice`, allowing diagnostic sinks to participate in
  the device lifecycle via `ArmSystemBuilder::addDevice()` and be discovered
  at runtime via `tryFindTypedDevice<IDiagnosticSink>("DiagnosticSink")`.
- Two-phase device startup managed by `SystemContext::connectAllDevices()`:
  1. **Phase 1 (`registerDevice`)**: each device registers itself (and any
     aliases) into `SystemContext`'s name-indexed `HardwareMap`.
  2. **Phase 2 (`connect`)**: each device looks up other devices by name via
     `SystemContext::tryFindDevice()` / `tryFindTypedDevice<T>()`.
- The former `ConnectionContext` intermediary has been removed; `SystemContext`
  now owns the persistent device registry, which remains accessible at runtime
  via `IArmSystem::tryFindDevice()` and `tryFindTypedDevice<T>()`.

### MightyOakLib — Application Layer

Namespace `Mo`. SDL3-based application framework:

- `EmulatorApp` — extends `Ag::App`; implements `initialise()`, `run()`,
  `shutdown()`.
- `CliOptions` — command-line parsing into `Arm::Options`.
- `EmulatorSession` — owns an `Arm::Options` and running `IArmSystem`.
- `AppState` / `AppContext` — state machine pattern for the application
  lifecycle. `AppContext::initialise()` creates an `EmulatorSession` and
  calls `createSystem()` to instantiate the emulated hardware.
- `EmulatorApp::initialiseRuntimeGlobals()` — locates the ROM directory
  via `Options::findRomImagePath()` at startup.
- `SessionRunningState` — concrete state for active emulation. Creates an SDL3
  renderer, launches the emulator on a background thread, and runs a
  `PeriodicEventProcessor` on the main thread synchronised to display refresh.
  Each frame calls `IVideoFrameProvider::getRawFrame()` to capture the indexed
  framebuffer and palette, expands to ARGB32 on the CPU (1/2/4/8 BPP), and
  uploads via `SDL_UpdateTexture()`. Looks up `IKeyboardController` and
  `IVideoFrameProvider` through `IArmSystem::tryFindTypedDevice()` by device
  name. Forwards SDL key and mouse events to `IKeyboardController`.

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

System ROM deployment: real RISC OS ROM images are stored in
`Source/MightyOakLib/Roms/` and copied to `<output_dir>/ROMs/` as a post-build
step on both `MightyOak` (EmulatorApp) and `ArmEmu_Tests` targets.

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
- **Headless RISC OS boot tests** (`Test_RiscOSBoot.cpp`) — loads a real
  RISC OS 3.10 ROM via `ArmSystemBuilder`, runs millions of cycles, and
  verifies the PC advances past reset, I2C probe, and hardware init.
  ROM files are copied to the test output directory at build time.
- **Diagnostic system tests** (`Test_Diagnostics.cpp`) — tests covering
  `RingBufferTrace` (capture, wrap, clear, dump), `BootProgressMonitor`
  (milestone detection for MEMC/IOC/VIDC/I2C), `CompositeDiagnosticSink`
  (forwarding), null-sink safety (using `<false>` template variant),
  live instruction trace integration (using `<true>` variant with
  `tryFindTypedDevice`), `IHardwareDevice` name/description verification,
  and `ArmSystemBuilder` integration (`setDiagnosticsEnabled(true)`,
  auto-connect via `addDevice()`, device name discovery).
- **Diagnostic boot trace** (`Test_MemcRomBoot.cpp::DiagnosticBootTrace`) —
  attaches a `CompositeDiagnosticSink` (RingBufferTrace + BootProgressMonitor)
  to the test ROM boot, verifies trace capture, and outputs boot progress.
- **Diagnostic boot investigation** (`Test_RiscOSBootDiag.cpp`) — 6 tests
  using `BootStallDetector`, `BootProgressMonitor`, and `RingBufferTrace` to
  investigate where RISC OS 3.10 boot stalls: stall detection with
  disassembly, boot progress with trace, exception vector integrity, MEMC
  page table dump, I2C/IOC activity analysis, and PC progression timeline.
- 21 AsmTools test files covering each pipeline stage; 29 ArmEmu test files
  covering CPU, hardware, diagnostics, and integration (including ARM250
  variant tests for ALU, co-processor, and data transfer).
- `EmuPerfTest` benchmark target for performance measurement.

## Technical Debt

### Blocking Issues (Preventing Boot to Desktop)

1. ~~**`SessionRunningState::runInternal()` is a stub.**~~ **RESOLVED.**
   `SessionRunningState` now overrides `configure()` and `runInternal()`. It
   creates an SDL renderer, launches the emulator on a background thread, and
   runs a `PeriodicEventProcessor` on the main thread synchronised to the
   display refresh rate. A periodic callback calls `IArmSystem::getRawFrame()`
   to obtain raw indexed framebuffer bytes and a 256-entry ARGB32 palette,
   expands them to ARGB32 on the CPU, and uploads via `SDL_UpdateTexture()`.
   The `getRawFrame()` interface is designed for future GPU shader-based
   palette lookup (upload R8 framebuffer + RGBA palette texture, sample in
   fragment shader) once the Ag SDL3Tools GPU wrapper supports texture upload,
   fragment sampler binding, and uniform push.

2. ~~**Five keyboard input methods are stubs.**~~ **RESOLVED.** Keyboard and
   mouse input is forwarded from SDL events to the `IKeyboardController`
   via `IArmSystem::getKeyboardController()`.

3. **No SDL keycode to Acorn keyboard matrix mapping.** Raw SDL scancodes are
   forwarded but no host-to-Acorn translation table has been applied yet.
   `IKeyboardController::setKeyMapping()` exists but no mapping data is
   provided.

4. **Cursor rendering not implemented.** `Display::renderFrame()` handles the
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
- ~~Commented-out dead code in `ArmSystem.inl` (old device connection approach).~~ **RESOLVED.** `ConnectionContext` removed; device lifecycle consolidated into `SystemContext::connectAllDevices()`.
- ArmDebugger: step-into / step-over not implemented; dark theme icons missing;
  ARMv3+ register display not handled.
- RiscPC `SystemModel` not supported — falls through to an error message.
