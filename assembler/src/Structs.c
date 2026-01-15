#include "Structs.h"

SystemTable* table_init() {
    size_t table_size = sizeof(SystemTable);
    SystemTable* stbl = malloc(table_size);
    memset(stbl, 0, table_size);
    stbl->next_var = N_REG + 1;
    return stbl;
}

void table_free(SystemTable* stbl) {
    for (int i = 0; i < MAX_LENGTH; i++) {
        while (stbl->buckets[i]) {
            SystemEntry* next = stbl->buckets[i]->next;
            free(stbl->buckets[i]);
            stbl->buckets[i] = next;
        }
    }
}

Instruction* create_instruction(size_t line_num) {
    Instruction* inst = malloc(sizeof(Instruction));
    if (!inst) {
        return NULL;
    }
    inst->line = line_num;
    inst->next = NULL;
    return inst;
}

void add_instruction(Instruction** head, Instruction** tail, Instruction* next) {
    if (!*head) {
        *head = next;
        *tail = next;
    } else {
        (*tail)->next = next;
        *tail = next;
    }
}

void list_free(Instruction* list) {
    while (list) {
        Instruction* next = list->next;
        free(list);
        list = next;
    }
}

size_t sys_hash(const char* key) {
    size_t seed = 5381;
    while (*key) {
        seed ^= (unsigned char)*key++;
        seed *= 53;
    }
    return seed % N_BUCKETS;
}

SystemEntry* create_entry(char* name, uint16_t addr) {
    SystemEntry* entry = malloc(sizeof(SystemEntry));
    memcpy(entry->symbol, name, MAX_LENGTH);
    entry->addr = addr;
    entry->next = NULL;
    return entry;
}

SystemEntry* find_entry(SystemTable* stbl, char* name) {
    size_t i = sys_hash(name);
    if (!stbl->buckets[i]) {
        return NULL;
    }

    SystemEntry* curr = stbl->buckets[i];
    while (curr) {
        if (strcmp(curr->symbol, name) == 0) {
            return curr;
        }
        curr = curr->next;
    }

    return NULL;
}

SystemEntry* add_entry(SystemTable* stbl, char* name, uint16_t addr) {
    size_t i = sys_hash(name);
    SystemEntry* entry = create_entry(name, addr);

    if (!stbl->buckets[i]) {
        stbl->buckets[i] = entry;
    } else {
        SystemEntry* curr = stbl->buckets[i];
        while (curr->next) {
            curr = curr->next;
        }

        curr->next = entry;
    }
    return entry;
}

void load_builtin(SystemTable* stbl) {
    for (uint16_t i = 0; i <= N_REG; i++) {
        char name[4];
        snprintf(name, sizeof(name), "R%i", i);
        add_entry(stbl, name, i);
    }

    add_entry(stbl, "SP"    , 0);
    add_entry(stbl, "LCL"   , 1);
    add_entry(stbl, "ARG"   , 2);
    add_entry(stbl, "THIS"  , 3);
    add_entry(stbl, "THAT"  , 4);

    add_entry(stbl, "SCREEN", 16384);
    add_entry(stbl, "KBD"   , 24576);
}

uint16_t get_inst_part(char* mnemonic, CompInst* tbl) {
    size_t i = 0;
    while (tbl[i].mnemonic) {
        if (strcmp(mnemonic, tbl[i].mnemonic) == 0) {
            return tbl[i].code;
        }
        i++;
    }
    return 0;
}

uint16_t get_comp(char* mnemonic) {
    return get_inst_part(mnemonic, comp_table);
}

uint16_t get_dest(char* mnemonic) {
    return get_inst_part(mnemonic, dest_table);
}

uint16_t get_jump(char* mnemonic) {
    return get_inst_part(mnemonic, jump_table);
}
