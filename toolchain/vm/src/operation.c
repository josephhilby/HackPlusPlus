#include <stdio.h>
#include <stdlib.h>

#include "../include/operation.h"

Operation* create_operation(void) {
    Operation* operation = calloc(1, sizeof(*operation));
    if (!operation) {
        printf("Failed to allocate memory for Operation\n");
        return NULL;
    }

    return operation;
}
