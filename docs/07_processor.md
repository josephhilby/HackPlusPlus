7. Processor Components

Control and datapath integration

07_processor.md

ALU

PC

CPU

# Processor Components

This section documents the computational and control units that form the Hack++ processor.

## Arithmetic Logic Unit (ALU)
<!-- Paste alu.md content here -->

**Related:**
- [Instruction Set Architecture](./09_instruction_set.md)
- [Routing & Control Gates](./03_routing.md)
- [Arithmetic Units](./04_arithmetic.md)

## Program Counter (PC)
<!-- Paste pc.md content here -->

## Central Processing Unit (CPU)
<!-- Paste cpu.md content here -->

**Related:**
- [ALU](#arithmetic-logic-unit-alu)
- [Program Counter](#program-counter-pc)
- [Instruction Set Architecture](./09_instruction_set.md)
- [Memory Hierarchy](./06_memory.md)

## Signal Flow Summary
Brief diagram or bullet list:
- D → ALU.x
- A/M → ALU.y
- ALU.out → {A, D, M}
- ALU.flags → Jump Logic → PC