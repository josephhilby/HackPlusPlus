# Hack++ CPU (Central Processing Unit)
The Hack++ CPU implements the nand2tetris Hack CPU contract. Each clock cycle it:
  - Fetches `instruction[16]` and (if needed) the memory operand `inM[16]` where `inM = RAM[A]`
  - Executes either an A-instruction (load a 15-bit constant into `A`) or a **C-instruction** (compute via the ALU, optionally store results, and optionally jump)
  - Drives the memory interface (`outM, writeM, addressM`) and the next instruction address (`pc`)

Outputs and timing model
  - `outM` and `writeM` are combinational (reflect the current instruction’s execution in the current cycle)
  - `addressM` (from `A`) and `pc` are clocked (their new values commit on the next tick)
  - If `reset == 1`, the program counter is forced to `0`

## Instruction Formats
**A-instruction (MSB = 0)**

Loads a 15-bit value into the A register:
  - `instruction[15] = 0`
  - `instruction[0..14]` → `A`

**C-instruction (MSB = 1)**

Controls ALU computation, destinations, and jump:
```java
1  1  1  a  c1 c2 c3 c4 c5 c6  d1 d2 d3  j1 j2 j3
^              comp              dest      jump
```
- `a`: selects ALU `y` input source (`A` vs `M`)
- `c1..c6`: ALU control bits (`zx,nx,zy,ny,f,no`)
- `d1 d2 d3`: destination enables (`A, D, M`)
- `j1 j2 j3`: jump condition (based on ALU flags `zr`, `ng`)

## Implementation
```java
CHIP CPU {

IN  inM[16],     // M value input  (M = contents of RAM[A])
instruction[16], // Instruction for execution
reset;           // Signals whether to re-start the current

OUT outM[16],    // M value output
writeM,          // Write to M?
addressM[15],    // Address in data memory (of M)
pc[15];          // address of next instruction

    PARTS:
    // Instruction: A or C
    And(a=instruction[15], b=true, out=insC);
    Mux16(a=instruction, b=ALUout, sel=insC, out=inA);

    // Control Bits: Comp
    Mux(a=false, b=instruction[12], sel=insC, out=readMem);
    Mux(a=false, b=instruction[11], sel=insC, out=zx);
    Mux(a=false, b=instruction[10], sel=insC, out=nx);
    Mux(a=false, b=instruction[9], sel=insC, out=zy);
    Mux(a=false, b=instruction[8], sel=insC, out=ny);
    Mux(a=false, b=instruction[7], sel=insC, out=f);
    Mux(a=false, b=instruction[6], sel=insC, out=no);

    // Control Bits: Dest
    Mux(a=true, b=instruction[5], sel=insC, out=loadA);
    Mux(a=false, b=instruction[4], sel=insC, out=loadD);
    Mux(a=false, b=instruction[3], sel=insC, out=writeM);

    // Control Bits: Jump
    Mux(a=false, b=instruction[2], sel=insC, out=lt0);
    Mux(a=false, b=instruction[1], sel=insC, out=eq0);
    Mux(a=false, b=instruction[0], sel=insC, out=gt0);

    // A Register
    ARegister(in=inA, load=loadA, out=outA, out[0..14]=addressM);

    // D Register
    DRegister(in=ALUout, load=loadD, out=x);

    // Jump Conditions
    Or(a=ng, b=zr, out=leq0);
    Not(in=leq0, out=ps);
    And(a=lt0, b=ng, out=jumpLt0);
    And(a=eq0, b=zr, out=jumpEq0);
    And(a=gt0, b=ps, out=jumpGt0);
    Or(a=jumpLt0, b=jumpEq0, out=jmp);
    Or(a=jmp, b=jumpGt0, out=jump);

    // PC
    PC(in=outA, load=jump, inc=true, reset=reset, out[0..14]=pc);

    // ALU
    Mux16(a=outA, b=inM, sel=readMem, out=y);
    ALU(x=x, y=y, zx=zx, nx=nx, zy=zy, ny=ny, f=f, no=no, out=ALUout, out=outM, zr=zr, ng=ng);
 }
```