## Basic Gates

### Multiplexer (Mux)
The MUX gate serves to use a selection (sel) input to select exactly one of two inputs (a, b).

#### Truth Table
a | b | sel | out
--|---|-----|----
0 | 0 | 0   | 0
0 | 1 | 0   | 0
1 | 0 | 0   | 1
1 | 1 | 0   | 1
0 | 0 | 1   | 0
0 | 1 | 1   | 1
1 | 0 | 1   | 0
1 | 1 | 1   | 1

#### HDL Implementation
To define our MUX we will look at the Sum of Products (SOP) from the truth table. This
results in `a ¬b ¬sel + a b ¬sel + ¬a b sel + a b sel`. Make the following substitutions
`p := (a ∧ ¬sel); q:= (b ∧ sel)`, and rewrite `(¬b ∧ p) v (b ∧ p) v (¬a ∧ q) v (a ∧ q)`. Follow
distributive laws `(p ∧ (b v ¬b)) v (q ∧ (a v ¬a))`, then negation `(p ∧ T) v (q ∧ T)`. Finally,
revert the substitution and end up with `(a ∧ ¬sel) v (b ∧ sel)`.

```c
CHIP Mux {
    IN a, b, sel;
    OUT out;

    PARTS:
    Not(in=sel, out=nsel);
    And(a=a, b=nsel, out=and0);
    And(a=b, b=sel, out=and1);
    Or(a=and0, b=and1, out=out);
}
```

```c
CHIP DMux {
    IN in, sel;
    OUT a, b;

    PARTS:
    Not(in=sel, out=nsel);
    And(a=nsel, b=in, out=a);
    And(a=sel, b=in, out=b);
}
```

/**
* 16-bit multiplexor:
* for i = 0, ..., 15:
* if (sel = 0) out[i] = a[i], else out[i] = b[i]
  */
  CHIP Mux16 {
  IN a[16], b[16], sel;
  OUT out[16];

  PARTS:
  Mux(a=a[0], b=b[0], sel=sel, out=out[0]);
  Mux(a=a[1], b=b[1], sel=sel, out=out[1]);
  Mux(a=a[2], b=b[2], sel=sel, out=out[2]);
  Mux(a=a[3], b=b[3], sel=sel, out=out[3]);
  Mux(a=a[4], b=b[4], sel=sel, out=out[4]);
  Mux(a=a[5], b=b[5], sel=sel, out=out[5]);
  Mux(a=a[6], b=b[6], sel=sel, out=out[6]);
  Mux(a=a[7], b=b[7], sel=sel, out=out[7]);
  Mux(a=a[8], b=b[8], sel=sel, out=out[8]);
  Mux(a=a[9], b=b[9], sel=sel, out=out[9]);
  Mux(a=a[10], b=b[10], sel=sel, out=out[10]);
  Mux(a=a[11], b=b[11], sel=sel, out=out[11]);
  Mux(a=a[12], b=b[12], sel=sel, out=out[12]);
  Mux(a=a[13], b=b[13], sel=sel, out=out[13]);
  Mux(a=a[14], b=b[14], sel=sel, out=out[14]);
  Mux(a=a[15], b=b[15], sel=sel, out=out[15]);
  }

/**
* Note: Values in little-endian
* 4-way 16-bit multiplexor:
* out = a if sel = 00
*       b if sel = 01
*       c if sel = 10
*       d if sel = 11
*/
CHIP Mux4Way16 {
IN a[16], b[16], c[16], d[16], sel[2];
OUT out[16];

    PARTS:
    Mux16(a=a, b=b, sel=sel[0], out=muxAB);
    Mux16(a=c, b=d, sel=sel[0], out=muxCD);
    Mux16(a=muxAB, b=muxCD, sel=sel[1], out=out);
}

/**
* Note: Values in little-endian
* 8-way 16-bit multiplexor:
* out = a if sel = 000
*       b if sel = 001
*       c if sel = 010
*       d if sel = 011
*       e if sel = 100
*       f if sel = 101
*       g if sel = 110
*       h if sel = 111
*/
CHIP Mux8Way16 {
IN a[16], b[16], c[16], d[16],
e[16], f[16], g[16], h[16],
sel[3];
OUT out[16];

    PARTS:
    Mux4Way16(a=a, b=b, c=c, d=d, sel=sel[0..1], out=muxABCD);
    Mux4Way16(a=e, b=f, c=g, d=h, sel=sel[0..1], out=muxEFGH);
    Mux16(a=muxABCD, b=muxEFGH, sel=sel[2], out=out);
}

/**
* 4-way demultiplexor:
* [a, b, c, d] = [in, 0, 0, 0] if sel = 00
*                [0, in, 0, 0] if sel = 01
*                [0, 0, in, 0] if sel = 10
*                [0, 0, 0, in] if sel = 11
*/
CHIP DMux4Way {
IN in, sel[2];
OUT a, b, c, d;

    PARTS:
    DMux(in=in, sel=sel[1], a=dMuxAB, b=dMuxCD);
    DMux(in=dMuxAB, sel=sel[0], a=a, b=b);
    DMux(in=dMuxCD, sel=sel[0], a=c, b=d);
}

/**
* 8-way demultiplexor:
* [a, b, c, d, e, f, g, h] = [in, 0,  0,  0,  0,  0,  0,  0] if sel = 000
*                            [0, in,  0,  0,  0,  0,  0,  0] if sel = 001
*                            [0,  0, in,  0,  0,  0,  0,  0] if sel = 010
*                            [0,  0,  0, in,  0,  0,  0,  0] if sel = 011
*                            [0,  0,  0,  0, in,  0,  0,  0] if sel = 100
*                            [0,  0,  0,  0,  0, in,  0,  0] if sel = 101
*                            [0,  0,  0,  0,  0,  0, in,  0] if sel = 110
*                            [0,  0,  0,  0,  0,  0,  0, in] if sel = 111
*/
CHIP DMux8Way {
IN in, sel[3];
OUT a, b, c, d, e, f, g, h;

    PARTS:
    DMux(in=in, sel=sel[2], a=dMuxABCD, b=dMuxEFGH);
    DMux4Way(in=dMuxABCD, sel=sel[0..1], a=a, b=b, c=c, d=d);
    DMux4Way(in=dMuxEFGH, sel=sel[0..1], a=e, b=f, c=g, d=h);
}