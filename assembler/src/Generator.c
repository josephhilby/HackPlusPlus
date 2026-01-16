#include "Generator.h"

void second_pass(FILE* dist, const Instruction* curr, SystemTable* stbl) {
    uint16_t inst;
    char buffer[16];

    while (curr) {
        // INST_A
        if (curr->type == INST_A) {
            inst = decode_a_inst(curr, stbl);
        // INST_C
        } else {
            inst = decode_c_inst(curr);
        }

        inst_to_bits(inst, buffer);

        write_bits(dist, buffer);
        curr = curr->next;

        if (curr) {
            fputc('\n', dist);
        }
    }
}

uint16_t decode_a_inst(const Instruction* inst, SystemTable* stbl) {
    if (inst->is_const) {
        return strtol(inst->ltrl, NULL, 10);
    }
    SystemEntry* entry = find_entry(stbl, inst->ltrl);

    if (!entry) {
        entry = add_entry(stbl, inst->ltrl, stbl->next_var);
        stbl->next_var++;
    }

    return entry->addr;
}

uint16_t decode_c_inst(const Instruction* inst) {
    const uint16_t comp_mask = decode_comp(inst->comp); // ...acccccc......
    const uint16_t dest_mask = decode_dest(inst->dest); // ..........ddd...
    const uint16_t jump_mask = decode_jump(inst->jump); // .............jjj

    return 0b1110000000000000 | comp_mask | dest_mask | jump_mask;
}

uint16_t decode_comp(const char* comp) {
    uint16_t a_mask = 0;
    uint16_t comp_mask = 0;

    char* mem = strchr(comp, 'M');
    if (mem) {
        a_mask = 0b0001000000000000;
        *mem = 'A';
    }

    comp_mask = get_comp(comp);

    return 0 | a_mask | comp_mask;
}

uint16_t decode_dest(const char* dest) {
    const uint16_t dest_mask = get_dest(dest);
    return 0 | dest_mask;
}

uint16_t decode_jump(const char* jump) {
    const uint16_t jump_mask = get_jump(jump);
    return 0 | jump_mask;
}

void inst_to_bits(const uint16_t inst, char* buffer) {
    for (int i = 0; i < 16; i++) {
        const uint16_t mask = 1u << (15 - i);
        buffer[i] = (inst & mask) ? '1' : '0';
    }
}

void write_bits(FILE* dist, const char* buffer) {
    fwrite(buffer, 1, 16, dist);
}
