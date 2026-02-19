# AAsm Assembler Syntax Reference

This document describes the 32-bit ARM assembly language syntax accepted by the
AAsm assembler tool built from the AsmTools library in this repository.

## General Structure

```
[.label] [instruction | directive] [; comment]
```

- **Statements** are separated by newlines or colons (`:`)
- **Comments** start with `;` and extend to end-of-line
- **Line continuation** uses backslash (`\`) at end of line
- All mnemonics, register names, and keywords are **case-insensitive**

## Labels

Labels are defined with a leading dot:

```arm
.myLabel                    ; Position label (marks current address)
.myConst    0x8000          ; Value label (assigns an explicit value)
```

Labels can be referenced by name in expressions and as branch targets.

## Numeric Literals

| Format | Prefix | Example |
|--------|--------|---------|
| Decimal | (none) | `42`, `1024` |
| Hexadecimal | `0x` or `&` | `0xDEADBEEF`, `&FF00` |
| Binary | `%` | `%10101010` |
| Floating point | (none) | `3.14159`, `1.5e-3` |
| Current address | `$` | `$ + 8` |

## Registers

### Core Registers

| Register | Aliases |
|----------|---------|
| R0-R3 | a1-a4 (APCS argument regs) |
| R4-R9 | v1-v6 (APCS variable regs) |
| R10 | sl (stack limit) |
| R11 | fp (frame pointer) |
| R12 | ip (intra-procedure scratch) |
| R13 | sp, SP (stack pointer) |
| R14 | lk, link, LR (link register) |
| R15 | pc, PC (program counter) |
| CPSR | Current Program Status Register (ARMv3+) |
| SPSR | Saved Program Status Register (ARMv3+) |

### FPA Registers

F0-F7 (80-bit extended precision floating point)

### Co-processor Registers

- **Register IDs:** CR0-CR15 (also `cr0`-`cr15`, `Cr0`-`Cr15`)
- **Co-processor IDs:** CP0-CP15

## Condition Codes

Almost all instructions accept a 2-letter condition suffix:

| Code | Meaning | Flags |
|------|---------|-------|
| EQ | Equal | Z set |
| NE | Not equal | Z clear |
| CS / HS | Carry set / unsigned higher or same | C set |
| CC / LO | Carry clear / unsigned lower | C clear |
| MI | Negative | N set |
| PL | Positive or zero | N clear |
| VS | Overflow | V set |
| VC | No overflow | V clear |
| HI | Unsigned higher | C set and Z clear |
| LS | Unsigned lower or same | C clear or Z set |
| GE | Signed greater or equal | N == V |
| LT | Signed less than | N != V |
| GT | Signed greater than | Z clear and N == V |
| LE | Signed less or equal | Z set or N != V |
| AL | Always (default, can be omitted) | - |
| NV | Never (reserved) | - |

Example: `ADDEQ R0, R1, #1` executes only if the Z flag is set.

## Expression Operators

Expressions can appear in immediate values, label definitions, and directives.

**Unary:** `-` (negate), `+` (absolute), `NOT` (bitwise not)

**Binary (arithmetic):** `+`, `-`, `*`, `/`, `MOD`

**Binary (bitwise):** `AND`, `OR`, `XOR` (or `EOR`), `<<` (shift left),
`>>` (shift right), `>>>` (arithmetic shift right)

**Grouping:** Parentheses `( )`

```arm
.MEMC_PAGE_32K    &600
.IRQ_VSYNC        1 << 3
.MODE_MASK        I_BIT OR F_BIT OR MODE_SVC
EQUD              ($ + 8) OR &F0000000
```

## Barrel Shifter Operand (Operand2)

Used by ALU and some data transfer instructions:

| Form | Syntax | Description |
|------|--------|-------------|
| Immediate | `#<expr>` | 8-bit value with even rotation |
| Register | `Rm` | Register value unshifted |
| Shift by constant | `Rm, LSL #n` | Logical shift left by immediate |
| Shift by register | `Rm, LSL Rs` | Logical shift left by register |
| Rotate with extend | `Rm, RRX` | 33-bit rotate through carry |

**Shift types:** `LSL`, `LSR`, `ASR`, `ROR`, `RRX`, `ASL` (synonym for `LSL`)

## Instructions

### ALU Instructions

```
<op>[<cond>][S] Rd, Rn, <operand2>    ; AND, EOR, SUB, RSB, ADD, ADC, SBC, RSC, ORR, BIC
<op>[<cond>][S] Rd, <operand2>         ; MOV, MVN (no Rn)
<op>[<cond>][P] Rn, <operand2>         ; TST, TEQ, CMP, CMN (no Rd, always set flags)
```

- `S` suffix updates CPSR flags (N, Z, C, V)
- `P` suffix on comparisons overwrites entire PSR (26-bit mode only)

```arm
MOV     R0, #0
ADD     R1, R2, R3, LSL #2
ADDEQS  R4, R5, #&FF
BICS    R6, R7, #&F0000000
CMP     R0, R1
TST     R3, #1
TEQP    PC, #(I_BIT OR F_BIT OR MODE_SVC)
```

### Multiply Instructions

```arm
MUL[<cond>][S]  Rd, Rm, Rs            ; Rd = Rm * Rs
MLA[<cond>][S]  Rd, Rm, Rs, Rn        ; Rd = Rm * Rs + Rn
```

**Long multiply (ARMv4+):**

```arm
UMULL[<cond>][S] RdLo, RdHi, Rm, Rs   ; Unsigned: RdHi:RdLo = Rm * Rs
UMLAL[<cond>][S] RdLo, RdHi, Rm, Rs   ; Unsigned accumulate
SMULL[<cond>][S] RdLo, RdHi, Rm, Rs   ; Signed
SMLAL[<cond>][S] RdLo, RdHi, Rm, Rs   ; Signed accumulate
```

### Branch Instructions

```arm
B[<cond>]   <target>      ; Branch (PC-relative, +/-32MB)
BL[<cond>]  <target>      ; Branch with Link (saves return in R14)
BX[<cond>]  Rm            ; Branch Exchange (ARMv5+)
```

Target can be a label or expression:

```arm
B       .loop
BL      SignedDivide
BEQ     .done
B       $ + &CafeB8
```

### Single Data Transfer (LDR/STR)

```
<LDR|STR>[<cond>][B|H|SB|SH][T] Rd, <address>
```

**Data type suffixes:**

| Suffix | Size | Notes |
|--------|------|-------|
| (none) | Word (32-bit) | |
| B | Unsigned byte | |
| H | Unsigned halfword (16-bit) | ARMv4+ |
| SB | Signed byte | ARMv4+ |
| SH | Signed halfword | ARMv4+ |
| T | User-mode privilege | For OS use |

**Addressing modes:**

```arm
LDR R0, [R1]                  ; Base register only
LDR R0, [R1, #8]              ; Pre-indexed, immediate offset
LDR R0, [R1, R2]              ; Pre-indexed, register offset
LDR R0, [R1, R2, LSL #2]      ; Pre-indexed, shifted register
STR R0, [R1, #4]!             ; Pre-indexed with writeback (!)
STR R0, [R1], #4              ; Post-indexed (Rn updated after access)
LDRB R0, [R1, #0]             ; Byte load
LDRH R0, [R1, #8]             ; Halfword load (ARMv4+)
LDRSB R0, [R1, #8]            ; Signed byte load (ARMv4+)
```

### Multi-Register Transfer (LDM/STM)

```
<LDM|STM>[<cond>]<mode> Rn[!], {<register_list>}[^]
```

**Addressing modes:**

| Stack notation | General notation | Direction |
|---------------|-----------------|-----------|
| FD (Full Descending) | DB / IA | Standard C stack |
| FA (Full Ascending) | IB / DA | |
| ED (Empty Descending) | DA / IB | |
| EA (Empty Ascending) | IA / DB | |

**Suffixes:**

- `!` -- writeback: update base register
- `^` -- user-mode registers or restore PSR from SPSR

**Register lists** use curly braces with commas and ranges:

```arm
STMFD   sp!, {v1-v6, ip, link}    ; Save registers (function prologue)
LDMFD   sp!, {v1-v6, ip, pc}      ; Restore and return
LDMIA   R0, {R1-R4}               ; Load 4 words from R0 upward
STMDB   R13!, {R0-R3}^            ; Store user-mode registers
LDMFD   SP!, {PC}^                ; Return from exception (restores PSR)
```

### Software Interrupt

```arm
SWI[<cond>] <24-bit expression>
```

```arm
SWI     0xFF
SWIEQ   0xCAFEBE
```

### Atomic Swap

```arm
SWP[<cond>][B] Rd, Rm, [Rn]   ; Rd = [Rn]; [Rn] = Rm (atomic)
```

`B` suffix for byte swap (default is word).

### PSR Transfer (ARMv3+)

```arm
MRS[<cond>] Rd, CPSR           ; Read current PSR
MRS[<cond>] Rd, SPSR           ; Read saved PSR
MSR[<cond>] <psr_field>, Rm    ; Write register to PSR
MSR[<cond>] <psr_field>, #imm  ; Write immediate to PSR
```

**PSR field specifiers:** `CPSR`, `SPSR`, `CPSR_all`, `CPSR_flg`, `CPSR_ctl`,
`CPSR_c`, `CPSR_f`, `CPSR_x`, `CPSR_s`, and combinations like `CPSR_cf`,
`CPSR_cx`, etc.

```arm
MRS     R0, CPSR
MSR     CPSR_ctl, R0
MSR     CPSR_flg, #&F0000000
```

### Breakpoint (ARMv5+)

```arm
BKPT <16-bit expression>       ; Cannot be conditionally executed
```

### ADR Pseudo-Instruction

```arm
ADR[<cond>] Rd, <label>        ; Load effective address (PC-relative)
```

May expand to 1-3 instructions depending on offset range (Single / Long /
Extended encoding).

### Co-Processor Instructions

```arm
CDP[<cond>]    CPn, op1, CRd, CRn, CRm[, op2]  ; Data processing
MCR[<cond>]    CPn, op1, Rd, CRn, CRm[, op2]    ; Core reg -> co-proc
MRC[<cond>]    CPn, op1, Rd, CRn, CRm[, op2]    ; Co-proc -> core reg
LDC[<cond>][L] CPn, CRd, <address>              ; Load co-proc
STC[<cond>][L] CPn, CRd, <address>              ; Store co-proc
```

`L` suffix for long (multi-word) transfer. Address operand uses same
pre/post-index syntax as LDR/STR.

### FPA Floating Point Instructions

**Data transfer:**

```arm
LDF[<cond>]<prec> Fn, <address>     ; Load FPA register
STF[<cond>]<prec> Fn, <address>     ; Store FPA register
LFM[<cond>] Fn, <count>, <address>  ; Load multiple FPA registers
SFM[<cond>] Fn, <count>, <address>  ; Store multiple FPA registers
```

**Dyadic operations** (two source operands):
`ADF`, `MUF`, `SUF`, `RSF`, `DVF`, `RDF`, `POW`, `RPW`, `RMF`, `FML`, `FDV`,
`FRD`, `POL`

**Monadic operations** (one source operand):
`MVF`, `MNF`, `ABS`, `RND`, `SQT`, `LOG`, `LGN`, `EXP`, `SIN`, `COS`, `TAN`,
`ASN`, `ACS`, `ATN`, `URD`, `NRM`

**Register transfer:** `FLT`, `FIX`, `WFS`, `RFS`, `WFC`, `RFC`

**Comparison:** `CMF`, `CMFE`, `CNF`, `CNFE`

**Precision suffixes:** `S` (single), `D` (double), `E` (extended), `P` (packed)

**Rounding suffixes:** `P` (plus/toward +inf), `M` (minus/toward -inf),
`Z` (toward zero)

```arm
MVFD    F0, #1.0
ADFD    F2, F0, F1
MUFS    F3, F4, F5
CMFE    F0, F1
FIXEZ   R0, F0
```

## Data Directives

| Directive | Alias | Size | Description |
|-----------|-------|------|-------------|
| `EQUB` | `DCB` | 8-bit | Byte value(s) |
| `EQUW` | `DCW` | 16-bit | Halfword value(s) |
| `EQUD` | `DCD` | 32-bit | Word value(s) |
| `EQUQ` | -- | 64-bit | Long word value(s) |
| `EQUS` | `DCS` | variable | Native string |
| `EQUSU8` | -- | variable | UTF-8 string |
| `EQUSU16` | -- | variable | UTF-16 string |
| `EQUSU32` | -- | variable | UTF-32 string |
| `EQUF` | -- | 32-bit | IEEE single float |
| `EQUR` | -- | 64-bit | IEEE double float |
| `EQUE` | -- | 96-bit | FPA extended real |

Multiple values can be comma-separated. Strings use single or double quotes
with C-style escapes (`\n`, `\t`, `\0`, `\xHH`, `\uHHHH`, `\UHHHHHHHH`).

```arm
EQUD    &DEADBEEF
EQUD    $, $, $                         ; Three copies of current address
EQUB    1, 2, 3 : ALIGN                 ; Bytes then realign to 4
EQUS    'Hello World!', 13, 10, 0       ; String with CR/LF/NUL
```

## Assembly Directives

| Directive | Description |
|-----------|-------------|
| `ALIGN [n]` | Align to n-byte boundary (default 4) |
| `%INCLUDE "file"` | Include another source file |
| `%ARMV2` ... `%ARMV7` | Select target instruction set |
| `%FPA` | Enable FPA co-processor instructions |
| `%VFPV1` | Enable VFP v1 instructions |
| `%ARM` | Assemble in ARM mode |
| `%THUMB` | Assemble in Thumb mode |
| `%26BIT` | Target 26-bit address space |
| `%32BIT` | Target 32-bit address space |

## String Escape Sequences

| Escape | Character |
|--------|-----------|
| `\\` | Backslash |
| `\0` | Null (0x00) |
| `\t` | Tab (0x09) |
| `\b` | Backspace (0x08) |
| `\r` | Carriage return (0x0D) |
| `\n` | Newline (0x0A) |
| `\xHH` | 2-digit hex byte |
| `\uHHHH` | 4-digit Unicode |
| `\UHHHHHHHH` | 8-digit Unicode |

## Common Patterns

### Exception Vector Table

```arm
B   Initial_Reset                  ; &00 - Reset
B   Initial_HandleUndefInstruction ; &04 - Undefined instruction
B   Initial_HandleSoftwareIrq      ; &08 - SWI
B   Initial_HandlePrefetchAbort    ; &0C - Prefetch abort
B   Initial_HandleDataAbort        ; &10 - Data abort
B   Initial_HandleAddressException ; &14 - Address exception (26-bit)
B   Initial_HandleIrq              ; &18 - IRQ
; &1C - FIQ handler starts here (no branch needed)
```

### Function Prologue/Epilogue (APCS)

```arm
STMFD   sp!, {v1-v6, ip, link}    ; Save registers
MOV     ip, sp                      ; Set frame pointer
SUB     sp, sp, #88                 ; Allocate locals
; ... function body ...
LDMFD   sp!, {v1-v6, ip, pc}       ; Restore and return
```

### Mode Switching (26-bit, ARMv2)

```arm
TEQP    PC, #(I_BIT OR F_BIT OR MODE_SVC)
MOV     R0, R0                      ; NOP after mode switch
```

### Literal Pool Pattern

```arm
LDR     R0, .memc_base_addr
; ... code ...
.memc_base_addr
EQUD    &03600000
```

### Hardware Manifest Constants

```arm
.MEMC_base          &03600000
.IOC_base           &03200000
.IOC_CONTROL        IOC_base + &00
.IRQ_VSYNC          1 << 3
.I_BIT              1 << 27
.F_BIT              1 << 26
.MODE_SVC           3
```

### Conditional Branch Chain

```arm
CMP     v2, a1
BEQ     .loop_end
ADD     v2, v2, #1
B       .loop_start
.loop_end
```

### Interrupt Handler Return

```arm
SUBS    PC, R14, #4                ; Return from IRQ (restores PSR in 26-bit)
LDMFD   SP!, {PC}^                 ; Return from exception (restores PSR)
```

## Immediate Constant Encoding Notes

ARM immediate constants in ALU instructions are encoded as an 8-bit value
with a 4-bit even rotation (0, 2, 4, ... 30 bit positions). This means not
all 32-bit values can be used as immediates. Valid examples:

- `#3` -- 0x03 with no rotation
- `#&FF` -- 0xFF with no rotation
- `#&FF00` -- 0xFF rotated right by 24
- `#&F00000` -- 0x0F rotated right by 8

If a value cannot be encoded as a single rotated 8-bit immediate, the
assembler will report an error. Use a literal pool (EQUD + LDR) or build
the value with multiple instructions instead.

The ADR pseudo-instruction can expand to up to 3 instructions (Single, Long,
Extended encoding) to handle larger PC-relative offsets.
