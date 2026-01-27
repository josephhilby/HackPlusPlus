# Hack++ Memory (RAM and Memory-Mapped I/O)
The Hack++ Memory module implements the complete data address space of the Hack platform, including general-purpose 
RAM and memory-mapped I/O devices. It provides a unified interface for reading and writing memory while internally 
routing accesses to RAM, screen memory, or keyboard input based on the high-order address bits.

The module operates synchronously for writes and combinationally for reads, ensuring deterministic behavior across 
instruction cycles.

## Address Map
Only the lower 32K words (16-bit) of the 15-bit address space are defined. The memory map is partitioned as follows:

| Address Range (Hex) | Size   | Region   | Description                      |
| ------------------- | ------ | -------- | -------------------------------- |
| `0x0000–0x3FFF`     | 16K    | RAM      | General-purpose data memory      |
| `0x4000–0x5FFF`     | 8K     | Screen   | Memory-mapped display buffer     |
| `0x6000`            | 1 word | Keyboard | Memory-mapped keyboard register  |
| `> 0x6000`          | —      | Invalid  | Reads return `0`, writes ignored |

Address decoding is performed using the two most significant address bits (`address[14..13]`), which select the 
active memory region.

## Reading, and Writing
1. Region Select: The high-order bits `address[14..13]` select one of four regions...
   - `00` → RAM (0x0000–0x3FFF)
   - `01` → Screen (0x4000–0x5FFF)
   - `10` → Keyboard (0x6000)
   - `11` → Invalid / unused

2. Write Routing: A `DMux4Way` gates the `load` signal so that only the selected region receives a write enable
3. Read Routing: A `Mux4Way16` selects the output from the active region and drives `out`

## Implementation
```java
CHIP Memory {
IN in[16], load, address[15];
OUT out[16];

    PARTS:
    // Select Region
    DMux4Way(in=load, sel=address[13..14], a=mem1, b=mem2, c=scr, d=key);
    Or(a=mem1, b=mem2, out=mem);
    
    // RAM (16K)
    RAM16K(in=in, load=mem, address=address[0..13], out=memOut);
    
    // MMIO (8K + 1)
    Screen(in=in, load=scr, address=address[0..12], out=scrOut);
    Keyboard(out=keyOut);
    
    // Read
    Mux4Way16(a=memOut, b=memOut, c=scrOut, d=keyOut, sel=address[13..14], out=out);
}
```
