#ifndef COMMAND_H
#define COMMAND_H

#include <stdio.h>

typedef void (*Emitter)(FILE* out, const char* arg1, const char* arg2);

typedef struct {
    const char* mnemonic;
    Emitter emit;
} Command;

#endif
