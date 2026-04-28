#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/token.h"

Token* create_token(const char* ltrl, size_t len, TokenClass cls) {
  if (!ltrl) {
    return NULL;
  }

  if (len >= MAX_LENGTH) {
    len = MAX_LENGTH - 1;
  }

  Token* tkn = calloc(1, sizeof(*tkn));
  if (!tkn) {
    printf("Failed to allocate memory for Token\n");
    return NULL;
  }

  memcpy(tkn->ltrl, ltrl, len);
  tkn->ltrl[len] = '\0';

  tkn->cls = cls;
  return tkn;
}

void free_token(Token* tkn) {
  if (!tkn) return;

  free(tkn);
}

void free_token_list(Token* head) {
  if (!head) return;
  Token* next;
  while (head) {
    next = head->next;
    free_token(head);
    head = next;
  }
}