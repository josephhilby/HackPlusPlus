/**
* The Hack Central Processing unit (CPU).
* Parses the binary code in the instruction input and executes it according to the
* Hack machine language specification. In the case of a C-instruction, computes the
* function specified by the instruction. If the instruction specifies to read a memory
* value, the inM input is expected to contain this value. If the instruction specifies
* to write a value to the memory, sets the outM output to this value, sets the addressM
* output to the target address, and asserts the writeM output (when writeM = 0, any
* value may appear in outM).
* If the reset input is 0, computes the address of the next instruction and sets the
* pc output to that value. If the reset input is 1, sets pc to 0.
* Note: The outM and writeM outputs are combinational: they are affected by the
* instruction's execution during the current cycle. The addressM and pc outputs are
* clocked: although they are affected by the instruction's execution, they commit to
* their new values only in the next cycle.
  */
  CHIP CPU {

  IN  inM[16],         // M value input  (M = contents of RAM[A])
  instruction[16], // Instruction for execution
  reset;           // Signals whether to re-start the current
  // program (reset==1) or continue executing
  // the current program (reset==0).

  OUT outM[16],        // M value output
  writeM,          // Write to M?
  addressM[15],    // Address in data memory (of M)
  pc[15];          // address of next instruction

  // Instruction A: (MSB = 0)
  //     value load in reg A (0bbb bbbb bbbb bbbb)
  //
  // Instruction C: (MSB = 1)
  //                        1
  //                5 43 2 109876 543 210
  //     ctrl inst (1 xx a cccccc ddd jjj)
  //         - xx: ignored (usually 11)
  //         - a: ALUin (y) from outA or inM
  //         - cccccc: what func ALU will compute (zx, nx, zy, ny, f, no)
  //         - ddd: what should accept ALUout (A reg, D reg, MEM)
  //         - jjj (zr, ng): what inst to fetch next (addr in reg A)
  //             - 100: comp < 0 -> jump
  //             - 010: comp = 0 -> jump
  //             - 001: comp > 0 -> jump

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