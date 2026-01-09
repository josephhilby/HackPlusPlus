#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LENGTH 128
#define N_BUCKETS 128
#define N_REG 15

// type
typedef enum { INST_A, INST_C, INST_L} InstType;

// instruction LL Node
typedef struct Instruction {
    InstType type;
    char ltrl[MAX_LENGTH];
    bool is_var;
    char dest[4];
    char comp[16];
    char jump[4];
    uint16_t line;
    struct Instruction* next;
} Instruction;

// ROM system entry LL for hashmap
typedef struct SystemEntry {
    char name[MAX_LENGTH];
    uint16_t addr;
    struct SystemEntry* next;
} SystemEntry;

typedef struct SystemTable {
    SystemEntry* buckets[N_BUCKETS];
    uint16_t next_var;
} SystemTable;

typedef struct {
    const char* mnemonic;
    uint8_t code;   // 6 bits
} CompEntry;

static const CompEntry comp_table[] = {
    {"0",   0b101010},
    {"1",   0b111111},
    {"-1",  0b111010},
    {"D",   0b001100},
    {"A",   0b110000},
    {"!D",  0b001101},
    {"!A",  0b110001},
    {"-D",  0b001111},
    {"-A",  0b110011},
    {"D+1", 0b011111},
    {"A+1", 0b110111},
    {"D-1", 0b001110},
    {"A-1", 0b110010},
    {"D+A", 0b000010},
    {"D-A", 0b010011},
    {"A-D", 0b000111},
    {"D&A", 0b000000},
    {"D|A", 0b010101},
};


SystemTable* table_init();
void table_free(SystemTable* table);
void list_free(Instruction* list);

size_t sys_hash(char* key);
SystemEntry* create_entry(char* name, uint16_t addr);
SystemEntry* find_entry(SystemTable* stbl, char* name);
SystemEntry* add_entry(SystemTable* stbl, char* name, uint16_t addr);
void load_builtin(SystemTable* stbl);
void load_var(SystemTable* stbl, char* name, uint16_t addr);

#endif //STRUCTS_H
