#include "structs.h"

Operation* create_operation(const char* command, const char* segment, const char* data) {
    Operation* operation = malloc(sizeof(Operation));
    memset(operation, 0, sizeof(Operation));
    strcpy(operation->command, command);
    strcpy(operation->segment, segment);
    strcpy(operation->data, data);
    return operation;
}