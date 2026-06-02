#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>

#include "../../shared/line.h"
#include "operation.h"
#include "token.h"

Operation *lex_line(char *line);
Operation *lex_token(const char *line, Token *op, size_t len);

#endif  // LEXER_H