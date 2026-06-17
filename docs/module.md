# Modules

Continuing this top-down deconstruction, we arrive at the modules of the hardware subsystem. As before, these will be subdivided into
smaller entities (components) and the appropriate architectural specifications will be presented.

<SystemHierarchy dataType="hardware" />

## Memory

From the computers perspective, the `CPU()` observes a **single, flat, address space**. However, the `Memory()` module is actually
composed of three separate storage components that hold its addressable data: `RAM16K`, `RAM8K`, and a `Register`.
When the `Memory()` module receives a read/write request to a specific address, it then has to handle the routing of that request
to the appropriate component internally.

This routing is done through a simple memory map that takes the first two bits of the address, following the opcode, and uses them to address the
three sections.

> - _Note 1: Consuming two more bits will limit all components to 8K._
> - _Note 2: We can `OR` two bit combinations to allow for a 16K component._

::: tip A-Instruction (Address)

```ISA_Pattern
 0        vv        v vvvv vvvv vvvv
 opcode   section   address
```

:::

::: details Memory Description

```hdl
CHIP Memory {
    IN keyboard[16], in[16], load, address[15];
    OUT out[16];

    PARTS:
    // Select the write region
    DMux4Way(in=load, sel=address[13..14],
             a=mem1, b=mem2, c=scr, d=key);

    // RAM (0x0000–0x3FFF); sel = { 0b00, 0b01 }
    Or(a=mem1, b=mem2, out=mem);
    RAM16K(in=in, load=mem, address=address[0..13], out=memOut);

    // MMIO
    // Screen (0x4000–0x5FFF); sel = 0b10
    RAM8K(in=in, load=scr, address=address[0..12], out=scrOut);

    // Keyboard (0x6000); sel = 0b11
    Or16Way(in=keyboard, out=isPressed);
    Mux16(a=false, b=keyboard, sel=isPressed, out=keyIn);
    Register(in=keyIn, load=true, out=keyOut);

    // Return 0 for addr > 0x6000
    Or16Way(in[0..12]=address[0..12], in[13..14]=false, out=ovr6000);
    Mux16(a=keyOut, b=false, sel=ovr6000, out=keyRtn);

    // Select the read region
    Mux4Way16(a=memOut, b=memOut, c=scrOut, d=keyRtn,
              sel=address[13..14], out=out);
}
```

:::

The memory map for this routing is as follows:

| Address Range (Hex) |     Selection Bits | Region   | Component  | Function                    |
| ------------------- | -----------------: | -------- | ---------- | --------------------------- |
| `0x0000–0x3FFF`     | { `0b00`, `0b01` } | RAM      | `RAM16K`   | General-purpose data memory |
| `0x4000–0x5FFF`     |             `0b10` | Screen   | `RAM8K`    | Screen framebuffer          |
| `0x6000`            |             `0b11` | Keyboard | `Register` | Key press input register    |

> - _Note: Anything over `0x6000` is ignored and returns a `0`._

## Instruction

The `CPU()` utilizes a **single, flat address space** and also emits a privilege bit from the `kernel flag` register.
By utilizing this dedicated bit, we avoid sacrificing address space for privilege management. Instead, routing between
the `ROM16K` (Kernel) and `ROM32K` (User) `Instruction()` components is handled with the state of the `kernel flag`, where 1
represents Kernel mode and 0 represents User mode.

To transition between these states, we implement a trap mechanism. The trap relies on a hardcoded "Trap Vector"
address; when the program jumps to this address, execution momentarily pauses, allowing the hardware to
automatically toggle the privilege level.

> - _Note: Keeping all 15-bits allows each component to recognize up to `32K - 1` address._

::: tip A-Instruction (Address)

```ISA_Pattern
 0        vvv vvvv vvvv vvvv
 opcode   address
```

---

_Special Case, the Trap Vector_

```ISA_Pattern
 0        111 1111 1111 1111
 opcode   magic address
```

:::

::: details Instruction Description

```hdl
CHIP Instruction {
    IN address[16], sel;
    OUT out[16];

    PARTS:
    // User Space (ROM 0)
    ROM32K(address=address, out=instructionProg);

    // Kernel Space (ROM 1)
    ROM16K(address=address[0..13], out=instructionOS);

    // Select the read region
    MUX16(a=instructionProg, b=instructionOS, sel=sel, out=instruction);
}
```

:::

::: warning Trap Vector (The Magic Address)

When the CPU detects a jump to `0x7FFF` — initiated by the sequence `@32767` `0;JMP` — it triggers a context switch:

- **User to Kernel:**
  - **The Intercept:** The `CPU()` intercepts the jump and toggles the `kernel flag` bit.
  - **The Context Save:** The `CPU()` saves a return address in a special link register (LR).
  - **ROM Switching:** The bit-flip redirects the `Instruction()` module, instantly swapping the active ROM bank from User to Kernel.

- **Kernel to User:**
  - **The Intercept:** The `CPU()` intercepts the jump and toggles the `kernel flag` bit.
  - **The Context Save:** The `CPU()` retrieves the return address from a the LR and uses it to set the Program Counter.
  - **ROM Switching:** The bit-flip redirects the `Instruction()` module, instantly swapping the active ROM bank from Kernel to User.

_`0x7FFF` is an unused `Memory()` address and the final addressable `Instruction()`. To prevent conflicts, our assembler prohibits assembled programs from referencing this location outside of a context switch. So, for the sacrifice of a single instruction word, we gain a dedicated parallel instruction space for the OS._

:::

## CPU

Single-Cycle Deterministic

::: tip A-Instruction (Constant)

```ISA_Pattern
 0        vvv vvvv vvvv vvvv
 opcode   constant
```

:::

::: tip C-Instruction (Compute)

```ISA_Pattern
 1        11       acccccc   000    000
 opcode   unused   comp      dest   jump
```

---

```ISA_Pattern
 1        11       acccccc   ddd    000
 opcode   unused   comp      dest   jump
```

:::

::: tip C-Instruction (Control)

```ISA_Pattern
 1        11       acccccc   000    jjj
 opcode   unused   comp      dest   jump
```

:::

::: details CPU Description

```hdl
CHIP CPU {
    IN  inM[16], instruction[16], reset;
    OUT outM[16], writeM, addressM[15], pc[15], kernel;

    PARTS:
    // A-Instruction Handling
    Not(in=instruction[15], out=isA);
    Mux16(a=instruction, b=ALUout, sel=instruction[15], out=inA);
    Register(in=inA, load=loadA, out=outA, out[0..14]=addressM);

    // Context Switch
    SysCall15(in=addressM, out=request);

    // C-Instruction Decoding
    And(a=instruction[15], b=true, out=insC);

    // Datapath: Control Bits
    And(a=instruction[12], b=true, out=readMem);
    And(a=instruction[11], b=true, out=zx);
    And(a=instruction[10], b=true, out=nx);
    And(a=instruction[9],  b=true, out=zy);
    And(a=instruction[8],  b=true, out=ny);
    And(a=instruction[7],  b=true, out=f);
    And(a=instruction[6],  b=true, out=no);

    // Datapath: Destination Bits
    Or(a=isA, b=instruction[5], out=loadA);
    And(a=insC, b=instruction[4], out=loadD);
    And(a=insC, b=instruction[3], out=writeM);

    // Datapath: Select Y Input (Reg A or RAM)
    Mux16(a=outA, b=inM, sel=readMem, out=y);

    // Datapath: Execute ALU Computation
    ALU(x=x, y=y, zx=zx, nx=nx, zy=zy, ny=ny, f=f, no=no,
        out=ALUout, out=outM, zr=zr, ng=ng);

    // Datapath: Store ALU Computation
    Register(in=ALUout, load=loadD, out=x);

    // Controlpath: Set Jump Condition
    And(a=instruction[2], b=true, out=lt0);
    And(a=instruction[1], b=true, out=eq0);
    And(a=instruction[0], b=true, out=gt0);

    // Controlpath: ALU Condition Flags
    Or(a=ng, b=zr, out=leq0);
    Not(in=leq0, out=ps);

    // Controlpath: Evaluate Jump Condition
    And(a=lt0, b=ng, out=jLt);
    And(a=eq0, b=zr, out=jEq);
    And(a=gt0, b=ps, out=jGt);

    Or3Way(in[0]=jLt, in[1]=jEq, in[2]=jGt, out=jump);
    And(a=insC, b=jump, out=exeJump);

    // Controlpath: Set Program Counter (ROM)
    PC(in=outA, load=exeJump, inc=true, reset=reset, request=request,
       out[0..14]=pc, kernel=kernel);
}
```

:::

**Two’s complement arithmetic**
All arithmetic is performed on 16-bit two’s complement values.

```text
0b 1 11 a c1 c2 c3 c4 c5 c6  d1 d2 d3  j1 j2 j3
   ^           comp            dest      jump
```

::: tip COMP

The `comp` field controls the ALU's computation.

The `a` bit selects the ALU's `y` input source:

- `a = 0` → `y = A-Register`
- `a = 1` → `y = RAM[A-Register]`

The `c1–c6` bits set flags that control the ALU’s internal pipeline:

- `c1`/`zx` → zero `x` input.
- `c2`/`nx` → negate `x` input.
- `c3`/`zy` → zero `y` input.
- `c4`/`ny` → negate `y` input.
- `c5`/`f` → select `add(x,y)` or `and(x,y)` function.
- `c6`/`no` → negate `result`.

::: details Comp Field

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

::: tip DEST

The `dest` field controls which storage element(s) (`A`,`M`,`D`) receive the ALU result.

::: details Dest Field

| dest   | d1  | d2  | d3  | effect                             |
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

::: tip JUMP

The `jump` field sets a condition that must be met for a jump to occur. To validate that condition
ALU flags are used:

- If ALU output is zero → `zr = 1`
- If ALU output is negative (two’s complement) → `ng = 1`

::: details Jump Field

| jump   | j1  | j2  | j3  | effect             |
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
