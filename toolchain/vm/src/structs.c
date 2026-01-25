#include "structs.h"

Operation* create_operation() {
    Operation* operation = malloc(sizeof(Operation));
    if (!operation) {
        printf("Failed to allocate memory for Operation\n");
        return NULL;
    }
    memset(operation, 0, sizeof(Operation));
    return operation;
}
