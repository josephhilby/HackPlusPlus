# Memory Hierarchy

This section documents the Hack++ **unified data memory subsystem**, including general-purpose RAM and memory-mapped I/O (MMIO).

The Hack platform exposes a single 15-bit address space to the CPU. The `Memory` chip implements that space by decoding the address and routing each access to one of:

* **RAM16K** (general-purpose data memory)
* **Screen** (memory-mapped display buffer)
* **Keyboard** (memory-mapped input register)

**Related:**

* [Sequential Building Blocks](./05_sequential.md)
* [Processor Components](./07_processor.md)
* [System Integration](./08_system.md)

---

## Design Notes

**Unified address space**
From the CPU’s perspective, all data accesses use the same interface:

* `address` selects the target word
* `in` is the write-data bus
* `load` is the write-enable
* `out` is the read-data bus

Internally, the `Memory` chip decodes the address and dispatches reads/writes to the correct region.

**Read vs write timing**
The memory subsystem follows the standard Hack timing model:

* **Read (combinational):** `out(t) = Memory[address(t)](t)`
* **Write (clocked):** if `load(t) = 1`, then `Memory[address(t)](t+1) = in(t)`

In other words, writes commit on the next clock edge; reads reflect the currently stored value.

**Address decoding via high bits**
Region selection is determined by the high-order address bits. In this implementation, `address[13..14]` partitions the address space into four 8K-sized quadrants.

---

### Memory Map
The Hack platform's RAM exposes 32K words of 16-bit, mapped as follows (decimal addresses):

| Address Range       | ASM Name        | Usage                                                |
|---------------------|-----------------|------------------------------------------------------|
| `RAM[0]`            | `R0`/`SP`       | Current top of the stack                             |
| `RAM[1]`            | `R1`/`LCL`      | Base of the current function's local segment         |
| `RAM[2]`            | `R2`/`ARG`      | Base of the current function's argument segment      |
| `RAM[3]`            | `R3`/`THIS`     | Base of the current function's `this` segment (heap) |
| `RAM[4]`            | `R4`/`THAT`     | Base of the current function's `that` segment (heap) |
| `RAM[5..12]`        | `R5..12`/`TEMP` | Segment for current function's temporary storage     |
| `RAM[13..14]`       | `R13..R14`      | General-purpose registers                            |
| `RAM[15]`           | `R15`/`RA`      | Return Address register                              |
| `RAM[16..255]`      | —               | Static variables (assigned at compile time)          |
| `RAM[256..2047]`    | —               | Stack                                                |
| `RAM[2048..16383]`  | —               | Heap                                                 |
| `RAM[16384..24575]` | `SCREEN`        | Memory-mapped video I/O (512×256 monochrome display) |
| `RAM[24576]`        | `KBD`           | Memory-mapped keyboard I/O (Last key pressed)        |
| `RAM[24577..32767]` | —               | Unused                                               |
<p align="right">(<a href="#Acknowledgments">see Acknowledgments, Charles Stevenson</a>)</p>

## Address Map

Only the lower portion of the 15-bit address space is defined by the Hack platform. The `Memory` chip implements the following map:

| Address Range (Hex) | Size   | Region   | Description                      |
| ------------------- | ------ | -------- | -------------------------------- |
| `0x0000–0x3FFF`     | 16K    | RAM      | General-purpose data memory      |
| `0x4000–0x5FFF`     | 8K     | Screen   | Memory-mapped display buffer     |
| `0x6000`            | 1 word | Keyboard | Memory-mapped keyboard register  |
| `> 0x6000`          | —      | Invalid  | Reads return `0`, writes ignored |

### Region select (address[14..13])

| `address[14..13]` | Region             | Notes                  |
| ----------------: | ------------------ | ---------------------- |
|              `00` | RAM (low 16K)      | `0x0000–0x3FFF`        |
|              `01` | Screen             | `0x4000–0x5FFF`        |
|              `10` | Keyboard / invalid | only `0x6000` is valid |
|              `11` | Invalid            | unused                 |

> Note: The Hack spec treats `0x6000` as keyboard and does not define behavior for addresses above it. This implementation returns `0` on reads and ignores writes for out-of-range addresses.

---

## Read and Write Routing

The `Memory` chip implements routing in three steps:

1. **Region select**
   The high bits `address[13..14]` choose one of four regions.

2. **Write routing**
   A `DMux4Way` gates the `load` signal so that only the selected region receives a write-enable. For the RAM half, the two RAM quadrants are OR’ed together to form the single `RAM16K` write enable.

3. **Read routing**
   A `Mux4Way16` selects the output from the active region and drives `out`.

---

## Implementation

```java
CHIP Memory {
IN in[16], load, address[15];
OUT out[16];

    PARTS:
    // Decode the write-enable into one-hot region enables
    DMux4Way(in=load, sel=address[13..14], a=mem1, b=mem2, c=scr, d=key);

    // Combine the two RAM quadrants into the single 16K RAM enable
    Or(a=mem1, b=mem2, out=mem);

    // RAM (0x0000–0x3FFF)
    RAM16K(in=in, load=mem, address=address[0..13], out=memOut);

    // MMIO
    // Screen (0x4000–0x5FFF)
    Screen(in=in, load=scr, address=address[0..12], out=scrOut);

    // Keyboard (0x6000)
    Keyboard(out=keyOut);

    // Read mux: select the active region’s output
    Mux4Way16(a=memOut, b=memOut, c=scrOut, d=keyOut, sel=address[13..14], out=out);
}
```

---

## Architectural Context

The memory subsystem is where sequential storage becomes **programmable state**:

* The CPU uses `addressM` to select a word and `writeM` to commit writes.
* The `Memory` chip routes the access to either RAM (program state), Screen (output), or Keyboard (input).
* Higher-level software abstractions (stack, heap, static segment, VM memory commands) ultimately lower into reads and writes through this single unified interface.

This is the bridge between instruction execution and observable I/O behavior on the Hack platform.
