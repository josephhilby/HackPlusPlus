#ifndef LEXER_H
#define LEXER_H

#include <ctype.h>
#include <regex.h>
#include <stdlib.h>
#include "Structs.h"

Instruction* lex_line(char* line, size_t line_num);
size_t find_start(char* buffer);

#endif //LEXER_H
