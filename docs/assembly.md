# Assembly Language, (in progress)

## Assembly Grammar (EBNF)

### Tokens

```regexp
integer := ^[0-9]+$
symbol  := [A-Za-z_$:.] [A-Za-z0-9_-]*
newline := [\r\n]
```

### Syntax

```ebnf
non-terminal  ::= production rule
---               ---
program       ::= { line }

line          ::= [ insrtuction | label ] newline

instruction   ::= a_instruction | c_instruction

a_instruction ::= "@" value

c_instruction ::= [ dest "=" ] comp [ ";" jump ]

dest          ::= dest_char { dest_char }
dest_char     ::= "A" | "D" | "M"

comp          ::=  "0" |  "1" | "-1"
                |  "A" |  "D" |  "M"
                | "!A" | "!D" | "!M"
                | "-A" | "-D" | "-M"
                | "A+1" | "D+1" | "M+1"
                | "A-1" | "D-1" | "M-1"
                | "D+A" | "D+M"
                | "D-A" | "D-M" | "A-D" | "M-D"
                | "D&A" | "D&M"
                | "D|A" | "D|M"

jump          ::= "JGT" | "JEQ" | "JGE" | "JLT"
                | "JNE" | "JLE" | "JMP"

label         ::= "(" symbol ")"

value         ::= constant | symbol

constant      ::= integer
```

#### Legend

- `{ … }` = zero or more
- `[ … ]` = optional (zero or one)
- `|` = alternative

#### Predefined Symbols

```
RAM Segments: R1..R15, TEMP, STACK, HEAP, SCREEN, KBD, MAIL
Stack Segments: SP, LCL, ARG, THIS, THAT
HEAP Segments: S_HD, S_CURR, L_HD, L_CURR
```

- `@constant`
- `@LABEL`
- `(LABEL)`

- `dest = comp; jump`
- `dest = comp`
- `comp;jump`

### PC Program from earlier in HACK

program from circuits

```text
// LOAD A, 0
@0
// LOAD A, 1
@1
// MOVE D, A
D=A
// LOAD A, 2
@2
// ADD A, D
D=D+A
// JNZ 1 (If D != 0, jump to address in A)
@1
D;JNE
```

**Two’s complement arithmetic**
All arithmetic is performed on 16-bit two’s complement values.

```
0b 1 11 a c1 c2 c3 c4 c5 c6  d1 d2 d3  j1 j2 j3
   ^           comp            dest      jump
```

::: tip COMP

The `comp` field controls the ALU's computation.

The `a` bit selects the ALU's `y` input source:

- `a = 0` → `y = A-Register`
- `a = 1` → `y = RAM[A-Register]`

The `c1–c6` bits set flags that control the ALU’s internal pipeline:

- `c1`/`zx` → zero `x` input.
- `c2`/`nx` → negate `x` input.
- `c3`/`zy` → zero `y` input.
- `c4`/`ny` → negate `y` input.
- `c5`/`f` → select `add(x,y)` or `and(x,y)` function.
- `c6`/`no` → negate `result`.

::: details Comp Field

| comp   | a   | c1  | c2  | c3  | c4  | c5  | c6  | effect                      |
| ------ | --- | --- | --- | --- | --- | --- | --- | --------------------------- |
| `0`    | 0   | 1   | 0   | 1   | 0   | 1   | 0   | Constant 0                  |
| `1`    | 0   | 1   | 1   | 1   | 1   | 1   | 1   | Constant 1                  |
| `-1`   | 0   | 1   | 1   | 1   | 0   | 1   | 0   | Constant -1                 |
| `D`    | 0   | 0   | 0   | 1   | 1   | 0   | 0   | D register                  |
| `A`    | 0   | 1   | 1   | 0   | 0   | 0   | 0   | A register                  |
| `M`    | 1   | 1   | 1   | 0   | 0   | 0   | 0   | RAM[A] value                |
| `!D`   | 0   | 0   | 0   | 1   | 1   | 0   | 1   | Bitwise NOT of D            |
| `!A`   | 0   | 1   | 1   | 0   | 0   | 0   | 1   | Bitwise NOT of A            |
| `!M`   | 1   | 1   | 1   | 0   | 0   | 0   | 1   | Bitwise NOT of RAM[A]       |
| `-D`   | 0   | 0   | 0   | 1   | 1   | 1   | 1   | Negation of D               |
| `-A`   | 0   | 1   | 1   | 0   | 0   | 1   | 1   | Negation of A               |
| `-M`   | 1   | 1   | 1   | 0   | 0   | 1   | 1   | Negation of RAM[A]          |
| `D+1`  | 0   | 0   | 1   | 1   | 1   | 1   | 1   | D plus 1                    |
| `A+1`  | 0   | 1   | 1   | 0   | 1   | 1   | 1   | A plus 1                    |
| `M+1`  | 1   | 1   | 1   | 0   | 1   | 1   | 1   | RAM[A] plus 1               |
| `D-1`  | 0   | 0   | 0   | 1   | 1   | 1   | 0   | D minus 1                   |
| `A-1`  | 0   | 1   | 1   | 0   | 0   | 1   | 0   | A minus 1                   |
| `M-1`  | 1   | 1   | 1   | 0   | 0   | 1   | 0   | RAM[A] minus 1              |
| `D+A`  | 0   | 0   | 0   | 0   | 0   | 1   | 0   | D plus A                    |
| `D+M`  | 1   | 0   | 0   | 0   | 0   | 1   | 0   | D plus RAM[A]               |
| `D-A`  | 0   | 0   | 1   | 0   | 0   | 1   | 1   | D minus A                   |
| `D-M`  | 1   | 0   | 1   | 0   | 0   | 1   | 1   | D minus RAM[A]              |
| `A-D`  | 0   | 0   | 0   | 0   | 1   | 1   | 1   | A minus D                   |
| `M-D`  | 1   | 0   | 0   | 0   | 1   | 1   | 1   | RAM[A] minus D              |
| `D&A`  | 0   | 0   | 0   | 0   | 0   | 0   | 0   | Bitwise AND of D and A      |
| `D&M`  | 1   | 0   | 0   | 0   | 0   | 0   | 0   | Bitwise AND of D and RAM[A] |
| `D\|A` | 0   | 0   | 1   | 0   | 1   | 0   | 1   | Bitwise OR of D and A       |
| `D\|M` | 1   | 0   | 1   | 0   | 1   | 0   | 1   | Bitwise OR of D and RAM[A]  |

:::

::: tip DEST

The `dest` field controls which storage element(s) (`A`,`M`,`D`) receive the ALU result.

::: details Dest Field

| dest   | d1  | d2  | d3  | effect                             |
| ------ | --- | --- | --- | ---------------------------------- |
| `null` | 0   | 0   | 0   | Value is not stored                |
| `M`    | 0   | 0   | 1   | RAM[A]                             |
| `D`    | 0   | 1   | 0   | D register                         |
| `MD`   | 0   | 1   | 1   | RAM[A] and D register              |
| `A`    | 1   | 0   | 0   | A register                         |
| `AM`   | 1   | 0   | 1   | A register and RAM[A]              |
| `AD`   | 1   | 1   | 0   | A register and D register          |
| `AMD`  | 1   | 1   | 1   | A register, RAM[A], and D register |

:::

::: tip JUMP

The `jump` field sets a condition that must be met for a jump to occur. To validate that condition
ALU flags are used:

- If ALU output is zero → `zr = 1`
- If ALU output is negative (two’s complement) → `ng = 1`

::: details Jump Field

| jump   | j1  | j2  | j3  | effect             |
| ------ | --- | --- | --- | ------------------ |
| `null` | 0   | 0   | 0   | No jump            |
| `JGT`  | 0   | 0   | 1   | If out > 0, jump   |
| `JEQ`  | 0   | 1   | 0   | If out = 0, jump   |
| `JGE`  | 0   | 1   | 1   | If out ≥ 0, jump   |
| `JLT`  | 1   | 0   | 0   | If out < 0, jump   |
| `JNE`  | 1   | 0   | 1   | If out ≠ 0, jump   |
| `JLE`  | 1   | 1   | 0   | If out ≤ 0, jump   |
| `JMP`  | 1   | 1   | 1   | Unconditional jump |

:::
