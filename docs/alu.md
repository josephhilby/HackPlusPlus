/**
* ALU (Arithmetic Logic Unit):
* Computes out = one of the following functions:
*                0, 1, -1,
*                x, y, !x, !y, -x, -y,
*                x + 1, y + 1, x - 1, y - 1,
*                x + y, x - y, y - x,
*                x & y, x | y
* on the 16-bit inputs x, y,
* according to the input bits zx, nx, zy, ny, f, no.
* In addition, computes the two output bits:
* if (out == 0) zr = 1, else zr = 0
* if (out < 0)  ng = 1, else ng = 0
  */
  // Implementation: Manipulates the x and y inputs
  // and operates on the resulting values, as follows:
  // if (zx == 1) sets x = 0        // 16-bit constant
  // if (nx == 1) sets x = !x       // bitwise not
  // if (zy == 1) sets y = 0        // 16-bit constant
  // if (ny == 1) sets y = !y       // bitwise not
  // if (f == 1)  sets out = x + y  // integer 2's complement addition
  // if (f == 0)  sets out = x & y  // bitwise and
  // if (no == 1) sets out = !out   // bitwise not

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