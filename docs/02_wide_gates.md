# Wide Combinational Gates
This section documents wide (multi-bit) combinational gates that operate on buses rather than single-bit signals. 
These components extend the primitive logic gates into 16-bit datapath elements and reduction logic used throughout 
the ALU, CPU, and memory system.

They form the bridge between bit-level logic and word-level computation.

**Related:**
- [Primitive Gates](./01_primitives.md)
- [Routing & Control Gates](./03_routing.md)
- [Arithmetic Units](./04_arithmetic.md)
- [Processor Components](./07_processor.md)

## Design Notes
**Bitwise extension**

These gates apply a single-bit operation across each bit position of a multi-bit bus, preserving bit alignment 
and enabling word-level computation.

**Datapath role**

Wide gates are used extensively in:
- The ALU (bitwise AND/OR, output negation)
- Flag generation (Or8Way for zero detection)
- Bus conditioning and masking

**Little Endian**

Data is stored in little-endian format such that the input for `gate16`, the most significant bit (MSB) will be `in[15]`,
and the least significant bit (LSB) will be `in[0]`.

## Gates
### Not16 — Bitwise Inverter
The **Not16 gate** is used to negate the output of the ALU (`no` control bit) and general-purpose signal inversion 
across 16-bit buses.

It inverts each bit of a 16-bit input bus independently.

#### Logic
```text 
For i = 0..15:
    out[i] = ¬in[i]
```

#### HDL
```java 
CHIP Not16 {
IN in[16];
OUT out[16];

    PARTS:
    // LSB
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
    
    // MSB
    Not(in=in[15], out=out[15]);
}
```

### And16 — Bitwise Enable
The **And16 gate** is used for masking and conditional propagation in the ALU and memory addressing logic.

It computes the logical AND across two 16-bit input buses, bit by bit.

#### Logic
```text
For i = 0..15:
    out[i] = a[i] ∧ b[i]
```

#### HDL
```java 
CHIP And16 {
IN a[16], b[16];
OUT out[16];

    PARTS:
    // LSB
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
    
    // MSB
    And(a=a[15], b=b[15], out=out[15]);
}
```
### Or16 — Bitwise Combine
The **Or16 gate** is used to merge data busses and intermediate results in teh ALU and 
control logic.

It computes the logical OR across two 16-bit input buses, bit by bit.

#### Logic
```text
For i = 0..15:
    out[i] = a[i] ∨ b[i]
```

#### HDL
```java
CHIP Or16 {
IN a[16], b[16];
OUT out[16];

    PARTS:
    // LSB
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
    
    // MSB
    Or(a=a[15], b=b[15], out=out[15]);
}
```

### Or8Way — Reduction OR
The **Or8Way gate** is used for computing the ALU flag `zr` by detecting if any bit
in the output is nonzero.

It computes a logical OR across all bits of an 8-bit bus, producing a single bit output.

#### Logic
```text
out = in[0] ∨ in[1] ∨ in[2] ∨ in[3] ∨ in[4] ∨ in[5] ∨ in[6] ∨ in[7]
```

#### HDL
```java 
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
```