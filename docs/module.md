# Functional Modules, (in progress)

## Data Modules

### Random Access Memory — RAM16K & MMIO

::: details Hardware Description

```hdl
CHIP Memory {
    IN in[16], load, address[15];
    OUT out[16];

    PARTS:
    // Select the write region
    DMux4Way(in=load, sel=address[13..14],
             a=mem1, b=mem2, c=scr, d=key);

    // Combine the two write quadrants into the single RAM16K
    Or(a=mem1, b=mem2, out=mem);

    // RAM (0x0000–0x3FFF); sel = { 0b00, 0b01 }
    RAM16K(in=in, load=mem, address=address[0..13], out=memOut);

    // MMIO
    // Screen (0x4000–0x5FFF); sel = 0b10
    Screen(in=in, load=scr, address=address[0..12], out=scrOut);

    // Keyboard (0x6000); sel = 0b11
    Keyboard(out=keyOut);

    // Select the read region
    Mux4Way16(a=memOut, b=memOut, c=scrOut, d=keyOut,
              sel=address[13..14], out=out);
}
```

:::

### Arithmetic and Logic Unit — ALU

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

## Control Modules

### Read Only Memory — ROM

::: details Hardware Description

```hdl
CHIP ROM16K {
    IN address[14];
    OUT out[16];

    PARTS:
    // implementation is omitted because it relies on
    // a pre-loaded hardware state rather than clocked flip-flops.
}
```

---

```hdl
CHIP ROM32K {
    IN address[15];
    OUT out[16];

    PARTS:
    ROM16K(address=address[0..13], out=out0);
    ROM16K(address=address[0..13], out=out1);

    Mux16(a=out0, b=out1,
          sel=address[14], out=out);
}
```

---

```hdl
CHIP Instruction {
    IN address[16], sel;
    OUT out[16];

    ROM16K(address=address[0..13], out=instructionOS);
    ROM32K(address=address, out=instructionProg);
    MUX16(a=instructionProg, b=instructionOS, sel=kernel, out=instruction);
}
```

:::

### Central Processing Unit — CPU

::: details Hardware Description

```hdl
CHIP CPU {
    IN  inM[16], instruction[16], reset;
    OUT outM[16], writeM, addressM[15], pc[15], kernel;

    PARTS:
    // A-Instruction Handling
    Not(in=instruction[15], out=isA);
    Mux16(a=instruction, b=ALUout, sel=instruction[15], out=inA);
    ARegister(in=inA, load=loadA, out=outA, out[0..14]=addressM);

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
    DRegister(in=ALUout, load=loadD, out=x);

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

::: details Hardware Interface Definition

```hdl
CHIP Computer {
    IN reset;

    PARTS:
    // Central Processing Unit
    CPU(reset=reset,
        inD=inD, writeD=load, addrD=addrD, outD=outD,
        inI=inI, kern=domain, outI=addrI);

    // Data + Memory-Mapped I/O (RAM)
    Memory(in=outD, load=load, address=addrD, out=inD);

    // Application or OS (ROM)
    Instruction(in=addrI, sel=domain, out=inI);
}
```

:::
