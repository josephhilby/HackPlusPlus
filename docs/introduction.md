# Hack++ Reference

Before diving into specifics, I would like to take a brief moment to conceptually talk about what a
computer is, what a program is, and how the abstractions implemented in this project work together
to make them. So, with that...

## What is a computer?

**A computer** is five cooperating subsystems that execute program instructions and operate on data.

::: warning Definition
**Data:** quantifiable values that can represent, user input, intermediate state, computed results, as well as many other things...
:::

::: tip Computer Subsystems
1. **Input** — receives program instructions and user input from the external environment
2. **Output** — returns intermediate state and computed results to the external environment
3. **Memory** — stores program instructions and data
4. **Datapath** — moves and performs arithmetic and logical operations on data
5. **Control Unit** — interprets program instructions and orchestrates their requested operations
:::

## What is a program?

**A program** is a set of instructions that are progressively lowered through multiple cooperating abstractions,
moving from abstract human intent toward specific machine execution. These lowerings can be thought of in three
broad categories.

::: info Program Lowering
1. **Design-Time** — transforms an idea into structured high-level source code
2. **Compile-Time** — analyzes and lowers source code into progressively lower-level intermediate representations (IR) and eventually machine code
3. **Runtime** — executes machine code on target hardware using agreed paradigms for resource orchestration
:::

## The Abstraction Ladder

These two primary abstractions form the conceptual framework for Hack++. They describe both how a computer is organized and
how its software is transformed to machine instructions. Using them as a base, we can layer on the more specific implementation
of the Hack++ hardware and Jack programming language.

### Software Abstractions

The Jack programming language is a high-level object oriented programming language similar to Java - in that each file is a class.
It utilizes a two-tier compiler with a stack based virtual machine to compile its code from high level source code (.jack), to bytecode
(.vm), to assembly (.asm), and finally a simple assembler to move the assembly to machine code binary (.hack).

```yml
Design-Time
┌──────────────────────┐
│      Human Idea      │
└──────────────────────┘
          │
          ▼
Compile Time
┌──────────────────────┐
│     File.jack        │
│         ▼            │
│     File.vm          │
│         ▼            │
│     File.asm         │
│         ▼            │
│     File.hack        │
└──────────────────────┘
          │
          ▼
Runtime
┌──────────────────────┐
│  Computer Execution  │
└──────────────────────┘
```

### Hardware Abstractions

The Hack++ computer is constructed through a strict hierarchy of increasingly capable structures. Each layer is built exclusively
from those defined below it, progressively assembling modules, subsystems, and ultimately a complete computer.

At the base of this all is a single universal primitive: **NAND**.

```yml
Embodied Computer
 ▲
 └── Control Unit, Datapath, Memory, Input, and Output Subsystems
      ▲
      └── Functional Modules (CPU, ALU, Registers, ROM, RAM, etc.)
           ▲
           └── Combinational & Sequential Circuits (Mux, Add, Bit, etc.)
                ▲
                └── Boolean Logic Gates (Not, And, Or, Xor)
                     ▲
                     └── Universal Gate (NAND)
```
