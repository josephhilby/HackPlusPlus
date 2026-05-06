#ifndef TABLES_H
#define TABLES_H

#include <stdint.h>

typedef struct {
    const char* mnemonic;
    uint16_t code;
} CompInst;

uint16_t get_inst_part(const char* mnemonic, const CompInst* tbl);
void clean_dest(const char* mnemonic, char buffer[4]);

uint16_t get_comp(const char* mnemonic);
uint16_t get_dest(const char* mnemonic);
uint16_t get_jump(const char* mnemonic);

#endif
