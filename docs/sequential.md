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

### Bit — 1-bit Register

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
