#ifndef GENERATOR_H
#define GENERATOR_H

#include "structs.h"

void generate(FILE* dest, Operation* opr);
Command* find_vm_operation(const char* mnemonic);
void emit_push(FILE* out, const char* arg1, int arg2);
void emit_pop(FILE* out, const char* arg1, int arg2);
void emit_add(FILE* out, const char* arg1, int arg2);


#endif //GENERATOR_H
