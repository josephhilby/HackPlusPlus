# Components

## Register

> **Also known as:** _word register_, _general-purpose register_

The **Register** is a 16-bit state element used throughout the CPU and memory hierarchy. It applies a single `load`
enable across 16 `Bit` cells, producing a word-sized storage primitive.

::: warning Note:
The remaining hardware described in these docs will quickly grow in complexity. To account for this, when prudent, words
will be translated from `16-bit` (bin) into `4-nybble` (hex)-called this because two nybbles makes one byte.

:::

::: details Hardware Description

```hdl
CHIP Register {
    IN in[16], load;
    OUT out[16];

    PARTS:
    // LSB
    Bit(in=in[0],  load=load, out=out[0]);
    Bit(in=in[1],  load=load, out=out[1]);
    Bit(in=in[2],  load=load, out=out[2]);
    Bit(in=in[3],  load=load, out=out[3]);
    Bit(in=in[4],  load=load, out=out[4]);
    Bit(in=in[5],  load=load, out=out[5]);
    Bit(in=in[6],  load=load, out=out[6]);
    Bit(in=in[7],  load=load, out=out[7]);
    Bit(in=in[8],  load=load, out=out[8]);
    Bit(in=in[9],  load=load, out=out[9]);
    Bit(in=in[10], load=load, out=out[10]);
    Bit(in=in[11], load=load, out=out[11]);
    Bit(in=in[12], load=load, out=out[12]);
    Bit(in=in[13], load=load, out=out[13]);
    Bit(in=in[14], load=load, out=out[14]);

    // MSB
    Bit(in=in[15], load=load, out=out[15]);
}
```

:::

::: tip Register Logic

```text
IF load(t) == 1:
    out(t+1) = in(t)
ELSE:
    out(t+1) = out(t)
```

<RegisterDemo />

:::

## Register Bank

> **Also known as:** _register file_

A **Register Bank** is a series of `n` registers, assembled as a memory hierarchy. It applies a single `load` enable
across all `n` registers, then routes the new `in` state to a single register, according to the provided address (`addr`).

::: tip Register Bank Logic

```text
IF load(t) == 1:
    Register[ADDR](t+1) = in(t)
ELSE:
    Bank(t+1) = Bank(t)
```

<RegisterBankDemo />

:::

### Memory Hierarchy

In order to achieve a bank of size `n`, increasingly larger banks are built by integrating eight instances from the
previous level. To select one of those eight components, we will need three dedicated bits (2^3=8). As this hierarchy
scales, there becomes a need to simultaneously select a target component and pass the remaining addressing information
into that component.

::: tip Hi-Lo Address Split

To distinguish between these roles, we will group the address bits into two categories:

1. `hi`: Three bits used to select the interior component.
2. `lo`: The remaining bits that are passed into the selected interior component.

:::

|       Chip | Words | Address bits | Built from    | Address split                             |
| ---------: | ----: | -----------: | ------------- | ----------------------------------------- |
| `Register` |     1 |            0 | 8× `Bit`      | none                                      |
|     `RAM8` |     8 |            3 | 8× `Register` | `sel = address[0..2]`                     |
|    `RAM64` |    64 |            6 | 8× `RAM8`     | `hi=address[3..5]`, `lo=address[0..2]`    |
|   `RAM512` |   512 |            9 | 8× `RAM64`    | `hi=address[6..8]`, `lo=address[0..5]`    |
|    `RAM4K` |  4096 |           12 | 8× `RAM512`   | `hi=address[9..11]`, `lo=address[0..8]`   |
|   `RAM16K` | 16384 |           14 | 8× `RAM4K`    | `hi=address[11..13]`, `lo=address[0..11]` |

::: details RAM8

```hdl
CHIP RAM8 {
    IN in[16], load, address[3];
    OUT out[16];

    PARTS:
    DMux8Way(in=load, sel=address,
             a=in0, b=in1, c=in2, d=in3,
             e=in4, f=in5, g=in6, h=in7);

    Register(in=in, load=in0, out=out0);
    Register(in=in, load=in1, out=out1);
    Register(in=in, load=in2, out=out2);
    Register(in=in, load=in3, out=out3);
    Register(in=in, load=in4, out=out4);
    Register(in=in, load=in5, out=out5);
    Register(in=in, load=in6, out=out6);
    Register(in=in, load=in7, out=out7);

    Mux8Way16(a=out0, b=out1, c=out2, d=out3,
              e=out4, f=out5, g=out6, h=out7,
              sel=address, out=out);
}
```

:::

::: details RAM64

```hdl
CHIP RAM64 {
    IN in[16], load, address[6];
    OUT out[16];

    PARTS:
    DMux8Way(in=load, sel=address[3..5],
             a=in0, b=in1, c=in2, d=in3,
             e=in4, f=in5, g=in6, h=in7);

    RAM8(in=in, load=in0, address=address[0..2], out=out0);
    RAM8(in=in, load=in1, address=address[0..2], out=out1);
    RAM8(in=in, load=in2, address=address[0..2], out=out2);
    RAM8(in=in, load=in3, address=address[0..2], out=out3);
    RAM8(in=in, load=in4, address=address[0..2], out=out4);
    RAM8(in=in, load=in5, address=address[0..2], out=out5);
    RAM8(in=in, load=in6, address=address[0..2], out=out6);
    RAM8(in=in, load=in7, address=address[0..2], out=out7);

    Mux8Way16(a=out0, b=out1, c=out2, d=out3,
              e=out4, f=out5, g=out6, h=out7,
              sel=address[3..5], out=out);
}
```

:::

::: details RAM512

```hdl
CHIP RAM512 {
    IN in[16], load, address[9];
    OUT out[16];

    PARTS:
    DMux8Way(in=load, sel=address[6..8],
             a=in0, b=in1, c=in2, d=in3,
             e=in4, f=in5, g=in6, h=in7);

    RAM64(in=in, load=in0, address=address[0..5], out=out0);
    RAM64(in=in, load=in1, address=address[0..5], out=out1);
    RAM64(in=in, load=in2, address=address[0..5], out=out2);
    RAM64(in=in, load=in3, address=address[0..5], out=out3);
    RAM64(in=in, load=in4, address=address[0..5], out=out4);
    RAM64(in=in, load=in5, address=address[0..5], out=out5);
    RAM64(in=in, load=in6, address=address[0..5], out=out6);
    RAM64(in=in, load=in7, address=address[0..5], out=out7);

    Mux8Way16(a=out0, b=out1, c=out2, d=out3,
              e=out4, f=out5, g=out6, h=out7,
              sel=address[6..8], out=out);
}
```

:::

::: details RAM4K

```hdl
CHIP RAM4K {
    IN in[16], load, address[12];
    OUT out[16];

    PARTS:
    DMux8Way(in=load, sel=address[9..11],
             a=in0, b=in1, c=in2, d=in3,
             e=in4, f=in5, g=in6, h=in7);

    RAM512(in=in, load=in0, address=address[0..8], out=out0);
    RAM512(in=in, load=in1, address=address[0..8], out=out1);
    RAM512(in=in, load=in2, address=address[0..8], out=out2);
    RAM512(in=in, load=in3, address=address[0..8], out=out3);
    RAM512(in=in, load=in4, address=address[0..8], out=out4);
    RAM512(in=in, load=in5, address=address[0..8], out=out5);
    RAM512(in=in, load=in6, address=address[0..8], out=out6);
    RAM512(in=in, load=in7, address=address[0..8], out=out7);

    Mux8Way16(a=out0, b=out1, c=out2, d=out3,
              e=out4, f=out5, g=out6, h=out7,
              sel=address[9..11], out=out);
}
```

:::

::: details RAM16K

```hdl
CHIP RAM16K {
    IN in[16], load, address[14];
    OUT out[16];

    PARTS:
    DMux8Way(in=load, sel=address[11..13],
             a=in0, b=in1, c=in2, d=in3,
             e=in4, f=in5, g=in6, h=in7);

    RAM4K(in=in, load=in0, address=address[0..11], out=out0);
    RAM4K(in=in, load=in1, address=address[0..11], out=out1);
    RAM4K(in=in, load=in2, address=address[0..11], out=out2);
    RAM4K(in=in, load=in3, address=address[0..11], out=out3);
    RAM4K(in=in, load=in4, address=address[0..11], out=out4);
    RAM4K(in=in, load=in5, address=address[0..11], out=out5);
    RAM4K(in=in, load=in6, address=address[0..11], out=out6);
    RAM4K(in=in, load=in7, address=address[0..11], out=out7);

    Mux8Way16(a=out0, b=out1, c=out2, d=out3,
              e=out4, f=out5, g=out6, h=out7,
              sel=address[11..13], out=out);
}
```

:::

### RAM Memory Map

The inside the `Memory()` module, the RAM16 component `16K` words of 16-bits at address `0x0000–0x3FFF`, mapped as follows:

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

## Program Counter

> **Also known as:** _instruction pointer_, _PC register_

The **Program Counter (PC)** is a 16-bit stateful counter that tracks the ROM address of the instruction in execution and
sequences to the next executable instruction. The execution of this sequencing is set by three control behaviors — reset,
load, and increment — with a defined priority order. This priority ordering guarantees deterministic behavior when multiple
control signals are asserted in the same cycle.

::: warning Execution Context
To demonstrate the **Program Counter** and its hardware-isolated vector logic, this simulation mimics a secure system boot followed
by a rolling user-to-kernel service loop.

::: details The Sequence:

- **Cold Boot (Manual Reset):** Clicking the **Reset** button asserts the `reset` flag with absolute priority. The hardware forces the system to boot securely into Kernel Mode (`kern = 1`) and vectors the program counter to address `0x0000` in the Kernel Bank.
- **Kernel Initialization (ROM 1):** The Kernel evaluates whether this is a cold start or a user initiated trap. On a cold boot, it executes an initialization routine, clears its boot flag (`CLR BOOT`) in register `D`, and triggers `MGK` to securely drop the execution state down into User Space at address `0x0000`.
- **User Space Execution (ROM 0):** The CPU sets up an initial data state (`MOVE A, 1`) and hits the magic instruction (`MGK`). The hardware instantly intercepts this as a deliberate system call trap, saves the next sequential user address (`0x0002`) into the Link Register (`LR`), sets `kern = 1`, and vectors the PC right back to `0x0000` inside the Kernel.
- **Kernel Trap Handler (ROM 1):** The Kernel wakes up at `0x0000` again. Because the boot flag already clear, the conditional branch fails, letting the CPU step linearly into the trap routine (`ADD A, 1`) to handle the user's request. It hits `MGK` at the end of the handler to drop back to User Space at the exact address stored in `LR`.
- **The Infinite Loop:** User space resumes progress exactly where it left off, executes a standard jump (`JMP 1`) to reset its local block, and strikes the `MGK` trap again—creating a continuous, rolling cycle of protected domain transitions.

:::

::: details Hardware Description

```hdl
CHIP PC {
    IN in[16], load, inc, reset, request;
    OUT out[16], kernel;

    PARTS:
    // STATE (t)
    // Context Flag (1=kernel, 0=user)
    Bit(in=update, load=loadUpdate, out=kernel);

    // Link Register (Returns to user instruction++)
    Register(in=plusOne, load=toKernel, out=returnAddr);

    // Main Program Counter
    Register(in=result, load=true, out=current, out=out);

    // NEXT-STATE (t+1):
    // Context State Engine
    XOR(a=request, b=kernel, out=change);
    MUX(a=change, b=true, sel=reset, out=update);
    OR(a=reset, b=request, out=loadUpdate);

    // Hardware Trap Routing
    NOT(in=kernel, out=user);
    AND(a=request, b=user, out=toKernel);
    AND(a=request, b=kernel, out=toUser);
    OR(a=reset, b=toKernel, out=toInit);

    // Instruction Increment
    Inc16(in=current, out=plusOne);

    // Instruction Priority (Low to High)
    Mux16(a=current, b=plusOne, sel=inc, out=step1);
    Mux16(a=step1, b=in, sel=load, out=step2);
    Mux16(a=step2, b=returnAddr, sel=toUser, out=step3);
    Mux16(a=step3, b=false, sel=toInit, out=result);
}
```

:::

::: tip PC Logic

```text
IF reset(t) == 1
    kernel(t+1) = 1
    out(t+1) = 0
ELSE IF request(t) == 1 && kernel(t) == 0
    // Trap to Kernel Vector(0)
    returnAddr(t) = out(t) + 1
    kernel(t+1) = 1
    out(t+1) = 0
ELSE IF request(t) == 1 && kernel(t) == 1
    // Trap Return to User
    kernel(t+1) = 0
    out(t+1) = returnAddr(t)
ELSE IF load(t) == 1
    out(t+1) = in(t)
ELSE IF inc(t) == 1
    out(t+1) = out(t) + 1
ELSE
    out(t+1) = out(t)
```

<PCDemo />

:::

## Arithmetic and Logic Unit

::: details Hardware Description

```hdl
CHIP ALU {
    IN  x[16], y[16],
        zx, nx, zy, ny, f, no;

    OUT out[16], zr, ng;

    PARTS:
    // Set xIn
    Mux16(a=x, b[0..15]=false, sel=zx, out=x);
    Not16(in=x, out=xNeg);
    Mux16(a=x, b=xNeg, sel=nx, out=xIn);

    // Set yIn
    Mux16(a=y, b[0..15]=false, sel=zy, out=y);
    Not16(in=y, out=yNeg);
    Mux16(a=wy, b=yNeg, sel=ny, out=yIn);

    // Compute f(xIn,yIn)
    And16(a=xIn, b=yIn, out=and);
    Add16(a=xIn, b=yIn, out=add);
    Mux16(a=and, b=add, sel=f, out=result);

    // Negate Result
    Not16(in=result, out=nResult);
    Mux16(a=result, b=nResult, sel=no,
          out=out
          out[15]=ng);

    // Compute Zero Flag
    Or16Way(in=out, out=notZr);
    Not(in=notZr, out=zr);
}
```

:::

**Two’s complement arithmetic**
All arithmetic is performed on 16-bit two’s complement values.
