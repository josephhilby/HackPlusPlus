# Hack++ PC (Program Counter)
The Hack++ Program Counter (PC) is a 16-bit stateful counter used to track the address of the next 
instruction to execute. It supports three control behaviors—reset, load, and increment—with a 
defined priority order.

The PC updates on the next clock tick; its output reflects the stored value for the current cycle.

## Update semantics (priority order)
On each cycle, the PC computes `out(t+1)` as:

```java
If reset(t) == 1
    out(t+1) = 0
Else if load(t) == 1
    out(t+1) = in(t)
Else if inc(t) == 1
    out(t+1) = out(t) + 1
Else
    out(t+1) = out(t) (hold)
```

This priority ordering guarantees deterministic behavior when multiple control signals are asserted in 
the same cycle (e.g., `reset` overrides `load` which overrides `inc`).

## Implementation
```java
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