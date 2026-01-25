## Logic Gates

### Nand -- Elementary Gate
The NAND gate is the fundamental building block of Hack++.
All other logic gates are constructed using NAND alone. This
gate acts to take the AND between two inputs and then invert 
the result.

#### Truth Table
a | b | out
--|---|----
0 | 0 | 1
0 | 1 | 1
1 | 0 | 1 
1 | 1 | 0 

#### Logic
```ini
out = ¬(a ∧ b)
```

### Not
The NOT gate serves to take any input and invert it.

#### Truth Table
in | out
---|----
 0 | 1
 1 | 0

#### HDL Implementation
By feeding the same input into both NAND inputs, the gate computes
`¬(in ∧ in)`, which simplifies to `¬in`.

```c
CHIP Not {
    IN in;
    OUT out;

    PARTS:
    Nand(a=in, b=in, out=out);
}
```

### And
The AND gate serves to validate both inputs are active.

#### Truth Table
a | b | out
--|---|----
0 | 0 | 0
0 | 1 | 0
1 | 0 | 0 
1 | 1 | 1 

#### HDL Implementation
By negating the NAND result, the gate computes
`¬(¬(a ∧ b))`, which simplifies to `(a ∧ b)`.

```c
CHIP And {
    IN a, b;
    OUT out;
    
    PARTS:
    Nand(a=a, b=b, out=in);
    Not(in=in, out=out);
}
```

### Or
The OR gate serves to validate at least one input is active.

#### Truth Table
a | b | out
--|---|----
0 | 0 | 0
0 | 1 | 1
1 | 0 | 1 
1 | 1 | 1 

#### HDL Implementation
By negating the inputs to NAND, the gate can compute
`¬(¬a ∧ ¬b)`, which by De Morgans Law simplifies to `(a v b)`.

```c
CHIP Or {
    IN a, b;
    OUT out;

    PARTS:
    Not(in=a, out=na);
    Not(in=b, out=nb);
    Nand(a=na, b=nb, out=out);
}
```

### Exclusive Or (Xor)
The XOR gate serves to validate exactly one input is active.

#### Truth Table
a | b | out
--|---|----
0 | 0 | 0
0 | 1 | 1
1 | 0 | 1 
1 | 1 | 0 

#### HDL Implementation
By ANDing the OR of two inputs with the negation of their AND, the result is XOR,
`(a v b) ∧ ¬(a ∧ b) = (a ⊕ b)`.

```c
CHIP Xor {
    IN a, b;
    OUT out;

    PARTS:
    Or(a=a, b=b, out=or);
    Nand(a=a, b=b, out=nand);
    And(a=or, b=nand, out=out);
}
```