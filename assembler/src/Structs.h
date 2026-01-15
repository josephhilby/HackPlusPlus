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

// INST LIST
// Instruction Type
typedef enum { INST_A, INST_C, INST_L } InstType;

// Instruction Node/List
typedef struct Instruction {
    InstType type;
    char ltrl[MAX_LENGTH];
    bool is_const;    // aka. numerical value
    char comp[16];
    char dest[4];
    char jump[4];
    uint16_t line;
    struct Instruction* next;
} Instruction;

// Instruction List Functions
Instruction* create_instruction(size_t line_num);
void add_instruction(Instruction** head, Instruction** tail, Instruction* next);
void list_free(Instruction* list);


// SYS TABLE
// System Table Entry Node
typedef struct SystemEntry {
    char symbol[MAX_LENGTH];
    uint16_t addr;
    struct SystemEntry* next;
} SystemEntry;

// System Table (Hash Map)
typedef struct SystemTable {
    SystemEntry* buckets[N_BUCKETS];
    uint16_t next_var;
} SystemTable;

// System Table Functions
SystemTable* table_init();
void load_builtin(SystemTable* stbl);
void table_free(SystemTable* table);
size_t sys_hash(const char* key);
SystemEntry* create_entry(char* name, uint16_t addr);
SystemEntry* find_entry(SystemTable* stbl, char* name);
SystemEntry* add_entry(SystemTable* stbl, char* name, uint16_t addr);


// COMP INSTRUCTION TABLES
// Comp Instruction Node
typedef struct {
    const char* mnemonic;
    uint16_t code;
} CompInst;

// Comp Tables (must turn any M to A)
static const CompInst comp_table[] = {
    {"0"  , 0b0000101010000000},
    {"1"  , 0b0000111111000000},
    {"-1" , 0b0000111010000000},
    {"D"  , 0b0000001100000000},
    {"A"  , 0b0000110000000000},
    {"!D" , 0b0000001101000000},
    {"!A" , 0b0000110001000000},
    {"-D" , 0b0000001111000000},
    {"-A" , 0b0000110011000000},
    {"D+1", 0b0000011111000000},
    {"A+1", 0b0000110111000000},
    {"D-1", 0b0000001110000000},
    {"A-1", 0b0000110010000000},
    {"D+A", 0b0000000010000000},
    {"D-A", 0b0000010011000000},
    {"A-D", 0b0000000111000000},
    {"D&A", 0b0000000000000000},
    {"D|A", 0b0000010101000000},
    {NULL , 0}
};

static const CompInst dest_table[] = {
    {""   , 0b0000000000000000},
    {"M"  , 0b0000000000001000},
    {"D"  , 0b0000000000010000},
    {"DM" , 0b0000000000011000},
    {"A"  , 0b0000000000100000},
    {"AM" , 0b0000000000101000},
    {"AD" , 0b0000000000110000},
    {"ADM", 0b0000000000111000},
    {NULL , 0}
};

static const CompInst jump_table[] = {
    {""   , 0b1110000000000000},
    {"JGT", 0b1110000000000001},
    {"JEQ", 0b1110000000000010},
    {"JGE", 0b1110000000000011},
    {"JLT", 0b1110000000000100},
    {"JNE", 0b1110000000000101},
    {"JLE", 0b1110000000000110},
    {"JMP", 0b1110000000000111},
    {NULL , 0}
};

// Comp Table Functions
uint16_t get_inst_part(char* mnemonic, CompInst* tbl);
uint16_t get_comp(char* mnemonic);
uint16_t get_dest(char* mnemonic);
uint16_t get_jump(char* mnemonic);

#endif //STRUCTS_H
