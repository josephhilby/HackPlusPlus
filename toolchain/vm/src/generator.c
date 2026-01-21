#include "generator.h"

// Dispatch table
static const Command vm_table[] = {
    {"push", emit_push},
    {"pop",  emit_pop},
    {"add",  emit_add},
    {NULL,   NULL}
};
// Use
// Operation *op = find_vm_operation(cmd);
// if (!op) error("unknown VM command");
// operation->emit(out, arg1, arg2);
void generate(FILE* dest, Operation* opr) {

}

Command* find_vm_operation(const char *mnemonic) {
    for (size_t i = 0; vm_table[i].mnemonic; i++) {
        if (strcmp(vm_table[i].mnemonic, mnemonic) == 0) {
            return &vm_table[i];
        }
    }
    return NULL;
}

void emit_push(FILE *out, const char *segment, int index) {
    if (strcmp(segment, "constant") == 0) {
        fprintf(out,
            "@%d\n"
            "D=A\n"
            "@SP\n"
            "A=M\n"
            "M=D\n"
            "@SP\n"
            "M=M+1\n",
            index);
    }
}

void emit_pop(FILE *out, const char *segment, int index) {
    if (strcmp(segment, "constant") == 0) {
        fprintf(out,
            "@SP\n"
            "M=M+1\n"
            "D=M\n",
            index);
    }
}

void emit_add(FILE *out, const char *segment, int index) {
    if (strcmp(segment, "constant") == 0) {
        fprintf(out,
            "@%d\n"
            "D=A\n"
            "@SP\n"
            "A=M\n"
            "M=D\n"
            "@SP\n"
            "M=M+1\n",
            index);
    }
}