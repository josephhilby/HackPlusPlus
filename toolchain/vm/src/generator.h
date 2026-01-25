#ifndef GENERATOR_H
#define GENERATOR_H

#include "structs.h"

void generate(FILE* dest, Operation* opr);
void set_filename(const char* path);
void emit_builtins(FILE* out);
const Command* find_vm_command(const char* mnemonic);
void emit_push(FILE* out, const char* segment, const char* data);
void emit_pop(FILE* out, const char* segment, const char* data);
void emit_add(FILE* out, const char* segment, const char* data);
void emit_sub(FILE* out, const char* segment, const char* data);
void emit_eq(FILE* out, const char* segment, const char* data);
void emit_lt(FILE* out, const char* segment, const char* data);
void emit_gt(FILE* out, const char* segment, const char* data);
void emit_neg(FILE* out, const char* segment, const char* data);
void emit_and(FILE* out, const char* segment, const char* data);
void emit_or(FILE* out, const char* segment, const char* data);
void emit_not(FILE* out, const char* segment, const char* data);

#endif //GENERATOR_H
