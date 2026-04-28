#ifndef OPERATION_H
#define OPERATION_H

#include <stdio.h>
#include <stdlib.h>
#include "common.h"

typedef struct {
    char ltrl[MAX_LENGTH];
    char command[MAX_TOKEN];
    char segment[MAX_TOKEN];
    char data[MAX_TOKEN];
} Operation;

Operation* create_operation(void);

#endif
