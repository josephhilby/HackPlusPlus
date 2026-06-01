# Sequential Circuits

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

## Memory Circuits

Hack++ RAM and ROM are built as a hierarchy of addressed register banks. Each level increases capacity by integrating
**eight** instances of the previous levels component. To address each interior component three bits are required
(2^3 = 8). However, as the hierarchy increases in complexity there is a need to both select an interior component AND pass
addressing information into that component.

To identify what bits are addressing the eight previous components and what bits are passed into the single selected previous
component the terms **hi** and **low** are used. Where **hi** references the three bits to select the it, and **low**
references the remaining bits passed into it.

#### Memory Circuit Hierarchy

|       Chip | Words | Address bits | Built from    | Address split                             |
| ---------: | ----: | -----------: | ------------- | ----------------------------------------- |
| `Register` |     1 |            0 | 8× `Bit`      | none                                      |
|     `RAM8` |     8 |            3 | 8× `Register` | `sel = address[0..2]`                     |
|    `RAM64` |    64 |            6 | 8× `RAM8`     | `hi=address[3..5]`, `lo=address[0..2]`    |
|   `RAM512` |   512 |            9 | 8× `RAM64`    | `hi=address[6..8]`, `lo=address[0..5]`    |
|    `RAM4K` |  4096 |           12 | 8× `RAM512`   | `hi=address[9..11]`, `lo=address[0..8]`   |
|   `RAM16K` | 16384 |           14 | 8× `RAM4K`    | `hi=address[11..13]`, `lo=address[0..11]` |

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

### DFF — Data Flip Flop

::: tip DFF Logic

```text
out(t+1) = in(t)
```

<DFFDemo />
:::

### Bit — 1-bit Register

> **Also known as:** _1-bit latch_, _storage cell_

The **Bit** is the smallest state element in the platform: a single-bit storage cell with a load-enable.

It is implemented by feeding the DFF’s previous output back through a MUX:

- when `load=0`, the cell recirculates and holds its value
- when `load=1`, the cell captures `in`

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

---

### Register — 16-bit Word Register

> **Also known as:** _word register_, _general-purpose register_

The **Register** is a 16-bit state element used throughout the CPU and memory hierarchy.

It applies a single `load` enable across 16 `Bit` cells, producing a word-sized storage primitive. As
this has grown in complexity, to keep the demos from here on out readable the binary values will be
represented in hex.

::: details Hardware Description

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

:::

::: tip Register Logic

```text
IF load(t) == 1:
    out(t+1) = in(t)
ELSE:
    out(t+1) = out(t)
```

<RegisterDemo />

:::

---

### RAM8 — 8-Word Register Bank

> **Also known as:** _register file (8×16)_

The **RAM8** is the smallest addressable memory: eight 16-bit registers with a 3-bit address.

::: details Hardware Description

```hdl
CHIP RAM8 {
    IN in[16], load, address[3];
    OUT out[16];

    PARTS:
    DMux8Way(in=load, sel=address,
             a=in0, b=in1, c=in2, d=in3,
             e=in4, f=in5, g=in6, h=in7);

    Register(in=in, load=in0, out=out0);
    Register(in=in, load=in1, out=out1);
    Register(in=in, load=in2, out=out2);
    Register(in=in, load=in3, out=out3);
    Register(in=in, load=in4, out=out4);
    Register(in=in, load=in5, out=out5);
    Register(in=in, load=in6, out=out6);
    Register(in=in, load=in7, out=out7);

    Mux8Way16(a=out0, b=out1, c=out2, d=out3,
              e=out4, f=out5, g=out6, h=out7,
              sel=address, out=out);
}
```

:::

::: tip Register Bank Logic

```text
IF load(t) == 1:
    Register[ADDR](t+1) = in(t)
ELSE:
    Bank(t+1) = Bank(t)
```

<RegisterBankDemo />

:::

---

### RAM64 / RAM512 / RAM4K / RAM16K — Hierarchical RAM

The remaining RAM blocks repeat the same decode/store/select pattern, each time stacking 8× of the previous level.

To keep this reference readable, their full HDL is included below in collapsible sections. But no demo or behavior
will be provided.

<details>
<summary><strong>RAM64 — 64-word memory (8× RAM8)</strong></summary>

```hdl
CHIP RAM64 {
    IN in[16], load, address[6];
    OUT out[16];

    PARTS:
    DMux8Way(in=load, sel=address[3..5],
             a=in0, b=in1, c=in2, d=in3,
             e=in4, f=in5, g=in6, h=in7);

    RAM8(in=in, load=in0, address=address[0..2], out=out0);
    RAM8(in=in, load=in1, address=address[0..2], out=out1);
    RAM8(in=in, load=in2, address=address[0..2], out=out2);
    RAM8(in=in, load=in3, address=address[0..2], out=out3);
    RAM8(in=in, load=in4, address=address[0..2], out=out4);
    RAM8(in=in, load=in5, address=address[0..2], out=out5);
    RAM8(in=in, load=in6, address=address[0..2], out=out6);
    RAM8(in=in, load=in7, address=address[0..2], out=out7);

    Mux8Way16(a=out0, b=out1, c=out2, d=out3,
              e=out4, f=out5, g=out6, h=out7,
              sel=address[3..5], out=out);
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
    DMux8Way(in=load, sel=address[6..8],
             a=in0, b=in1, c=in2, d=in3,
             e=in4, f=in5, g=in6, h=in7);

    RAM64(in=in, load=in0, address=address[0..5], out=out0);
    RAM64(in=in, load=in1, address=address[0..5], out=out1);
    RAM64(in=in, load=in2, address=address[0..5], out=out2);
    RAM64(in=in, load=in3, address=address[0..5], out=out3);
    RAM64(in=in, load=in4, address=address[0..5], out=out4);
    RAM64(in=in, load=in5, address=address[0..5], out=out5);
    RAM64(in=in, load=in6, address=address[0..5], out=out6);
    RAM64(in=in, load=in7, address=address[0..5], out=out7);

    Mux8Way16(a=out0, b=out1, c=out2, d=out3,
              e=out4, f=out5, g=out6, h=out7,
              sel=address[6..8], out=out);
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
    DMux8Way(in=load, sel=address[9..11],
             a=in0, b=in1, c=in2, d=in3,
             e=in4, f=in5, g=in6, h=in7);

    RAM512(in=in, load=in0, address=address[0..8], out=out0);
    RAM512(in=in, load=in1, address=address[0..8], out=out1);
    RAM512(in=in, load=in2, address=address[0..8], out=out2);
    RAM512(in=in, load=in3, address=address[0..8], out=out3);
    RAM512(in=in, load=in4, address=address[0..8], out=out4);
    RAM512(in=in, load=in5, address=address[0..8], out=out5);
    RAM512(in=in, load=in6, address=address[0..8], out=out6);
    RAM512(in=in, load=in7, address=address[0..8], out=out7);

    Mux8Way16(a=out0, b=out1, c=out2, d=out3,
              e=out4, f=out5, g=out6, h=out7,
              sel=address[9..11], out=out);
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
    DMux8Way(in=load, sel=address[11..13],
             a=in0, b=in1, c=in2, d=in3,
             e=in4, f=in5, g=in6, h=in7);

    RAM4K(in=in, load=in0, address=address[0..11], out=out0);
    RAM4K(in=in, load=in1, address=address[0..11], out=out1);
    RAM4K(in=in, load=in2, address=address[0..11], out=out2);
    RAM4K(in=in, load=in3, address=address[0..11], out=out3);
    RAM4K(in=in, load=in4, address=address[0..11], out=out4);
    RAM4K(in=in, load=in5, address=address[0..11], out=out5);
    RAM4K(in=in, load=in6, address=address[0..11], out=out6);
    RAM4K(in=in, load=in7, address=address[0..11], out=out7);

    Mux8Way16(a=out0, b=out1, c=out2, d=out3,
              e=out4, f=out5, g=out6, h=out7,
              sel=address[11..13], out=out);
}
```

</details>

## Component Circuits

### PC — Program Counter

> **Also known as:** _instruction pointer_, _PC register_

The **Program Counter (PC)** is a 16-bit stateful counter that tracks the ROM address of the instruction in execution and
sequences to the next executable instruction. It supports three control behaviors — reset, load, and increment — with a defined priority order. This priority ordering guarantees deterministic behavior when multiple control signals are asserted in the
same cycle.

::: details Hardware Description

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

:::

::: tip PC Logic

```text
IF reset(t) == 1
    out(t+1) = 0
ELSE IF load(t) == 1
    out(t+1) = in(t)
ELSE IF inc(t) == 1
    out(t+1) = out(t) + 1
ELSE
    out(t+1) = out(t)
```

<PCDemo />

:::
