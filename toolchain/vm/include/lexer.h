#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>

#include "operation.h"
#include "../../shared/line.h"

Operation* lex_line(char* line);
Operation* lex_operation(const char* line, Operation* op, size_t len);

#endif //LEXER_H
