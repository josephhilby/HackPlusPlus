#include <stdlib.h>

#include "Structs.h"
#include "Parser.h"
#include "Generator.h"


int main(const int argc, char* argv[]) {
     // comment out for testing
     if (argc != 3) {
         printf("Usage: ./main <input_file> <output_file>\n");
         return 1;
     }

    //Open Source File
    FILE* src = fopen(argv[1], "r");
    if (src == NULL) {
        printf("Error opening source file\n");
        return 1;
    }

    // Open Destination File
    FILE* dest = fopen(argv[2], "w");
    if (dest == NULL) {
        printf("Error opening output file\n");
        fclose(src);
        return 1;
    }

    // Init System Table
    SystemTable* stbl = table_init();
    if (stbl == NULL) {
        fclose(src);
        return 1;
    }

    // Load Builtin Values
    load_builtin(stbl);

    // Two Pass Assembler
    Instruction* list = first_pass(src, stbl);
    second_pass(dest, list, stbl);

    // Deallocate Data Structs
    table_free(stbl);
    list_free(list);

    // Close Files
    fclose(src);
    fclose(dest);
    return 0;
}
