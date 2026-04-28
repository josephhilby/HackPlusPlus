#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "../include/symbol_table.h"

SystemTable* create_table(void) {
    SystemTable* stbl = calloc(1, sizeof(*stbl));
    if (!stbl) {
        printf("Failed to allocate memory for SystemTable\n");
        return NULL;
    }

    stbl->next_var = N_REG + 1;
    return stbl;
}

void table_free(SystemTable* stbl) {
    if (!stbl) return;

    for (size_t i = 0; i < N_BUCKETS; i++) {
        SystemEntry* curr = stbl->buckets[i];

        while (curr) {
            SystemEntry* next = curr->next;
            free(curr);
            curr = next;
        }
    }

    free(stbl);
}

size_t sys_hash(const char* key) {
    size_t seed = 5381;

    while (*key) {
        seed ^= (unsigned char)*key++;
        seed *= 53;
    }

    return seed % N_BUCKETS;
}

SystemEntry* create_entry(const char* name, const uint16_t addr) {
    SystemEntry* entry = calloc(1, sizeof(*entry));
    if (!entry) {
        printf("Failed to allocate memory for SystemEntry\n");
        return NULL;
    }

    snprintf(entry->symbol, sizeof(entry->symbol), "%s", name);
    entry->addr = addr;
    return entry;
}

SystemEntry* find_entry(const SystemTable* stbl, const char* name) {
    if (!stbl || !name) return NULL;

    const size_t i = sys_hash(name);

    for (SystemEntry* curr = stbl->buckets[i]; curr; curr = curr->next) {
        if (strcmp(curr->symbol, name) == 0) {
            return curr;
        }
    }

    return NULL;
}

SystemEntry* add_entry(SystemTable* stbl, const char* name, const uint16_t addr) {
    if (!stbl || !name) return NULL;

    const size_t i = sys_hash(name);

    SystemEntry* entry = create_entry(name, addr);
    if (!entry) return NULL;

    entry->next = stbl->buckets[i];
    stbl->buckets[i] = entry;

    return entry;
}

void load_builtin(SystemTable* stbl) {
    if (!stbl) return;

    for (uint16_t i = 0; i <= N_REG; i++) {
        char name[4];
        snprintf(name, sizeof(name), "R%i", i);
        add_entry(stbl, name, i);
    }

    add_entry(stbl, "SP",     0);
    add_entry(stbl, "LCL",    1);
    add_entry(stbl, "ARG",    2);
    add_entry(stbl, "THIS",   3);
    add_entry(stbl, "THAT",   4);
    add_entry(stbl, "SCREEN", 16384);
    add_entry(stbl, "KBD",    24576);
}
