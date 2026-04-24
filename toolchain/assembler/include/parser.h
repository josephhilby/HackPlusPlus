#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "instruction.h"
#include "symbol_table.h"

Instruction* first_pass(FILE* src, SystemTable* stbl);
void parse_c_inst(Instruction* inst);

#endif //PARSER_H
