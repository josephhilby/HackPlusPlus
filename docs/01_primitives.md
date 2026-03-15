# Primitive Logic Gates

This section documents the primitive logic gates that form the physical and logical foundation of the Hack++ hardware stack.
All higher-level components—datapath elements, registers, memory, and the CPU—are constructed exclusively from these gates.

At the base of the system is the **NAND gate**, from which all other logic is derived.

## Gates

### NAND — Universal Gate

> **Also known as:** *Functional complete primitive*

The **NAND gate** is the single primitive from which all combinational and sequential logic in 
Hack++ is constructed.

It computes the logical AND of two inputs and then inverts the result. Because NAND is functionally 
complete, every other gate in this system can be expressed as a composition of NAND gates.

::: tip Behavior

**Identity := `¬(a ∧ b)`**

<NandGate />

:::

---

### NOT — Inverter Gate

> **Also known as:** *Logical complement*

The **NOT gate** performs signal inversion and is the core of bitwise negation, control-signal 
inversion, and two’s-complement arithmetic throughout the datapath and control logic.

It computes the inverse of its input.

<div class="callout-grid">

::: tip Behavior

**Identity:** `¬(in ∧ in) = ¬in`

**Truth Table:**

| in | out |
| -- | --- |
| 0  | 1   |
| 1  | 0   |

:::

::: tip HDL

```hdl
CHIP Not {
    IN in;
    OUT out;

    PARTS:
    Nand(a=in, b=in, out=out);
}
```

:::

</div>

---

### AND — Enable Gate

The **AND gate** qualifies signal propagation and control enables by allowing a value to pass only when all conditions are asserted.

It is widely used in:

* Write-enable qualification
* Jump-condition evaluation
* Masked datapath propagation

**Also known as:** *Enable gate*, *Qualifier*

#### Behavior

```text
¬(¬(a ∧ b)) = a ∧ b
```

#### Truth Table

| a | b | out |
| - | - | --- |
| 0 | 0 | 0   |
| 0 | 1 | 0   |
| 1 | 0 | 0   |
| 1 | 1 | 1   |

#### HDL

```java
CHIP And {
IN a, b;
OUT out;

    PARTS:
    Nand(a=a, b=b, out=nand);
    Not(in=nand, out=out);
}
```

---

### OR — Combine Gate

The **OR gate** aggregates multiple signal sources into a single logical result.

It is commonly used for:

* Flag reduction (`zr`, jump conditions)
* Multi-source control logic
* Signal merging in the datapath

**Also known as:** *Signal combiner*, *Logical merge*

#### Behavior

```text
¬(¬a ∧ ¬b) = a ∨ b
```

*Note: Derived using De Morgan’s Law.*

#### Truth Table

| a | b | out |
| - | - | --- |
| 0 | 0 | 0   |
| 0 | 1 | 1   |
| 1 | 0 | 1   |
| 1 | 1 | 1   |

#### HDL

```java
CHIP Or {
IN a, b;
OUT out;

    PARTS:
    Not(in=a, out=na);
    Not(in=b, out=nb);
    Nand(a=na, b=nb, out=out);
}
```

---

### XOR — Sum Gate

The **XOR gate** produces a high output when its inputs differ.

It is the fundamental building block of **binary addition**, forming the sum path in half-adders, full-adders, and the ALU’s arithmetic pipeline.

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

---

## Architectural Context

Primitive logic gates define the **physical semantics of computation** in Hack++.

* **NAND** establishes the universal basis for all signal transformation
* **NOT, AND, OR** form the foundation of control qualification and signal aggregation
* **XOR** introduces arithmetic structure by enabling binary addition

Together, these gates form the boundary where **boolean algebra becomes machine behavior**, enabling the construction of word-level datapaths, stateful storage, and ultimately a programmable computer system.
