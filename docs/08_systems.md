# 08 — System Integration

This section documents the top-level composition of Hack++ into a complete, executing computer. At this layer, the processor, instruction memory, and data / I/O memory are unified into a closed execution loop that continuously fetches, decodes, executes, and commits program state.

**Related:**

* [Processor Components](./07_processor.md)
* [Memory Hierarchy](./06_memory.md)
* [Instruction Set Architecture](./09_instruction_set.md)

---

## Design Notes

**Modified von Neumann (Modified Harvard) Architecture**
Instruction memory (ROM) and data memory (RAM / MMIO) are physically separate subsystems, but logically unified through the CPU interface. This preserves simple control flow while allowing instruction fetch and data access to be reasoned about independently.

**Memory-mapped I/O**
All interaction with the external environment (screen, keyboard) occurs through ordinary memory reads and writes. No special I/O instructions exist at the ISA level.

**Deterministic execution**
The system’s behavior is fully determined by:

* The contents of ROM (the program)
* The current state of RAM
* The current values presented by memory-mapped input devices

This makes the platform suitable for simulation, debugging, and formal reasoning.

---

## Hack++ Computer — Top-Level Machine

The **Hack++ Computer** composes the CPU, instruction memory, and data / I/O memory into a single executable system.

### Interface

* Input: `reset`
* Internal buses: `instruction`, `inM`, `outM`, `address`, `pc`

### Reset Semantics

* `reset = 1`
  Forces the CPU’s program counter to `0`. On the next clock tick, the instruction at `ROM[0]` is fetched.

* `reset = 0`
  Normal execution continues, with the program counter advancing or jumping based on instruction semantics.

---

## Structural Overview

```text
           ┌────────────┐
           │   ROM32K   │
           │ (Program)  │
           └─────┬──────┘
                 │ instruction[16]
                 ▼
           ┌────────────┐
           │    CPU     │
           │ (Control + │
           │ Datapath)  │
           └─────┬──────┘
      outM[16]   │ addressM[15]
                 ▼
           ┌────────────┐
           │   Memory   │
           │ (RAM +     │
           │  MMIO)     │
           └─────┬──────┘
                 │ inM[16]
                 └───────────────┐
                                 ▼
                                CPU
```

This loop forms a closed feedback system:

```text
ROM → CPU → Memory → CPU → PC → ROM
```

---

## Execution Cycle

Hack++ follows a simple, single-cycle conceptual model. Each instruction completes its work within one clock interval, and architectural state is committed on the clock edge.

### Phases

1. **Fetch**

    * `pc(t)` addresses ROM
    * `instruction(t) = ROM[pc(t)]`

2. **Decode**

    * CPU classifies instruction (A vs C)
    * Control bits are extracted and routed to the ALU, registers, and jump logic

3. **Execute**

    * ALU computes `out(t)` from `x(t)` and `y(t)`
    * Flags `zr`, `ng` are generated

4. **Memory**

    * If destination includes `M`, `outM(t)` and `addressM(t)` are driven
    * Memory routes write enables or read values

5. **Writeback (Clock Edge)**

    * `A(t+1)`, `D(t+1)`, `RAM(t+1)`, `pc(t+1)` commit based on control signals

6. **Next Fetch**

    * The updated `pc` addresses the next instruction in ROM

### Summary Loop

```text
Fetch → Decode → Execute → Memory → Commit → Next PC
```

---

## Memory Map (System View)

From the system perspective, the CPU observes a **single, flat, 15-bit address space**. The physical routing is hidden behind the Memory subsystem.

| Address Range (Hex) | Size   | Region   | Function                    |
| ------------------- | ------ | -------- | --------------------------- |
| `0x0000–0x3FFF`     | 16K    | RAM      | General-purpose data memory |
| `0x4000–0x5FFF`     | 8K     | Screen   | Display framebuffer         |
| `0x6000`            | 1 word | Keyboard | Input register              |
| `> 0x6000`          | —      | Invalid  | Ignored / reads return `0`  |

This map enables programs to treat I/O as ordinary memory access, preserving a uniform programming model across the ISA and VM layers.

---

## HDL — Top-Level Integration

```java
CHIP Computer {

IN reset;

    PARTS:
    // Instruction Memory (ROM)
    ROM32K(address=pc, out=instruction);
    
    // Data + Memory-Mapped I/O
    Memory(in=in, load=load, address=address, out=inM);
    
    // Central Processing Unit
    CPU(inM=inM, instruction=instruction, reset=reset,
        outM=in, writeM=load, addressM=address, pc=pc);
}
```

---

## Architectural Context

This layer closes the full **abstraction ladder**:

* **Hardware** builds upward from NAND → Gates → ALU → CPU → Computer
* **Software** lowers downward from High-Level Language → VM → ISA → Machine Code

The **Computer** is the meeting point: where symbolic programs become physical signal transitions, and where computation becomes observable behavior.

Together with the [Instruction Set Architecture](./09_instruction_set.md), this defines the complete contract between *program* and *machine*.
