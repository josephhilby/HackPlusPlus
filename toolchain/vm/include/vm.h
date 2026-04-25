#ifndef VM_H
#define VM_H

#include "operation.h"
#include "lexer.h"
#include "parser.h"
#include "generator.h"

int translate_vm(const char* in, const char* out);

#endif //VM_H
