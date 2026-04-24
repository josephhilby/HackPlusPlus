#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "instruction.h"
#include "symbol_table.h"
#include "parser.h"
#include "generator.h"

// int assemble_bin(const char* in, const char* out);
int assemble_txt(const char* in, const char* out);

#endif //ASSEMBLER_H
