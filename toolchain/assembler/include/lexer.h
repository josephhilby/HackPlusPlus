#ifndef LEXER_H
#define LEXER_H

#include <ctype.h>
#include <regex.h>
#include <stdlib.h>
#include "line.h"
#include "instruction.h"

Instruction* lex_line(char* line, size_t line_num);
Instruction* lex_a_inst(const char* line, Instruction* inst, const size_t len);
Instruction* lex_c_inst(const char* line, Instruction* inst, const size_t len);
Instruction* lex_l_inst(const char* line, Instruction* inst, const size_t len);

#endif //LEXER_H
