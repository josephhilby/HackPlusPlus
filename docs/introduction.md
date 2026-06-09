# Hack++ Reference

Before diving into implementation, we will decompose our ultimate goal—building a functioning computer and software system—into its component parts. This top-down decomposition will allow us to establish clear architectural principles and enforce a strict separation of concerns across all subsequent components.

<SystemHierarchy />

::: details Computer Hardware Interface

```hdl
CHIP Computer {
    IN reset;

    PARTS:
    // Central Processing Unit
    CPU(reset=reset,
        inD=inD, writeD=load, addrD=addrD, outD=outD,  // Databus
        inI=inI, kern=domain, outI=addrI);             // Controlbus

    // Data + Memory-Mapped I/O (RAM)
    Memory(in=outD, load=load, address=addrD, out=inD);

    // Application or OS (ROM)
    Instruction(in=addrI, sel=domain, out=inI);
}
```

:::

## Hardware Subsystem

Rather than a single monolithic entity, a computer's hardware is an ecosystem of cooperating modules that collect input, execute
instructions, manipulate data, and deliver output.

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
