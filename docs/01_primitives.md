# Primitive Logic Gates
This section documents the primitive logic gates used as the physical foundation of the Hack++ hardware stack. 
All higher-level components—arithmetic units, registers, memory, and the CPU—are constructed exclusively from these gates.

At the base of the system is the NAND gate, from which all other logic is derived.

**Related:**
- [Wide Combinational Gates](./02_wide_gates.md)
- [Routing & Control Gates](./03_routing.md)
- [Arithmetic Units](./04_arithmetic.md)

## Design Notes
**Single-primitive constraint**

All gates above are derived solely from the NAND gate, enforcing a minimal and uniform hardware foundation.

**Abstraction ladder**

These primitives form the base layer for:
- Wide gates (`Not16`, `And16`, `Or16`)
- Routing logic (`Mux`, `DMux`)
- Arithmetic units (`Add16`, `Inc16`)
- Sequential elements (`Register`, `RAM`)
- Processor and system components (`ALU`, `CPU`, `Computer`)

## Gates
### NAND — Universal Gate
The **NAND gate** is the single primitive from which all combinational and sequential logic in Hack++ 
is derived. As such it has no hardware description language (HDL) representation.

It computes the logical AND of two inputs, and then inverts the result.

#### Logic
```text
out = ¬(a ∧ b)
```

#### Truth Table
| a | b | out |
| - | - | --- |
| 0 | 0 | 1   |
| 0 | 1 | 1   |
| 1 | 0 | 1   |
| 1 | 1 | 0   |

### NOT — Inverter Gate
The **NOT gate** is the core of all signal conditioning, bitwise negation, and control-signal 
inversion throughout all routing and jump logic.

It computes the inverse of its input.

#### Logic
```text
¬(in ∧ in) = ¬in
```

#### Truth Table
| in | out |
| -- | --- |
| 0  | 1   |
| 1  | 0   |

#### HDL
```java
CHIP Not {
IN in;
OUT out;

    PARTS:
    Nand(a=in, b=in, out=out);
}
```

### AND — Enable Gate
The **AND gate** is used in control gating to qualify write enables, jump conditions, and masked signal propagation

It computes `1` only when both inputs are `1`.

#### Logic
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

### OR — Combine Gate
The **OR gate** is used in signal aggregation to flag reduction (`zr`), jump condition evaluation, 
and multi-source control logic.

It computes `1` if at least one input is `1`.

#### Logic
```text
¬(¬a ∧ ¬b) = a ∨ b
```
*Note: Use De Morgan's Law.*

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

### XOR — Sum Gate
The **XOR gate** is used in generating sum bits in adders and ALU addition logic.

It computes `1` when exactly one input is `1`.

#### Logic
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