# A_INSTRUCTION (MSB = 0)

```
0 vvvvvvvvvvvvvvv
```

@xxx, where xxx is an integer from 0 to 32767, or a symbol that is bound to such an integer.

# C_INSTRUCTION (MSB = 1)

```
111 a c1 c2 c3 c4 c5 c6 d1 d2 d3 j1 j2 j3
```

## comp (a, c1–c6)

| comp   | a | c1 | c2 | c3 | c4 | c5 | c6 | effect                      |
| ------ | - | -- | -- | -- | -- | -- | -- |-----------------------------|
| `0`    | 0 | 1  | 0  | 1  | 0  | 1  | 0  | Constant 0                  |
| `1`    | 0 | 1  | 1  | 1  | 1  | 1  | 1  | Constant 1                  |
| `-1`   | 0 | 1  | 1  | 1  | 0  | 1  | 0  | Constant -1                 |
| `D`    | 0 | 0  | 0  | 1  | 1  | 0  | 0  | D register                  |
| `A`    | 0 | 1  | 1  | 0  | 0  | 0  | 0  | A register                  |
| `M`    | 1 | 1  | 1  | 0  | 0  | 0  | 0  | RAM[A] value                |
| `!D`   | 0 | 0  | 0  | 1  | 1  | 0  | 1  | Bitwise NOT of D            |
| `!A`   | 0 | 1  | 1  | 0  | 0  | 0  | 1  | Bitwise NOT of A            |
| `!M`   | 1 | 1  | 1  | 0  | 0  | 0  | 1  | Bitwise NOT of RAM[A]       |
| `-D`   | 0 | 0  | 0  | 1  | 1  | 1  | 1  | Negation of D               |
| `-A`   | 0 | 1  | 1  | 0  | 0  | 1  | 1  | Negation of A               |
| `-M`   | 1 | 1  | 1  | 0  | 0  | 1  | 1  | Negation of RAM[A]          |
| `D+1`  | 0 | 0  | 1  | 1  | 1  | 1  | 1  | D plus 1                    |
| `A+1`  | 0 | 1  | 1  | 0  | 1  | 1  | 1  | A plus 1                    |
| `M+1`  | 1 | 1  | 1  | 0  | 1  | 1  | 1  | RAM[A] plus 1               |
| `D-1`  | 0 | 0  | 0  | 1  | 1  | 1  | 0  | D minus 1                   |
| `A-1`  | 0 | 1  | 1  | 0  | 0  | 1  | 0  | A minus 1                   |
| `M-1`  | 1 | 1  | 1  | 0  | 0  | 1  | 0  | RAM[A] minus 1              |
| `D+A`  | 0 | 0  | 0  | 0  | 0  | 1  | 0  | D plus A                    |
| `D+M`  | 1 | 0  | 0  | 0  | 0  | 1  | 0  | D plus RAM[A]               |
| `D-A`  | 0 | 0  | 1  | 0  | 0  | 1  | 1  | D minus A                   |
| `D-M`  | 1 | 0  | 1  | 0  | 0  | 1  | 1  | D minus RAM[A]              |
| `A-D`  | 0 | 0  | 0  | 0  | 1  | 1  | 1  | A minus D                   |
| `M-D`  | 1 | 0  | 0  | 0  | 1  | 1  | 1  | RAM[A] minus D              |
| `D&A`  | 0 | 0  | 0  | 0  | 0  | 0  | 0  | Bitwise AND of D and A      |
| `D&M`  | 1 | 0  | 0  | 0  | 0  | 0  | 0  | Bitwise AND of D and RAM[A] |
| `D\|A` | 0 | 0  | 1  | 0  | 1  | 0  | 1  | Bitwise OR of D and A       |
| `D\|M` | 1 | 0  | 1  | 0  | 1  | 0  | 1  | Bitwise OR of D and RAM[A]  |


## dest (d1–d3)

| dest   | d1 | d2 | d3 | Effect                             |
| ------ | -- | -- | -- | ---------------------------------- |
| `null` | 0  | 0  | 0  | Value is not stored                |
| `M`    | 0  | 0  | 1  | RAM[A]                             |
| `D`    | 0  | 1  | 0  | D register                         |
| `MD`   | 0  | 1  | 1  | RAM[A] and D register              |
| `A`    | 1  | 0  | 0  | A register                         |
| `AM`   | 1  | 0  | 1  | A register and RAM[A]              |
| `AD`   | 1  | 1  | 0  | A register and D register          |
| `AMD`  | 1  | 1  | 1  | A register, RAM[A], and D register |

## jump (j1–j3)

| jump   | j1 | j2 | j3 | Effect             |
| ------ | -- | -- | -- | ------------------ |
| `null` | 0  | 0  | 0  | No jump            |
| `JGT`  | 0  | 0  | 1  | If out > 0, jump   |
| `JEQ`  | 0  | 1  | 0  | If out = 0, jump   |
| `JGE`  | 0  | 1  | 1  | If out ≥ 0, jump   |
| `JLT`  | 1  | 0  | 0  | If out < 0, jump   |
| `JNE`  | 1  | 0  | 1  | If out ≠ 0, jump   |
| `JLE`  | 1  | 1  | 0  | If out ≤ 0, jump   |
| `JMP`  | 1  | 1  | 1  | Unconditional jump |

