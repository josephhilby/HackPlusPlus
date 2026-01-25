/**
* 16-bit Not gate:
* for i = 0, ..., 15:
* out[i] = Not(a[i])
  */
  CHIP Not16 {
  IN in[16];
  OUT out[16];

  PARTS:
  Not(in=in[0], out=out[0]);
  Not(in=in[1], out=out[1]);
  Not(in=in[2], out=out[2]);
  Not(in=in[3], out=out[3]);
  Not(in=in[4], out=out[4]);
  Not(in=in[5], out=out[5]);
  Not(in=in[6], out=out[6]);
  Not(in=in[7], out=out[7]);
  Not(in=in[8], out=out[8]);
  Not(in=in[9], out=out[9]);
  Not(in=in[10], out=out[10]);
  Not(in=in[11], out=out[11]);
  Not(in=in[12], out=out[12]);
  Not(in=in[13], out=out[13]);
  Not(in=in[14], out=out[14]);
  Not(in=in[15], out=out[15]);
  }


/**
* 16-bit And gate:
* for i = 0, ..., 15:
* out[i] = a[i] And b[i]
  */
  CHIP And16 {
  IN a[16], b[16];
  OUT out[16];

  PARTS:
  And(a=a[0], b=b[0], out=out[0]);
  And(a=a[1], b=b[1], out=out[1]);
  And(a=a[2], b=b[2], out=out[2]);
  And(a=a[3], b=b[3], out=out[3]);
  And(a=a[4], b=b[4], out=out[4]);
  And(a=a[5], b=b[5], out=out[5]);
  And(a=a[6], b=b[6], out=out[6]);
  And(a=a[7], b=b[7], out=out[7]);
  And(a=a[8], b=b[8], out=out[8]);
  And(a=a[9], b=b[9], out=out[9]);
  And(a=a[10], b=b[10], out=out[10]);
  And(a=a[11], b=b[11], out=out[11]);
  And(a=a[12], b=b[12], out=out[12]);
  And(a=a[13], b=b[13], out=out[13]);
  And(a=a[14], b=b[14], out=out[14]);
  And(a=a[15], b=b[15], out=out[15]);
  }

/**
* 16-bit Or gate:
* for i = 0, ..., 15:
* out[i] = a[i] Or b[i]
  */
  CHIP Or16 {
  IN a[16], b[16];
  OUT out[16];

  PARTS:
  Or(a=a[0], b=b[0], out=out[0]);
  Or(a=a[1], b=b[1], out=out[1]);
  Or(a=a[2], b=b[2], out=out[2]);
  Or(a=a[3], b=b[3], out=out[3]);
  Or(a=a[4], b=b[4], out=out[4]);
  Or(a=a[5], b=b[5], out=out[5]);
  Or(a=a[6], b=b[6], out=out[6]);
  Or(a=a[7], b=b[7], out=out[7]);
  Or(a=a[8], b=b[8], out=out[8]);
  Or(a=a[9], b=b[9], out=out[9]);
  Or(a=a[10], b=b[10], out=out[10]);
  Or(a=a[11], b=b[11], out=out[11]);
  Or(a=a[12], b=b[12], out=out[12]);
  Or(a=a[13], b=b[13], out=out[13]);
  Or(a=a[14], b=b[14], out=out[14]);
  Or(a=a[15], b=b[15], out=out[15]);
  }

/**
* 8-way Or gate:
* out = in[0] Or in[1] Or ... Or in[7]
  */
  CHIP Or8Way {
  IN in[8];
  OUT out;

  PARTS:
  Or(a=in[0], b=in[1], out=or0);
  Or(a=in[2], b=in[3], out=or1);
  Or(a=in[4], b=in[5], out=or2);
  Or(a=in[6], b=in[7], out=or3);

  Or(a=or0, b=or1, out=or4);
  Or(a=or2, b=or3, out=or5);

  Or(a=or4, b=or5, out=out);
  }