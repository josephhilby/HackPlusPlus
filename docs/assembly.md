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
R1..R15, SP, LCL, ARG, THIS, THAT, TEMP, SCREEN, KBD
```

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
