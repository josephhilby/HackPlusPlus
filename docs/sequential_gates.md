/**
* 1-bit register:
* If load is asserted, the register's value is set to in;
* Otherwise, the register maintains its current value:
* if (load(t)) out(t+1) = in(t), else out(t+1) = out(t)
  */
  CHIP Bit {
  IN in, load;
  OUT out;

  PARTS:
  Mux(a=dff, b=in, sel=load, out=mux);
  DFF(in=mux, out=dff, out=out);
  }

/**
* 16-bit register:
* If load is asserted, the register's value is set to in;
* Otherwise, the register maintains its current value:
* if (load(t)) out(t+1) = int(t), else out(t+1) = out(t)
  */
  CHIP Register {
  IN in[16], load;
  OUT out[16];

  PARTS:
  Bit(in=in[0], load=load, out=out[0]);
  Bit(in=in[1], load=load, out=out[1]);
  Bit(in=in[2], load=load, out=out[2]);
  Bit(in=in[3], load=load, out=out[3]);
  Bit(in=in[4], load=load, out=out[4]);
  Bit(in=in[5], load=load, out=out[5]);
  Bit(in=in[6], load=load, out=out[6]);
  Bit(in=in[7], load=load, out=out[7]);
  Bit(in=in[8], load=load, out=out[8]);
  Bit(in=in[9], load=load, out=out[9]);
  Bit(in=in[10], load=load, out=out[10]);
  Bit(in=in[11], load=load, out=out[11]);
  Bit(in=in[12], load=load, out=out[12]);
  Bit(in=in[13], load=load, out=out[13]);
  Bit(in=in[14], load=load, out=out[14]);
  Bit(in=in[15], load=load, out=out[15]);
  }

/**
* Memory of eight 16-bit registers.
* If load is asserted, the value of the register selected by
* address is set to in; Otherwise, the value does not change.
* The value of the selected register is emitted by out.
  */
  CHIP RAM8 {
  IN in[16], load, address[3];
  OUT out[16];

  PARTS:
  DMux8Way(in=load, sel=address, a=in0, b=in1, c=in2, d=in3, e=in4, f=in5, g=in6, h=in7);
  Register(in=in, load=in0, out=out0);
  Register(in=in, load=in1, out=out1);
  Register(in=in, load=in2, out=out2);
  Register(in=in, load=in3, out=out3);
  Register(in=in, load=in4, out=out4);
  Register(in=in, load=in5, out=out5);
  Register(in=in, load=in6, out=out6);
  Register(in=in, load=in7, out=out7);
  Mux8Way16(a=out0, b=out1, c=out2, d=out3, e=out4, f=out5, g=out6, h=out7, sel=address, out=out);
  }

/**
* Memory of sixty four 16-bit registers.
* If load is asserted, the value of the register selected by
* address is set to in; Otherwise, the value does not change.
* The value of the selected register is emitted by out.
  */
  CHIP RAM64 {
  IN in[16], load, address[6];
  OUT out[16];

  PARTS:
  DMux8Way(in=load, sel=address[3..5], a=in0, b=in1, c=in2, d=in3, e=in4, f=in5, g=in6, h=in7);
  RAM8(in=in, load=in0, address=address[0..2], out=out0);
  RAM8(in=in, load=in1, address=address[0..2], out=out1);
  RAM8(in=in, load=in2, address=address[0..2], out=out2);
  RAM8(in=in, load=in3, address=address[0..2], out=out3);
  RAM8(in=in, load=in4, address=address[0..2], out=out4);
  RAM8(in=in, load=in5, address=address[0..2], out=out5);
  RAM8(in=in, load=in6, address=address[0..2], out=out6);
  RAM8(in=in, load=in7, address=address[0..2], out=out7);
  Mux8Way16(a=out0, b=out1, c=out2, d=out3, e=out4, f=out5, g=out6, h=out7, sel=address[3..5], out=out);
  }

/**
* Memory of 512 16-bit registers.
* If load is asserted, the value of the register selected by
* address is set to in; Otherwise, the value does not change.
* The value of the selected register is emitted by out.
  */
  CHIP RAM512 {
  IN in[16], load, address[9];
  OUT out[16];

  PARTS:
  DMux8Way(in=load, sel=address[6..8], a=in0, b=in1, c=in2, d=in3, e=in4, f=in5, g=in6, h=in7);
  RAM64(in=in, load=in0, address=address[0..5], out=out0);
  RAM64(in=in, load=in1, address=address[0..5], out=out1);
  RAM64(in=in, load=in2, address=address[0..5], out=out2);
  RAM64(in=in, load=in3, address=address[0..5], out=out3);
  RAM64(in=in, load=in4, address=address[0..5], out=out4);
  RAM64(in=in, load=in5, address=address[0..5], out=out5);
  RAM64(in=in, load=in6, address=address[0..5], out=out6);
  RAM64(in=in, load=in7, address=address[0..5], out=out7);
  Mux8Way16(a=out0, b=out1, c=out2, d=out3, e=out4, f=out5, g=out6, h=out7, sel=address[6..8], out=out);
  }

/**
* Memory of 4K 16-bit registers.
* If load is asserted, the value of the register selected by
* address is set to in; Otherwise, the value does not change.
* The value of the selected register is emitted by out.
  */
  CHIP RAM4K {
  IN in[16], load, address[12];
  OUT out[16];

  PARTS:
  DMux8Way(in=load, sel=address[9..11], a=in0, b=in1, c=in2, d=in3, e=in4, f=in5, g=in6, h=in7);
  RAM512(in=in, load=in0, address=address[0..8], out=out0);
  RAM512(in=in, load=in1, address=address[0..8], out=out1);
  RAM512(in=in, load=in2, address=address[0..8], out=out2);
  RAM512(in=in, load=in3, address=address[0..8], out=out3);
  RAM512(in=in, load=in4, address=address[0..8], out=out4);
  RAM512(in=in, load=in5, address=address[0..8], out=out5);
  RAM512(in=in, load=in6, address=address[0..8], out=out6);
  RAM512(in=in, load=in7, address=address[0..8], out=out7);
  Mux8Way16(a=out0, b=out1, c=out2, d=out3, e=out4, f=out5, g=out6, h=out7, sel=address[9..11], out=out);
  }

/**
* Memory of 16K 16-bit registers.
* If load is asserted, the value of the register selected by
* address is set to in; Otherwise, the value does not change.
* The value of the selected register is emitted by out.
  */
  CHIP RAM16K {
  IN in[16], load, address[14];
  OUT out[16];

  PARTS:
  DMux8Way(in=load, sel=address[11..13], a=in0, b=in1, c=in2, d=in3, e=in4, f=in5, g=in6, h=in7);
  RAM4K(in=in, load=in0, address=address[0..11], out=out0);
  RAM4K(in=in, load=in1, address=address[0..11], out=out1);
  RAM4K(in=in, load=in2, address=address[0..11], out=out2);
  RAM4K(in=in, load=in3, address=address[0..11], out=out3);
  RAM4K(in=in, load=in4, address=address[0..11], out=out4);
  RAM4K(in=in, load=in5, address=address[0..11], out=out5);
  RAM4K(in=in, load=in6, address=address[0..11], out=out6);
  RAM4K(in=in, load=in7, address=address[0..11], out=out7);
  Mux8Way16(a=out0, b=out1, c=out2, d=out3, e=out4, f=out5, g=out6, h=out7, sel=address[11..13], out=out);
  }
