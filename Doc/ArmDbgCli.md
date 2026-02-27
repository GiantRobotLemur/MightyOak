# ArmDbgCli - Scriptable ARM Emulator Debugger

ArmDbgCli is a command-line tool for debugging ARM emulator sessions via
script files. It provides a non-interactive, repeatable way to configure an
emulated system, execute code, and inspect state without modifying the
emulator source or rebuilding. The architecture is split into a library
(`ArmDbg`) containing the core logic and a thin CLI wrapper (`ArmDbgCli`).

# Running ArmDbgCli

```
ArmDbg [options] [script-file]

Options:
  -s, --script <path>   Path to debugger script file
  -r, --rom <path>      Override ROM search path
  -?, --help            Display help
```

The script path can be given as a positional argument or via `--script`. If
`--rom` is not specified, the tool searches default locations relative to its
own directory (e.g. `ROMs/`, `EmulatorApp/Debug/ROMs/`).

**Examples:**
```
ArmDbg boot_debug.dbg
ArmDbg --script boot_debug.dbg --rom /path/to/roms
```

# Script Syntax

Scripts are plain text files, processed line by line.

- Lines beginning with `#` (after optional leading whitespace) are comments.
- Blank lines and whitespace-only lines are ignored.
- Tokens are separated by spaces or tabs.
- An inline `#` terminates the line (acts as a trailing comment).
- Command names are **case-insensitive**.
- Addresses and numeric arguments accept both decimal (`4096`) and hexadecimal
  (`0x1000`) notation.

The `echo` command is special-cased: everything after `echo ` is treated as a
single argument, preserving internal spacing.

If any command fails, the script continues executing subsequent lines. The
overall exit status reflects whether all commands succeeded.

# Commands

## System Configuration

These commands set up the emulated hardware. They do not require `init` and
must be issued before it.

### config model \<name\>

Set the hardware model.

| Name | Effect |
|------|--------|
| `Archimedies`, `A3xx` | Archimedes (also sets CPU to ARM2) |
| `A540` | Archimedes with ARM3 |
| `RiscPC` | Risc PC |

Default: `Archimedies`.

### config cpu \<name\>

Set the processor variant.

| Name | Processor |
|------|-----------|
| `ARM2` | ARMv2 |
| `ARM3` | ARMv2a (ARM3) |
| `ARM6`, `ARM610` | ARMv3 |
| `ARM7`, `ARM710` | ARMv3 |
| `ARM810` | ARMv4 |

Default: `ARM2`.

### config rom \<preset\|path\>

Set the system ROM. The argument is first matched against known presets; if
no match is found it is treated as a file path.

| Preset |
|--------|
| `RiscOS_3_10` |
| `RiscOS_3_11` |
| `RiscOS_3_00` |
| `RiscOS_2_00` |
| `Arthur_1_20` |

Default: `RiscOS_3_10`.

### config ram \<sizeKb\>

Set RAM size in kilobytes.

Default: `4096` (4 MB).

## System Initialisation

### init

Create the emulated system using the current configuration. This must be
called before any execution or inspection commands. The system is created
with diagnostics enabled and a ring buffer trace (4096 instruction entries).

Output: `System initialised.`

## Execution Control

All execution commands require `init`.

### run \<cycles\>

Run for the specified number of CPU cycles.

Output: `Executed N instructions in M cycles.`

### step \[count\]

Single-step one or more instructions. Default count is 1.

Output: `PC = 0xXXXXXXXX` after each step sequence.

### continue

Run until a stop condition is reached.

Output: `Stopped after N instructions, M cycles.`

### break \<addr\>

Set a breakpoint at the given address.

Output: `Breakpoint set at 0xXXXXXXXX.`

## State Inspection

All inspection commands require `init`.

### regs

Dump all 16 general-purpose registers (R0-R15), CPSR value, processor mode,
and condition flags.

```
=== Registers ===
  R0  = 0x00000000  R1  = 0xffffffff  R2  = 0x03e005f4  R3  = 0x40000000
  R4  = 0x90000000  R5  = 0x001ffffd  R6  = 0x80000000  R7  = 0x00000000
  R8  = 0x00000000  R9  = 0x00000000  R10 = 0x00000000  R11 = 0x00000000
  R12 = 0x00000000  SP  = 0x00000000  LR  = 0x8f80064f  PC  = 0x0f80065f
  CPSR = 0x0c000003  Mode: SVC26  [----IF]
```

### reg \<name\>

Read a single register. Accepts `r0`-`r15`, `pc`, `lr`, `sp`, `cpsr`
(case-insensitive).

Output: `<name> = 0xXXXXXXXX (decimal)`

### mem \<addr\> \<count\> \[p\|l\]

Hex dump of memory. `count` is in bytes. The optional third argument selects
physical (`p`, `phys`, `physical`) or logical (default) addressing.

```
  0x03800000: e3 a0 00 00 e1 a0 f0 0e  00 00 00 90 e0 90 e0 0f  |................|
  0x03800010: 00 00 00 90 e0 90 f0 0f  ea 00 00 6a e5 9f 30 00  |...........j..0.|
```

### disasm \<addr\> \<count\> \[p\|l\]

Disassemble `count` instructions starting at `addr`. Addressing mode as per
`mem`.

```
    0x03800000: e3a00000  MOV R0, #0
    0x03800004: e1a0f00e  MOV R15, R14
```

### pc

Show the current PC with a window of surrounding disassembled instructions
(8 before, 8 after the current instruction). The current instruction is
marked with `>>>`.

```
=== PC Context ===
  PC = 0x0380065c

    0x03800658: e0933003  ADDS R3, R3, R3
>>> 0x0380065c: 3afffffd  BCC $+0x658
    0x03800660: e59f5000  LDR R5, [R15]
```

### irq

Display IOC interrupt status and masks for IRQ A, IRQ B, and FIRQ. Active
and enabled bits are shown individually with their symbolic names.

```
=== IRQ State ===
  IRQ Status A: 0x90  Mask A: 0x00
    [4] POnRes   Status=SET Mask=dis
    [7] FhSet    Status=SET Mask=dis
  IRQ Status B: 0x00  Mask B: 0x00
  FIRQ Status: 0x80  FIRQ Mask: 0x00
```

IRQ A bit names: POR, IFlBnk, RII, TFlight, POnRes, Timer0, Timer1, FhSet.
IRQ B bit names: PodFIQ, SndBuf, Serial, WiniIRQ, DiscCh, PodIRQ, KartTx, KartRx.

### memc

Dump the MEMC logical-to-physical page table. Scans the 32 MB logical
address space at 32 KB granularity and reports all mapped pages.

```
=== MEMC Page Table ===
  Logical Addr  -> Physical Addr  PPL  Present
  ------------     -------------  ---  -------
  0x00000000  -> 0x02030000   25  Yes
  0x00008000  -> 0x02038000   25  Yes
  Total mapped pages: 2 (at 32KB granularity)
```

### ioc

Display IOC state: control register value and timer 0-3 latch values.

```
=== IOC State ===
  Control Register: 0xXX
  Timer 0 Latch: 0xXXXX
  Timer 1 Latch: 0xXXXX
  Timer 2 Latch: 0xXXXX
  Timer 3 Latch: 0xXXXX
```

### trace \[count\]

Display the last `count` entries from the instruction trace ring buffer.
Default count is 20.

```
=== Trace (last 5 of 4096 instructions) ===
  [   0] 0x038021cc: e081100a  ADD R1, R1, R10
  [   1] 0x038021d0: e081100b  ADD R1, R1, R11
  [   2] 0x038021d4: e1500002  CMP R0, R2
  [   3] 0x038021d8: 3afffff4  BCC $+0x21B0
  [   4] 0x038021b0: e8b00ff0  LDMIA R0!, {R4-R11}
```

## Output

### echo \[message\]

Print a message to stdout. Useful for annotating script output. If no
message is given, prints a blank line. Does not require `init`.

# Example Script

```sh
# boot_debug.dbg - Inspect early RISC OS boot state
config model Archimedies
config cpu ARM2
config rom RiscOS_3_10
config ram 4096
init

echo === Running 150M cycles (past I2C init) ===
run 150000000

regs
pc
irq

echo === MEMC page table ===
memc

echo === Last 30 instructions ===
trace 30

echo === ROM entry point disassembly ===
disasm 0x03800000 16 phys

echo === First 64 bytes of RAM ===
mem 0x00000000 64
```

# Build Targets

| Target | Type | Description |
|--------|------|-------------|
| `ArmDbg` | Static library | Core debugger logic (parsing, session, formatting) |
| `ArmDbg_Tests` | Test executable | 47 unit tests (GTest) |
| `ArmDbgCli` | CLI executable | Thin wrapper, outputs to stdout |

```
cmake --build MightyOakBuild --config Debug --target ArmDbgCli
cmake --build MightyOakBuild --config Debug --target ArmDbg_Tests
```

# Source Layout

```
Source/
  ArmDbg/
    CMakeLists.txt
    ScriptLexer.hpp / .cpp         # Tokenize script lines
    CommandDefs.cpp                 # Command name mappings
    CommandParser.hpp / .cpp       # Tokens -> ParsedCommand
    StateFormatter.hpp / .cpp      # Format registers/memory/disasm/IRQ
    DebugSession.cpp               # System lifecycle and command dispatch
    ScriptRunner.cpp               # Line-by-line script execution
    Test/
      Test_Main.cpp
      Test_ScriptLexer.cpp         # 12 tests
      Test_CommandParser.cpp       # 15 tests
      Test_StateFormatter.cpp      #  6 tests
      Test_DebugSession.cpp        #  8 tests
      Test_ScriptRunner.cpp        #  6 tests
  ArmDbgCli/
    CMakeLists.txt
    ArmDbgCli_Main.cpp
  Include/
    ArmDbg.hpp                     # Umbrella public header
    ArmDbg/
      CommandDefs.hpp              # DbgCommand enum, ParsedCommand struct
      DebugSession.hpp             # Session class declaration
      ScriptRunner.hpp             # Runner class declaration
```
