# Hack++ Reference

Before exploring the granular details of Hack++, we must first establish the foundational contract between hardware and software. By decomposing the
system into these two primary domains, we can then define the architectural constraints—a 16-bit ISA, Harvard-based separation, and a privileged
User-Kernel model—that will enforce a strict separation of concerns and ensure system integrity across every subsequent module.

<SystemHierarchy />

## Hack++ System

At the absolute foundation of the Hack++ architecture is a **16-bit word and instruction size**. This single choice dictates the width of our datapaths,
storage structures, and software instructions.

::: tip 16-Bit Model

- **Data Size:** Every internal CPU register, and memory location in RAM or ROM, stores exactly one 16-bit value
  (2 bytes, or 4 nybbles), between `0` to `65,535`, or `64Kib`.
- **Instruction Size:** To account for the systems two instruction types, the first bit of each instruction is reserved as
  a flag that signifies its type (a.k.a., its opcode). This further reduces the values to `0` to `32,767`, or `32Kib`.

:::

### Hardware-Software Interface

Within this constraint, we define our Instruction Set Architecture (ISA) as the essential bridge between hardware and software. On the software
side, the ISA provides a set of binary codes that command machine behavior; on the hardware side, it acts as the architectural specification that
hard-wires those commands into physical logic. This ISA is composed of two fundamental execution patterns: the A-Instruction (Address/Constant selection)
and the C-Instruction (Compute or Control)."

::: tip A-Instruction (Address or Constant)

A opcode of `0` will denote an `a_instruction`, with the remaining 15 bits as the binary value of an integer between `0` and `32,767`.
This integer could be used as an address in RAM or ROM, or constant value to be loaded in a register to be computed.

```
0b 0vvv vvvv vvvv vvvv
   ^     address
```

:::

::: tip C-Instruction (Compute or Control)

A opcode of `1` will denote a `c_instruction`, the following two `ones` are unused, and the remaining groups (a, comp, dest, and jump) will control
the desired ALU computation, RAM destination for the result, and ROM jump criteria. This will be expanded on later.

```
0b 111 a c1 c2 c3 c4 c5 c6 d1 d2 d3 j1 j2 j3
   ^          comp           dest     jump
```

:::

::: warning Trap Vector

Our addressable RAM space ends at `0x6000`, leaving a reserved "hole" in the A-Instruction address space from `0x6001` to `0x7FFF`. We designate `0x7FFF` within this gap
as a "Magic Address." When the CPU detects a jump to this location—triggered by the sequence `@32767` followed by `0;JMP`—it triggers an atomic context switch.

- The Switch: The CPU intercepts the jump and toggles the domain bit.
- Bank Switching: This bit-flip redirects the Instruction module, instantly swapping the active ROM bank between User and Kernel modes.

This implementation provides a hardened, hardware-enforced barrier that isolates application logic from system resources while effectively doubling our total instruction capacity.

:::

## Hardware Subsystem

Rather than a single monolithic entity, a computer's hardware is an ecosystem of cooperating modules that collect input, execute
instructions, manipulate data, and deliver output.

::: details Hardware Diagram

```hdl
CHIP Computer {
    IN reset;

    PARTS:
    // Central Processing Unit
    CPU(reset=reset,
        inD=inD, writeD=load, addrD=addrD, outD=outD,  // Datapath
        inI=inI, kern=domain, outI=addrI);             // Controlpath

    // Data + Memory-Mapped I/O (RAM)
    Memory(in=outD, load=load, address=addrD, out=inD);

    // Application or OS (ROM)
    Instruction(in=addrI, sel=domain, out=inI);
}
```

:::

### Harvard Model

To orchestrate these components we employ the Harvard model, which completely separates instructions and data into distinct
physical modules. This explicit boundary removes instruction code-space segmentation from our memory management requirements,
simplifying our runtime environment.

::: tip Harvard Model

1. **Central Processing Unit (CPU):** The primary execution engine of the computer.
2. **Memory (RAM):** Volatile storage for maintaining intermediate states, variables, and I/O.
3. **Instruction (ROM):** Non-volatile storage housing instruction sets.

:::

Additionally, this architectural split complements the software-level User-Kernel model, allowing us to isolate
hardware-enforced context switches entirely within a single CPU-driven component: the `Instruction()` module.

### Hardware Breakdown Levels

To build this architecture from the ground up, our hardware implementation scales through four distinct layers of abstraction:

::: info Hardware Breakdown Levels

- **Modules (CPU, Memory, Instruction):** The macro-level components that define the top-level computer.
- **Components (ALU, PC, Register, Register Bank, etc.):** Complex circuits designed to fulfill a module required behavior.
- **Circuits (Mux, Add, Bit, etc.):** Composite implementations of logic gates, allowing for atomic level machine behavior and time-dependent state.
- **Boolean Logic Gates (Not, And, Or, Xor):** Time-independent logical behavior.

:::

## Software Subsystem

A software program in execution is ultimately a stream of binary values written for the hardware's native instruction set. High-level
concepts—such as printing characters, graphics, and user input—do not exist within the core hardware, but must be constructed above
it.

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
                            LR Retrieved**         Mailboxes Set (SID, SA1, SA2)
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

### User-Kernel Model

To protect system integrity and abstract hardware complexities from the application developer, we adhere to a strict User-Kernel
model. This architecture divides the software subsystem into two distinct operational domains:

::: tip User-Kernel Model

- **OS Domain (Kernel Space):** The privileged domain responsible for resource management (`Memory`, `Sys`), application support (`Screen`, `Output`, `Keyboard`), and core language extensions (`Math`, `String`).
- **Application Domain (User Space):** The unprivileged domain. It is a highly permissive instruction block reserved entirely for user-level programs and custom utilities to safely execute within.

:::

### Software Toolchain

Building programs directly in native binary is inefficient. To avoid this, we will utilize an external compilation toolchain
to translate high-level syntax down to execution-ready machine code through several layers:

::: info Intermediate Representations

- **High-Level Language (Jack):** A human-readable, structured, object-oriented language used to write both user applications and the underlying OS classes.
- **VM Bytecode:** A platform-independent intermediate representation that establishes stack-based operations and virtual memory segments.
- **Assembly (ISA):** A human-readable text representation of the hardware's exact instruction set architecture.
- **Machine Binary:** The final stream of raw ones and zeros executed directly by the CPU.

:::
