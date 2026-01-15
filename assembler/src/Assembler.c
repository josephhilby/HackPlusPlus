#include <stdlib.h>

#include "Structs.h"
#include "Parser.h"
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

    // Init System Table
    SystemTable* stbl = table_init();
    if (!stbl) {
        fclose(src);
        fclose(dest);
        return 1;
    }

    // Load Builtin Values (i.e., Predefined Symbols, RAM)
    load_builtin(stbl);

    // TWO PASS ASSEMBLER
    // First Pass: create instruction list, and populate label symbols (ROM) in system table
    Instruction* list = first_pass(src, stbl);
    // Second Pass: generate output code, populate/manage variable symbols (RAM) in system table
    second_pass(dest, list, stbl);

    // Deallocate Data Structs
    table_free(stbl);
    list_free(list);

    // Close Files
    fclose(src);
    fclose(dest);
    return 0;
}
