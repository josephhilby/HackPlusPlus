#include "structs.h"
#include "lexer.h"
#include "parser.h"
#include "generator.h"

int translate_asm(const char* in, const char* out) {
    // Open Source File
    FILE* src = fopen(in, "r");
    if (!src) {
        printf("Error opening source file\n");
        return 1;
    }

    // Set File Name
    set_filename(in);

    // Open Destination File
    FILE* dest = fopen(out, "w");
    if (!dest) {
        printf("Error opening output file\n");
        fclose(src);
        return 1;
    }

    // Write Builtins
    emit_builtins(dest);

    // ONE PASS TRANSLATOR
    char buffer[MAX_LENGTH];
    while (fgets(buffer, MAX_LENGTH, src)) {
        Operation* next = lex_line(buffer);
        parse_operation(next);
        generate(dest, next);
        free(next);
    }

    // Close Files
    fclose(src);
    fclose(dest);
    return 0;
}
