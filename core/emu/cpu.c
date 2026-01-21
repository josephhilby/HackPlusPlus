// Assumes:
//  - ROM is an array of 16-bit instructions
//  - mem_read16 / mem_write16 handle RAM + memory-mapped IO (SCREEN/KBD)
// Hack spec recap:
//  - A-instruction: 0vvvvvvvvvvvvvvv  => A = v, PC++
//  - C-instruction: 111accccccdddjjj
//      a selects A vs M (RAM[A]) for the "y" input in comp
//      ddd selects destinations (A, D, M)
//      jjj selects jump based on ALU output