#include "Parser.h"

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
                parse_c_type(next);
            }

            add_instruction(&head, &tail, next);
            line_num++;
        }

        // INST_L
        if (next->type == INST_L) {
            add_entry(stbl, next->ltrl, next->line);
        }
    }

    return head;
}

// INST Type C
void parse_c_type(Instruction* inst) {
    long i;
    char* dest = inst->ltrl;
    char* comp = strchr(dest, '=');
    char* jump = strchr(dest, ';');

    if (comp) {
        i = comp - dest;
        memcpy(inst->dest, dest, i);

        // dest=comp;jump
        if (jump) {
            i = jump - comp - 1;
            memcpy(inst->comp, comp + 1, i);
            memcpy(inst->jump, jump + 1, strlen(jump));
        // dest=comp
        } else {
            memcpy(inst->comp, comp + 1, strlen(comp));
        }
    // comp;jump
    } else {
        i = jump - dest;
        memcpy(inst->comp, dest, i);
        memcpy(inst->jump, jump + 1, strlen(jump));
    }
}
