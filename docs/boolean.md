# Boolean Logic Gates

This section documents the boolean logic gates that form the foundation of the Hack++ hardware suite. Every other hardware
abstraction — circuits, modules, or subsystems — is ultimately just a composition of these gates, which in turn are just
various arrangements of the universal primitive 'NAND.' At a high-level these gates come in two distinct types, based on the unit of
data that they operate on.

::: info Gate Types

- **Primitive Gates** operate on bits.
- **Wide Gates** operate on words.
  :::

## The Word

Before jumping into describing the gate types it is important to understand the concept that drives their primary distinction, the word.

Imagine a system with only primitive gates; such a system could only move and process a single bit (`1` or `0`) of data
at any given time. As you could imagine, this limitation would be extremely restrictive for any meaningful computation.

Instead, computers group multiple bits together into a **word**, creating a wider fundamental unit of data to operate on. The
size of a word is a design choice, balancing capability against complexity. In Hack++, that balance resulted in a word
size of 16 bits.

This sets the width of the Datapath. It means that at most 16 bits of data can move through the system in a single operation
(or clock cycle). The effects of this decision will ripple throughout the remaining hardware and software design, determining the
size of registers, ALU data limits, memory values, total addressable space, and primitive data types. In this way, the word size shapes
the overall architecture of the machine.

## The Primitive Gates

Even though the primitive gates are aggregated into increasingly complex components, they also retain their own distinct conceptual
roles across the system. At a high level, they can be viewed as implementing the following tasks:

::: info Primitive Roles

- **NAND** establishes the universal basis from which all other digital logic can be constructed.
- **NOT, AND, OR** implement decision and routing logic used throughout the Datapath and Control Unit.
- **XOR** enables binary arithmetic, forming the core of the ALU.
  :::

Together, these gates — in conjunction with the data flip-flop (DFF) discussed in _Sequential Circuits_ — represent
the boundary where mathematics becomes machine behavior.

### NAND — Universal Gate

> **Also known as:** _Functionally complete primitive_

The **NAND gate** is the single primitive from which all combinational and sequential logic in
Hack++ is constructed.

It computes the logical AND of two inputs and then inverts the result. Because NAND is functionally
complete, every other gate in this system can be expressed as a composition of NAND gates.

::: tip NAND(a, b)

<NandGate />

:::

---

### NOT — Inverter Gate

> **Also known as:** _Negation_, _Logical complement_

The **NOT gate** performs signal inversion and is the core of bitwise negation, control-signal
inversion, and two’s-complement arithmetic throughout the datapath and control logic.

::: details Hardware Description

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

> **Also known as:** _Qualifier_

The **AND gate** controls enables by allowing a value to pass only when all conditions are asserted. It is widely
used in: write-enable qualification, jump-condition evaluation, and masked datapath propagation.

::: details Hardware Description

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

> **Also known as:** _Signal combiner_, _Logical merge_

The **OR gate** aggregates multiple signal sources into a single logical result.

It is commonly used for: flag reduction (`zr`, jump conditions), multi-source control logic, and datapath signal
merging.

_Note: Derived using De Morgan’s Law._

::: details Hardware Description

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

> **Also known as:** _Sum gate_, _Difference detector_

The **XOR gate** produces a high output when its inputs differ.

It is the fundamental building block of **binary addition**, forming the sum path in half-adders, full-adders,
and the ALU’s arithmetic pipeline.

::: details Hardware Description

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

## The Wide Gates

With the word size set, the system can now expand its Datapath by combining primitive gates to create wide (multi-bit)
combinational gates. These will operate on word-sized buses rather than single-bit signals and form the architectural bridge
between bit-level logic and word-level computation.

::: warning The Plan
All wide gates will be constructed strictly from their single-bit equivalents:

- `Not → Not16`
- `And → And16`
- `Or → { Or16, Or16Way }`

**Bit ordering (bus convention)**
Declarations define **width** (e.g., `in[16]` is 16 bits wide), while usage defines **index**.
Signals use a 0-indexed convention: `in[0]` is the least significant bit (LSB) and `in[15]` is the most
significant bit (MSB). This is keeping with the nand2tetris hdl convention, not a memory endianness rule.
:::

### Not16 — Bitwise Inverter

> **Also known as:** _Bitwise negator_, _Word inverter_

The **Not16 gate** performs a parallel bitwise inversion across a 16-bit input bus.

It is used in: ALU output negation (`no` control bit), two’s complement formation (via downstream adders),
general-purpose bus inversion and masking

::: details Hardware Description

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

::: tip NOT16(wordA)
<WideGateDemo gate="Not16" />
:::

---

### And16 — Bitwise Enable

> **Also known as:** _Bus mask_, _Word enable_

The **And16 gate** computes a parallel logical AND across two 16-bit input buses.

It is used for: masking intermediate ALU results, qualifying memory addresses, and conditional propagation of data
paths.

::: details Hardware Description

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

::: tip AND16(wordA, wordB)
<WideGateDemo gate="And16" />
:::

### Or16 — Bitwise Combine

> **Also known as:** _Bus merge_, _Word combine_

The **Or16 gate** computes a parallel logical OR across two 16-bit input buses.

It is used for: merging ALU intermediate results, combining bus sources, and forming reduction trees for flag logic.

::: details Hardware Description

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

::: tip OR16(wordA, wordB)
<WideGateDemo gate="Or16" />
:::

---

### Or3/16Way — Reduction OR

> **Also known as:** _Non-zero detector_

The **Or3Way gate** reduces 3-bits to a single status bit by computing the logical OR of all inputs.

It is a core component of **CPU flag generation**, most notably for helping the CPU set the PC’s load flag.

The **Or16Way gate** reduces a 16-bit bus to a single status bit by computing the logical OR of all inputs.

It is a core component of **ALU flag generation**, most notably for helping compute the zero flag (`zr`).

::: warning Change HACK to HACK++
In the nand2tetris system this gate is made to be an `Or8Way gate`. However, the only use of this gate, in
hardware, is in setting the ALU's zero flag and CPU's jump condition. Because of this, the gate has been
split into two and made as efficient as possible to allow for faster processing.

Due to the simplicity of these circuits only the `Or3Way gate` demo will be provided.
:::

::: details Hardware Description

```hdl
CHIP Or3Way {
    IN in[3];
    OUT out;

    PARTS:
    Or(a=in[0],  b=in[1],  out=or0);
    Or(a=in[2], b=or0, out=out);
}
```

:::

::: details Hardware Description

```hdl
CHIP Or16Way {
    IN in[16];
    OUT out;

    PARTS:
    Or(a=in[0],  b=in[1],  out=or0);
    Or(a=in[2],  b=in[3],  out=or1);
    Or(a=in[4],  b=in[5],  out=or2);
    Or(a=in[6],  b=in[7],  out=or3);
    Or(a=in[8],  b=in[9],  out=or4);
    Or(a=in[10], b=in[11], out=or5);
    Or(a=in[12], b=in[13], out=or6);
    Or(a=in[14], b=in[15], out=or7);

    Or(a=or0, b=or1, out=or00);
    Or(a=or2, b=or3, out=or01);
    Or(a=or4, b=or5, out=or02);
    Or(a=or6, b=or7, out=or03);

    Or(a=or00, b=or01, out=or000);
    Or(a=or02, b=or03, out=or001);

    Or(a=or000, b=or001, out=out);
}
```

:::

::: tip OR3WAY(in)

<ReductionGateDemo />

:::
