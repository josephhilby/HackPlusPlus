#include "structs.h"

void set_filename(const char* path) {
    // this will break on windows
    const char* slash = strrchr(path, '/');
    const char* name = slash ? slash + 1 : path;

    strncpy(filename, name, sizeof(filename));
    filename[sizeof(filename) - 1] = '\0';

    char* dot = strrchr(filename, '.');
    if (dot) *dot = '\0';
}

Operation* create_operation() {
    Operation* operation = malloc(sizeof(Operation));
    if (!operation) {
        printf("Failed to allocate memory for Operation\n");
        return NULL;
    }
    memset(operation, 0, sizeof(Operation));
    return operation;
}
