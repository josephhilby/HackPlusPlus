#include <stdio.h>

#include "../include/assembler.h"

int main(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "usage: %s input.asm output.hack\n", argv[0]);
        return 1;
    }

    return assemble_asm(argv[1], argv[2]);
}
