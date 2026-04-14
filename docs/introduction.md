# Hack++ Reference

This section provides a quick structural overview of the Hack++ hardware and software systems. From logic 
gates, to circuits, to modules, to subsystems, to a fully programmable computer system, and supporting software 
toolchain, these elements work synchronously in order to constitute the Hack++ Computer. 

But, before getting into the specifics of the Hack++ computer and programs, there are a few important terms to clarify...

## What is a computer?

**A computer** can be best understood as five cooperating subsystems that execute program instructions and operates on 
data — quantifiable values representing, user input, intermediate state, and computed results.

::: tip Computer Subsystems
1. **Input** — receives program instructions and user input from the external environment
2. **Output** — returns intermediate state and computed results to the external environment
3. **Memory** — stores program instructions and data
4. **Datapath** — moves and performs arithmetic and logical operations on data
5. **Control Unit** — interprets program instructions and orchestrates their requested operations
:::

## What is a program?

**A program** is a set of instructions that are progressively lowered through multiple cooperating abstractions, 
moving from human intent toward machine execution.

::: info Program Lowering
1. **Design-Time** — transforms an idea into structured high-level source code
2. **Compile-Time** — analyzes and lowers source code into progressively lower-level representations
3. **Execution-Time** — executes machine code on target hardware
:::

## The Abstraction Ladder

These abstractions form the conceptual framework for Hack++, describing both how a computer is organized and 
how software is transformed. Hardware and software approach the computer abstraction from opposite directions: hardware 
builds upward from physical logic, while software lowers downward from human intent until both meet at the 
executing machine.

### Software Abstractions

Programs begin as structured source code and are gradually lowered into executable machine behavior:

```yml
┌──────────────────────┐
│      Human Idea      │
└──────────────────────┘
          │
          ▼
┌──────────────────────┐
│     Source Code      │
└──────────────────────┘
          │
          ▼
┌──────────────────────┐
│    Low-Level Code    │
└──────────────────────┘
          │
          ▼
┌──────────────────────┐
│     Machine Code     │
└──────────────────────┘
          │
          ▼
┌──────────────────────┐
│     The Computer     │
└──────────────────────┘
```

### The Computer Abstraction

These modules collectively realize the five classical computer subsystems. Together, these subsystems form the primary
abstraction: a programmable computer.

| **Hack++ Modules**                                                           | **Subsystem**     |
|------------------------------------------------------------------------------|-------------------|
| Program, Keyboard                                                            | **Input**         |
| Screen                                                                       | **Output**        |
| Random Access Memory (RAM), Read Only Memory (ROM), Memory-mapped I/O (MMIO) | **Memory**        |
| Arithmetic and Logic Unit (ALU), Data Bus, Address Bus                       | **Datapath**      |
| Central Processing Unit (CPU), Registers, Control Bus                        | **Control Unit**  |

### Hardware Abstractions

Hack++ is constructed through a strict hierarchy of increasingly capable structures. Each layer is built exclusively
from those defined below it, progressively assembling modules, subsystems, and ultimately a complete computer.

At the base of this all is a single universal primitive: **NAND**.

```yml
The Computer
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
