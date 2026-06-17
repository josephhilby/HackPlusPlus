#ifndef TOKEN_H
#define TOKEN_H

#include <stddef.h>

#include "../../shared/common.h"

typedef enum {
  // Structural & Literal
  TOKEN_SYMBOL,
  TOKEN_INTEGER_CONSTANT,
  TOKEN_STRING_CONSTANT,
  TOKEN_IDENTIFIER,

  TOKEN_KEYWORD,  // Allows keyword check (type >= TOKEN_KEYWORD)

  // Program
  TOKEN_CLASS = TOKEN_KEYWORD,
  TOKEN_METHOD,
  TOKEN_FUNCTION,
  TOKEN_CONSTRUCTOR,
  TOKEN_STATIC,
  TOKEN_FIELD,

  // Types
  TOKEN_INT,
  TOKEN_BOOLEAN,
  TOKEN_CHAR,
  TOKEN_VOID,
  TOKEN_VAR,

  // Statements
  TOKEN_LET,
  TOKEN_DO,
  TOKEN_IF,
  TOKEN_ELSE,
  TOKEN_WHILE,
  TOKEN_RETURN,

  // Expressions
  TOKEN_TRUE,
  TOKEN_FALSE,
  TOKEN_NULL_KEYWORD,
  TOKEN_THIS
} TokenType;

typedef struct Token {
  char ltrl[MAX_LENGTH];
  TokenType type;
  struct Token *next;
} Token;

Token *create_token(const char *ltrl, size_t len, TokenType type);
void check_identifier_is_keyword(Token *tkn);
void free_token(Token *tkn);
void free_token_list(Token *head);

#endif  // TOKEN_H
