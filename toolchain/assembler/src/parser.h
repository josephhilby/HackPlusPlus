#ifndef PARSER_H
#define PARSER_H

#include "structs.h"
#include "lexer.h"

Instruction* first_pass(FILE* src, SystemTable* stbl);
void parse_c_inst(Instruction* inst);

#endif //PARSER_H
