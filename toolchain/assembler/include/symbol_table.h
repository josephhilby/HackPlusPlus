#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#define N_BUCKETS 128
#define N_REG 15

typedef struct SystemEntry {
    char symbol[MAX_LENGTH];
    uint16_t addr;
    struct SystemEntry* next;
} SystemEntry;

typedef struct SystemTable {
    SystemEntry* buckets[N_BUCKETS];
    uint16_t next_var;
} SystemTable;

SystemTable* create_table(void);
void load_builtin(SystemTable* stbl);
void table_free(SystemTable* stbl);

size_t sys_hash(const char* key);
SystemEntry* create_entry(const char* name, uint16_t addr);
SystemEntry* find_entry(const SystemTable* stbl, const char* name);
SystemEntry* add_entry(SystemTable* stbl, const char* name, uint16_t addr);

#endif
