#ifndef GENERATOR_H
#define GENERATOR_H

#include<ctype.h>
#include "Structs.h"

void second_pass(FILE* dist, Instruction* list, SystemTable* stbl);
uint16_t decode_c_inst(Instruction* inst);
uint16_t decode_comp(char* comp);
uint16_t decode_dest(char* dest);
uint16_t decode_jump(char* jump);
void inst_to_bits(uint16_t inst, char* buffer);
void write_bits(FILE* dist, char* buffer);

#endif //GENERATOR_H
