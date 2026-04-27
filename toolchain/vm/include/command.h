#ifndef COMMAND_H
#define COMMAND_H

#include <stdio.h>

// Operation := Command Segment Data
typedef void (*VmEmitter)(FILE* out, const char* segment, const char* data);

typedef struct {
    const char* mnemonic;
    VmEmitter emit;
} VmCommand;

#endif
