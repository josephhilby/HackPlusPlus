#ifndef TOKEN_H
#define TOKEN_H

#include <stddef.h>

#include "../../shared/common.h"

typedef enum {
  keyword,
  symbol,
  integerConstant,
  stringConstant,
  identifier
} TokenClass;

typedef struct Token {
    char ltrl[MAX_LENGTH];
    TokenClass cls;
    struct Token* next;
} Token;

Token* create_token(const char* ltrl, size_t len, TokenClass cls);
void free_token(Token* tkn);
void free_token_list(Token* head);

#endif //TOKEN_H
