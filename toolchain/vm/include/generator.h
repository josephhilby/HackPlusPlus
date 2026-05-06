#ifndef GENERATOR_H
#define GENERATOR_H

#include <stdio.h>

#include "operation.h"
#include "command.h"

void generate(FILE* dest, Operation* opr);
void set_filename(const char* path);
void reset_generator(void);
void generate_bootstrap(FILE* out);

#endif //GENERATOR_H
