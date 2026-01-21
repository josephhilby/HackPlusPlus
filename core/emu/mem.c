//
// Created by Joseph Hilby on 1/19/26.
//

#include "const.h"
#include "mem.h"

// screen is 256 x 512 = 131,072 pixles, represented in bits (1=black, 0=white)
// MMIO is an array of 8K words of 16-bits each, 16 KiB in RAM, uint16_t screen[0..8192]
// each row has 32 words (32 words * 16 bits/word = 512 bits)
// mapping is screen[i], where i(r, c) = (r * 32) + (c / 16)
// then logicaly shift by the index = c % 16
// (screen[i] << index) with a bytemask mask = 1 << index
// screen[i] |= mask
// row (0 <= r <= 255), col (0 <= c <= 511)
// the screen itself is offset by 16384, so screen = RAM[16384](first word in screen)

static uint16_t ram[RAMSIZE];

void screen_test() {
    for (int i = SCREEN_BASE; i < SCREEN_BASE + SCREEN_LEN; i++) {
        int word = i - SCREEN_BASE;
        int row = word / 32;
        if (row % 8 < 4) {
            ram[i] = 0x0000;
        } else {
            ram[i] = 0xFFFF;
        }
    }
}

// placeholder for later dynamic allocation
void mem_init() {
    mem_reset();
    screen_test();
    return;
}

void mem_reset() {
    for (int i = 0; i < RAMSIZE; i++) {
        if (SCREEN_BASE <= i && i < SCREEN_BASE + SCREEN_LEN) {
            ram[i] = 0xFFFF;
        } else {
            ram[i] = 0x0000;
        }
    }
}

const uint16_t* mem_read(uint16_t addr) {
    return ram + addr;
}

void mem_write(uint16_t addr, uint16_t data) {
    if (addr >= SCREEN_BASE) {
        return;
    }
    ram[addr] = data;
}

const uint16_t* screen_read() {
    return mem_read(SCREEN_BASE);
}