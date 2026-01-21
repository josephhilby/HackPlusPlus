#include "structs.h"

SystemTable* create_table() {
    const size_t table_size = sizeof(SystemTable);
    SystemTable* stbl = malloc(table_size);
    if (!stbl) {
        printf("Failed to allocate memory for SystemTable\n");
        return NULL;
    }
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
    free(stbl);
}

Instruction* create_instruction(const size_t line_num) {
    const size_t instruction_size = sizeof(Instruction);
    Instruction* inst = malloc(instruction_size);
    if (!inst) {
        printf("Failed to allocate memory for Instruction\n");
        return NULL;
    }
    memset(inst, 0, instruction_size);
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

SystemEntry* create_entry(const char* name, const uint16_t addr) {
    const size_t entry_size = sizeof(SystemEntry);
    SystemEntry* entry = malloc(entry_size);
    if (!entry) {
        printf("Failed to allocate memory for SystemEntry\n");
        return NULL;
    }
    memset(entry, 0, entry_size);
    snprintf(entry->symbol, sizeof(entry->symbol), "%s", name);
    entry->addr = addr;
    entry->next = NULL;
    return entry;
}

SystemEntry* find_entry(const SystemTable* stbl, const char* name) {
    const size_t i = sys_hash(name);
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

SystemEntry* add_entry(SystemTable* stbl, const char* name, const uint16_t addr) {
    const size_t i = sys_hash(name);
    SystemEntry* entry = create_entry(name, addr);
    if (!entry) {
        return NULL;
    }

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

uint16_t get_inst_part(const char* mnemonic, const CompInst* tbl) {
    size_t i = 0;
    while (tbl[i].mnemonic) {
        if (strcmp(mnemonic, tbl[i].mnemonic) == 0) {
            return tbl[i].code;
        }
        i++;
    }
    return 0;
}

uint16_t get_comp(const char* mnemonic) {
    return get_inst_part(mnemonic, comp_table);
}

void clean_dest(const char* mnemonic, char buffer[4]) {
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
    if (has_a) {
        buffer[i++] = 'A';
    }
    if (has_d) {
        buffer[i++] = 'D';
    }
    if (has_m) {
        buffer[i++] = 'M';
    }
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
