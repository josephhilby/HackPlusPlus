# Hack++ ALU (Arithmetic Logic Unit)

The Hack++ ALU is a 16-bit arithmetic/logic unit derived from the nand2tetris hardware platform. It takes two 
16-bit inputs (`x, y`) plus six control bits (`zx, nx, zy, ny, f, no`) and produces:
- `out[16]`: a 16-bit result
- `zr`: 1 iff `out` is `0b0000 0000 0000 0000`
- `ng`: 1 iff `out[15]` is `0b1` (i.e., negative number in two's complement)

This ALU design is intentionally minimal: rather than implementing many separate operations directly, it normalizes 
inputs with simple bitwise transforms, computes either AND or ADD, then optionally negates the result.

## Control Bits
The ALU is defined by the following deterministic pipeline:
1. Preprocess x
   - If `zx == 1` → set `x = 0`
   - If `nx == 1` → set `x = !x`

2. Preprocess y
   - If `zy == 1` → set `y = 0`
   - If `ny == 1` → set `y = !y`

3. Core operation select
   - If `f == 1` → out = `x + y` (16-bit two’s complement addition)
   - If `f == 0` → out = `x & y`

4. Optional output negate
   - If `no == 1` → `out = !out`

5. Flags
   - `zr = 1` iff all bits of `out` are 0
   - `ng = out[15]` (MSB)

This pipeline yields the standard Hack ALU repertoire: constants (`0, 1, −1`), passthroughs (`x, y`), unary ops 
(`!x`, `−x`, etc.), increments/decrements, add/subtract variants, and bitwise AND/OR.

## Implementation
```java
CHIP ALU {
IN  
x[16], y[16], // 16-bit inputs        
zx,           // zero the x input?
nx,           // negate the x input?
zy,           // zero the y input?
ny,           // negate the y input?
f,            // compute (out = x + y) or (out = x & y)?
no;           // negate the out output?

OUT
out[16],      // 16-bit output
zr,           // if (out == 0) equals 1, else 0
ng;           // if (out < 0)  equals 1, else 0

    PARTS:
    // x: zx, nx
    Mux16(a=x, b[0..15]=false, sel=zx, out=w0);
    Not16(in=w0, out=w1);
    Mux16(a=w0, b=w1, sel=nx, out=xin);

    // y: zy, ny
    Mux16(a=y, b[0..15]=false, sel=zy, out=w2);
    Not16(in=w2, out=w3);
    Mux16(a=w2, b=w3, sel=ny, out=yin);

    // f
    And16(a=xin, b=yin, out=and);
    Add16(a=xin, b=yin, out=add);
    Mux16(a=and, b=add, sel=f, out=result);

    // no, ng
    Not16(in=result, out=nResult);
    Mux16(a=result, b=nResult, sel=no, out[15]=ng, out[8..15]=msb, out[0..7]=lsb);

    // zr, out
    Or8Way(in=msb, out=w4);
    Or8Way(in=lsb, out=w5);
    Or(a=w4, b=w5, out=w6);
    Not(in=w6, out=zr);
    Or16(a[8..15]=msb, a[0..7]=lsb, b=false, out=out);
}
```