# Hack++ Reference

Before examining the hardware and software subsystems in isolation, we begin by analyzing the Hack++ system as an integrated whole.
This approach helps us define our core architectural specifications at a high level — 16-bit ISA, Harvard-based memory separation, and
a privileged User-Kernel model — establishing a unified foundation prior to diving down into lower-level specificity.

<SystemHierarchy dataType="default" />

## Hack++

At the absolute foundation of the Hack++ architecture is a **16-bit word and fixed instruction size**. This 16-bit size will dictate many
future design decision including: datapath width, storage structure size, and software instructions.

Within this constraint, we can start to define our **Instruction Set Architecture (ISA)** — the essential bridge between hardware and software.

::: info The ISA

- On the `software` side, the ISA provides a set of binary codes that can be invoked to command machine behavior.
- On the `hardware` side, the ISA acts as the architectural specification that hard-wires those commands into physical logic.

:::

Our ISA will be composed of two fundamental instruction types: the `a_instruction` (Address/Constant selection) and the `c_instruction`
(Compute or Control). To distinguish between the two, the first bit (a.k.a., opcode) in any instruction will be reserved to denote its type.

> - _Note: Consuming a bit for the opcode will limit all consents and addresses to `32K`._

::: tip A-Instruction (Address or Constant)

An opcode of `0` will denote an `a_instruction`, leaving the remaining 15-bits able to encode an integer between `0` and `32,767`.
This integer could be used as an address in the `Memory()` or `Instruction()` modules, or a constant value to be loaded into the
`CPU()` and computed.

```ISA_Pattern
 opcode   integer
```

:::

::: tip C-Instruction (Compute or Control)

An opcode of `1` will denote a `c_instruction`, the remaining bits can then be grouped to accomplish three specific tasks: setting the desired
`CPU()` computation, selecting the destination in `Memory()` where that computation's output is placed, and controlling the `Instruction()`
jump criteria by which that computation's output will be compared.

These three tasks can be grouped in one of three ways: `compute`, `compute destination`, or `compute jump`.

```ISA_Pattern
 opcode   compute  [  destination  |  jump  ]
```

> **Legend**
>
> - `[ … ]` = optional (zero or one)
> - `|` = alternative

:::

## Software

A software program in execution is ultimately a set of binary values written for the hardware's native ISA. High-level
concepts — such as printing characters, graphics, and user input — do not exist within the core architecture, but must be
constructed above it.

To relieve the user of this task, we will construct an Operating System Kernel to handle these functions and Standard Library
(`<libj>`) that will act as an interface for the user program to access these kernel functions.

::: tip User-Kernel Model

- **OS Domain (Kernel Space):** The privileged domain responsible for resource management (`Memory`, `Sys`), application support (`Screen`, `Output`, `Keyboard`), and core language extensions (`String`).
- **Standard Library Domain (User Space):** A section of the unprivileged domain responsible for some helper subroutines (`Math`) and exposing the kernel space resources to the user (`Libj`).
- **Application Domain (User Space):** The unprivileged domain. It is a highly permissive instruction block reserved entirely for user-level programs and custom utilities to safely execute within.

:::

::: details Software Diagram

```text
                           ( Reset Vector )
                          [ Hardware Reset ]
                                  │
                                  ▼
               ┌──────────────────────────────────────┐
               │         KERNEL MODE (kern = 1)       │◄───────┐
               │                                      │        │
               │  • Addr 0x0000                       │        │
               │  • Check Boot Flag:                  │        │
               │    - Cold init  -OR-                 │        │
               │      Request Dispatch (by SID #)     │        │
               └──────────────────┬───────────────────┘        │
                                  │                            |
                        [ Lower to User Mode ]       [ User Calls <Libj> ]
                       Mailboxes Set (SA1, SA2)*           LR Saved
                            LR Retrieved**       Mailboxes Set (SID, SA1, SA2)
                             Trap Return                  Trap Vector
                                  │                            │
                                  ▼                            │
               ┌──────────────────────────────────────┐        │
               │          USER MODE (kern = 0)        │        │
               │                                      │        │
               │  • Runs Users-Space Loop             │        │
               │  • Direct 'hardware' access blocked  │────────┘
               └──────────────────┬───────────────────┘
                                  │
                          [ Hardware Reset ]
                                  │
                                 ...
                       ( back to Reset Vector )

* Mailboxes used for returning ptr (malloc), char (keyboard), etc.
** Link Register (LR) not used on cold boot (link to 0x0000).
```

:::

## Hardware

A computer's hardware is best thought of as an ecosystem of cooperating modules that collect input, execute instructions, manipulate data,
and deliver output.

To orchestrate these modules we employ the Harvard model, which isolates instructions and data into separate
physical modules.

::: tip Harvard Model

1. **Central Processing Unit (CPU):** The primary execution engine of the computer.
2. **Memory (RAM):** Volatile storage for maintaining intermediate states, variables, and I/O.
3. **Instruction (ROM):** Non-volatile storage housing instruction sets.

:::

::: details Hardware Description

```hdl
CHIP Computer {
    IN reset, keyboard[16];

    PARTS:
    // Central Processing Unit
    CPU(reset=reset,
        inD=inD, writeD=load, addrD=addrD, outD=outD,  // Datapath
        inI=inI, kern=domain, outI=addrI);             // Controlpath

    // Data + Memory-Mapped I/O (RAM)
    Memory(key=keyboard, in=outD, load=load, address=addrD, out=inD);

    // Application or OS (ROM)
    Instruction(in=addrI, sel=domain, out=inI);
}
```

:::

This architectural split complements the software-level User-Kernel model, allowing us to isolate
hardware-enforced context switches entirely within a single CPU-driven component, the `Instruction()` module.
