#include "lexer.h"
#include "generator.h"

int translate_asm(const char* in, const char* out) {
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

    // ONE PASS TRANSLATOR
    char buffer[MAX_LENGTH];
    while (fgets(buffer, MAX_LENGTH, src)) {
        Operation* next = lex_line(buffer);
        generate(dest, next);
    }

    // Close Files
    fclose(src);
    fclose(dest);
    return 0;
}