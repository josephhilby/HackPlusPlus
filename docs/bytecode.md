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

line          ::= [ command ] [ comment ] newline
comment       ::= "//" { any_char_except_newline }

command       ::= arithmetic | memory | branching | function

arithmetic    ::= "add" | "sub" | "neg" | "eq" | "gt" | "lt" | "and" | "or" | "not"

memory        ::= ("push" | "pop") segment index  (* segment[index] *)
segment       ::= "local" | "argument" | "this" | "that" | "temp" | "static" | "pointer" | "constant"
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

### Legend

- `{ … }` = zero or more
- `[ … ]` = optional (zero or one)
- `|` = alternative
- Mnemonics and identifiers are **case-sensitive**
