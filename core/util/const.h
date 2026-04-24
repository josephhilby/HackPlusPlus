#define RAMSIZE 32768
#define SCREEN_BASE 16384
#define SCREEN_LEN 8192

// screen is 256 x 512 = 131,072 pixles, represented in bits (1=black, 0=white)
// MMIO is an array of 8K words of 16-bits each, 16 KiB in RAM, uint16_t screen[0..8192]
// each row has 32 words (32 words * 16 bits/word = 512 bits)
// mapping is screen[i], where i(r, c) = (r * 32) + (c / 16)
// then logicaly shift by the index = c % 16
// (screen[i] << index) with a bytemask mask = 1 << index
// screen[i] |= mask
// row (0 <= r <= 255), col (0 <= c <= 511)
// the screen itself is offset by 16384, so screen = RAM[16384](first word in screen)