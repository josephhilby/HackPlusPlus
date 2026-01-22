#ifndef LEXER_H
#define LEXER_H

#include <ctype.h>
#include "structs.h"

Operation* lex_line(char* line);
Operation* lex_operation(char* line, Operation* op, size_t len, size_t start);
void lex_comment(char* line);
size_t find_start(const char* buffer);

#endif //LEXER_H
