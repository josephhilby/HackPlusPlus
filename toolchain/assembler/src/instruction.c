#include <stdio.h>
#include <stdlib.h>

#include "../include/instruction.h"

Instruction* create_instruction(const size_t line_num) {
    Instruction* inst = calloc(1, sizeof(*inst));
    if (!inst) {
        printf("Failed to allocate memory for Instruction\n");
        return NULL;
    }

    inst->line = line_num;
    return inst;
}

void add_instruction(Instruction** head, Instruction** tail, Instruction* next) {
    if (!head || !tail || !next) return;

    if (!*head) {
        *head = next;
        *tail = next;
        return;
    }

    (*tail)->next = next;
    *tail = next;
}

void list_free(Instruction* list) {
    while (list) {
        Instruction* next = list->next;
        free(list);
        list = next;
    }
}
