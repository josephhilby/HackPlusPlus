# Primitive Logic Gates

This section documents the primitive logic gates that form the physical and logical foundation of the 
Hack++ hardware stack. Every higher-level component of the computer — **the datapath, control unit, memory system, 
input devices, and output interfaces** — is ultimately constructed from combinations of these gates.

Each gate serves distinct roles across the system

* **NAND** establishes the universal basis from which all other digital logic can be constructed
* **NOT, AND, OR** implement the decision logic used throughout control circuits, multiplexers, and datapath routing
* **XOR** enables binary arithmetic, forming the core of adders and arithmetic logic units (ALUs)

From these small Boolean primitives emerge the mechanisms that drive a computer: signals are combined to control 
execution, stored to represent memory, manipulated to perform arithmetic, and propagated to input and output devices.

Together, these gates represent the boundary where **boolean algebra becomes machine behavior**, enabling the construction 
of a programmable computing system.

## The Gates

### NAND — Universal Gate

> **Also known as:** *Functional complete primitive*

The **NAND gate** is the single primitive from which all combinational and sequential logic in 
Hack++ is constructed.

It computes the logical AND of two inputs and then inverts the result. Because NAND is functionally 
complete, every other gate in this system can be expressed as a composition of NAND gates.

::: tip Logic

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

::: tip Logic

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

::: tip Logic

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

::: tip Logic

<OrGate />

:::

---

### XOR — Sum Gate

The **XOR gate** produces a high output when its inputs differ.

It is the fundamental building block of **binary addition**, forming the sum path in half-adders, full-adders, 
and the ALU’s arithmetic pipeline.

**Also known as:** *Sum gate*, *Difference detector*

#### Behavior

```text
(a ∨ b) ∧ ¬(a ∧ b)
```

#### Truth Table

| a | b | out |
| - | - | --- |
| 0 | 0 | 0   |
| 0 | 1 | 1   |
| 1 | 0 | 1   |
| 1 | 1 | 0   |

#### HDL

```java
CHIP Xor {
IN a, b;
OUT out;

    PARTS:
    Or(a=a, b=b, out=or);
    Nand(a=a, b=b, out=nand);
    And(a=or, b=nand, out=out);
}
```
