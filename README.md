<!-- PROJECT LOGO -->
<br />
<div align="center">
  <a href="https://github.com/<your_repo>">
    <img src="assets/banner.png" alt="Logo" width="80%">
  </a>
</div>

---

<!-- ABOUT THE PROJECT -->
## About The Project
Hack++ is a first-principles computer system built from the ground up, starting with the elementary NAND logic 
gate and extending through an assembler, virtual machine, and operating system. The project follows the methodology 
outlined in the book [*The Elements of Computing Systems*](https://www.nand2tetris.org/book) (commonly known as nand2tetris).

This work represents a full reimplementation and extension of the baseline Hack platform with an emphasis on:
- Systems-level understanding
- Clean architectural boundaries
- Practical tooling (emulator, web UI, and test harnesses)
  
If you are interested in computer architecture, compilers, or operating systems, I strongly recommend the 
book—it provides the conceptual foundation for everything implemented here.

### Requirements
- Docker

### Get Started
```shell
docker build -t hack-webemu-static -f docker/Dockerfile .
docker run --rm -p 8080:8080 hack-webemu-static
```

Once running, open your browser and navigate to: `http://localhost:8080`

### Roadmap
- [x] Complete Nand2Tetris baseline implementation
- [x] Create front end web UI
    - [x] Create app.js, style.css, index.html
    - [x] Create server.h/c to provide updates for screen and keyboard MMIO
    - [x] Connect server.c to app.js via websocket
      - app.js ⇄ (HTTP/WS) ⇄ server.c ⇄ mem.c
    - [x] Update README
- [ ] Emulate HACK CPU, and MEMORY.
    - [ ] CPU
    - [x] MEM
- [ ] Rework baseline implementation from Python to C
    - [x] Assembler
    - [x] VM
    - [ ] Compiler
    - [ ] OS
- [ ] Test with Google Test (unit) and LLVM (leak)
    - [x] Assembler
    - [x] VM
    - [ ] Compiler
    - [ ] OS

## Architecture
At its core, Hack++ follows a von Neumann architecture. Programs and data are stored in memory, accessed and 
manipulated by a central processing unit (CPU) composed of:
- Registers — for holding intermediate values and addresses
- ALU (Arithmetic Logic Unit) — for performing integer arithmetic and bitwise logic

The CPU itself is intentionally minimal. It contains only two programmer-visible registers:
- D Register — data register
- A Register — address register

The ALU operates on 16-bit signed integers and supports a constrained set of operations:
- Addition and subtraction
- Bitwise AND and OR
- Unary negation and bitwise NOT

Memory is divided into two logical regions:
- ROM (Read-Only Memory) — stores program instructions
- RAM (Random Access Memory) — stores program state, stack, heap, and memory-mapped I/O

This strict separation enables a clean instructional pipeline from: hardware → ISA → assembler → VM → OS → application.


### Memory Map
The Hack platform exposes 32K words of 16-bit RAM, mapped as follows (decimal addresses):

| Address Range      | Name   | Usage                                                |
|--------------------| ------ |------------------------------------------------------|
| `RAM[0]`           | `SP`   | Base of the current programs stack                   |
| `RAM[1]`           | `LCL`  | Base of the current function’s local segment         |
| `RAM[2]`           | `ARG`  | Base of the current function’s argument segment      |
| `RAM[3]`           | `THIS` | Base of the current function’s `this` segment (heap) |
| `RAM[4]`           | `THAT` | Base of the current function’s `that` segment (heap) |
| `RAM[5–12]`        | —      | Temp segment storage                                 |
| `RAM[13–15]`       | —      | General-purpose registers (R15 used as return)       |
| `RAM[16–255]`      | —      | Static variables                                     |
| `RAM[256–2047]`    | —      | Stack                                                |
| `RAM[2048–16383]`  | —      | Heap                                                 |
| `RAM[16384–24575]` | —      | Memory-mapped video I/O                              |
| `RAM[24576]`       | —      | Memory-mapped keyboard I/O                           |
| `RAM[24577..32767]`| -      | Unused                                               |
*This table was adapted documentation under the MIT License. See Attribution section for details.*

### VM Memory Segments
The VM exposes eight logical memory segments to every function:

| Segment    | Purpose                    | Allocation / Behavior                                                    |
| ---------- | -------------------------- | ------------------------------------------------------------------------ |
| `argument` | Stores function arguments  | Dynamically allocated by the VM when a function is entered               |
| `local`    | Stores local variables     | Dynamically allocated and initialized to zero when a function is entered |
| `static`   | Stores static variables    | Allocated per `.vm` file and shared across all functions in that file    |
| `this`     | General-purpose segment    | Used to manipulate heap-based data structures                            |
| `that`     | General-purpose segment    | Used to manipulate heap-based data structures                            |
| `pointer`  | Base address selectors     | Two-entry segment; `pointer 0` aligns `this`, `pointer 1` aligns `that`  |
| `temp`     | Temporary variables        | Eight multi-purpose registers shared across all functions                |
| `constant` | Immediate values (0–32767) | Pseudo-segment; not stored in RAM                                        |
*This table was adapted documentation under the MIT License. See Attribution section for details.*

### VM Specification

#### Data Types
- integer - 16-bit signed via two's complement 
- bool - -1: true, 0: false


#### VM Grammar (EBNF)
```ebnf
non-terminal  ::= production rule
---               ---
program       ::= { line }

line          ::= [ command ] [ comment ] newline
comment       ::= "//" { any_char_except_newline }

command       ::= arithmetic | memory | branching | function

arithmetic    ::= "add" | "sub" | "neg" | "eq" | "gt" | "lt" | "and" | "or" | "not"

memory        ::= ("push" | "pop") segment index  (* segment[index] *)
segment       ::= "constant" | "local" | "argument" | "this" | "that" | "temp" | "pointer" | "static"
index         ::= integer

branching     ::= ("label" | "goto" | "if-goto") label

function      ::= "function" function_name nlocals | "call" function_name integer | "return"

label         ::= identifier
function_name ::= identifier
nlocals       ::= integer  (* number of locals allocated by callee*)
nargs         ::= integer  (* number args passed by caller *)
```
>**Legend:**
>- `{ … }` = zero or more
>- `[ … ]` = optional (zero or one)
>- `|` = alternative
>- Keywords (`"LET"`, `"DEF"`, etc.) are case-sensitive

**Tokens**
```regexp
integer    := ^[0-9]+$
identifier := ^[A-Za-z_$:.][A-Za-z0-9_-]*
newline    := [\r\n]
```

### Assembly Specification

#### Grammar (EBNF)
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
>**Legend:**
>- `{ … }` = zero or more
>- `[ … ]` = optional (zero or one)
>- `|` = alternative
>- Keywords (`"LET"`, `"DEF"`, etc.) are case-sensitive

**Predefined Symbols**
```
R1..R15, SP, LCL, ARG, THIS, THAT, SCREEN, KBD
```

**Tokens**
```regexp
integer := ^[0-9]+$
symbol  := [A-Za-z_$:.] [A-Za-z0-9_-]*
newline := [\r\n]
```
## Attribution

This denoted work in my documentation was adapted from work originally authored by Charles Stevenson, licensed 
under the MIT License.

Stevenson, C. (2024-05-30). CodeWriter.java — Hack VM memory model documentation.
Original source repository:
https://github.com/brucesdad13/nand2tetris-vm-translator

The content has been reformatted and edited for clarity and consistency within the Hack++ project README.
The original author retains full credit for the underlying technical description.
