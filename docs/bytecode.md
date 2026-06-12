# Bytecode Language (VM), (in progress)

## VM Grammar (EBNF)

### Tokens

```regexp
integer    := ^[0-9]+$
identifier := ^[A-Za-z_$:.][A-Za-z0-9_-]*$
newline    := [\r\n]
```

### Syntax

```ebnf
non-terminal  ::= production rule
---               ---
program       ::= { line }

line          ::= [ command ] newline

command       ::= arithmetic | memory | branching | function

arithmetic    ::= "add" | "sub" | "neg" | "eq" | "gt"
                | "lt" | "and" | "or" | "not"

memory        ::= ("push" | "pop") segment index  (* segment[index] *)

segment       ::= "local" | "argument" | "this" | "that"
                | "temp" | "static" | "pointer" | "constant"
index         ::= integer

branching     ::= ("label" | "goto" | "if-goto") label

function      ::= "function" function_name nlocals
               |  "call" function_name nargs
               |  "return"

label         ::= identifier
function_name ::= identifier
nlocals       ::= integer  (* number of locals allocated by callee *)
nargs         ::= integer  (* number of args passed by caller *)
```

#### Legend

- `{ … }` = zero or more
- `[ … ]` = optional (zero or one)
- `|` = alternative
- Mnemonics and identifiers are **case-sensitive**

#### RAM Map

The RAM16 component's `16K` words (`0x0000–0x3FFF`), are mapped as follows:

| Address Range (Dec) | ASM Name            | Usage                                                       |
| ------------------- | ------------------- | ----------------------------------------------------------- |
| `RAM[0]`            | `R0`/`SP`           | Current top of the stack                                    |
| `RAM[1]`            | `R1`/`LCL`          | Base of the current function's local segment                |
| `RAM[2]`            | `R2`/`ARG`          | Base of the current function's argument segment             |
| `RAM[3]`            | `R3`/`THIS`         | Base of the current function's `this` segment (heap object) |
| `RAM[4]`            | `R4`/`THAT`         | Base of the current function's `that` segment (heap array)  |
| `RAM[5..12]`        | `R5..12`/`TEMP`     | Virtual Registers for current function's temporary storage  |
| `RAM[13..15]`       | `R13..R15`          | General-purpose registers (`TMP`,`FRAME`,`RET` in Hack ++)  |
| `RAM[16..255]`      | -                   | Static variables (indexed by class)                         |
| `RAM[256..2044]`    | `S_ST`, `S_END`     | Stack (Grows downward, originally `256 -> 2047`)            |
| `RAM[2045..16380]`  | `H_END`, `H_ST`     | Heap (Allocates upward, originally `16380 -> 2048`)         |
| `RAM[16381..16383]` | `SID`, `SA1`, `SA2` | Mailboxes for System Calls                                  |

To make a memory segments of

```text
     Address            Content
+---------------+---------------------+
| 0 - 15        | Virtual Registers   |
+---------------+---------------------+
| 16 - 255      | Static Variables    |
+---------------+---------------------+
|               |                     |
| 256 - 2044    | Stack (Grows Down)  |
|               |                     |
+---------------+---------------------+
|               |                     |
|               |                     |
|               |                     |
| 2045 - 16380  | Heap (Allocates Up) |
|               |                     |
|               |                     |
|               |                     |
+---------------+---------------------+
| 16381 - 16383 | System Mailboxs     |
+---------------+---------------------+
```

#### Stack Frame Map

```text
  Address       | [Frame] Content           | Value  | Note
+---------------+---------------------------+--------+-------------------+
| 256           | [Init] Argument 0         | 0      | entrypoint*       |
+---------------+---------------------------+--------+-------------------+
| 257           | [Init] Return Address     | ...    | HALT addr         |
| 258           | [Init] Saved LCL          | 256    |                   |
| 259           | [Init] Saved ARG          | 256    |                   |
| 260           | [Init] Saved THIS         | 0      |                   |
| 261           | [Init] Saved THAT         | 0      |                   |
| 262           | [Init] Local 0            | 0      | 0 if no locals    |
+---------------+---------------------------+--------+-------------------+
| 263           | [Init] Working Stack      | ...    |                   |
| 264           |                           | ...    | calls main()      |
+---------------+---------------------------+--------+-------------------+
| 265           | [Main] Argument 0         | 0      | 0 if no args      |
+---------------+---------------------------+--------+-------------------+
| 266           | [Main] Return Address     | ...    | main + 1 addr     |
| 267           | [Main] Saved LCL          | 262    |                   |
| 268           | [Main] Saved ARG          | 256    |                   |
| 269           | [Main] Saved THIS         | 0      |                   |
| 270           | [Main] Saved THAT         | 0      |                   |
| 271           | [Main] Local 0            | 2045   | x = Foo.new()     |
| 272           | [Main] Local 1            | ...    | y = x.bar(1)**    |
+---------------+---------------------------+--------+-------------------+
| 273 (ARG)***  | [Foo.Bar] Argument 0      | 2045   | Foo's `THIS`      |
| 274           | [Foo.Bar] Argument 1      | 1      |                   |
+---------------+---------------------------+--------+-------------------+
| 275           | [Foo.Bar] Return Address  | ...    | foo.bar + 1 addr  |
| 276           | [Foo.Bar] Saved LCL       | 272    |                   |
| 277           | [Foo.Bar] Saved ARG       | 265    |                   |
| 278           | [Foo.Bar] Saved THIS      | 0      |                   |
| 279           | [Foo.Bar] Saved THAT      | 0      |                   |
| 280 (LCL)     | [Foo.Bar] Local 0         | 0      |                   |
+---------------+---------------------------+--------+-------------------+
| 281           | [Foo.Bar] Working Stack   | ...    |                   |
| 282           |                           | ...    |                   |
| 283           |                           | ...    |                   |
+---------------+---------------------------+--------+-------------------+
| 284           | [Helper] return value     | ...    | Access with pop   |
+---------------+---------------------------+--------+-------------------+
| 285 (SP)      |                           | ...    | Next free spot    |
+---------------+---------------------------+--------+-------------------+

*   Boot sets SP, LCL, ARG to 256, and THIS, THAT to 0 then calls
    `Sys.init`
**  Assumes `bar` is `obj.method` call (access to obj heap addr 'THIS'),
    not a `class.function` (static) call
*** ARG becomes the return value address to pass foo.bar return back
    to main
```

#### Heap Map

#### Screen Map

The Hack platform screen exposes 8K words of 16-bits at address `0x4000–0x5FFF`, each bit represents a screen pixel.

- 131,072 pixels
- 256 rows (16 words / column)
- 512 cols (32 words / row)
- `word = SCREEN + (32 x row) + (col / 16)`
- `offset = col % 16`
- 1 = black, 0 = white

| Row Index | Start Address | End Address |
| --------- | ------------- | ----------- |
| Row 0     | 0x4000        | 0x401F      |
| Row 1     | 0x4020        | 0x403F      |
| ...       | ...           | ...         |
| Row 255   | 0x5FE0        | 0x5FFF      |
