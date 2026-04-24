#ifndef OPERATION_H
#define OPERATION_H

#include <stdio.h>
#include <stdlib.h>
#include "common.h"

typedef struct {
    char ltrl[MAX_LENGTH];
    char command[10];
    char segment[10];
    char data[MAX_LENGTH - 20];
} Operation;

Operation* create_operation(void);

#endif
