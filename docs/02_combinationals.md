# Combinational Circuits

This section documents the combinational circuits used by Hack++ for data routing and arithmetic operations. Unlike 
the previous sections primitive gates, which implement individual boolean operations, these components exist to perform 
specific tasks required by higher levels of the abstraction ladder.

Combinational circuits operate only on their current inputs and produce outputs without depending on any previous state, 
forming the building blocks for datapaths, control logic, and instruction decoding.

## Routing Circuits

These components do not compute new values; instead, they **select**, **fan in/out**, and **qualify** 
existing signals, forming the backbone of instruction decoding, register loading, memory writes, and control flow.

::: warning Fan-in and Fan-out
- **Multiplexers (MUX)** implement *fan-in*: many sources → one destination
- **Demultiplexers (DMUX)** implement *fan-out*: one source → many destinations
:::

### MUX — Selector Circuit

> **Also known as:** *Selector*, *Data switch*

The **Multiplexer (MUX)** selects exactly one of two inputs based on a single control signal.

```text
If sel = 0 → out = a
If sel = 1 → out = b
```

It is the fundamental building block of instruction decoding, ALU input selection, and register loading.

::: details Definition

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

### DMUX — Distributor Circuit

> **Also known as:** *Distributor*, *Write decoder*

The **Demultiplexer (DMUX)** routes a single input to exactly one of two outputs based on a control signal.

```text
If sel = 0 → { a = in, b = 0  }
If sel = 1 → { a = 0 , b = in }
```

It is used to implement **write enables**, **register selection**, and **memory-mapped output routing**.


::: details Definition

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

::: tip DMUX(sel)
<DMuxCircuit />
:::

## Arithmetic Circuits
This section documents the combinational arithmetic building blocks used to implement integer addition and increment
operations in Hack++. These components define the **carry-propagation backbone** of the datapath and are used directly
in the ALU (`f=1` path), address sequencing, and loop/control constructs.

Arithmetic units sit above primitive logic and routing: they introduce *cross-bit coupling* via carry signals, turning
independent bitwise operations into true word-level arithmetic.


### HalfAdder — 1-bit Sum

> **Also known as:** *1-bit adder*, *sum/carry generator*

The **HalfAdder** computes the sum of two one-bit inputs, producing:

* `sum`: the low bit of `a + b`
* `carry`: the high bit of `a + b`

It is the base primitive of multi-bit addition.

#### Behavior

```text
sum   = a ⊕ b
carry = a ∧ b
```

#### HDL

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

---

### FullAdder — 1-bit Sum with Carry-In

> **Also known as:** *carry-propagating adder cell*

The **FullAdder** computes the sum of three one-bit inputs (`a`, `b`, and carry-in `c`), producing:

* `sum`: the low bit of `a + b + c`
* `carry`: the high bit of `a + b + c`

It is constructed from two half adders plus an OR gate to combine carry outputs.

#### Behavior

```text
(a + b + c) = 2·carry + sum
```

#### HDL

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

## The Wide Circuits

::: warning The Plan
All wide routing circuits are built strictly from their single-bit equivalents, preserving the abstraction ladder:

- `Mux → Mux16 → Mux4Way16 → Mux8Way16`
- `DMux → DMux4Way → DMux8Way`

**Bit ordering (bus convention)**
Signals use a fixed bit-indexing convention: `in[0]` is the least significant bit (LSB) and `in[15]` is the most
significant bit (MSB). This is a bus-ordering convention, not a memory endianness rule.
:::

### Mux16 — Bus Selector

The **Mux16 circuit** extends the single-bit MUX across a 16-bit bus, allowing entire words to be conditionally selected.

It is heavily used in:

* ALU input selection (`A` vs `M`)
* Instruction decoding
* PC source selection

#### Behavior

```text
For i = 0..15:
    If sel = 0 → out[i] = a[i]
    If sel = 1 → out[i] = b[i]
```

#### HDL

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

---

### Mux4Way16 — Word Selector (4-to-1)

The **Mux4Way16 gate** selects one of four 16-bit inputs using a 2-bit control signal.

It is typically used in:

* Multi-source bus arbitration
* ROM and memory bank selection

#### Behavior

```text
If sel = 00 → out = a
If sel = 01 → out = b
If sel = 10 → out = c
If sel = 11 → out = d
```

#### HDL

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

---

### Mux8Way16 — Word Selector (8-to-1)

The **Mux8Way16 gate** selects one of eight 16-bit inputs using a 3-bit control signal.

It forms the basis of hierarchical bus selection and large fan-in datapaths.

#### Behavior

```text
If sel = 000 → out = a
If sel = 001 → out = b
If sel = 010 → out = c
If sel = 011 → out = d
If sel = 100 → out = e
If sel = 101 → out = f
If sel = 110 → out = g
If sel = 111 → out = h
```

#### HDL

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

---

### DMux4Way — Write Decoder (1-to-4)

The **DMux4Way gate** routes a single control or data signal to one of four outputs.

It is used in:

* Register file write selection
* Memory region decoding

#### Behavior

```text
If sel = 00 → [a, b, c, d] = [in, 0, 0, 0]
If sel = 01 → [a, b, c, d] = [0, in, 0, 0]
If sel = 10 → [a, b, c, d] = [0, 0, in, 0]
If sel = 11 → [a, b, c, d] = [0, 0, 0, in]
```

#### HDL

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

---

### DMux8Way — Write Decoder (1-to-8)

The **DMux8Way gate** routes a single control or data signal to one of eight outputs.

It forms the basis of hierarchical write decoding for large memory blocks and register banks.

#### Behavior

```text
If sel = 000 → [a, b, c, d, e, f, g, h] = [in, 0, 0, 0, 0, 0, 0, 0]
If sel = 001 → [a, b, c, d, e, f, g, h] = [0, in, 0, 0, 0, 0, 0, 0]
If sel = 010 → [a, b, c, d, e, f, g, h] = [0, 0, in, 0, 0, 0, 0, 0]
If sel = 011 → [a, b, c, d, e, f, g, h] = [0, 0, 0, in, 0, 0, 0, 0]
If sel = 100 → [a, b, c, d, e, f, g, h] = [0, 0, 0, 0, in, 0, 0, 0]
If sel = 101 → [a, b, c, d, e, f, g, h] = [0, 0, 0, 0, 0, in, 0, 0]
If sel = 110 → [a, b, c, d, e, f, g, h] = [0, 0, 0, 0, 0, 0, in, 0]
If sel = 111 → [a, b, c, d, e, f, g, h] = [0, 0, 0, 0, 0, 0, 0, in]
```

#### HDL

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
---

### Add16 — 16-bit Ripple-Carry Adder

> **Also known as:** *ripple-carry adder*, *word adder*

The **Add16** unit adds two 16-bit two’s complement values.

Carries propagate from the LSB upward in a ripple-carry chain. The final carry-out from bit 15 is ignored, matching the Hack arithmetic model.

#### Behavior

```text
out = a + b   (mod 2^16)
```

#### HDL

```hdl
CHIP Add16 {
    IN a[16], b[16];
    OUT out[16];

    PARTS:
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
    FullAdder(a=a[15], b=b[15], c=carry14, sum=out[15], carry=dead);
}
```

---

### Inc16 — 16-bit Incrementer

> **Also known as:** *PC incrementer*, *+1 unit*

The **Inc16** unit increments a 16-bit input by 1.

It is implemented by adding the constant value `1` to the input bus. This is frequently used for sequential 
address generation (e.g., `PC+1`) and loop/index increments.

#### Behavior

```text
out = in + 1   (mod 2^16)
```

#### HDL

```hdl
CHIP Inc16 {
IN in[16];
OUT out[16];

    PARTS:
    Add16(a=in[0..15], b[0]=true, b[1..15]=false, out=out[0..15]);
}
```