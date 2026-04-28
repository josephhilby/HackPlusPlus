#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../../shared/common.h"

typedef enum { INST_A, INST_C, INST_L } InstType;

typedef struct Instruction {
    InstType type;
    char ltrl[MAX_LENGTH];
    bool is_const;
    char comp[16];
    char dest[4];
    char jump[4];
    uint16_t line;
    struct Instruction* next;
} Instruction;

Instruction* create_instruction(size_t line_num);
void add_instruction(Instruction** head, Instruction** tail, Instruction* next);
void list_free(Instruction* list);

#endif
