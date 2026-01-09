#include "Parser.h"

Instruction* first_pass(FILE* src, SystemTable* stbl) {
    Instruction* head = NULL;
    Instruction* tail = NULL;
    size_t line_num = 0;
    char buffer[MAX_LENGTH];
    memset(buffer, 0, MAX_LENGTH);

    while (fgets(buffer, MAX_LENGTH, src)) {
        Instruction* next = lex_line(buffer, line_num);

        // comment line
        if (next == NULL) {
            continue;
        }

        // first instruction INST_A or INST_C
        if (next->type != INST_L) {
            if (head == NULL) {
                head = next;
                tail = next;
            } else {
                tail->next = next;
                tail = next;
            }

            line_num++;

            if (tail->type == INST_C) {
                parse_c_type(tail);
            }
        } else {
            add_entry(stbl, next->ltrl, next->line);
        }
    }

    return head;
}

// INST Type C
// dest=comp;jump
void parse_c_type(Instruction* inst) {
    int i;
    char* dest = inst->ltrl;
    char* comp = strchr(dest, '=');
    char* jump = strchr(dest, ';');

    if (comp != NULL) {
        i = comp - dest;
        memcpy(inst->dest, dest, i);

        // dest=comp;jump
        if (jump != NULL) {
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
