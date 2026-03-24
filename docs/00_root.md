# Hack++ Reference

This section provides a structural overview of the complete Hack++ hardware and software stack, from primitive logic 
gates, to components, to subsystems, to fully programmable computer system, and supporting software toolchain. Each 
layer is constructed strictly from the layer beneath it, forming a continuous abstraction ladder that incrementally 
builds a working machine.

## What is a computer?

**The core of a computer** can be best understood as five cooperating subsystems...

::: tip Computer Subsystems
1. **Input** — receives programs, data, and user interaction from the external environment
2. **Output** — displays computed results to the external environment
3. **Memory** — stores program instructions and data (including user input)
4. **Datapath** — moves data and performs arithmetic and logical operations, as instructed
5. **Control Unit** — orchestrates instruction execution and program flow
:::

## What is a program?

**A program** can be understood as a set of instructions that lowered through four cooperating abstractions that get
progressively closer to machine execution over time...

::: info Software Toolchain
1. **Design-Time** — transforms an idea into structured high-level source code written by a human
2. **Compile-Time** — analyzes and translates source code into increasingly lower-level intermediate representations (IR)
3. **Run-Time** — models basic program execution through a standard virtual machine (VM) abstraction
    - In Hack++, these runtime behaviors are compiled away through translation rather than executed by the VM itself.
4. **Execution-Time** — executes machine instructions defined by the hardware instruction set architecture (ISA)
:::

## The Abstractions

These abstractions form the conceptual framework for Hack++, describing both how a computer is organized and 
how software is transformed. Hardware and software approach the computer abstraction from opposite directions: hardware 
builds upward from physical logic, while software lowers downward from human intent until both meet at the executing machine.

### Software Abstractions

Programs begin as structured source code and are gradually lowered into executable machine behavior:

```yml
                ┌── Human Idea (Design-time)
                ▼
           ┌── High-Level Language (Compile-time)
           ▼
      ┌── Virtual Machine (Runtime)
      ▼
 ┌── Instruction Set Architecture (Execution-time)
 ▼
The Computer
```

### The Computer Abstraction

These components collectively realize the five classical computer subsystems.

| **Component**    | **Hack++ Realization**                     |
|------------------|--------------------------------------------|
| **Input**        | Program (ROM), Keyboard (Memory)           |
| **Output**       | Screen (Memory)                            |
| **Memory**       | RAM, ROM, Memory-mapped I/O                |
| **Datapath**     | ALU, arithmetic circuits, routing circuits |
| **Control Unit** | CPU instruction decoding and control logic |

### Hardware Abstractions

Hack++ is constructed through a strict hierarchy of increasingly capable structures.
Each layer is built exclusively from the components defined below it, gradually assembling the
subsystems required for a complete computer.

At the base of this all is a single universal primitive: **NAND**.

```yml
The Computer
 ▲
 └── Control Unit, Datapath, Memory, Input, and Output Subsystems
      ▲
      └── Sequential Chips (Bit, Register, Program Counter)
           ▲
           └── Routing & Arithmetic Circuits (Mux, DMux, Add, Inc)
                ▲
                └── Boolean Logic Gates (Not, And, Or, Xor)
                     ▲
                     └── Universal Gate (NAND)
```
