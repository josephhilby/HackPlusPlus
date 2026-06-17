# Computer Subsystems, (in progress)

This section documents how the core hardware blocks of Hack++ integrate to form a functional, unified computing system. At this stage, individual primitives drop away, exposing the macro-level interaction between memory configurations, data routing lines, and the execution engine.

::: warning The Big Picture

The architecture balances system resources across four tightly integrated micro-architectural boundaries:

- Memory Subsystem: Maps physical chips into a unified address window.
- I/O Subsystems: Bridges human interactions straight to the data bus.
- Datapath Subsystem: Moves and processes physical information.
- Control Unit Subsystem: Coordinates execution streams, boot flows, and hardware traps.

:::

::: tip The Von Neumann Subsystems

1. **Central Processing Unit (CPU):** The primary execution engine of the computer, formed by the tight integration of two core subsystems:
   - **Control Unit:** The operational "brain." Driven by a Program Counter (PC), it continuously fetches instructions from memory, decodes them, and asserts the control signals required to orchestrate the entire machine.
   - **Datapath:** The operational "muscle." Composed of the Arithmetic Logic Unit (ALU) and internal registers, it routes hardware signals and executes all mathematical and logical operations.
2. **Memory Systems:** The physical storage units containing binary states, structurally partitioned by access patterns:
   - **Random Access Memory (RAM):** Dedicated volatile storage for maintaining intermediate data states, variables, and dynamic runtime stacks.
   - **Read-Only Memory (ROM):** Dedicated non-volatile storage housing the foundational instruction sets, initialization routines, or program code.
3. **Input & Output (I/O) via MMIO:** The peripheral interfaces connecting the compute engine to the external environment. Rather than utilizing dedicated I/O ports, the system implements **Memory-Mapped I/O (MMIO)**, assigning peripherals directly to reserved segments of the unified memory map:
   - **Input (Keyboard):** Continuously polls the physical hardware interface and updates a dedicated memory address with the corresponding character keycode.
   - **Output (Screen):** Rasterizes visual data directly from a designated "Screen Memory Map" (framebuffer) segment of RAM, projecting the binary states onto the physical display interface.

:::

## Memory Subsystem

From the system perspective, the CPU observes a **single, flat, 15-bit address space**. The physical routing is
handled within the Memory subsystem by the highest 2-bits `[14..13]`. Because 2^15 will produce 32K words this allows
the system to address 4x 8K sections.

| Address Range (Hex) | address[14..13] | Size   | Region   | Function                    |
| ------------------- | --------------: | ------ | -------- | --------------------------- |
| `0x0000–0x3FFC`     |   `0b00 / 0b01` | 16K    | RAM      | General-purpose data memory |
| `0x4000–0x5FFF`     |          `0b10` | 8K     | Screen   | Display framebuffer         |
| `0x6000`            |          `0b11` | 1 word | Keyboard | Input register              |
| `> 0x6000`          |          `0b11` | —      | Invalid  | Ignored (reads return `0`)  |

### RAM Memory Map

The Hack platform's RAM exposes 16K words of 16-bits at address `0x0000–0x3FFF`, mapped as follows:

| Address Range (word) | ASM Name            | Usage                                                       |
| -------------------- | ------------------- | ----------------------------------------------------------- |
| `RAM[0]`             | `R0`/`SP`           | Current top of the stack                                    |
| `RAM[1]`             | `R1`/`LCL`          | Base of the current function's local segment                |
| `RAM[2]`             | `R2`/`ARG`          | Base of the current function's argument segment             |
| `RAM[3]`             | `R3`/`THIS`         | Base of the current function's `this` segment (heap object) |
| `RAM[4]`             | `R4`/`THAT`         | Base of the current function's `that` segment (heap array)  |
| `RAM[5..12]`         | `R5..12`/`TEMP`     | Virtual Registers for current function's temporary storage  |
| `RAM[13..15]`        | `R13..R15`          | General-purpose registers (`TMP`,`FRAME`,`RET` in Hack ++)  |
| `RAM[16..255]`       | -                   | Static variables (indexed by class)                         |
| `RAM[256..2044]`     | `S_ST`, `S_END`     | Stack (Grows downward, originally `256 -> 2047`)            |
| `RAM[2045..16380]`   | `H_END`, `H_ST`     | Heap (Allocates upward, originally `16380 -> 2048`)         |
| `RAM[16381..16383]`  | `SID`, `SA1`, `SA2` | Mailboxes for System Calls                                  |

<p align="right">(<a href="#Acknowledgments">see Acknowledgments, Charles Stevenson</a>)</p>

## Input and Output Subsystems

SCREEN = 16384
KBD = 24576

### Screen Memory Map

The Hack platform screen exposes 8K words of 16-bits at address `0x4000–0x5FFF`, each bit represents a screen pixel.

- 131,072 pixels
- 256 rows (16 words / column)
- 512 cols (32 words / row)
- word = SCREEN + (32 x row) + (col / 16)
- offset = col % 16
- 1 = black, 0 = white

```text
    Row Index | Start Address | End Address
  +-----------+---------------+-------------+
  | Row 0     | 0x4000        | 0x401F      |
  | Row 1     | 0x4020        | 0x403F      |
  | ...       | ...           | ...         |
  | Row 255   | 0x5FE0        | 0x5FFF      |
  +-----------+---------------+-------------+
```

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

The instruction memory is split into two physically isolated chips: a 32K-word User Program ROM and a 16K-word Kernel OS ROM. Switching between them is handled automatically via a hardware privilege bit (kernel) driven by the Program Counter.

#### The Boot & Trap Handshake Protocol

Because a hardware reset and a software system call both vector to Kernel address 0x0000, the OS uses a low-overhead software handshake to determine the entry context.

1. Cold Boot Detection (Hardware Initialization)
   - The Mechanism: On power-on, physical RAM stabilizes with chaotic, non-zero garbage data (mirrored by the hardware simulator).
   - The Result: The Kernel reads register R13. If it contains a non-zero value, a Cold Boot is identified. The Kernel branches to Sys.init to build the memory map, sets stack/heap boundaries, and overwrites R13 with 0x0000 to prime the system.

2. System Call Interception (Software Traps)
   - The Execution: A standard library (libj) wrapper loads the function ID into SID, packs arguments into SA1/SA2, and calls the magic hardware vector @32767 (0b0111111111111111).
   - The Hardware Action: SysCall15 decoding logic instantly triggers the PC to snapshot the return track (returnAddr = PC + 1), flip kernel high, and jump to Kernel address 0x0000.
   - The Kernel Action: The Kernel wakes up and reads R13. Because it is exactly 0x0000, the OS identifies a legitimate Software Interrupt. It bypasses the boot sequence, executes the task requested by SID, and returns to userspace by reloading the PC with returnAddr.

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

The `c1–c6` bits control the ALU’s internal pipeline (`zx, nx, zy, ny, f, no`).

::: details C-Instruction Comp Field

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

:::

##### Destination Control — `dest` field (`d1–d3`)

The `dest` field controls which storage elements receive the ALU result.

::: details C-Instruction Dest Field

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

:::

##### Jump Control — `jump` field (`j1–j3`)

Jump decisions are made using the ALU flags:

- `zr = 1` iff `out == 0`
- `ng = 1` iff `out < 0` (two’s complement)

::: details C-Instruction Jump Field

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

:::
