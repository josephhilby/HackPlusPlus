/*
 Flags: bit 0 is LSB
    bit 0: 1 running, 0 stopped
    bit 1: 1 program in rom, 0 rom empty
    bit 2: 1 error, 0 normal
 */
#define FLAG_RUNNING 0x0001 // 0b 0000 0000 0000 0001
#define FLAG_LOADED  0x0002 // 0b 0000 0000 0000 0010
#define FLAG_ERROR   0x0004 // 0b 0000 0000 0000 0100

/*
 Screen is 256 (rows) x 512 (cols) = 131,072 pixles, represented in bits (1=black, 0=white)
    131,072 bits / 16 (bits/word)
    8192 words
    MMIO Screen offset in RAM by 16384 words
    screen[index] = RAM[16384 + inded]

 Accessing screen as a function of row and column
    Note: row (0 <= r <= 255), col (0 <= c <= 511)
    First, access the word
        f(r, c) = (r * 32) + (c / 16) = screen word
    Second, add the screen offset in RAM
        screen word += 16384
    Third, access the bit with a bytemask
        offset = c % 16
        mask = 1 << offset
        RAM[screen word] |= mask
*/
#define SCREEN_WORDS 8192

//#define RAM_WORDS 24577
#define ROM_WORDS 32768