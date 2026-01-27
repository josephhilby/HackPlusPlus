# Hack++ Computer (Top-Level System)
The Hack++ Computer is the top-level integration of the processor and memory subsystems. It composes the CPU, 
instruction memory (ROM), and data memory (RAM + memory-mapped I/O) into a complete, executable computing platform.

When reset is asserted, the system restarts program execution from address 0. When reset is de-asserted, the program 
stored in ROM executes continuously, interacting with memory-mapped devices such as the screen and keyboard.

## Reset semantics
- `reset = 1`
  - Forces the CPU’s program counter to `0`. On the next tick, the instruction at `ROM[0]` is fetched
- `reset = 0`
  - Normal execution continues, with the program counter advancing or jumping based on instruction semantics

## Design notes

#### Modified von Neumann (or Modified Harvard) architecture:
Instruction memory (ROM) and data memory (RAM / MMIO) are physically separate, but unified at the system 
level through the CPU.

#### Memory-mapped I/O:
All external interaction (screen, keyboard) occurs via standard memory read/write operations. The CPU does not 
require special I/O instructions.

#### Deterministic execution:
The machine’s behavior is fully determined by the ROM contents and the state of memory-mapped inputs, making it 
suitable for both simulation and formal reasoning.

## Implementation
```java
CHIP Computer {

IN reset;

    PARTS:
    // Instruction Memory
    ROM32K(address=pc, out=instruction);
    
    // Data and MMIO Memory
    Memory(in=in, load=load, address=address, out=inM);
    
    // CPU
    CPU(inM=inM, instruction=instruction, reset=reset, outM=in, writeM=load, addressM=address, pc=pc);
}
```