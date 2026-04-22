#ifndef COMPUTER_H
#define COMPUTER_H

#include <stdint.h>
#include <stddef.h>

// 8 byte struct
typedef struct state_t {
  uint16_t pc;      // 2 bytes for PC
  uint16_t flags;   // 2 bytes for flags
  uint32_t cycles;  // 4 bytes for cycles
} state_t;

/*
 Flags: bit 0 is LSB
    bit 0: 1 running, 0 stopped
    bit 1: 1 program in rom, 0 rom empty
    bit 2: 1 error, 0 normal
 */
#define FLAG_RUNNING 0x0001 // 0b 0000 0000 0000 0001
#define FLAG_LOADED  0x0002 // 0b 0000 0000 0000 0010
#define FLAG_ERROR   0x0004 // 0b 0000 0000 0000 0100

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