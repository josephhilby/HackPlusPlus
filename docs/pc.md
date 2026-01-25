/**
* A 16-bit counter.
* if      reset(t): out(t+1) = 0
* else if load(t):  out(t+1) = in(t)
* else if inc(t):   out(t+1) = out(t) + 1
* else              out(t+1) = out(t)
  */
  CHIP PC {
  IN in[16], reset, load, inc;
  OUT out[16];

  PARTS:
  Register(in=result, load=true, out=count, out=out);

  Inc16(in=count, out=w0);
  Mux16(a=count, b=w0, sel=inc, out=w1);
  Mux16(a=w1, b=in, sel=load, out=w2);
  Mux16(a=w2, b[0..15]=false, sel=reset, out=result);
  }
