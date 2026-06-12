# Modules

Continuing this top down deconstruction, we arrive at the hardware subsystems modules. As before these will be subdivided into
smaller entities (components) and an overall architectural specification will be selected to help solidify separation of concerns
and module interface.

<SystemHierarchy dataType="hardware" />

## Hardware Subsystem

As discussed in the last section the Hack++ hardware follows the Harvard model. From this model we derive our three modules:
`Memory()`, `Instruction()`, and `CPU()`.

### Interface

#### The A-Instruction

```
0b 0 vvv vvvv vvvv vvvv
   ^      address
```

#### The C-Instruction

```
0b 1 11 a c1 c2 c3 c4 c5 c6 d1 d2 d3 j1 j2 j3
   ^           comp           dest     jump
```

## Memory Module

From the computers perspective, the CPU observes a **single, flat, 15-bit address space**. The physical routing is
handled within the `Memory()` subsystem by the highest 2-bits `address[13..14]` allowing the module to access and
address `4x 8K` sections.

::: details Hardware Description

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

### Memory Map

Of these addressable sections two are combined into a single `16K` section for RAM, one is used for
the screen MMIO, and the last is a single address for the keyboard MMIO.

| Address Range (Hex) | address[13..14] | Size   | Region   | Function                    |
| ------------------- | --------------: | ------ | -------- | --------------------------- |
| `0x0000–0x1FFF`     |          `0b00` | 16K    | RAM16    | General-purpose data memory |
| `0x2000–0x3FFF`     |          `0b01` | 16K    | RAM16    | General-purpose data memory |
| `0x4000–0x5FFF`     |          `0b10` | 8K     | Screen   | Display framebuffer         |
| `0x6000`            |          `0b11` | 1 word | Keyboard | Input register              |
| `> 0x6000`          |          `0b11` | —      | Invalid  | Ignored (reads return `0`)  |

## Instruction Module

::: details Hardware Description

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

### Privilege Levels

## CPU Module

Single-Cycle Deterministic Controller

::: details Hardware Description

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

### Single Cycle Deterministic
