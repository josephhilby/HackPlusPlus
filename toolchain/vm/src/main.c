#include "vm.h"
#include <stdio.h>

int main(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "usage: %s input.vm output.asm\n", argv[0]);
        return 1;
    }

    return translate_vm(argv[1], argv[2]);
}
