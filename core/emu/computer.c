#include "computer.h"

#define SCREEN_WORDS 8192
//#define RAM_WORDS 24577
#define ROM_WORDS 32768

static state_t STATE = {
  .pc = 0,
  .flags = 0,
  .cycles = 0
};

// Refactor to mem.c
// 0: white, 1: black
static uint16_t FRAMEBUFFER[SCREEN_WORDS] = {0};
static uint16_t ROM[ROM_WORDS] = {0};
//static uint16_t RAM[RAM_WORDS] = {0};
static size_t ROM_SIZE = 0;

void init(void) {
  STATE.pc = 0;
  STATE.flags = 0;
  STATE.cycles = 0;
  ROM_SIZE = 0;
}

uint16_t* get_rom_ptr(void) {
  return ROM;
}

void commit_rom(size_t length) {
  if (length == 0 || length > ROM_WORDS) {
    STATE.flags |= FLAG_ERROR;
    return;
  }

  ROM_SIZE = length;

  STATE.pc = 0;
  STATE.cycles = 0;
  STATE.flags = FLAG_LOADED;
}

void run(void) {
  if ((STATE.flags & FLAG_LOADED) == 0) {
    STATE.flags |= FLAG_ERROR;
    return;
  }

  STATE.flags |= FLAG_RUNNING;
  STATE.flags &= ~FLAG_ERROR;
}

void stop(void) {
  STATE.flags &= ~FLAG_RUNNING;
}

void step(void) {
  if ((STATE.flags & FLAG_LOADED) == 0) {
    STATE.flags |= FLAG_ERROR;
    return;
  }

  if (STATE.flags & FLAG_RUNNING) {
    return;
  }

  if (ROM_SIZE == 0) {
    return;
  }

  // Simple mock execution
  if (STATE.pc < ROM_SIZE - 1) {
    STATE.pc += 1;
  }

  STATE.cycles += 1;

  STATE.flags &= ~FLAG_RUNNING;
  STATE.flags &= ~FLAG_ERROR;

  // simple visible framebuffer change
  FRAMEBUFFER[STATE.pc % SCREEN_WORDS] = 0xFFFF;
}

void reset(void) {
  if ((STATE.flags & FLAG_LOADED) == 0) {
    STATE.flags |= FLAG_ERROR;
    return;
  }

  STATE.pc = 0;
  STATE.cycles = 0;
  STATE.flags = FLAG_LOADED;

  for (size_t i = 0; i < SCREEN_WORDS; i++) {
    FRAMEBUFFER[i] = 0;
  }
}

void set_keyboard(uint16_t value) {
  (void)value;
}

const state_t* get_state_ptr(void) {
  return &STATE;
}

const uint16_t* get_framebuffer_ptr(void) {
  return FRAMEBUFFER;
}