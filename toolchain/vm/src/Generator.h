#ifndef GENERATOR_H
#define GENERATOR_H

#include "Structs.h"

void generate(FILE* dest, Operation opr);
Command* find_vm_operation(const char *mnemonic);
void emit_push(FILE *out, const char *arg1, int arg2);


#endif //GENERATOR_H
