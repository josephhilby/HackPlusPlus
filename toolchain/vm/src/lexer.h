#ifndef LEXER_H
#define LEXER_H

#include <ctype.h>
#include "structs.h"

Operation* lex_line(char* line);
Operation* lex_operation(const char* line, Operation* op, size_t len);

#endif //LEXER_H
