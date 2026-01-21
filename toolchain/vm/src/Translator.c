#include "Lexer.h"
#include "Generator.h"

int main(const int argc, char* argv[]) {
    // Check Args
    if (argc != 3) {
        printf("Usage: ./main <input_file> <output_file>\n");
        return 1;
    }

    // Open Source File
    FILE* src = fopen(argv[1], "r");
    if (!src) {
        printf("Error opening source file\n");
        return 1;
    }

    // Open Destination File
    FILE* dest = fopen(argv[2], "w");
    if (!dest) {
        printf("Error opening output file\n");
        fclose(src);
        return 1;
    }

    // ONE PASS TRANSLATOR
    char buffer[MAX_LENGTH];
    while (fgets(buffer, MAX_LENGTH, src)) {
        const Operation* next = lex_line(buffer);
        generate(dest, &next);
    }

    // Close Files
    fclose(src);
    fclose(dest);
    return 0;
}