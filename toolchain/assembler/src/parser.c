#include "parser.h"

Instruction* first_pass(FILE* src, SystemTable* stbl) {
    Instruction* head = NULL;
    Instruction* tail = NULL;
    size_t line_num = 0;
    char buffer[MAX_LENGTH];
    memset(buffer, 0, MAX_LENGTH);

    while (fgets(buffer, MAX_LENGTH, src)) {
        Instruction* next = lex_line(buffer, line_num);

        // empty line
        if (!next) {
            continue;
        }

        // INST_A or INST_C
        if (next->type == INST_A || next->type == INST_C) {
            if (next->type == INST_C) {
                parse_c_inst(next);
            }

            add_instruction(&head, &tail, next);
            line_num++;
        }

        // INST_L
        if (next->type == INST_L) {
            add_entry(stbl, next->ltrl, next->line);
            free(next);
        }
    }

    return head;
}

// INST Type C
void parse_c_inst(Instruction* inst) {
    long i;
    // dest=comp;jump, dest=comp, comp;jump
    const char* first = inst->ltrl;
    //     =comp;jump,     =comp,
    const char* second = strchr(first, '=');
    //          ;jump,          ,     ;jump
    const char* third = strchr(first, ';');

    if (second) {
        // dest=...
        i = second - first;
        memcpy(inst->dest, first, i);
        inst->dest[i] = '\0';

        if (third) {
            // =comp;jump
            i = third - second - 1;
            memcpy(inst->comp, second + 1, i);
            inst->comp[i] = '\0';
            memcpy(inst->jump, third + 1, strlen(third));
            inst->jump[strlen(third)] = '\0';
        } else {
            // =comp
            memcpy(inst->comp, second + 1, strlen(second));
            inst->comp[strlen(second)] = '\0';
        }
    } else {
        // comp;jump
        i = third - first;
        memcpy(inst->comp, first, i);
        inst->comp[i] = '\0';
        memcpy(inst->jump, third + 1, strlen(third));
        inst->jump[strlen(third)] = '\0';
    }
}
