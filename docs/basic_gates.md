## Basic Gates

### Multiplexer (Mux)
The MUX gate serves to use a selection (sel) input to select exactly one of two inputs (a, b).

#### Truth Table
a | b | sel | out
--|---|-----|----
0 | 0 | 0   | 0
0 | 1 | 0   | 0
1 | 0 | 0   | 1
1 | 1 | 0   | 1
0 | 0 | 1   | 0
0 | 1 | 1   | 1
1 | 0 | 1   | 0
1 | 1 | 1   | 1

#### HDL Implementation
To define our MUX we will look at the Sum of Products (SOP) from the truth table. This
results in `a ¬b ¬sel + a b ¬sel + ¬a b sel + a b sel`. Make the following substitutions
`p := (a ∧ ¬sel); q:= (b ∧ sel)`, and rewrite `(¬b ∧ p) v (b ∧ p) v (¬a ∧ q) v (a ∧ q)`. Follow
distributive laws `(p ∧ (b v ¬b)) v (q ∧ (a v ¬a))`, then negation `(p ∧ T) v (q ∧ T)`. Finally,
revert the substitution and end up with `(a ∧ ¬sel) v (b ∧ sel)`.

```c
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

```c
CHIP DMux {
    IN in, sel;
    OUT a, b;

    PARTS:
    Not(in=sel, out=nsel);
    And(a=nsel, b=in, out=a);
    And(a=sel, b=in, out=b);
}
```

