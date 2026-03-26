# Boolean Logic Gates

This section documents the boolean logic gates that form the foundation of the Hack++. Every 
higher-level hardware circuits, or subsystem, of the computer — **the datapath, control unit, memory system, 
input devices, and output interfaces** — is ultimately a construction of various combinations of these gates. Signals 
are combined to datapaths, stored to represent memory, manipulated to perform arithmetic, and propagated 
to input and output devices.

## The Primitive Gates

While the primitives get used throughout increasingly complex circuits. They also retain their own distinct roles 
across the system:

* **NAND** establishes the universal basis from which all other digital logic can be constructed
* **NOT, AND, OR** implement the decision logic used throughout datapath routing
* **XOR** enables binary arithmetic, forming the core of the control unit

Together, these five gates represent the boundary where **boolean algebra becomes machine behavior**, enabling 
the construction of a programmable computing system.

### NAND — Universal Gate

> **Also known as:** *Functional complete primitive*

The **NAND gate** is the single primitive from which all combinational and sequential logic in 
Hack++ is constructed.

It computes the logical AND of two inputs and then inverts the result. Because NAND is functionally 
complete, every other gate in this system can be expressed as a composition of NAND gates.

::: tip NAND(a, b)

<NandGate />

:::

---

### NOT — Inverter Gate

> **Also known as:** *Negation*, *Logical complement*

The **NOT gate** performs signal inversion and is the core of bitwise negation, control-signal 
inversion, and two’s-complement arithmetic throughout the datapath and control logic.

It computes the inverse of its input.

::: details Definition

```hdl
CHIP Not {
    IN in;
    OUT out;

    PARTS:
    Nand(a=in, b=in, out=out);
}
```
:::

::: tip NOT(in)

<NotGate />

:::

---

### AND — Enable Gate

> **Also known as:** *Qualifier*

The **AND gate** controls enables by allowing a value to pass only when all conditions are asserted. It is widely 
used in: write-enable qualification, jump-condition evaluation, and masked datapath propagation.

::: details Definition
```hdl
CHIP And {
    IN a, b;
    OUT out;

    PARTS:
    Nand(a=a, b=b, out=nand);
    Not(in=nand, out=out);
}
```
:::

::: tip AND(a, b)

<AndGate />

:::

---

### OR — Combine Gate

> **Also known as:** *Signal combiner*, *Logical merge*

The **OR gate** aggregates multiple signal sources into a single logical result.

It is commonly used for: flag reduction (`zr`, jump conditions), multi-source control logic, and datapath signal 
merging.

*Note: Derived using De Morgan’s Law.*

::: details Definition
```hdl
CHIP Or {
    IN a, b;
    OUT out;

    PARTS:
    Not(in=a, out=na);
    Not(in=b, out=nb);
    Nand(a=na, b=nb, out=out);
}
```
:::

::: tip OR(a, b)

<OrGate />

:::

---

### XOR — Sum Gate

> **Also known as:** *Sum gate*, *Difference detector*

The **XOR gate** produces a high output when its inputs differ.

It is the fundamental building block of **binary addition**, forming the sum path in half-adders, full-adders, 
and the ALU’s arithmetic pipeline.

::: details Definition
```hdl
CHIP Xor {
    IN a, b;
    OUT out;

    PARTS:
    Or(a=a, b=b, out=or);
    Nand(a=a, b=b, out=nand);
    And(a=or, b=nand, out=out);
}
```
:::

::: tip XOR(a, b)

<XorGate />

:::

## The Word
With the above primitive gates established, the first abstraction layer is almost complete. The system now has the means
to move and process a single fundamental unit of 'data', a binary (`1` or `0`); however, processing any useful amount 
of data this way is extremely restrictive. So, if a single binary is not enough, how many binaries should the system 
group together to form a more workable fundamental unit of 'data'?

Ultimately, this becomes a compromise between capability and complexity. To keep things reasonably functional but still
(relatively) easily understandable, Hack++ will group 16 binaries to form what is called a **'word'**. This is now the limit
to how much information can move through the system at any one time. 

## The Wide Gates
With the word size set, the system can now expand its primitive gates and create appropriately sized wide (multi-bit) 
combinational gates. These will operate on buses rather than single-bit signals and form the architectural bridge 
between bit-level logic and word-level computation.

::: warning The Plan
All wide gates will be constructed strictly from their single-bit equivalents:

- `Not → Not16`
- `And → And16`
- `Or → Or16 → Or8Way`

Signals will use a fixed indexing convention: `in[0]` is the least significant bit (LSB) and `in[15]` is the most
significant bit (MSB). This will define logical bit position, not memory endianness.
:::

### Not16 — Bitwise Inverter

> **Also known as:** *Bitwise negator*, *Word inverter*

The **Not16 gate** performs a parallel bitwise inversion across a 16-bit input bus.

It is used in: ALU output negation (`no` control bit), two’s complement formation (via downstream adders), 
general-purpose bus inversion and masking

::: details Definition

```hdl
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
:::

---

### And16 — Bitwise Enable

> **Also known as:** *Bus mask*, *Word enable*

The **And16 gate** computes a parallel logical AND across two 16-bit input buses.

It is used for: masking intermediate ALU results, qualifying memory addresses, and conditional propagation of data 
paths.

::: details Definition
```hdl
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
:::

---

### Or16 — Bitwise Combine

> **Also known as:** *Bus merge*, *Word combine*

The **Or16 gate** computes a parallel logical OR across two 16-bit input buses.

It is used for: merging ALU intermediate results, combining bus sources, and forming reduction trees for flag logic.

::: details Definition

```hdl
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
:::

---

### Or8Way — Reduction OR

> **Also known as:** *Zero detector stage*

The **Or8Way gate** reduces an 8-bit bus to a single status bit by computing the logical OR of all inputs.

It is a core component of **flag generation**, most notably for computing the ALU’s zero flag (`zr`).

::: details Definition

```hdl
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
:::