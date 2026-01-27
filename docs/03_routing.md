# Routing & Control Gates

This section documents the routing and control gates that direct data flow and control signals across the Hack++ datapath.
These components do not compute new values; instead, they **select**, **fan out**, and **qualify** existing signals, forming the backbone of instruction decoding, register loading, memory writes, and control flow.

They represent the architectural boundary between **computation** (arithmetic/logic) and **control** (selection and enable).

**Related:**

* [Primitive Gates](./01_primitives.md)
* [Wide Combinational Gates](./02_wide_gates.md)
* [Arithmetic Units](./04_arithmetic.md)
* [Processor Components](./07_processor.md)

---

## Design Notes

**Control vs. datapath separation**
Routing gates define *where* values go, not *what* values are. In Hack++, this separation makes the CPU’s behavior transparent: control bits from the instruction word are decoded into MUX select lines and DMUX enable lines that steer data across the machine.

**Fan-in and fan-out**

- **Multiplexers (MUX)** implement *fan-in*: many sources → one destination
- **Demultiplexers (DMUX)** implement *fan-out*: one source → many destinations

**Hierarchical construction**
All wide routing gates are built strictly from their single-bit equivalents, preserving the abstraction ladder:

- `Mux → Mux16 → Mux4Way16 → Mux8Way16`
- `DMux → DMux4Way → DMux8Way`

**Bit ordering (bus convention)**
Signals use a fixed bit-indexing convention: `in[0]` is the least significant bit (LSB) and `in[15]` is the most
significant bit (MSB). This is a bus-ordering convention, not a memory endianness rule.

---

## Gates

### MUX — Selector Gate

The **Multiplexer (MUX)** selects exactly one of two inputs based on a single control signal.

It is the fundamental building block of instruction decoding, ALU input selection, and register loading.

**Also known as:** *Selector*, *Data switch*

#### Behavior

```text
If sel = 0 → out = a
If sel = 1 → out = b
```

#### Truth Table

| a | b | sel | out |
| - | - | --- | --- |
| 0 | 0 | 0   | 0   |
| 0 | 1 | 0   | 0   |
| 1 | 0 | 0   | 1   |
| 1 | 1 | 0   | 1   |
| 0 | 0 | 1   | 0   |
| 0 | 1 | 1   | 1   |
| 1 | 0 | 1   | 0   |
| 1 | 1 | 1   | 1   |

#### Logic

```text
out = (a ∧ ¬sel) ∨ (b ∧ sel)
```

#### HDL

```java
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

---

### DMUX — Distributor Gate

The **Demultiplexer (DMUX)** routes a single input to exactly one of two outputs based on a control signal.

It is used to implement **write enables**, **register selection**, and **memory-mapped output routing**.

**Also known as:** *Distributor*, *Write decoder*

#### Behavior

```text
If sel = 0 → a = in, b = 0
If sel = 1 → a = 0,  b = in
```

#### HDL

```java
CHIP DMux {
IN in, sel;
OUT a, b;

    PARTS:
    Not(in=sel, out=nsel);
    And(a=nsel, b=in, out=a);
    And(a=sel, b=in, out=b);
}
```

---

### Mux16 — Bus Selector

The **Mux16 gate** extends the single-bit MUX across a 16-bit bus, allowing entire words to be conditionally selected.

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

```java
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

```java
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

```java
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

```java
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

```java
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

## Architectural Context

Routing and control gates are the **physical realization of the instruction format** defined in the ISA.

* **Destination bits (`d1–d3`)** drive DMUX structures that enable specific registers or memory writes
* **Computation select bits (`a`, `c1–c6`)** drive MUX trees that select ALU inputs and operations
* **Jump bits (`j1–j3`)** combine flag logic and MUXed PC inputs to determine control flow

Together, these gates form the bridge between *symbolic instructions* and *physical signal paths*.
