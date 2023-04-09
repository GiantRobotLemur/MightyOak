# AsmTools - Documentation

The AsmTools library contains an assembler, disassembler and code formatter for
32-bit ARM assembly language up to ARMv4. Most of these capabilities are
intended for internal use, i.e. unit tests for the processor core emulator,
but a few have been given CLI interfaces.

The library expects and produces assembly code in the style of the assembler
embedded in BBC BASIC V which formed part of RISC OS on the Archimedes. While
not all of the BBC BASIC programming language is available, some of its
operators are. Also, the style of comments, instructions, data directives and
labels matches the BASIC assembler.

Under RISC OS, machine code programs were flat binary files with no internal
structure. They were loaded at an assumed address and execution would start
at the first word of the file. The CLI tools created from the library also
follow this model for the moment.

## AAsm

AAsm is a command line tool which assembles source code into a flat binary
executable. It operates on a single source file, but additional source code
can be included, much as it is in any other C, C++ or assembly language
program.

To minimally invoke the tool, use:

```
# AAsm MyProgram.arm -o MyProgram
```

In the above example, MyProgram.arm is expected to be ARM assembly language
source code text in some Unicode-compatible format. The MyProgram binary
file will be produced if assembly is successful. The full set of command line
options can be found by running with the --help command line options, but are
also listed below:

| Option                           | Description                                      |
| -------------------------------- | ------------------------------------------------ |
| -?,--help                        | Display a summary of command line options.       |
| -o,--output &lt;output file&gt;  | Specifies the target binary file to write.       |
| -i &lt;include dir&gt;           | Specifies a folder to search for include files.  |
| -s,--instructionset &lt;arch&gt; | Specifies the initial instruction set to support |
|                                  |   with valid values being ARMv2, ARMv2a, ARMv3,  |
|                                  |   ARMv4, ARMv5, ARMv6 and ARMv7.                 |
| -x,--extension &lt;ext&gt;       | Specifies an instruction set extension to enable |
|                                  |   Useful values are FPA and BreakPt.             |
| -b,--base &lt;address&gt;        | Sets the expected base address of the binary.    |

If no base address is specified, the result is expected to be position
independent code.

## Syntax

The instruction and expression syntax is akin to that of the BASIC assembler:

```
; This is a commment
.main
    MOV R0,#0       ; Set the inital counter
.counterLoop
    ADDS R0,R0,#1   ; Increment the counter
    BVC counterLoop ; Count until overflow
    MOV PC,R14      ; Return
```

Note in the above example, the use of PC instead of R15. Other register alisas
exist using their RISC OS-centric or APCS definitions:

* A1 - A4 : R0 - R3 (APCS argument registers)
* V1 - V6 : R4 - R9 (APCS variable registers)
* SL : R10 (Stack Limit)
* FP : R11 (Frame Pointer)
* IP : R12 (Temporary workspace pointer)
* SP : R13 (Stack Pointer)
* LK or LINK : R14 (Link register)
* PC : R15 (Program Counter)

In addition to the obvious, all register references except those appearing in
a register list of STM/LDM instructions are evaluated as numeric expressions
with a fixed set of constant symbols defining the register names. In that way
expressions can be used to define register references. This will perhaps become
more useful when macros and subroutines are supported.

In register expressions, the register symbols will supersede any previously 
defined variable names which overlap. All fixed symbols, keywords and
directives are _case insensitive_. All user defined symbols, such as assembly
labels, are _case sensitive_.

Like BASIC, the assembler is line based. A new line represents a statement
terminator unless the line is terminated with a backslash character (\\). Also
multiple statements can be fitted onto the same line if the colon (:)
character is used as a statement terminator. Finally, as seen above, the
semi-colon character (;) marks the rest of the line as comment text which will be
ignored. While a line comment can appear after a statement continuation marker,
any other characters between a '\\' and the end of the line will produce a warning.

```
.myLabel : ADD R1, R2, \ ; Continue the rest of the instruction on the next line
            R3, LSL #4
```

### Labels

Currently, the only way to define a symbol in an assembly language program is
to define a label. A label definition begins with a period (.) followed by the
identifier to define. Identifiers must start with a letter or underscore (_)
character. Subsequent characters, if there are any can be a letter, underscore
or decimal digit. There is no maximum size for a symbol.

A label identifier can be referenced in expressions anywhere else in the
translation unit in which it is defined. The symbol evaluates to the object
code address at which it was defined. That address is the base address of the
target binary, if one was defined, plus the offset of the label within the
object code.

Labels are primarily used in branch instructions, PC-relative addressing modes
and the ADR pseudo-instruction. In such cases the offset between the label and
the instruction using it will be taken into account when calculating the
resultant offset.

### Directives

A number of directives exist in the assembler. Those which produce object code
are expressed with mnemonics much like instructions. Those directives which
affect the state of the assemble process are proceeded by a percent (%)
character. The following data directives are defined:

| Directive | Description                                         |
| --------- | --------------------------------------------------- |
| EQUB      | Encodes bytes or quoted ASCII strings.              |
| EQUW      | Encodes 16-bit words.                               |
| EQUD      | Encodes 32-bit double words.                        |
| EQUQ      | Encodes 64-bit quad words.                          |
| EQUS      | Encodes strings in the native 8-bit encoding.       |
| EQUSU8    | Encodes Unicode strings in the UTF-8 encoding.      |
| EQUSU16   | Encodes Unicode strings in the UTF-16 encoding.     |
| EQUSU32   | Encodes Unicode strings in the UTF-32 encoding.     |
| EQUF      | Encodes real values with 32-bit single precision.   |
| EQUR      | Encodes real values with 64-bit double precision.   |
| EQUE      | Encodes real values with 96-bit extended precision. |
| DCB       | An alias for EQUB.                                  |
| DCW       | An alias for EQUW.                                  |
| DCD       | An alias for EQUD.                                  |
| DCS       | An alias for EQUS                                   |

Some of the above directives are extensions to the BASIC assembler and exist
to support the Floating Point Accelerator (FPA) co-processor. Also, unlike the
BASIC assembler, multiple values can be expressed in each directive statement
if they are separated by a comma (,). For example:

```
EQUD 0xDEADBEEF, 123456, &CAFEBABE
EQUB "This is some text followed by raw ASCII codes.", 13, 10
EQUF 3.14159, 42.3. -69.9, 11e-4
```

Note the use of raw ASCII codes in the EQUB statement and the use of the
ampersand (&) to express a hexadecimal literal for compatibility with the
BASIC assembler. In fact, a numeric literal can be prefixed with '&' or '0x'
to specify a hexadecimal value or '0b' for a binary literal. String literals
can contain the following escape sequences:

| Sequence   | Description                                      |
| ---------- | ------------------------------------------------ |
| \\\\       | To produce a single '\\' character.              |
| \0         | A null character (ASCII 0).                      |
| \b         | A bell character (ASCII 7).                      |
| \t         | A tab character (ASCII 9).                       |
| \r         | A form feed character (ASCII 10).                |
| \n         | A line feed character (ASCII 13).                |
| \x##       | An 8-bit ASCII character where # are hex digits. |
| \u####     | A UTF-16 encoded Unicode character.              |
| \U######## | A UTF-32 encoded Unicode character.              |

Any other escaped single character results in itself. So a quote can be
escaped to appear in a string delimited by the same quote character. String
literals can be delimited by either single (') or double (") quotes.

In addition to data directives, the following assembly directives are defined
to affect the assembly process:

| Directive                  | Description                                                         |
| -------------------------- | ------------------------------------------------------------------- |
| ALIGN                      | Pads the object code offset to the next multiple of 4 bytes.        |
| ALIGN &lt;value&gt;        | Pads the object code offset to a specified address boundary.        |
| %include &lt;file name&gt; | Parses the specified source file before continuing.                 |
| %ARMv2                     | Limits the assembler to understanding ARMv2 instructions.           |
| %ARMv2a                    | Limits the assembler to understanding ARMv2a instructions.          |
| %ARMv3                     | Limits the assembler to understanding ARMv3 instructions.           |
| %ARMv4                     | Limits the assembler to understanding ARMv4 instructions.           |
| %FPA                       | Enables use of FPA co-processor instructions.                       |
| %ARM                       | Expects the following statements to be 32-bit ARM instructions.     |
| %Thumb                     | Expects the following statements to be 16-bit Thumb instructions.   |
| %26bit                     | Expects the following instructions to be executed in a 26-bit mode. |
| %32bit                     | Expects the following instructions to be executed in a 32-bit mode. |

A final directive is the ADR-pseudo instruction. It is an alias for an ADD
or SUB instruction which helps facilitate PC-relative addressing:

```
.myData
    EQUD 0xCAFEBABE, 0xDEADBEEF, 0x12345678

.myCode: 
    ADR R0,myData   ; Move the address of myData into R0.
    LDR R1,[R0,R1]  ; Load a word from the myData array.
```

### Expressions

A number of unary and binary operators can appear in expressions used to
specify the values in data directives and instruction operands:

| Operator                      | Description                       |
| ----------------------------- | --------------------------------- |
| NOT &lt;expr&gt;              | Bitwise NOT.                      |
| -&lt;expr&gt;                 | 2s compliment negation.           |
| +&lt;expr&gt;                 | 2s compliment absolute.           |
| &lt;expr&gt; + &lt;expr&gt;   | Addition or string concatenation. |
| &lt;expr&gt; - &lt;expr&gt;   | Subtraction.                      |
| &lt;expr&gt; * &lt;expr&gt;   | Multiplication.                   |
| &lt;expr&gt; / &lt;expr&gt;   | Division.                         |
| &lt;expr&gt; << &lt;expr&gt;  | Logical shift left.               |
| &lt;expr&gt; >> &lt;expr&gt;  | Logical shift right.              |
| &lt;expr&gt; >>> &lt;expr&gt; | Arithmetic shift right.           |
| &lt;expr&gt; MOD &lt;expr&gt; | Remainder from division.          |
| &lt;expr&gt; AND &lt;expr&gt; | Logical AND.                      |
| &lt;expr&gt; OR &lt;expr&gt;  | Logical OR.                       |
| &lt;expr&gt; XOR &lt;expr&gt; | Logical XOR.                      |
| &lt;expr&gt; EOR &lt;expr&gt; | An alias for the XOR operator.    |

Precedence is enforced based on operator:

| Precedence Group              |
| ----------------------------- |
| Unary -, Unary +, Logical NOT |
| Multiply, divide, modulus     |
| Addition, subtraction         |
| Shift operators               |
| Logical AND                   |
| Logical XOR/EOR               |
| Logical OR                    |

In addition to operators, the '$' symbol can be used in expressions. It
evaluates to the current object code address, i.e. the offset of the
current statement plus any base address specified.

## Supported Instructions

Various instructions sets and extensions are supported and will be allowed
depending on what is selected on the command line or by using assembly
directives. All instruction 32-bit instruction sets are compatible with the
18 standard condition codes, i.e. 16 conditions plus two aliases.

### ARMv2

Supports the basic ALU instructions: ADD, ADC, SUB, RSB, SBS, RSC, AND,
EOR/XOR, ORR, BIC, MOV, MVN, CMP, CMN, TST, TEQ. The comparison instructions
will support the 'P' suffix when in 26-bit mode. The standard MUL and MLA
instructions are also supported.

For data transfer, LDR, STR, LDM and STM instructions are supported. The LDR
and STR instructions support PC-relative addressing much like that which is
available via the ADR directive.

The flow control instructions B, BL and SWI are all supported.

### ARMv2a

This corresponds to the ARM 3 processor which included one extra instruction,
SWP, and generic co-processor use via the CDP, LDC, STC, MRC and MCR
instructions.

Co-processors are identified with the fixed symbols C0-C15, CP0-CP15 or P0-P15.
Co-processor registers are identified with the fixed symbols CR0-CR15.

### ARMv3

The ARMv3 architecture introduces the use of the separate PSR register and thus
the MRS and MSR instructions are supported. The status registers can be
referred to as CPSR or SPSR and components of the status registers with the
following suffixes:

* _all - All components.
* _flg - The status flag component.
* _ctrl - The control component.
* _c - The Control component (can be mixed with other single letter suffixes).
* _x - The eXtension component (can be mixed with other single letter suffixes).
* _s - The Status component (can be mixed with other single letter suffixes).
* _f - The Flags component (can be mixed with other single letter suffixes).

If no suffix is provided, the _all suffix is assumed.

### ARMv4

The ARMv4 architecture defines the LDR/STR instructions which operate on
16-bit half-words and signed bytes. It also defines the long multiply
instructions UMULL, UMLAL, SMULL, SMLAL.

### FPA

The Floating Point Accelerator is a co-processor used with ARMv2a and ARMv3
parts. While it can be accessed using generic co-processor instructions the
`%FPA` directive is intended to enable recognition to the specific FPA
instruction set.

__This support is currently incomplete.__

### Thumb

Support for the BX instruction and 16-bit thumb instructions
__does not exist at the current time__.

### Breakpoint

The ARMv5 BKPT instruction has been implemented as an instruction enabled by
default for internal use testing the emulator.

# ADisasm

The disassembly functionality of the library can be accessed externally using
the ADisasm command line tool. ADisasm takes a flat binary file an attempts to
disassemble what it can of the contents, outputting the results as text.

To minimally invoke the tool, use:

```
# ADisasm MyProgram
```

In the above example, MyProgram is expected to be binary file containing
assembled ARM machine code and data. If no output file is specified, the
output will be streamed to stdout. The full set of command line options can be
found by running with the --help command line options, but are also listed
below for convenience:

| Option                           | Description                                      |
| -------------------------------- | ------------------------------------------------ |
| -?,--help                        | Display a summary of command line options.       |
| -o,--output &lt;output file&gt;  | Specifies the target text file to write.         |
| -s,--instructionset &lt;arch&gt; | Specifies the instruction set to interpret.      |
|                                  |   With valid values being ARMv2, ARMv2a, ARMv3,  |
|                                  |   ARMv4, ARMv5, ARMv6 and ARMv7.                 |
| -x,--extension &lt;ext&gt;       | Specifies an instruction set extension to enable |
|                                  |   Useful values are FPA and BreakPt.             |
| -b,--base &lt;address&gt;        | Sets the expected base address of the binary.    |

If no base address is specified, the code is expected to be position
independent.

