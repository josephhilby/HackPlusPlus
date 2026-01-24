#ifndef GENERATOR_H
#define GENERATOR_H

#include "structs.h"

void generate(FILE* dest, Operation* opr);
void set_filename(const char* path);
const Command* find_vm_command(const char* mnemonic);
void emit_push(FILE *out, const char *segment, const char* data);
void emit_pop(FILE *out, const char *segment, const char* data);
void emit_add(FILE *out, const char *segment, const char* data);
void emit_sub(FILE *out, const char *segment, const char* data);

#endif //GENERATOR_H
