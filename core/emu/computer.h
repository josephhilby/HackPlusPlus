#ifndef COMPUTER_H
#define COMPUTER_H

#include <stdint.h>
#include <stddef.h>
#include "../util/const.h"

// 8 byte struct
typedef struct state_t {
  uint16_t pc;      // +0 offset for PC
  uint16_t flags;   // +2 offset for flags
  uint32_t cycles;  // +4 offset for cycles
} state_t;

void init(void);

uint16_t* get_rom_ptr(void);
void commit_rom(size_t length);

void run(void);
void stop(void);
void step(void);
void reset(void);

void set_keyboard(uint16_t value);

const state_t* get_state_ptr(void);
const uint16_t* get_framebuffer_ptr(void);

#endif //COMPUTER_H