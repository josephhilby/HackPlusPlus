# Jack Language, (in progress)

## Jack Grammar (EBNF)

### Tokens

```regexp
comment         := //[^\r\n]*[\r\n]
whiteSpace      := [ \t]
keyword         := (class|constructor|function|method|field
                   |static|var|int|char|boolean|void|true|false|null
                   |this|let|do|if|else|while|return)
symbol          := ([{}()\[\].,;+\-*/&|<>=~])
integerConstant := [0-9]+ (* 0 <= integer <= 32767 *)
stringConstant  := "([^"\r\n]*)"
identifier      := ([A-Za-z_][A-Za-z0-9_]*)
newline         := [\r\n]
```

::: warning Change HACK to HACK++

The Jack language has been restricted to line comments only (e.g., // comment).
Also, to keep the grammar simple, comments and whitespace are removed by the Jack
lexer and are therefore not considered in the remainder of the compilation process.

:::

### Syntax

```ebnf
non-terminal    ::= production rule
---                 ---
program         ::= class { newline }

class           ::= "class" className "{"
                      { classVarDec }
                      { subroutineDec }
                    "}"

classVarDec     ::= ("static" | "field") type varName { "," varName } ";"
type            ::= "int" | "char" | "boolean" | className

subroutineDec   ::= ("constructor" | "function" | "method")
                    ("void" | type) subroutineName "(" parameterList ")"
                    subroutineBody
parameterList   ::= [ type varName { "," type varName } ]

subroutineBody  ::= "{" { varDec } statements "}"
varDec          ::= "var" type varName { "," varName } ";"

className       ::= identifier
subroutineName  ::= identifier
varName         ::= identifier

(* Statements *)
statements      ::= { statement }
statement       ::= letStatement | ifStatement | whileStatement
                  | doStatement | returnStatement

letStatement    ::= "let" varName [ "[" expression "]" ] "=" expression ";"
ifStatement     ::= "if" "(" expression ")" "{"
                       statements
                       "}" [ "else" "{" statements "}" ]
whileStatement  ::= "while" "(" expression ")" "{"
                       statements
                    "}"
doStatement     ::= "do" subroutineCall ";"
returnStatement ::= "return" [ expression ] ";"

(* Expressions *)
expression      ::= term { op term }
term            ::= integerConstant | stringConstant | keywordConstant
                  | varName
                  | varName "[" expression "]"
                  | "(" expression ")"
                  | unaryOp term
                  | subroutineCall

subroutineCall  ::= subroutineName "(" expressionList ")"
                  | ( className | varName ) "." subroutineName "(" expressionList ")"

expressionList  ::= [ expression { "," expression } ]

op              ::= "+" | "-" | "*" | "/" | "&" | "|" | "<" | ">" | "="
unaryOp         ::= "-" | "~"
keywordConstant ::= "true" | "false" | "null" | "this"
```

#### Legend

- `{ … }` = zero or more
- `[ … ]` = optional (zero or one)
- `|` = alternative
- Mnemonics and identifiers are **case-sensitive**
