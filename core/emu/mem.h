//
// Created by Joseph Hilby on 1/19/26.
//

#ifndef MEM_H
#define MEM_H

#include <stdint.h>

void screen_test();
void mem_init();
void mem_reset();
const uint16_t* mem_read(uint16_t addr);
void mem_write(uint16_t addr, uint16_t data);
const uint16_t* screen_read();

#endif //MEM_H
