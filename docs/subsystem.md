# Computer Subsystems, (in progress)

## Memory Subsystem

From the system perspective, the CPU observes a **single, flat, 15-bit address space**. The physical routing is
hidden behind the Memory subsystem.

| Address Range (Hex) | address[14..13] | Size   | Region   | Function                    |
| ------------------- | --------------: | ------ | -------- | --------------------------- |
| `0x0000–0x3FFF`     |          `0b00` | 16K    | RAM      | General-purpose data memory |
| `0x4000–0x5FFF`     |          `0b01` | 8K     | Screen   | Display framebuffer         |
| `0x6000`            |          `0b10` | 1 word | Keyboard | Input register              |
| `> 0x6000`          |          `0b11` | —      | Invalid  | Ignored (reads return `0`)  |

### RAM Memory Map

The Hack platform's RAM exposes 16K words of 16-bits at address `0x0000–0x3FFF`, mapped as follows:

| Address Range (word) | ASM Name        | Usage                                                       |
| -------------------- | --------------- | ----------------------------------------------------------- |
| `RAM[0]`             | `R0`/`SP`       | Current top of the stack                                    |
| `RAM[1]`             | `R1`/`LCL`      | Base of the current function's local segment                |
| `RAM[2]`             | `R2`/`ARG`      | Base of the current function's argument segment             |
| `RAM[3]`             | `R3`/`THIS`     | Base of the current function's `this` segment (heap object) |
| `RAM[4]`             | `R4`/`THAT`     | Base of the current function's `that` segment (heap array)  |
| `RAM[5..12]`         | `R5..12`/`TEMP` | Virtual Registers for current function's temporary storage  |
| `RAM[13..15]`        | `R13..R15`      | General-purpose registers (`TMP`,`FRAME`,`RET` in Hack ++)  |
| `RAM[16..255]`       | —               | Static variables (assigned at compile time)                 |
| `RAM[256..2047]`     | —               | Stack                                                       |
| `RAM[2048..16383]`   | —               | Heap                                                        |

<p align="right">(<a href="#Acknowledgments">see Acknowledgments, Charles Stevenson</a>)</p>

## Input and Output Subsystems

SCREEN = 16384
KBD = 24576

### Screen Memory Map

The Hack platform screen exposes 8K words of 16-bits at address `0x4000–0x5FFF`, each bit represents a screen pixel.

- 131,072 pixels
- 256 rows
- 512 cols
- word = SCREEN + 32r + (c/16)
- pixel = RAM[word][c % 16]
- 1 = black, 0 = white

### Keyboard Memory map

The Hack platform screen exposes 1 word of 16-bits at address `0x6000`. When a key is pressed
a 16-bit character code appears at RAM[KBD]. When no key is pressed RAM[KBD] = 0.

## Datapath Subsystem

### RAM

### A, D, M Registers

- Accumulator Registers
  - **A register** — address / operand register
  - **M register** - virtual registers R1-R15
- Destination Register
  - **D register** — data register

### ALU

Conceptually talk about what it can do with data. leave the dirty implementation of instruciton to compution to the
control path section. End with how is this turing complete.

Can:

- Output a constant {0, 1, -1}
- Pass a register vlaue {ADM}
- Pass on a negated or bitwise not register value
- Increment or decrement a register value
- Add two registers
- Subtract two registers
- Bitwise And two registers
- Bitwise Or two registers

## Control Unit Subsystem

### ROM

Consists of 32K words of 16-bits

- OS (range in hex)
  - Init (bootstrap)
  - "System Calls"
- Program (range in hex)

### Instructions

All Hack++ instructions are 16 bits wide. The most significant bit (MSB) determines the instruction class:

- `MSB = 0b0xx` → A-instruction (address / constant load)
- `MSB = 0b111` → C-instruction (compute, store, and/or jump)

This creates a gap we can exploit in a bit to allow for a simple context switch. As the last valid A instruction
is 0x6000 and first valid C instruction is 0xE000 with a hole at (0x8000-0xDFFF). This lets us use 0x7FFF, or
0b 0111 1111 1111 1111 in bin and `@32767` in our assembly, to be used not as an address but a request for a
context switch. This context switch will act as a switch between the OS and User Program Space, allowing the current
RAM state to be preserved between the two and freeing up the space in ROM that previously was utilized for the OS
to instead be used for more elaborate user programs.

Must preserve previous A-reg when this magic address is called as it will have the effect of `@0` the a reg.

It is also important to note that not all C instructions are used. Only 1,792 (28 ALU x 8 Destination x 8 Jumps)
of the 8,192 (0xE000-0xFFFF). So there is much more room to play with.

#### A-Instruction

```
0b 0vvv vvvv vvvv vvvv
   ^     address
```

Where zero is the opcode denoting the `a_instruction`, and the remaining 15 locations are a binary value denoting an
integer is one of 32768 (i.e., 2^15) integers between 0 and 32767.

Added magic address to allow for a context switch to kernel mode.

```
0b 0111 1111 1111 1111
```

This works as the max address the RAM has to consider is the keyboard (`0b 0110 0000 0000 0001`). So no program
should be requesting this via an A instruction, and it is not a valid C instruction.

#### C-Instruction

```
0b 111 a c1 c2 c3 c4 c5 c6 d1 d2 d3 j1 j2 j3
   ^          comp           dest     jump
```

Where the one is the opcode denoting the `c_instruction`, the following two ones are unused, and the
remaining groups (comp, dest, and jump) are mapped according to the following tables.

### CPU

##### ALU Control — `comp` field (`a, c1–c6`)

The `a` bit selects the ALU's `y` input:

- `a = 0` → `y = A`
- `a = 1` → `y = M` (i.e., `RAM[A]`)

The `c1–c6` bits control the ALU’s internal pipeline (`zx, nx, zy, ny, f, no`). See docs/alu.md
for the full control-bit semantics.

| comp   | a   | c1  | c2  | c3  | c4  | c5  | c6  | effect                      |
| ------ | --- | --- | --- | --- | --- | --- | --- | --------------------------- |
| `0`    | 0   | 1   | 0   | 1   | 0   | 1   | 0   | Constant 0                  |
| `1`    | 0   | 1   | 1   | 1   | 1   | 1   | 1   | Constant 1                  |
| `-1`   | 0   | 1   | 1   | 1   | 0   | 1   | 0   | Constant -1                 |
| `D`    | 0   | 0   | 0   | 1   | 1   | 0   | 0   | D register                  |
| `A`    | 0   | 1   | 1   | 0   | 0   | 0   | 0   | A register                  |
| `M`    | 1   | 1   | 1   | 0   | 0   | 0   | 0   | RAM[A] value                |
| `!D`   | 0   | 0   | 0   | 1   | 1   | 0   | 1   | Bitwise NOT of D            |
| `!A`   | 0   | 1   | 1   | 0   | 0   | 0   | 1   | Bitwise NOT of A            |
| `!M`   | 1   | 1   | 1   | 0   | 0   | 0   | 1   | Bitwise NOT of RAM[A]       |
| `-D`   | 0   | 0   | 0   | 1   | 1   | 1   | 1   | Negation of D               |
| `-A`   | 0   | 1   | 1   | 0   | 0   | 1   | 1   | Negation of A               |
| `-M`   | 1   | 1   | 1   | 0   | 0   | 1   | 1   | Negation of RAM[A]          |
| `D+1`  | 0   | 0   | 1   | 1   | 1   | 1   | 1   | D plus 1                    |
| `A+1`  | 0   | 1   | 1   | 0   | 1   | 1   | 1   | A plus 1                    |
| `M+1`  | 1   | 1   | 1   | 0   | 1   | 1   | 1   | RAM[A] plus 1               |
| `D-1`  | 0   | 0   | 0   | 1   | 1   | 1   | 0   | D minus 1                   |
| `A-1`  | 0   | 1   | 1   | 0   | 0   | 1   | 0   | A minus 1                   |
| `M-1`  | 1   | 1   | 1   | 0   | 0   | 1   | 0   | RAM[A] minus 1              |
| `D+A`  | 0   | 0   | 0   | 0   | 0   | 1   | 0   | D plus A                    |
| `D+M`  | 1   | 0   | 0   | 0   | 0   | 1   | 0   | D plus RAM[A]               |
| `D-A`  | 0   | 0   | 1   | 0   | 0   | 1   | 1   | D minus A                   |
| `D-M`  | 1   | 0   | 1   | 0   | 0   | 1   | 1   | D minus RAM[A]              |
| `A-D`  | 0   | 0   | 0   | 0   | 1   | 1   | 1   | A minus D                   |
| `M-D`  | 1   | 0   | 0   | 0   | 1   | 1   | 1   | RAM[A] minus D              |
| `D&A`  | 0   | 0   | 0   | 0   | 0   | 0   | 0   | Bitwise AND of D and A      |
| `D&M`  | 1   | 0   | 0   | 0   | 0   | 0   | 0   | Bitwise AND of D and RAM[A] |
| `D\|A` | 0   | 0   | 1   | 0   | 1   | 0   | 1   | Bitwise OR of D and A       |
| `D\|M` | 1   | 0   | 1   | 0   | 1   | 0   | 1   | Bitwise OR of D and RAM[A]  |

##### Destination Control — `dest` field (`d1–d3`)

The `dest` field controls which storage elements receive the ALU result.

| dest   | d1  | d2  | d3  | Effect                             |
| ------ | --- | --- | --- | ---------------------------------- |
| `null` | 0   | 0   | 0   | Value is not stored                |
| `M`    | 0   | 0   | 1   | RAM[A]                             |
| `D`    | 0   | 1   | 0   | D register                         |
| `MD`   | 0   | 1   | 1   | RAM[A] and D register              |
| `A`    | 1   | 0   | 0   | A register                         |
| `AM`   | 1   | 0   | 1   | A register and RAM[A]              |
| `AD`   | 1   | 1   | 0   | A register and D register          |
| `AMD`  | 1   | 1   | 1   | A register, RAM[A], and D register |

##### Jump Control — `jump` field (`j1–j3`)

Jump decisions are made using the ALU flags:

- `zr = 1` iff `out == 0`
- `ng = 1` iff `out < 0` (two’s complement)

| jump   | j1  | j2  | j3  | Effect             |
| ------ | --- | --- | --- | ------------------ |
| `null` | 0   | 0   | 0   | No jump            |
| `JGT`  | 0   | 0   | 1   | If out > 0, jump   |
| `JEQ`  | 0   | 1   | 0   | If out = 0, jump   |
| `JGE`  | 0   | 1   | 1   | If out ≥ 0, jump   |
| `JLT`  | 1   | 0   | 0   | If out < 0, jump   |
| `JNE`  | 1   | 0   | 1   | If out ≠ 0, jump   |
| `JLE`  | 1   | 1   | 0   | If out ≤ 0, jump   |
| `JMP`  | 1   | 1   | 1   | Unconditional jump |
