
## Emu flow
app.js ⇄ (HTTP/WS) ⇄ server.c ⇄ mem.c

## Hack++

### VM
EBNF
```ebnf
non-terminal  ::= production rule
---               ---
program       ::= { statement }

statement     ::= command [ segment index ]

command       ::= arithmetic | memory_access | program_flow | function_call

arithmetic    ::= "add" | "sub" | "neg" | "eq" | "gt" | "lt" | "and" | "or" | "not"
memory_access ::= ("push" | "pop") segment index
program_flow  ::= ("label" | "goto" | "if-goto") identifier
function_call ::= ("function" identifier number) | ("call" identifier number) | ("return")

segment       ::= "constant" | "local" | "argument" | "this" | "that" | "temp" | "pointer" | "static"

index         ::= integer | identifier
```

```regexp
integer    := ^[0-9]+$
identifier := [A-Za-z_$:.] [A-Za-z0-9_-]*
comment    := ^//.*$
```

### ASM
EBNF
```ebnf
non-terminal  ::= production rule
---               ---
program       ::= { statement }

statement     ::= a_instruction | c_instruction | label

a_instruction ::= "@" value

c_instruction ::= [ dest "=" ] comp [ ";" jump ]

dest          ::= dest_char { dest_char }
dest_char     ::= "A" | "D" | "M"

comp           ::= "0" |  "1" | "-1"
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

predefined symbols = R1..R15, SP, LCL, ARG, THIS, THAT, SCREEN, KBD

```regexp
integer := ^[0-9]+$
symbol  := [A-Za-z_$:.] [A-Za-z0-9_-]*
comment := ^//.*$
```

```regexp
VARIABLE @xxx
    constant := ^[0-9]+$
    symbol   :
        predefined := R0..R15, SP, LCL, ARG, THIS, THAT, SCREEN, KBD
        label      := ^\( [A-Za-z_$:.] [A-Za-z0-9_-]* \)$
        variable   := [A-Za-z_$:.] [A-Za-z0-9_-]* [^predefined]

COMMENT //
    comment := ^\\\\
```

### Memory Map
Typical MEM layout (words: 1 word := 16 bits): 16K RAM, 8K Screen, 1 Keyboard, 8K - 1 Unused

| VM Segment | Hack Mapping                        |
| ---------- |-------------------------------------|
| `constant` | immediate (not in RAM)              |
| `local`    | RAM[LCL + i]                        |
| `argument` | RAM[ARG + i]                        |
| `this`     | RAM[THIS + i]                       |
| `that`     | RAM[THAT + i]                       |
| `temp`     | RAM[5 + i] (i=0..7)                 |
| `pointer`  | RAM[3 + i] (i=0, 1; 0→THIS, 1→THAT) |
| `static`   | RAM[16 + n] (per-file namespace)    |

MEM[0..16383]:
RAM[0..15] Virtual Registers
nmum            addr  typical_value  vm_segment
R0/SP:           0       256         constant #
R1/LCL:          1       300          local
R2/ARG:          2       400         argument
R3/THIS:         3       3000        pointer 0
R4/THAT:         4       3010        pointer 1
R5..12/TEMP:   5..12           temp 0..7 (map: n - 5)
R13:             13
R14:             14
R15:             15
Static:        16..255        static 0..239 (map: n - 16)
Stack:         256..2047
Free:            2048     (os defined for heap, globals, etc.)

MMIO[16384..24576]:
Screen[16384..24575]
Keyboard[24576]

Reserved / unused[24577..32767]