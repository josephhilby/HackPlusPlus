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

line          ::= [ insrtuction | label ] [ comment ] newline
comment       ::= "//" { any_char_except_newline }

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

jump          ::= "JGT" | "JEQ" | "JGE" | "JLT" | "JNE" | "JLE" | "JMP"

label         ::= "(" symbol ")"

value         ::= constant | symbol

constant      ::= integer (* 0 <= integer <= 32767 *)
```

**Legend:**

- `{ … }` = zero or more
- `[ … ]` = optional (zero or one)
- `|` = alternative
- Mnemonics for dest, comp, jump (e.g., `"AM"`, `"D+A"`, `"JEQ"`) are case-sensitive
- Mnemonics for comp, jump (e.g., `"D+A"`, `"JEQ"`) are not comutive

**Predefined Symbols**

```
R1..R15, SP, LCL, ARG, THIS, THAT, TEMP, SCREEN, KBD
```
