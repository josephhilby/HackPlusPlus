#ifndef GENERATOR_H
#define GENERATOR_H

#include <string.h>
#include "operation.h"
#include "command.h"

void generate(FILE* dest, Operation* opr);
void set_filename(const char* path);

#endif //GENERATOR_H
