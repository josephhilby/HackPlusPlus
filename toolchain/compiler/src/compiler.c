#include <stdio.h>

#include "../include/compiler.h"

int compile_jack(const char* in, const char* out) {
  // Open Source File
  FILE* src = fopen(in, "r");
  if (!src) {
    printf("Error opening source file\n");
    return 1;
  }

  // Open Destination File
  FILE* dest = fopen(out, "w");
  if (!dest) {
    printf("Error opening output file\n");
    fclose(src);
    return 1;
  }

  // Close Files
  fclose(src);
  fclose(dest);
  return 0;
}
