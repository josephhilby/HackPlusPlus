# Hack++ Reference

Before exploring the granular details of Hack++, we will first decompose the system into its primary hardware and software subsystems.
By establishing these two domains early, we can select the best high-level architectural specifications for this project — 16-bit ISA, the
Harvard-based physical separation, and the privileged User-Kernel model.

<SystemHierarchy dataType="default" />

## Hack++ System

At the absolute foundation of the Hack++ architecture is a **16-bit word and fixed instruction size**. This 16-bit size will dictate the width of all datapaths
storage structures, and software instructions.

### Instruction Set Architecture

Within this constraint, we can start to define our **Instruction Set Architecture (ISA)** — the essential bridge between hardware and software.

::: warning The ISA

- On the `software` side, the ISA provides a set of binary codes that can be invoked to command machine behavior.
- On the `hardware` side, the ISA acts as the architectural specification that hard-wires those commands into physical logic.

:::

The ISA will be composed of two fundamental instruction types: the `a_instruction` (Address/Constant selection) and the `c_instruction`
(Compute or Control). To choose between these two, the first bit in an instruction will be used to denote what type is being selected
(a.k.a., opcode).

_This limits all consents and addresses to `32K`._

::: tip A-Instruction (Address or Constant)

An opcode of `0` will denote an `a_instruction`, with the remaining 15-bits encoding an integer between `0` and `32,767`.
This integer could be used as an address in the `Memory()` / `Instruction()` module, or constant value to be loaded and computed.

```
 opcode integer
```

:::

::: tip C-Instruction (Compute or Control)

An opcode of `1` will denote a `c_instruction`, the next two groups (a & compute) will set the desired `CPU()` computation. Following that a
destination group will control the location in `Memory()` where the output is placed and, finally, the jump group will control the
`Instruction()` jump criteria.

```
 opcode compute destination jump
```

:::

## Hardware Subsystem

Rather than a single monolithic entity, a computer's hardware is best thought of as an ecosystem of cooperating modules that collect input, execute
instructions, manipulate data, and deliver output.

::: details Hardware Diagram

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

### Harvard Model

To orchestrate these components we employ the Harvard model, which completely separates instructions and data into separate
physical modules. This explicit boundary removes instruction code-space segmentation from our memory management requirements,
simplifying our runtime environment.

::: tip Harvard Model

1. **Central Processing Unit (CPU):** The primary execution engine of the computer.
2. **Memory (RAM):** Volatile storage for maintaining intermediate states, variables, and I/O.
3. **Instruction (ROM):** Non-volatile storage housing instruction sets.

:::

Additionally, this architectural split complements the software-level User-Kernel model, allowing us to isolate
hardware-enforced context switches entirely within a single CPU-driven component, the `Instruction()` module.

## Software Subsystem

A software program in execution is ultimately a stream of binary values written for the hardware's native instruction set. High-level
concepts—such as printing characters, graphics, and user input—do not exist within the core hardware, but must be constructed above
it.

To relieve the user of this responsibility we will construct an Operating System (OS), that will be exposed to the user via a simple
standard library `<libj>`. This OS will handle all hardware communications as well as add a few helper functions to help overcome the
ISA's relatively limited functionality.

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

### User-Kernel Model

To protect the OS from the user and their program, we adhere to a strict User-Kernel model. This architecture divides the software subsystem
into two distinct operational domains:

::: tip User-Kernel Model

- **OS Domain (Kernel Space):** The privileged domain responsible for resource management (`Memory`, `Sys`), application support (`Screen`, `Output`, `Keyboard`), and core language extensions (`Math`, `String`).
- **Application Domain (User Space):** The unprivileged domain. It is a highly permissive instruction block reserved entirely for user-level programs and custom utilities to safely execute within.

:::

To navigate between these two spaces we will use a Trap Vector, a specialized instruction sequence that has a unique hardcoded CPU response. Briefly
taking control from the `Instruction()` module, setting the computer in a specific state, then returning control.

::: warning Trap Vector (The Magic Address)

When the CPU detects a jump to `0x7FFF`—initiated by the sequence `@32767` `0;JMP`—it triggers a context switch:

- **The Intercept:** The CPU intercepts the jump and toggles the `kernel flag` bit.
- **The Context Save:** The CPU saves a return address in a special link register (LR).
- **ROM Switching:** This bit-flip redirects the `Instruction()` module, instantly swapping the active ROM bank between User and Kernel.

_`0x7FFF` is an unused `Memory()` address and the final addressable `Instruction()`. To prevent conflicts, our assembler prohibits assembled programs from referencing this location outside of a context switch. So, for the sacrifice of a single instruction word, we gain a dedicated parallel instruction space for the OS._

:::
