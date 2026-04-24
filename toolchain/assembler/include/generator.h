#ifndef GENERATOR_H
#define GENERATOR_H

#include<ctype.h>
#include "instruction.h"
#include "symbol_table.h"
#include "tables.h"

void second_pass(FILE* dist, const Instruction* curr, SystemTable* stbl);
uint16_t decode_a_inst(const Instruction* inst, SystemTable* stbl);
uint16_t decode_c_inst(const Instruction* inst);
uint16_t decode_comp(const char* comp);
uint16_t decode_dest(const char* dest);
uint16_t decode_jump(const char* jump);
void inst_to_bits(uint16_t inst, char* buffer);
void write_bits(FILE* dist, const char* buffer);

#endif //GENERATOR_H
