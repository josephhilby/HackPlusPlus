# OS, (in progress)

### HACK to HACK++

magic address triggers a system call, this is a hardware problem that changes CPU contexts and trap to kernel at address 0,
then control is back to software. this will require a bootstrap that first checks for a cold boot or system call

```
(KERNEL_ENTRY)
    // If R13 is 0, trap.
    // Else, cold boot.
    @R13
    D=M
    @KERNEL_TRAP
    D;JEQ

    // Cold Boot
    @256
    D=A
    @SP
    M=D
    @LCL
    M=0
    @ARG
    M=0
    // Call Sys.init

(KERNEL_TRAP)
    // Call Sys.trap
```

Need to write an OS syscall router (Sys.trap) so that every user program will have a `libj` (library jack) that will contain
the system call IDs (one-to-one map to os functions). these can be loaded at the bottom of the heap. labels `SID`
(SID != 0), `SA1`, `SA2` to pass id, and two args to the kernel.

need to wrap every libj syscall with a magic number in R13 (0x0000)

### Page Sizes

Looking at the provided pong.asm.
consider a stack of 256-2044, heap 2045-16380, mailboxes of 16381..16383

Use small and large page sizes to divide up the 14,336 heap into 112 uniform 128-word master blocks:

- Large (128-word):
  - 104 master blocks
    - 104 large pages (one-to-one)
- Small (16-word):
  - 8 master blocks
    - 8 small blocks (8 to 1)
    - 64 small pages total

#### Page Descriptor Tables

- Small: Index 0..63 for small pages
  - S_HD for zero index addr (first indexed page)
  - S_CURR for curr head index (first free page)
- Large: Index 0..103 for large pages
  - L_HD for zero index addr
  - L_CURR for curr head index

```text
Bit:   15         9  8   7   6   5        0
      ┌────────────┬───┬───┬───┬───────────┐
      │ NEXT_INDEX │ A │ L │ E | RESERVED  │
      └────────────┴───┴───┴───┴───────────┘
```

- NEXT_INDEX: Links directly to the next free block index in the list
- A (Active): 0 = Free, 1 = Allocated
- L (Linked): 0 = Terminal Block, 1 = Contiguous with Index + 1
- T (End List?): 0 = false, 1 = true

Mapping

- size = 4 if S_HD, 7 if L_HD
- `ptr = x_HD + (table_index << size)`
- `table_index = (ptr - x_HD) >> size`

#### Screen Map

The Hack platform screen exposes 8K words of 16-bits at address `0x4000–0x5FFF`,
each bit represents a screen pixel.

- 131,072 pixels
- 256 rows (16 words / column)
- 512 cols (32 words / row)
- `word = SCREEN + (32 x row) + (col / 16)`
- `offset = col % 16`
- 1 = black, 0 = white

| Row Index | Start Address | End Address |
| --------- | ------------- | ----------- |
| Row 0     | 0x4000        | 0x401F      |
| Row 1     | 0x4020        | 0x403F      |
| ...       | ...           | ...         |
| Row 255   | 0x5FE0        | 0x5FFF      |
