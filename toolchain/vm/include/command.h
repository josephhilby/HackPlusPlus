#ifndef COMMAND_H
#define COMMAND_H

#include <stdio.h>

/*
  Convention:
     Operation := Command Segment Data
  Interface:
     emitter(segment, data) -> set of ISA instructions
  Note: _seg and _dat used to denote unused interface variable
*/

typedef void (*VmEmitter)(FILE* out, const char* segment, const char* data);

typedef struct {
    const char* mnemonic; // command mnemonic
    VmEmitter emit;       // command emitter function
} VmCommand;

#endif
