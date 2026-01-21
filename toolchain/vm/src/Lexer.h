#ifndef LEXER_H
#define LEXER_H

#include <ctype.h>
#include "Structs.h"

Operation* lex_line(char* line);
void lex_comment(char* line);
size_t find_start(const char* buffer);

#endif //LEXER_H
