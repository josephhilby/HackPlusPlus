# Circuits

This section documents the combinational circuits used by Hack++ for data routing and arithmetic operations. Unlike
the previous sections primitive gates, which implement individual boolean operations, these components exist to perform
specific tasks required by higher levels of the abstraction ladder.

Combinational circuits operate only on their current inputs and produce outputs without depending on any previous state,
forming the building blocks for datapaths, control logic, and instruction decoding.

::: warning The Plan
Again, these single-bit circuits will be combined to accommodate word size computations. All wide routing circuits are
built strictly from their single-bit equivalents, preserving the abstraction ladder:

- Routing Circuits:
  - `Mux → Mux16 → Mux4Way16 → Mux8Way16`
  - `DMux → DMux4Way → DMux8Way`
  - `SysCall15`
- Arithmetic Circuits:
  - `HalfAdder → FullAdder → Add16`

**Bit ordering (bus convention)**
Declarations define **width** (e.g., `in[16]` is 16 bits wide), while usage defines **index**.
Signals use a 0-indexed convention: `in[0]` is the least significant bit (LSB) and `in[15]` is the most
significant bit (MSB). This is keeping with the nand2tetris hdl convention, not a memory endianness rule.
:::

## Combinational Circuits

### Routing Circuits

These components do not compute new values; instead, they **select**, **fan in/out**, and **qualify**
existing signals, forming the backbone of instruction decoding, register loading, memory writes, and control flow.

::: warning Definition

- **Fan-In**: Multiplexers (MUX), many sources → one destination
- **Fan-Out**: Demultiplexers (DMUX), one source → many destinations
  :::

#### MUX — Selector Circuit

> **Also known as:** _Selector_, _Data switch_

The **Multiplexer (MUX)** selects exactly one of two inputs based on a single control signal.

It is the fundamental building block of instruction decoding, ALU input selection, and register loading.

::: details Hardware Description

```hdl
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

:::

::: tip MUX(a, b, sel)
<MuxCircuit />
:::

---

#### Mux16 — Bus Selector

The **Mux16 circuit** extends the single-bit MUX across a 16-bit bus, allowing entire words to be conditionally selected.

It is heavily used in:

- ALU input selection (`A` vs `M`)
- Instruction decoding
- PC source selection

::: details Hardware Description

```hdl
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
```

:::

::: tip MUX16(wordA, wordB, sel)
<MuxWay16Demo :ways="2" />
:::

---

#### Mux4Way16 — Word Selector (4-to-1)

The **Mux4Way16 gate** selects one of four 16-bit inputs using a 2-bit control signal.

It is typically used in:

- Multi-source bus arbitration
- ROM and memory bank selection

::: details Hardware Description

```hdl
CHIP Mux4Way16 {
    IN a[16], b[16], c[16], d[16], sel[2];
    OUT out[16];

    PARTS:
    Mux16(a=a, b=b, sel=sel[0], out=muxAB);
    Mux16(a=c, b=d, sel=sel[0], out=muxCD);
    Mux16(a=muxAB, b=muxCD, sel=sel[1], out=out);
}
```

:::

::: tip MUX4WAY16(wordA, wordB, wordC, wordD, sel)
<MuxWay16Demo :ways="4" />
:::

---

#### Mux8Way16 — Word Selector (8-to-1)

The **Mux8Way16 gate** selects one of eight 16-bit inputs using a 3-bit control signal.

It forms the basis of hierarchical bus selection and large fan-in datapaths.

::: details Hardware Description

```hdl
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
```

:::

::: tip MUX4WAY16(wordA, wordB, wordC, wordD, wordE, wordF, wordG, wordH, sel)
<MuxWay16Demo :ways="8" />
:::

---

#### DMUX — Distributor Circuit

> **Also known as:** _Distributor_, _Write decoder_

The **Demultiplexer (DMUX)** routes a single input to exactly one of two outputs based on a control signal.

It is used to implement **write enables**, **register selection**, and **memory-mapped output routing**.

::: details Hardware Description

```hdl
CHIP DMux {
    IN in, sel;
    OUT a, b;

    PARTS:
    Not(in=sel, out=nsel);
    And(a=nsel, b=in, out=a);
    And(a=sel, b=in, out=b);
}
```

:::

::: tip DMUX(in, sel)
<DMuxCircuit />
:::

---

#### DMux4Way — Write Decoder (1-to-4)

The **DMux4Way gate** routes a single control or data signal to one of four outputs.

It is used in:

- Register file write selection
- Memory region decoding

::: details Hardware Description

```hdl
CHIP DMux4Way {
    IN in, sel[2];
    OUT a, b, c, d;

    PARTS:
    DMux(in=in, sel=sel[1], a=dMuxAB, b=dMuxCD);
    DMux(in=dMuxAB, sel=sel[0], a=a, b=b);
    DMux(in=dMuxCD, sel=sel[0], a=c, b=d);
}
```

:::

::: tip DMUX4WAY(wordIN, sel)
<DMuxWay16Demo :ways="4" />
:::

---

#### DMux8Way — Write Decoder (1-to-8)

The **DMux8Way gate** routes a single control or data signal to one of eight outputs.

It forms the basis of hierarchical write decoding for large memory blocks and register banks.

::: details Hardware Description

```hdl
CHIP DMux8Way {
    IN in, sel[3];
    OUT a, b, c, d, e, f, g, h;

    PARTS:
    DMux(in=in, sel=sel[2], a=dMuxABCD, b=dMuxEFGH);
    DMux4Way(in=dMuxABCD, sel=sel[0..1], a=a, b=b, c=c, d=d);
    DMux4Way(in=dMuxEFGH, sel=sel[0..1], a=e, b=f, c=g, d=h);
}
```

:::

::: tip DMUX8WAY(wordIN, sel)
<DMuxWay16Demo :ways="8" />
:::

#### SysCall15 — System Call Mask

::: warning Change HACK to HACK++
This is a novel gate for the Hack++ implementation. This gate was made to allow for the Hack++ computer
to separate its instruction memory into two sections, the first for the OS and the second for User
Programs.
:::

The **SysCall15 gate** flags the use of a specific address.

It forms the basis for the Hack++ platform to implement context switching at a hardware level and
separate its ROM space into Kernel (OS) and User Space.

The address this circuit flags is:

- `0b0111 1111 1111 1111` or `@32767`

::: details Hardware Description

```hdl
CHIP SysCall15 {
    IN in[15];
    OUT out;

    PARTS:
    // Level 1
    // LSBs
    And(a=in[0],  b=in[1],  out=and0);
    And(a=in[2],  b=in[3],  out=and1);
    And(a=in[4],  b=in[5],  out=and2);
    And(a=in[6],  b=in[7],  out=and3);
    And(a=in[8],  b=in[9],  out=and4);
    And(a=in[10], b=in[11], out=and5);
    And(a=in[12], b=in[13], out=and6);
    // MSBs
    And(a=in[14], b=true, out=and7);

    // Level 2
    And(a=and0, b=and1, out=and00);
    And(a=and2, b=and3, out=and01);
    And(a=and4, b=and5, out=and02);
    And(a=and6, b=and7, out=and03);

    // Level 3
    And(a=and00, b=and01, out=and000);
    And(a=and02, b=and03, out=and001);

    // Level 4
    And(a=and000, b=and001, out=out);
}
```

:::

### Arithmetic Circuits

Consider the addition of two single bits. Depending on their values, the operation produces one of four possible outcomes:

- `0 + 0 = 00`
- `1 + 0 = 01`
- `0 + 1 = 01`
- `1 + 1 = 10`

Notice that for each outcome, the least significant bit (LSB) perfectly matches the output of an `XOR` gate, while the most
significant bit (MSB) matches the output of an `AND` gate. To represent all four outcomes, any adder circuit must output two
bits as well. These are:

- the `sum` bit (LSB)
- the `carry` bit (MSB)

In binary arithmetic, the carry bit occupies the next higher power-of-two column (2^1). This is conceptually identical to
decimal addition, where carrying a number shifts it into the next column (10^1).

::: tip Binary Addition Logic (Half Adder)
<ArithmeticDemo type="half" />
:::

While this simple configuration — known as a Half Adder — works perfectly for two isolated bits, it is unable to accept a
`carry-in` from a previous operation. To handle this we can combine two Half Adders to create a Full Adder that can add
two bits and accept a carry. These can then be linked together to form a **carry-propagation backbone**, this allows a
circuit to be scaled to the size of a word.

::: tip Binary Addition Logic (Full Adder)
<ArithmeticDemo type="full" />
:::

#### HalfAdder — 1-bit Sum

> **Also known as:** _1-bit adder_, _sum/carry generator_

The **HalfAdder** computes the sum of two one-bit inputs, producing:

- `sum`: the LSB `a + b`
- `carry`: the MSB of `a + b`

It is the base primitive of multi-bit addition.

::: details Hardware Description

```hdl
CHIP HalfAdder {
    IN a, b;       // 1-bit inputs
    OUT sum,       // LSB of a + b
        carry;     // MSB of a + b

    PARTS:
    Xor(a=a, b=b, out=sum);
    And(a=a, b=b, out=carry);
}
```

:::

::: tip HALFADDER(a, b)
<HalfAdderCircuit />
:::

---

#### FullAdder — 1-bit Sum with Carry-In

> **Also known as:** _carry-propagating adder cell_

The **FullAdder** computes the sum of three one-bit inputs (`a`, `b`, and carry-in `c`), producing:

- `sum`: the LSB of `a + b + c`
- `carry`: the MSB of `a + b + c`

It is constructed from two half adders plus an OR gate to combine carry outputs.

::: details Hardware Description

```hdl
CHIP FullAdder {
    IN a, b, c;    // 1-bit inputs
    OUT sum,       // LSB of a + b + c
        carry;     // MSB of a + b + c

    PARTS:
    HalfAdder(a=a,    b=b, sum=sum0,   carry=carry0);
    HalfAdder(a=sum0, b=c, sum=sum,    carry=carry1);
    Or(a=carry0, b=carry1, out=carry);
}
```

:::

::: tip FULLADDER(a, b, c)
<FullAdderCircuit />
:::

---

#### Add16 — 16-bit Ripple-Carry Adder

> **Also known as:** _ripple-carry adder_, _word adder_

The **Add16** unit adds two 16-bit two’s complement values.

Carries propagate from the LSB upward in a ripple-carry chain. The final carry-out from bit 15 is ignored,
matching the Hack arithmetic model (mod 2^16).

::: details Hardware Description

```hdl
CHIP Add16 {
    IN a[16], b[16];
    OUT out[16];

    PARTS:
    // LSB
    HalfAdder(a=a[0],  b=b[0],  sum=out[0],  carry=carry0);

    FullAdder(a=a[1],  b=b[1],  c=carry0,  sum=out[1],  carry=carry1);
    FullAdder(a=a[2],  b=b[2],  c=carry1,  sum=out[2],  carry=carry2);
    FullAdder(a=a[3],  b=b[3],  c=carry2,  sum=out[3],  carry=carry3);
    FullAdder(a=a[4],  b=b[4],  c=carry3,  sum=out[4],  carry=carry4);
    FullAdder(a=a[5],  b=b[5],  c=carry4,  sum=out[5],  carry=carry5);
    FullAdder(a=a[6],  b=b[6],  c=carry5,  sum=out[6],  carry=carry6);
    FullAdder(a=a[7],  b=b[7],  c=carry6,  sum=out[7],  carry=carry7);
    FullAdder(a=a[8],  b=b[8],  c=carry7,  sum=out[8],  carry=carry8);
    FullAdder(a=a[9],  b=b[9],  c=carry8,  sum=out[9],  carry=carry9);
    FullAdder(a=a[10], b=b[10], c=carry9,  sum=out[10], carry=carry10);
    FullAdder(a=a[11], b=b[11], c=carry10, sum=out[11], carry=carry11);
    FullAdder(a=a[12], b=b[12], c=carry11, sum=out[12], carry=carry12);
    FullAdder(a=a[13], b=b[13], c=carry12, sum=out[13], carry=carry13);
    FullAdder(a=a[14], b=b[14], c=carry13, sum=out[14], carry=carry14);

    // MSB
    FullAdder(a=a[15], b=b[15], c=carry14, sum=out[15], carry=dead);
}
```

:::

::: tip ADD16(wordA, wordB)
<Add16Demo />
:::

---

#### Inc16 — 16-bit Incrementor

> **Also known as:** _PC incrementor_, _+1 unit_

The **Inc16** unit increments a 16-bit input by 1.

It is implemented by adding the constant value `1` to the input bus. This is frequently used for sequential
address generation (e.g., `PC+1`).

::: details Hardware Description

```hdl
CHIP Inc16 {
IN in[16];
OUT out[16];

    PARTS:
    Add16(a=in[0..15], b[0]=true, b[1..15]=false, out=out[0..15]);
}
```

:::

::: tip INC16
<Inc16Demo />
:::

::: details Hardware Description

```hdl
CHIP ROM16K {
    IN address[14];
    OUT out[16];

    PARTS:
    // implementation is omitted because it relies on
    // a pre-loaded hardware state rather than clocked flip-flops.
}
```

---

```hdl
CHIP ROM32K {
    IN address[15];
    OUT out[16];

    PARTS:
    ROM16K(address=address[0..13], out=out0);
    ROM16K(address=address[0..13], out=out1);

    Mux16(a=out0, b=out1,
          sel=address[14], out=out);
}
```

:::

## Sequential Circuits

This section documents the stateful (time dependent) building blocks of the Hack++ hardware stack.

Unlike combinational logic — which maps inputs to outputs without regard to time — sequential components
**store state across time cycles**. This introduces time as an input (`t`, `t+1`) and enables the construction
of program counters, registers, and eventually memory.

**Cycle semantics (`t` → `t+1`):**
Borrowing the nand2tetris timing schema, combinational outputs are reflected as signals in the _current_ cycle (`t`),
while state updates become visible in the _next_ cycle (`t+1`).

::: info Basic State Pattern

```text
IF flagged:
  out(t+1) = state
ELSE:
  out(t+1) = out(t)
```

This can be read as: if the conditional flag is `1` at time `t` (`reset(t)`, `load(t)`, `inc(t)`), then modify
the component to accept a new state (`0`, `in(t)`,`out(t) + 1`) to be set (or latched) by `t+1`. If no flag is set then
retain the previous state (`out(t)`).

:::

### Memory Circuits

::: warning The Plan
As before the single-bit circuits will be combined to accommodate word size computations. However, as state can now
be maintained over time, these wide circuits can then be combined to allow the storage and retrieval of previously
computed words.

- `DFF → Bit → Register → RAM8 → RAM64 → RAM512 → RAM4K → RAM16K`

**Bit ordering (bus convention)**
Declarations define **width** (e.g., `in[16]` is 16 bits wide), while usage defines **index**.
Signals use a 0-indexed convention: `in[0]` is the least significant bit (LSB) and `in[15]` is the most
significant bit (MSB). This is keeping with the nand2tetris hdl convention, not a memory endianness rule.
:::

#### DFF — Data Flip Flop

> **Also known as:** _1-bit latch_

A DFF is the core unit on which all sequential circuits are built. This circuit receives an signal form the
computers clock, and at the end of each time unit (cycle) updates its output to match the input from the previous
cycle.

::: tip DFF Logic

```text
out(t+1) = in(t)
```

<DFFDemo />
:::

#### Bit — 1-bit Register

> **Also known as:** _storage cell_

The **Bit** consists of a DFF wrapped in some additional conditional logic that allows some control over
what previous clock cycle value the DFF accepts.

It is implemented by feeding the DFF’s previous output back and a new input through a MUX:

- when `load=0`, the cell recirculates its output and holds its value
- when `load=1`, the cell captures the new input and updates its value

::: details Hardware Description

```hdl
CHIP Bit {
    IN in, load;
    OUT out;

    PARTS:
    Mux(a=dff, b=in, sel=load, out=mux);
    DFF(in=mux, out=dff, out=out);
}
```

:::

::: tip Bit Logic

```text
IF load(t) == 1:
    out(t+1) = in(t)
ELSE:
    out(t+1) = out(t)
```

<BitDemo />
:::
