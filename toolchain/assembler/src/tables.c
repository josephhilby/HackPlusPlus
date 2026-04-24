#include "tables.h"

static const CompInst comp_table[] = {
    {"0",   0b0000101010000000},
    {"1",   0b0000111111000000},
    {"-1",  0b0000111010000000},
    {"D",   0b0000001100000000},
    {"A",   0b0000110000000000},
    {"!D",  0b0000001101000000},
    {"!A",  0b0000110001000000},
    {"-D",  0b0000001111000000},
    {"-A",  0b0000110011000000},
    {"D+1", 0b0000011111000000},
    {"A+1", 0b0000110111000000},
    {"D-1", 0b0000001110000000},
    {"A-1", 0b0000110010000000},
    {"D+A", 0b0000000010000000},
    {"D-A", 0b0000010011000000},
    {"A-D", 0b0000000111000000},
    {"D&A", 0b0000000000000000},
    {"D|A", 0b0000010101000000},
    {NULL,  0}
};

static const CompInst dest_table[] = {
    {"",    0b0000000000000000},
    {"M",   0b0000000000001000},
    {"D",   0b0000000000010000},
    {"DM",  0b0000000000011000},
    {"A",   0b0000000000100000},
    {"AM",  0b0000000000101000},
    {"AD",  0b0000000000110000},
    {"ADM", 0b0000000000111000},
    {NULL,  0}
};

static const CompInst jump_table[] = {
    {"",    0b0000000000000000},
    {"JGT", 0b0000000000000001},
    {"JEQ", 0b0000000000000010},
    {"JGE", 0b0000000000000011},
    {"JLT", 0b0000000000000100},
    {"JNE", 0b0000000000000101},
    {"JLE", 0b0000000000000110},
    {"JMP", 0b0000000000000111},
    {NULL,  0}
};

uint16_t get_inst_part(const char* mnemonic, const CompInst* tbl) {
    if (!mnemonic || !tbl) return 0;

    for (size_t i = 0; tbl[i].mnemonic; i++) {
        if (strcmp(mnemonic, tbl[i].mnemonic) == 0) {
            return tbl[i].code;
        }
    }

    return 0;
}

uint16_t get_comp(const char* mnemonic) {
    return get_inst_part(mnemonic, comp_table);
}

void clean_dest(const char* mnemonic, char buffer[4]) {
    if (!mnemonic || !buffer) return;

    bool has_a = false;
    bool has_d = false;
    bool has_m = false;

    while (*mnemonic) {
        if (*mnemonic == 'A') {
            has_a = true;
        } else if (*mnemonic == 'D') {
            has_d = true;
        } else if (*mnemonic == 'M') {
            has_m = true;
        } else {
            buffer[0] = '\0';
            return;
        }

        mnemonic++;
    }

    int i = 0;
    if (has_a) buffer[i++] = 'A';
    if (has_d) buffer[i++] = 'D';
    if (has_m) buffer[i++] = 'M';
    buffer[i] = '\0';
}

uint16_t get_dest(const char* mnemonic) {
    char clean_mnemonic[4];
    clean_dest(mnemonic, clean_mnemonic);

    return get_inst_part(clean_mnemonic, dest_table);
}

uint16_t get_jump(const char* mnemonic) {
    return get_inst_part(mnemonic, jump_table);
}
