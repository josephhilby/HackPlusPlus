#ifndef LEXER_H
#define LEXER_H

#include <ctype.h>
#include <regex.h>
#include <stdlib.h>
#include "Structs.h"

Instruction* lex_line(char* line, size_t line_num);
Instruction* lex_inst_a(const char* line, Instruction* inst, size_t start, size_t len);
Instruction* lex_inst_l(const char* line, Instruction* inst, size_t start, size_t len);
Instruction* lex_inst_c(const char* line, Instruction* inst, size_t start, size_t len);

void lex_comment(char* line);
size_t find_start(const char* buffer);

#endif //LEXER_H
