#ifndef VM_H
#define VM_H

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "operation.h"
#include "lexer.h"
#include "parser.h"
#include "generator.h"

int translate_vm(const char* in, const char* out);

#endif //VM_H
