# Hack++ Instruction Set Architecture (ISA)
This document defines the binary encoding and execution semantics of the Hack++ instruction set. The CPU 
decodes each 16-bit instruction into control signals that drive the ALU, register file (A, D), memory interface, 
and program counter (PC).

For execution details, see:
- docs/cpu.md — instruction decode and control flow
- docs/alu.md — ALU control bits and flag generation
- docs/pc.md — jump and sequencing behavior

### Instruction Set Architecture
To instruct the CPU, the ROM is loaded with a program in the form of a `.hack` binary file. That file is
assembled from a more human-readable assembly language file (`.asm`). The mapping between the two can be found
in `\docs`. However, as to not get bogged down in ones and zeros here, we will skip the binary and proceed directly
to the assembly language syntax and describe it in Extended Backus–Naur Form (EBNF).

While it looks imposing it really boils down to two steps:
1. Determine the instruction:
    - `a_instruction` - load an address into the A Register
    - `c_instruction` - perform a computation
2. Map the given instructions mnemonics to binary, such that:
    - `a_instruction` - `value` is mapped to a binary address
    - `c_instruction` - `comp`, `dest`, `jump` is mapped to a binary instruction

Labels, comments, and lines can all be ignored, for now. With that, below is the EBNF for the Hack++ assembly language.

#### Assembly Grammar (EBNF)
**Tokens**
```regexp
integer := ^[0-9]+$
symbol  := [A-Za-z_$:.] [A-Za-z0-9_-]*
newline := [\r\n]
```

```ebnf
non-terminal  ::= production rule
---               ---
program       ::= { line }

line          ::= [ insrtuction | label ] [ comment ] newline
comment       ::= "//" { any_char_except_newline }

instruction   ::= a_instruction | c_instruction

a_instruction ::= "@" value

c_instruction ::= [ dest "=" ] comp [ ";" jump ]

dest          ::= dest_char { dest_char }
dest_char     ::= "A" | "D" | "M"

comp          ::=  "0" |  "1" | "-1"
                |  "A" |  "D" |  "M"
                | "!A" | "!D" | "!M"
                | "-A" | "-D" | "-M"
                | "A+1" | "D+1" | "M+1"
                | "A-1" | "D-1" | "M-1"
                | "D+A" | "D+M"
                | "D-A" | "D-M" | "A-D" | "M-D"
                | "D&A" | "D&M"
                | "D|A" | "D|M"

jump          ::= "JGT" | "JEQ" | "JGE" | "JLT" | "JNE" | "JLE" | "JMP"

label         ::= "(" symbol ")"

value         ::= constant | symbol

constant      ::= integer (* 0 <= integer <= 32767 *)
```
**Legend:**
- `{ … }` = zero or more
- `[ … ]` = optional (zero or one)
- `|` = alternative
- Mnemonics for dest, comp, jump (e.g., `"AM"`, `"D+A"`, `"JEQ"`) are case-sensitive
- Mnemonics for comp, jump (e.g., `"D+A"`, `"JEQ"`) are not comutive

**Predefined Symbols**
```
R1..R15, SP, LCL, ARG, THIS, THAT, TEMP, SCREEN, KBD
```

A quick example could be:
```asm
// Foo.asm
...
@BAR   // load address attributed to BAR label in ROM into A Register (a_instruction)
0;JMP  // Jump to instruction in ROM at BAR (c_instruction)
...
(BAR)  // Address label being jumped to (label)
@SP    // First line of code after the jump (a_instruction)
...
```

## Instruction Determination
All Hack++ instructions are 16 bits wide. The most significant bit (MSB) determines the instruction class:
- `MSB = 0b0` → A-instruction (address / constant load)
- `MSB = 0b1` → C-instruction (compute, store, and/or jump)

## A-Instruction

```
0b 0vvv vvvv vvvv vvvv
   ^     address
```
Where zero is the opcode denoting the `a_instruction`, and the remaining 15 locations are a binary value denoting an
integer is one of 32768 (i.e., 2^15) integers between 0 and 32767.


## C-Instruction

```
0b 111 a c1 c2 c3 c4 c5 c6 d1 d2 d3 j1 j2 j3
   ^          comp           dest     jump
```

Where the one is the opcode denoting the `c_instruction`, the following two ones are unused, and the 
remaining groups (comp, dest, and jump) are mapped according to the following tables.

### ALU Control — `comp` field (`a, c1–c6`)
The `a` bit selects the ALU's `y` input:
- `a = 0` → `y = A`
- `a = 1` → `y = M` (i.e., `RAM[A]`)

The `c1–c6` bits control the ALU’s internal pipeline (`zx, nx, zy, ny, f, no`). See docs/alu.md 
for the full control-bit semantics.

| comp   | a | c1 | c2 | c3 | c4 | c5 | c6 | effect                      |
| ------ | - | -- | -- | -- | -- | -- | -- |-----------------------------|
| `0`    | 0 | 1  | 0  | 1  | 0  | 1  | 0  | Constant 0                  |
| `1`    | 0 | 1  | 1  | 1  | 1  | 1  | 1  | Constant 1                  |
| `-1`   | 0 | 1  | 1  | 1  | 0  | 1  | 0  | Constant -1                 |
| `D`    | 0 | 0  | 0  | 1  | 1  | 0  | 0  | D register                  |
| `A`    | 0 | 1  | 1  | 0  | 0  | 0  | 0  | A register                  |
| `M`    | 1 | 1  | 1  | 0  | 0  | 0  | 0  | RAM[A] value                |
| `!D`   | 0 | 0  | 0  | 1  | 1  | 0  | 1  | Bitwise NOT of D            |
| `!A`   | 0 | 1  | 1  | 0  | 0  | 0  | 1  | Bitwise NOT of A            |
| `!M`   | 1 | 1  | 1  | 0  | 0  | 0  | 1  | Bitwise NOT of RAM[A]       |
| `-D`   | 0 | 0  | 0  | 1  | 1  | 1  | 1  | Negation of D               |
| `-A`   | 0 | 1  | 1  | 0  | 0  | 1  | 1  | Negation of A               |
| `-M`   | 1 | 1  | 1  | 0  | 0  | 1  | 1  | Negation of RAM[A]          |
| `D+1`  | 0 | 0  | 1  | 1  | 1  | 1  | 1  | D plus 1                    |
| `A+1`  | 0 | 1  | 1  | 0  | 1  | 1  | 1  | A plus 1                    |
| `M+1`  | 1 | 1  | 1  | 0  | 1  | 1  | 1  | RAM[A] plus 1               |
| `D-1`  | 0 | 0  | 0  | 1  | 1  | 1  | 0  | D minus 1                   |
| `A-1`  | 0 | 1  | 1  | 0  | 0  | 1  | 0  | A minus 1                   |
| `M-1`  | 1 | 1  | 1  | 0  | 0  | 1  | 0  | RAM[A] minus 1              |
| `D+A`  | 0 | 0  | 0  | 0  | 0  | 1  | 0  | D plus A                    |
| `D+M`  | 1 | 0  | 0  | 0  | 0  | 1  | 0  | D plus RAM[A]               |
| `D-A`  | 0 | 0  | 1  | 0  | 0  | 1  | 1  | D minus A                   |
| `D-M`  | 1 | 0  | 1  | 0  | 0  | 1  | 1  | D minus RAM[A]              |
| `A-D`  | 0 | 0  | 0  | 0  | 1  | 1  | 1  | A minus D                   |
| `M-D`  | 1 | 0  | 0  | 0  | 1  | 1  | 1  | RAM[A] minus D              |
| `D&A`  | 0 | 0  | 0  | 0  | 0  | 0  | 0  | Bitwise AND of D and A      |
| `D&M`  | 1 | 0  | 0  | 0  | 0  | 0  | 0  | Bitwise AND of D and RAM[A] |
| `D\|A` | 0 | 0  | 1  | 0  | 1  | 0  | 1  | Bitwise OR of D and A       |
| `D\|M` | 1 | 0  | 1  | 0  | 1  | 0  | 1  | Bitwise OR of D and RAM[A]  |


### Destination Control — `dest` field (`d1–d3`)
The `dest` field controls which storage elements receive the ALU result.

| dest   | d1 | d2 | d3 | Effect                             |
| ------ | -- | -- | -- | ---------------------------------- |
| `null` | 0  | 0  | 0  | Value is not stored                |
| `M`    | 0  | 0  | 1  | RAM[A]                             |
| `D`    | 0  | 1  | 0  | D register                         |
| `MD`   | 0  | 1  | 1  | RAM[A] and D register              |
| `A`    | 1  | 0  | 0  | A register                         |
| `AM`   | 1  | 0  | 1  | A register and RAM[A]              |
| `AD`   | 1  | 1  | 0  | A register and D register          |
| `AMD`  | 1  | 1  | 1  | A register, RAM[A], and D register |

### Jump Control — `jump` field (`j1–j3`)
Jump decisions are made using the ALU flags:

- `zr = 1` iff `out == 0`
- `ng = 1` iff `out < 0` (two’s complement)

| jump   | j1 | j2 | j3 | Effect             |
| ------ | -- | -- | -- | ------------------ |
| `null` | 0  | 0  | 0  | No jump            |
| `JGT`  | 0  | 0  | 1  | If out > 0, jump   |
| `JEQ`  | 0  | 1  | 0  | If out = 0, jump   |
| `JGE`  | 0  | 1  | 1  | If out ≥ 0, jump   |
| `JLT`  | 1  | 0  | 0  | If out < 0, jump   |
| `JNE`  | 1  | 0  | 1  | If out ≠ 0, jump   |
| `JLE`  | 1  | 1  | 0  | If out ≤ 0, jump   |
| `JMP`  | 1  | 1  | 1  | Unconditional jump |

