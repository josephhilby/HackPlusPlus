# Sequential Chips

This section documents the stateful (clocked) building blocks of the Hack++ hardware stack.

Unlike combinational logic—which maps inputs to outputs within the same cycle—sequential components 
**store state across cycles**. This introduces the notion of time (`t`, `t+1`) and enables architectural state 
such as registers, counters, and addressable memory.

**Related:**

## Design Notes

**Cycle semantics (`t` → `t+1`)**
In nand2tetris timing, combinational outputs reflect signals in the *current* cycle (`t`), while state updates 
commit on the clock edge and become visible in the *next* cycle (`t+1`).

**Load / enable discipline**
All state elements follow the same pattern:

```text
If load(t) == 1: out(t+1) = in(t)
Else:            out(t+1) = out(t)
```

This enables deterministic gating of writes and clean composition into larger storage structures.

**Hierarchical construction**
Sequential elements form a strict ladder:

`DFF → Bit → Register → RAM8 → RAM64 → RAM512 → RAM4K → RAM16K`

Each RAM level uses:

* a **demultiplexer** to decode *which* sub-block receives `load`
* a bank of **sub-blocks** to store values
* a **multiplexer** to select the addressed output

**Bit ordering (bus convention)**
As elsewhere, `in[0]` is the LSB and `in[15]` is the MSB.

---

## Components

### PC — Program Counter

The **Program Counter (PC)** is a 16-bit stateful counter that tracks the address of the next instruction to execute.

It supports three control behaviors—reset, load, and increment—with a defined priority order. The PC updates on 
the next clock tick; its output reflects the stored value for the current cycle.

**Also known as:** *instruction pointer*, *PC register*

#### Update semantics (priority order)

On each cycle, the PC computes `out(t+1)` as:

```text
If reset(t) == 1
    out(t+1) = 0
Else if load(t) == 1
    out(t+1) = in(t)
Else if inc(t) == 1
    out(t+1) = out(t) + 1
Else
    out(t+1) = out(t) (hold)
```

This priority ordering guarantees deterministic behavior when multiple control signals are asserted in the same cycle.

#### HDL

```hdl
CHIP PC {
    IN in[16], reset, load, inc;
    OUT out[16];

    PARTS:
    // State (t)
    Register(in=result, load=true, out=count, out=out);

    // Next-State (t+1)
    Inc16(in=count, out=w0);
    Mux16(a=count, b=w0, sel=inc, out=w1);
    Mux16(a=w1, b=in, sel=load, out=w2);
    Mux16(a=w2, b[0..15]=false, sel=reset, out=result);
}
```

---

### Bit — 1-bit Register

The **Bit** is the smallest state element in the platform: a single-bit storage cell with a load-enable.

It is implemented by feeding the DFF’s previous output back through a MUX:

* when `load=0`, the cell recirculates and holds its value
* when `load=1`, the cell captures `in`

**Also known as:** *1-bit latch (clocked)*, *storage cell*

#### Behavior

```text
If load(t) == 1: out(t+1) = in(t)
Else:            out(t+1) = out(t)
```

#### HDL

```hdl
CHIP Bit {
    IN in, load;
    OUT out;

    PARTS:
    Mux(a=dff, b=in, sel=load, out=mux);
    DFF(in=mux, out=dff, out=out);
}
```

---

### Register — 16-bit Word Register

The **Register** is a 16-bit state element used throughout the CPU and memory hierarchy.

It applies a single `load` enable across 16 `Bit` cells, producing a word-sized storage primitive.

**Also known as:** *word register*, *general-purpose register (structural)*

#### Behavior

```text
If load(t) == 1: out(t+1) = in(t)
Else:            out(t+1) = out(t)
```

#### HDL

```hdl
CHIP Register {
    IN in[16], load;
    OUT out[16];

    PARTS:
    Bit(in=in[0],  load=load, out=out[0]);
    Bit(in=in[1],  load=load, out=out[1]);
    Bit(in=in[2],  load=load, out=out[2]);
    Bit(in=in[3],  load=load, out=out[3]);
    Bit(in=in[4],  load=load, out=out[4]);
    Bit(in=in[5],  load=load, out=out[5]);
    Bit(in=in[6],  load=load, out=out[6]);
    Bit(in=in[7],  load=load, out=out[7]);
    Bit(in=in[8],  load=load, out=out[8]);
    Bit(in=in[9],  load=load, out=out[9]);
    Bit(in=in[10], load=load, out=out[10]);
    Bit(in=in[11], load=load, out=out[11]);
    Bit(in=in[12], load=load, out=out[12]);
    Bit(in=in[13], load=load, out=out[13]);
    Bit(in=in[14], load=load, out=out[14]);
    Bit(in=in[15], load=load, out=out[15]);
}
```

---

## RAM Hierarchy

Hack++ RAM is built as a hierarchy of addressed register banks. Each level increases capacity by composing 
**eight** instances of the previous level.

### Structural pattern (reused at every level)

At each level:

* **Decode**: `DMux8Way(load, sel=hi_addr)` generates eight one-hot write enables
* **Store**: eight sub-RAM blocks receive the same `in` but only one receives `load=1`
* **Select**: `Mux8Way16(sel=hi_addr)` chooses which sub-block drives `out`

This pattern is identical for `RAM8 → RAM16K`; only the address slicing changes.

### Capacity overview

|     Chip | Words | Address bits | Built from    | Address split                             |
| -------: | ----: | -----------: | ------------- | ----------------------------------------- |
|   `RAM8` |     8 |            3 | 8× `Register` | `sel = address[0..2]`                     |
|  `RAM64` |    64 |            6 | 8× `RAM8`     | `hi=address[3..5]`, `lo=address[0..2]`    |
| `RAM512` |   512 |            9 | 8× `RAM64`    | `hi=address[6..8]`, `lo=address[0..5]`    |
|  `RAM4K` |  4096 |           12 | 8× `RAM512`   | `hi=address[9..11]`, `lo=address[0..8]`   |
| `RAM16K` | 16384 |           14 | 8× `RAM4K`    | `hi=address[11..13]`, `lo=address[0..11]` |

> Note: These are *word-addressed* memories (each address selects a 16-bit word).

---

### RAM8 — 8-Word Register Bank

The **RAM8** is the smallest addressable memory: eight 16-bit registers with a 3-bit address.

**Also known as:** *register file (8×16)*

#### HDL

```hdl
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
```

---

### RAM64 / RAM512 / RAM4K / RAM16K — Hierarchical RAM

The remaining RAM blocks repeat the same decode/store/select pattern, each time stacking 8× of the previous level.

To keep this reference readable, their full HDL is included below in collapsible sections.

<details>
<summary><strong>RAM64 — 64-word memory (8× RAM8)</strong></summary>

```hdl
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
```

</details>

<details>
<summary><strong>RAM512 — 512-word memory (8× RAM64)</strong></summary>

```hdl
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
```

</details>

<details>
<summary><strong>RAM4K — 4096-word memory (8× RAM512)</strong></summary>

```hdl
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
```

</details>

<details>
<summary><strong>RAM16K — 16384-word memory (8× RAM4K)</strong></summary>

```hdl
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
```

</details>

---

## Architectural Context

Sequential elements are where Hack++ transitions from *pure combinational logic* to *architectural state*.

* The **PC** turns instruction flow into an explicit state machine (`pc(t) → pc(t+1)`), enabling sequencing and control flow.
* **Bits and Registers** form the CPU’s programmer-visible state (`A`, `D`) and internal storage.
* The **RAM hierarchy** provides scalable addressed storage built from the same load/enable semantics, setting up the unified address space documented in [Memory Hierarchy](./06_memory.md).

These components establish the machine’s memory of the past—without them, computation would have no persistence across cycles.
