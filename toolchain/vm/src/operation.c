#include "../include/operation.h"

#include <stdio.h>
#include <stdlib.h>

Operation *create_operation(void) {
  Operation *operation = calloc(1, sizeof(*operation));
  if (!operation) {
    printf("Failed to allocate memory for Operation\n");
    return NULL;
  }

  return operation;
}
