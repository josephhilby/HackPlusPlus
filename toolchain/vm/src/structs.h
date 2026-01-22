#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LENGTH 128

char filename[256];

void set_filename(const char* path);

typedef struct {
    char ltrl[MAX_LENGTH];
    char command[10];
    char segment[10];
    char data[MAX_LENGTH - 20];
} Operation;

Operation* create_operation();

// Command descriptor
typedef void (*Emitter)(FILE* out, const char* arg1, const char* arg2);

typedef struct {
    const char* mnemonic;
    Emitter emit;
} Command;

#endif //STRUCTS_H
