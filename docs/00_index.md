# Hack++ Hardware Reference

This section documents the complete Hack++ hardware stack, from primitive logic gates to the full computer system, and
software toolchain. Each layer is built strictly from the layer below it, forming a complete abstraction ladder.

Hardware layers define execution semantics (signals, state transitions, and memory effects). While software layers 
define surface syntax and structure (formal grammars and program representations) that are deterministically lowered 
into machine behavior.

## Abstraction Ladder (Construction vs. Conceptual Role)
| Construction                                                 | Conceptual Role                                |
|--------------------------------------------------------------|------------------------------------------------|
| **NAND**                                                     | Physical primitive                             |
| **Logic Gates** (`Not`, `And`, `Or`, `Xor`)                  | Boolean operations                             |
| **Arithmetic & Routing** (`Add16`, `Inc16`, `Mux*`, `DMux*`) | **Datapath** (compute + select)                |
| **Registers** (`Bit`, `Register`, `PC`)                      | **State** (clocked storage)                    |
| **Memory** (`RAM*`, `Memory` / MMIO)                         | **Memory subsystem** (addressed storage + I/O) |
| **ALU**                                                      | Datapath core (compute + flags)                |
| **CPU**                                                      | Control + datapath integration                 |
| **Computer**                                                 | System integration (CPU + ROM + RAM/MMIO)      |
| **ISA**                                                      | Execution abstraction (ISA, linear)            |
| **VM**                                                       | Runtime abstraction (VM, stack)                |
| **High-Level Language**                                      | Compile-time abstraction (AST, tree)           |

*Note:* The ladder is bidirectional. Hardware layers are constructed upward from NAND to Computer, while software 
layers are lowered downward from Compiler to machine code executed by the CPU.

## Sections
1. [Primitive Gates](./01_primitives.md)
2. [Wide Combinational Gates](./02_wide_gates.md)
3. [Routing & Control Gates](./03_routing.md)
4. [Arithmetic Units](./04_arithmetic.md)
5. [Sequential Building Blocks](./05_sequential.md)
6. [Memory Hierarchy](./06_memory.md) 
7. [Processor Components](./07_processor.md)
8. [System Integration](./08_system.md)
9. [Instruction Set Architecture](./09_instruction_set.md)

## Reading Guide
- Start at **Primitive Gates-Memory Hierarchy** to understand the physical logic foundation
- Jump to **Processor Components** to see how computation and control flow are implemented
- Use **System Integration** to understand how the full machine executes programs
- Refer to **Instruction Set Architecture** and the **EBNF grammars** to see how formal syntax maps onto concrete machine behavior.