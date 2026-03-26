# Hack++ Reference

This section provides a quick structural overview of the Hack++ hardware and software systems. From logic 
gates, to circuits, to subsystems, to fully programmable computer system, and supporting software toolchain, these
systems work synchronously in to constitute the Hack++ Computer. 

But, before getting into the specifics of the Hack++ computer and programs, there are a few important terms to clarify...

## What is a computer?

**A computer** can be best understood as five cooperating subsystems...

::: tip Computer Subsystems
1. **Input** — receives programs, data, and user interaction from the external environment
2. **Output** — displays computed results to the external environment
3. **Memory** — stores program instructions and data (including user input)
4. **Datapath** — moves data and performs arithmetic and logical operations, as instructed
5. **Control Unit** — orchestrates instruction execution and program flow
:::

## What is a program?

**A program** can be understood as a set of instructions that are lowered, or transformed, through multiple cooperating 
abstractions inorder to get progressively closer to machine execution...

::: info Software Toolchain
1. **Design-Time** — transforms an idea into structured high-level source code written by a human
2. **Compile-Time** — analyzes and translates source code into increasingly lower-level intermediate representations (IR)
3. **Run-Time** — defines the semantic behavior of program execution (often modeled as a virtual machine)
   - In Hack++, these runtime behaviors are compiled away through translation rather than executed by a VM.
4. **Execution-Time** — executes machine instructions defined by the hardware instruction set architecture (ISA)
:::

## The Abstractions

These abstractions form the conceptual framework for Hack++, describing both how a computer is organized and 
how software is transformed. Hardware and software approach the computer abstraction from opposite directions: hardware 
builds upward from physical logic, while software lowers downward from human intent until both meet at the 
executing machine.

### Software Abstractions

Programs begin as structured source code and are gradually lowered into executable machine behavior:

```yml
          ┌──────────────────────┐
          │     Human Idea       │
          └──────────────────────┘
                    │
              (Design-Time)
                    ▼
          ┌──────────────────────┐
          │    Source Code       │
          └──────────────────────┘
                    │
              (Compile-Time)
                    ▼
          ┌──────────────────────┐
          │  Lower-Level Program │
          │   (IR / Assembly)    │
          └──────────────────────┘
                    │
             (Run-Time Model)
                    ▼
          ┌──────────────────────┐
          │   Machine Program    │
          │   (Compiled Output)  │
          └──────────────────────┘
                   │
            (Execution-Time)
                   ▼
          ┌──────────────────────┐
          │    The Computer      │
          └──────────────────────┘
```

### The Computer Abstraction

These components collectively realize the five classical computer subsystems.

| **Hack++ Component**                                                         | **Subsystem**     |
|------------------------------------------------------------------------------|-------------------|
| Program, Keyboard                                                            | **Input**         |
| Screen                                                                       | **Output**        |
| Random Access Memory (RAM), Read Only Memory (ROM), Memory-mapped I/O (MMIO) | **Memory**        |
| Arithmetic and Logic Unit (ALU), Data Bus, Address Bus                       | **Datapath**      |
| Central Processing Unit (CPU), Control Bus                                   | **Control Unit**  |

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
      └── Functional Modules (ALU, Register)
           ▲
           └── Combinational & Sequential Circuits (Mux, Add, Bit, etc.)
                ▲
                └── Boolean Logic Gates (Not, And, Or, Xor)
                     ▲
                     └── Universal Gate (NAND)
```
