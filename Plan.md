# Archimedes A3000 Emulation Completion Roadmap

## Context

MightyOak has a solid foundation: full ARMv2/ARMv2a CPU core, MEMC page translation, IOC interrupts/timers, VIDC10 video registers, and display pixel rendering for all BPP modes. However, the system cannot yet boot RISC OS to desktop because several critical integration pieces are missing: the emulator run loop, SDL display pipeline, keyboard/mouse event delivery, and I2C/CMOS RAM. This roadmap targets **A3000 (ARM2, 8MHz, 1MB RAM, RISC OS 3.10)** booting to an interactive desktop.

---

## Phase 0: MEMC Reset State Verification and ROM Boot Sanity (1-2 weeks)

**Goal:** Confirm that ARM2+MemcHardware can execute RISC OS 3.10 from reset through POST without crashing.

**Tasks:**
1. **Create a headless boot test harness** (e.g. `Test_RomBoot.cpp` or standalone tool):
   - Create `Options` with `ProductionModel::A3000` preset
   - Build system via `ArmSystemBuilder`
   - Run on background thread with timeout, drain `GuestEventQueue` for VSync events
   - Dump final register state (PC, mode, last exception vector hit)

2. **Verify MEMC reset page mapping** — the current 4MB page-size hack in `MemcHardware::reset()` maps low ROM to logical address 0. Confirm:
   - First instruction fetched at address 0 is the first word of `RiscOS_3_10.rom`
   - After RISC OS reprograms MEMC to normal page sizes, mappings work correctly

3. **Verify interrupt timing** — VSync IRQ, IOC Timer 0/1, and KART timer must fire:
   - VSync at ~50 Hz (PAL) after RISC OS programs VIDC timing
   - Timer 0 as centisecond clock (2 MHz IOC clock rate)
   - KART counter for keyboard serial handshake

4. **Store Cinit register** — `MemcHardware.cpp:173` (`case 3: // Cinit`) is currently empty. Store the value even if cursor rendering is not yet done.

5. **Debug and fix boot-blocking issues** — likely: page mapping edge cases during RAM size detection, data abort handling for RAM probing

**Milestone:** Headless test shows VSync events at ~50 Hz, PC not stuck at exception vector, runs 10+ seconds emulated time without crashing.

**Key files:**
- `Source/ArmEmu/MemcHardware.cpp` — reset(), page mapping, Cinit stub (line 173)
- `Source/ArmEmu/VIDC10.cpp` — VSync scheduling
- `Source/ArmEmu/IOC.cpp` — timers, KART
- `Source/ArmEmu/ArmSystemBuilder.cpp` — system construction (line 200)

---

## Phase 1: SDL Display Pipeline (2-3 weeks)

**Goal:** Display emulated framebuffer in an SDL window with the emulator running on a background thread.

**Architecture:** Two threads:
- **Emulator thread:** Runs `IArmSystem::run()` in a loop, posts VSync events via `GuestEventQueue`
- **SDL thread:** Event loop, calls `Display::renderFrame()` → uploads ARGB32 to SDL GPU texture → presents

**Tasks:**
1. **Implement `EmulatorApp::run()`** (currently TODO at `EmulatorApp.cpp:89`):
   - Create `IArmSystem` from the `Options` via `ArmSystemBuilder`
   - Start emulator on background thread

2. **Create an EmulatorSession class** (`Source/MightyOakLib/EmulatorSession.cpp`):
   - Owns `IArmSystemUPtr`, emulator `std::thread`, `Display`, pixel buffer
   - Provides `start()`, `stop()`, `renderFrame()` methods
   - Emulator loop: repeatedly calls `run()`, checks for `HostIrq` to stop

3. **Create SDL display presentation**:
   - `Ag::SDL3::Window` at initial RISC OS mode resolution
   - `Ag::SDL3::GPU::Device`, `Swapchain`, `Texture` (ARGB8888)
   - On VSync or periodic tick: `Display::renderFrame()` → transfer buffer → GPU copy → present
   - Use `Ag::SDL3::PeriodicEventProcessor` for frame-locked operation

4. **Handle display mode changes** — detect when `FrameInfo.Width/Height` changes, recreate texture

**Milestone:** SDL window shows RISC OS boot sequence: splash screen, memory test bar, and (eventually) desktop background.

**Key files:**
- `Source/MightyOakLib/EmulatorApp.cpp` — run() (line 89)
- `Source/ArmEmu/Display.cpp` — renderFrame() (already works)
- `Source/Ag/Source/Include/Ag/Sdl3Tools/GPUObjects.hpp` — GPU rendering wrappers
- `Source/Ag/Source/Include/Ag/Sdl3Tools/EventProcessor.hpp` — PeriodicEventProcessor

---

## Phase 2: KART Keyboard Protocol and Key Input (2-3 weeks)

**Goal:** Complete keyboard controller so RISC OS receives key events and KART handshake succeeds.

**Critical note:** RISC OS will not boot to desktop without a successful KART handshake. There is a **confirmed bug** in the current state machine.

**Tasks:**
1. **Fix KART protocol bug** (`AcornKeyboardController.cpp:117`):
   - After receiving RAK2, state transitions to `ReceivedRAK1` instead of `Initialised` — this causes an infinite handshake loop

2. **Extend KART state machine** for post-init commands:
   - `RQID` (0x20): respond with keyboard ID (A3000 = 1)
   - `LEDS` (0x00-0x07): acknowledge LED state with SACK/NACK
   - `RQMP` (0x22): respond with mouse delta bytes
   - `RQPD` (0x40-0x4F): respond with key state

3. **Implement 5 TODO methods** in `AcornKeyboardController.cpp`:
   - `keyDown()` (line 165): encode as `KDDA | row`, `KDDA | col` → `writeKartByte()`
   - `keyUp()` (line 176): encode as `KUDA | row`, `KUDA | col`
   - `mouseButtonDown()` (line 187): Left=0x70, Middle=0x71, Right=0x72
   - `mouseButtonUp()` (line 198): same scancodes with KUDA prefix
   - `mouseDelta()` (line 209): signed X/Y delta bytes

4. **Fix `tryMapMouseButton()`** (line 41): populate actual Acorn scancodes (currently returns 0 for all)

5. **Create SDL→Acorn scancode mapping table** (new file or in session setup):
   - Map `SDL_Scancode` to Acorn keyboard matrix (row, column) pairs
   - Call `setKeyMapping()` during initialization

6. **Wire SDL events to keyboard controller** in EmulatorSession:
   - `SDL_EVENT_KEY_DOWN/UP` → `keyDown()/keyUp()`
   - Thread-safe: `IOC::writeKartByte()` already uses lock-free queue

**Milestone:** RISC OS completes keyboard init, typing produces characters on the desktop.

**Key files:**
- `Source/ArmEmu/AcornKeyboardController.cpp` — KART state machine, 5 TODOs (lines 85-212)
- `Source/ArmEmu/AcornKeyboardController.hpp` — protocol constants, state enum
- `Source/Include/ArmEmu/IKeyboardController.hpp` — interface
- `Source/ArmEmu/IOC.cpp` — KART counter, Rx/Tx queues

---

## Phase 3: I2C Bus and CMOS RAM (1-2 weeks)

**Goal:** Provide 256 bytes of battery-backed CMOS RAM via I2C so RISC OS reads configuration at boot.

**Why needed:** RISC OS reads CMOS for screen mode (byte 0x10), country (0x00), boot options (0x05), and more. Without it, boot may stall at I2C probe or use fallback defaults.

**Architecture:** I2C is bit-banged through IOC control lines C0 (SDA) and C1 (SCL). The PCF8583 RTC/CMOS chip is at I2C address 0x50.

**Tasks:**
1. **Create I2C bus controller** (`Source/ArmEmu/I2CBus.cpp/.hpp`):
   - Monitor SCL/SDA transitions on IOC control pin writes
   - Detect START (SDA falls while SCL high) and STOP (SDA rises while SCL high)
   - Clock in 8 data bits + ACK/NACK on 9th clock
   - State machine: Idle → AddressByte → ReadData / WriteData

2. **Create CMOS RAM device** (`Source/ArmEmu/CmosRam.cpp/.hpp`):
   - I2C slave at address 0x50 (7-bit)
   - 256 bytes storage with sequential read/write
   - Sensible defaults: MODE 12 (0x0C at byte 0x10), UK (0x01 at byte 0x00)
   - Optional file persistence for CMOS state across sessions

3. **Connect I2C to IOC control pins**:
   - Hook IOC control register writes to feed SCL/SDA to I2C bus
   - I2C bus drives SDA input back via `setControlPinInputState()`

**Milestone:** RISC OS reads CMOS at boot, boots into configured screen mode.

**Key files:**
- `Source/ArmEmu/IOC.cpp` — control register read/write, pin state
- `Source/Include/ArmEmu/IOC.hpp` — IOC public interface
- New: `Source/ArmEmu/I2CBus.cpp/.hpp`
- New: `Source/ArmEmu/CmosRam.cpp/.hpp`

---

## Phase 4: Mouse Input and Cursor Rendering (2 weeks)

**Goal:** Render hardware cursor sprite and deliver mouse movement for a visible, responsive pointer.

**Tasks:**
1. **Store Cinit in MemcHardware** (if not done in Phase 0):
   - `writeMEMC()` case 3 → `_cursorInitAddr` field + accessor

2. **Implement cursor rendering in `Display`**:
   - Read cursor position from VIDC10: HCSR (X), VCSR (Y start), VCER (Y end)
   - Read cursor pixels from RAM at Cinit: 32px wide, 2bpp (8 bytes/line)
   - 2bpp values: 0=transparent, 1-3=cursor colours from VIDC10
   - Composite over main framebuffer in `renderFrame()` or separate pass

3. **Complete `mouseDelta()` in keyboard controller**:
   - Format KART mouse data packets: `MDAT | (deltaX & mask)`, `MDAT | (deltaY & mask)`
   - Wire `SDL_EVENT_MOUSE_MOTION` → `mouseDelta(xrel, yrel)`

4. **Relative mouse capture**:
   - `SDL_SetWindowRelativeMouseMode()` to capture mouse
   - Key binding (e.g. Ctrl+Alt or F12) to release

**Milestone:** Desktop shows working mouse pointer that responds to movement. Clicking icons works.

**Key files:**
- `Source/ArmEmu/Display.cpp` — cursor rendering addition
- `Source/ArmEmu/MemcHardware.cpp` — Cinit register (line 173)
- `Source/ArmEmu/VIDC10.cpp` — cursor colour/position registers (already stored)
- `Source/ArmEmu/AcornKeyboardController.cpp` — mouseDelta() (line 209)

---

## Phase 5: Integration, Stabilization, and Polish (2-4 weeks)

**Goal:** Stable 30+ minute boot-to-desktop with all pieces working together.

**Tasks:**
1. **Timing calibration**: IOC Timer 0 at correct 2 MHz rate, VSync period matches PAL (20ms)
2. **Verify SWI handling**: RISC OS SWI vector (0x08) dispatch, link register in 26-bit mode
3. **Data abort handling**: RISC OS probes RAM via intentional data aborts — verify abort vector (0x10) works in 26-bit mode
4. **Address exception**: ARMv2 accesses above 0x3FFFFFF trigger address exceptions
5. **Display polish**: border colour rendering, interlace mode handling
6. **Emulator lifecycle**: clean shutdown on window close, reset via Ctrl+Break, real-time speed throttling (pace to VSync)
7. **Unit tests**: I2C bit-bang sequences, KART protocol state machine, cursor rendering, MEMC page mapping edge cases

**Milestone:** RISC OS 3.10 boots to working desktop with cursor, keyboard, mouse, correct colours, stable for 30+ minutes.

---

## Phase Dependency Graph

```
Phase 0: MEMC Reset + Boot Verification
    |
    +------> Phase 1: SDL Display Pipeline
    |
    +------> Phase 2: KART Keyboard Protocol    (parallel with Phase 1)
    |
    +------> Phase 3: I2C Bus + CMOS RAM         (parallel with Phases 1-2)
                 |
                 v
             Phase 4: Mouse + Cursor Rendering    (needs Phases 1, 2, 3)
                 |
                 v
             Phase 5: Integration + Polish         (needs all previous)
```

Phases 1, 2, and 3 can proceed in parallel after Phase 0.

---

## Risks

| Risk | Impact | Mitigation |
|------|--------|------------|
| MEMC reset page mapping is non-standard (4MB hack) | Boot fails at POST | Disassemble RISC OS POST to understand expected state |
| KART handshake bug (confirmed: line 117) | Desktop never reached | Fix early in Phase 2 — critical blocker |
| I2C timing sensitivity | CMOS reads misinterpreted | Update I2C output on control register *reads*, not writes |
| Frame rendering thread safety | Visual artifacts | Render during VSync blanking interval (when event is posted) |
| RISC OS expects hardware behaviour not yet documented | Random crashes | Use other emulators (Arculator, ArcEm) as reference for edge cases |

---

## Not In Scope (for this milestone)

- FPA floating point coprocessor
- Floppy disc controller (WD1772) / disc images (ADF)
- Hard disc controller (ST506/IDE)
- Sound output / audio DMA
- Podule/expansion bus
- Serial port (RS-232)
- IOEB / 82c710 (A5000-class hardware)
- Joystick support
- ARM3 cache simulation
- Thumb instruction set
