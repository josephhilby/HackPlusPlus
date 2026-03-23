# Arithmetic Units

This section documents the combinational arithmetic building blocks used to implement integer addition and increment 
operations in Hack++. These components define the **carry-propagation backbone** of the datapath and are used directly 
in the ALU (`f=1` path), address sequencing, and loop/control constructs.

Arithmetic units sit above primitive logic and routing: they introduce *cross-bit coupling* via carry signals, turning 
independent bitwise operations into true word-level arithmetic.


## Design Notes

**Two’s complement arithmetic**
All arithmetic is performed on 16-bit two’s complement values. Overflow beyond bit 15 is ignored (wraparound modulo 2¹⁶), matching the Hack ALU specification.

**Carry propagation**
Unlike wide gates (which operate bitwise in parallel), adders must propagate carry from lower bits to higher bits. This introduces a deterministic dependency chain that defines the critical path of addition.

**Hierarchical construction**
Arithmetic units are built strictly from lower-level components:

`Xor + And → HalfAdder`
`HalfAdder + HalfAdder + Or → FullAdder`
`HalfAdder + FullAdder×15 → Add16`
`Add16 (+ constant 1) → Inc16`

**Bit ordering (bus convention)**
Arithmetic uses the standard bus convention: `in[0]` is the least significant bit (LSB) and `in[15]` is the most significant bit (MSB).

---

## Units

### HalfAdder — 1-bit Sum

The **HalfAdder** computes the sum of two one-bit inputs, producing:

* `sum`: the low bit of `a + b`
* `carry`: the high bit of `a + b`

It is the base primitive of multi-bit addition.

**Also known as:** *1-bit adder*, *sum/carry generator*

#### Behavior

```text
sum   = a ⊕ b
carry = a ∧ b
```

#### HDL

```java
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

The **FullAdder** computes the sum of three one-bit inputs (`a`, `b`, and carry-in `c`), producing:

* `sum`: the low bit of `a + b + c`
* `carry`: the high bit of `a + b + c`

It is constructed from two half adders plus an OR gate to combine carry outputs.

**Also known as:** *carry-propagating adder cell*

#### Behavior

```text
(a + b + c) = 2·carry + sum
```

#### HDL

```java
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

---

### Add16 — 16-bit Ripple-Carry Adder

The **Add16** unit adds two 16-bit two’s complement values.

Carries propagate from the LSB upward in a ripple-carry chain. The final carry-out from bit 15 is ignored, matching the Hack arithmetic model.

**Also known as:** *ripple-carry adder*, *word adder*

#### Behavior

```text
out = a + b   (mod 2^16)
```

#### HDL

```java
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

The **Inc16** unit increments a 16-bit input by 1.

It is implemented by adding the constant value `1` to the input bus. This is frequently used for sequential address generation (e.g., `PC+1`) and loop/index increments.

**Also known as:** *PC incrementer*, *+1 unit*

#### Behavior

```text
out = in + 1   (mod 2^16)
```

#### HDL

```java
CHIP Inc16 {
IN in[16];
OUT out[16];

    PARTS:
    Add16(a=in[0..15], b[0]=true, b[1..15]=false, out=out[0..15]);
}
```

---

## Architectural Context

Arithmetic units provide the core mechanism for **word-level computation** in Hack++.

* The **ALU** relies on `Add16` to implement its arithmetic mode (`f=1`) and, combined with inversion (`Not16`), supports subtraction and negation patterns.
* The **Program Counter** and sequencing logic depend on increment behavior (`Inc16`) to advance instruction flow.
* Higher-level software abstractions (VM arithmetic, stack pointer updates, address computations) ultimately lower into repeated applications of these adders.

These units are where boolean logic becomes **integer arithmetic**, enabling the machine’s higher-level semantics (comparisons, branching decisions, and memory addressing) to be expressed in hardware.
