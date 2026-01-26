# Wide Combinational Gates

This section documents wide (multi-bit) combinational gates that operate on buses rather than single-bit signals.
These components extend the primitive logic gates into 16-bit datapath elements and reduction logic used throughout
the ALU, CPU, and memory system.

They form the architectural bridge between **bit-level logic** and **word-level computation**.

**Related:**

* [Primitive Gates](./01_primitives.md)
* [Routing & Control Gates](./03_routing.md)
* [Arithmetic Units](./04_arithmetic.md)
* [Processor Components](./07_processor.md)

---

## Design Notes

**Bitwise extension**
Wide gates apply a single-bit operation independently across each bit position of a multi-bit bus. This preserves bit
alignment while enabling word-level computation without introducing cross-bit dependencies.

**Datapath role**
These components appear directly in the Hack++ datapath:

* **ALU** — bitwise AND/OR and output negation (`no` control bit)
* **Flag generation** — reduction OR (`Or8Way`) for zero detection (`zr`)
* **Bus conditioning** — masking, merging, and intermediate value propagation

**Bit ordering (bus convention)**
Signals use a fixed indexing convention: `in[0]` is the least significant bit (LSB) and `in[15]` is the most
significant bit (MSB). This defines logical bit position, not memory endianness.

**Hierarchical construction**
All wide gates are constructed strictly from their single-bit equivalents, preserving the abstraction ladder:

`Not → Not16`
`And → And16`
`Or → Or16 → Or8Way`

---

## Gates

### Not16 — Bitwise Inverter

The **Not16 gate** performs a parallel bitwise inversion across a 16-bit input bus.

It is used in:

* ALU output negation (`no` control bit)
* Two’s complement formation (via downstream adders)
* General-purpose bus inversion and masking

**Also known as:** *Word inverter*, *Bitwise negator*

#### Behavior

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

---

### And16 — Bitwise Enable

The **And16 gate** computes a parallel logical AND across two 16-bit input buses.

It is used for:

* Masking intermediate ALU results
* Qualifying memory addresses
* Conditional propagation of data paths

**Also known as:** *Bus mask*, *Word enable*

#### Behavior

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

---

### Or16 — Bitwise Combine

The **Or16 gate** computes a parallel logical OR across two 16-bit input buses.

It is used for:

* Merging ALU intermediate results
* Combining bus sources
* Forming reduction trees for flag logic

**Also known as:** *Bus merge*, *Word combine*

#### Behavior

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

---

### Or8Way — Reduction OR

The **Or8Way gate** reduces an 8-bit bus to a single status bit by computing the logical OR of all inputs.

It is a core component of **flag generation**, most notably for computing the ALU’s zero flag (`zr`).

**Also known as:** *Reduction OR*, *Zero detector stage*

#### Behavior

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

---

## Architectural Context

Wide combinational gates form the **word-level surface of the datapath**.

- **ALU** uses `And16`, `Or16`, and `Not16` to implement its logical and negation stages
- **CPU control logic** uses `Or8Way` to collapse multi-bit ALU outputs into single-bit condition flags
- **Memory and bus systems** use these gates to mask, merge, and condition values as they traverse the machine

Together, these components translate *bitwise logic* into *architectural state transitions* that can be observed and controlled by the ISA.
