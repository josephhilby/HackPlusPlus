#include "Generator.h"

void second_pass(FILE* dist, Instruction* list, SystemTable* stbl) {
    Instruction* curr = list;
    uint16_t inst;
    char buffer[16];

    while (curr != NULL) {
        // INST_A
        if (curr->type == INST_A) {
            if (!curr->is_var) {
                inst = (uint16_t)atoi(curr->ltrl);
            } else {
                SystemEntry* entry = find_entry(stbl, curr->ltrl);

                if (entry == NULL) {
                    entry = add_entry(stbl, curr->ltrl, stbl->next_var);
                    stbl->next_var++;
                }

                inst = entry->addr;
            }
        // INST_C
        } else {
            inst = decode_c_inst(curr);
        }

        inst_to_bits(inst, buffer);

        write_bits(dist, buffer);
        curr = curr->next;

        if (curr != NULL) {
            fputc('\n', dist);
        }
    }
}

uint16_t decode_c_inst(Instruction* inst) {
    uint16_t type_mask = 0xE000;                  // 111.............
    uint16_t comp_mask = decode_comp(inst->comp); // ...acccccc......
    uint16_t dest_mask = decode_dest(inst->dest); // ..........ddd...
    uint16_t jump_mask = decode_jump(inst->jump); // .............jjj

    return 0 | type_mask | comp_mask | dest_mask | jump_mask;;
}

uint16_t decode_comp(char* comp) {
    uint16_t a_mask = 0;
    uint16_t comp_mask = 0;
    char* mem = strchr(comp, 'M');
    if (mem != NULL) {
        a_mask = 1;
        *mem = 'A';
    }

    for (size_t i = 0; i < sizeof(comp_table)/sizeof(comp_table[0]); i++) {
        if (strcmp(comp, comp_table[i].mnemonic) == 0) {
            comp_mask = comp_table[i].code;
            break;
        }
    }

    a_mask = a_mask << 12;
    comp_mask = comp_mask << 6;

    return 0 | a_mask | comp_mask;
}

uint16_t decode_dest(char* dest) {
    uint16_t dest_mask = 0;

    if (strchr(dest, 'A') != NULL) {
        dest_mask |= 0b100;
    }

    if (strchr(dest, 'D') != NULL) {
        dest_mask |= 0b010;
    }

    if (strchr(dest, 'M') != NULL) {
        dest_mask |= 0b001;
    }

    dest_mask = dest_mask << 3;

    return 0 | dest_mask;
}

uint16_t decode_jump(char* jump) {
    uint16_t jump_mask = 0;

    if (strchr(jump, 'L') != NULL) {
        jump_mask |= 0b100;
    }

    if (strchr(jump, 'E') != NULL) {
        jump_mask |= 0b010;
    }

    if (strchr(jump, 'G') != NULL) {
        jump_mask |= 0b001;
    }

    if (strcmp(jump, "JMP") == 0) {
        jump_mask = 0b111;
    }

    if (strcmp(jump, "JNE") == 0) {
        jump_mask = 0b101;
    }

    return 0 | jump_mask;
}

void inst_to_bits(uint16_t inst, char* buffer) {
    for (int i = 0; i < 16; i++) {
        uint16_t mask = 1u << (15 - i);
        buffer[i] = (inst & mask) ? '1' : '0';
    }
}

void write_bits(FILE* dist, char* buffer) {
    fwrite(buffer, 1, 16, dist);
}
