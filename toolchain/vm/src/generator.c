#include "generator.h"

char filename[256];
long return_id = 1;

void set_filename(const char* path) {
    // this will break on windows
    const char* slash = strrchr(path, '/');
    const char* name = slash ? slash + 1 : path;

    strncpy(filename, name, sizeof(filename));
    filename[sizeof(filename) - 1] = '\0';

    char* dot = strrchr(filename, '.');
    if (dot) *dot = '\0';
}

// Dispatch table
static const Command vm_table[] = {
    {"push", emit_push},
    {"pop",  emit_pop},
    {"add",  emit_add},
    {"sub",  emit_sub},
    {"eq",  emit_eq},
    {"lt",  emit_lt},
    {"gt",  emit_gt},
    {"neg",  emit_neg},
    {"and",  emit_and},
    {"or",  emit_or},
    {"not",  emit_not},
    {NULL,   NULL}
};

// Operation := Command Segment Data
void generate(FILE* dest, Operation* op) {
    if (!op) {
        return;
    }
    const Command* command = find_vm_command(op->command);
    if (!command) {
        fprintf(dest, "error: command not found\n");
    }
    command->emit(dest, op->segment, op->data);
}

const Command* find_vm_command(const char *mnemonic) {
    for (size_t i = 0; vm_table[i].mnemonic; i++) {
        if (strcmp(vm_table[i].mnemonic, mnemonic) == 0) {
            return &vm_table[i];
        }
    }
    return NULL;
}

// true = -1, false = 0, use R15 as RA
void emit_builtins(FILE *out) {
    fprintf(out,
        "@START\n"
        "0;JMP\n"
        "(RETURN)\n"
        "@R15\n"
        "A=M\n"
        "0;JMP\n"
        "(TRUE)\n"
        "@SP\n"
        "A=M-1\n"
        "M=-1\n"
        "@RETURN\n"
        "0;JMP\n"
        "(FALSE)\n"
        "@SP\n"
        "A=M-1\n"
        "M=0\n"
        "@RETURN\n"
        "0;JMP\n"
        "(START)\n");
}

// push:
//   segment := argument, local, static, constant, this, that, pointer, temp
//   index   := non neg int
void emit_push(FILE *out, const char *segment, const char* data) {
    if (strcmp(segment, "constant") == 0) {
        fprintf(out,
            "@%s\n"    // # in reg A
            "D=A\n"    // # in reg D
            "@SP\n"    // SP ptr in reg A
            "A=M\n"    // SP addr in reg A
            "M=D\n"    // # in RAM[SP]
            "@SP\n"    // SP ptr in reg A
            "M=M+1\n", // SP addr++
            data);
    }
    else if (strcmp(segment, "static") == 0) {
        fprintf(out,
            "@%s.%s\n" // # in reg A
            "D=M\n"    // # in reg D
            "@SP\n"    // SP ptr in reg A
            "A=M\n"    // SP addr in reg A
            "M=D\n"    // # in RAM[SP]
            "@SP\n"    // SP ptr in reg A
            "M=M+1\n", // SP addr++
            filename, data);
    }
}

// push:
//   segment := argument, local, static, this, that, pointer, temp
//   index   := non neg int
void emit_pop(FILE *out, const char *segment, const char* data) {
    if (strcmp(segment, "static") == 0) {
        fprintf(out,
            "@SP\n"    // SP ptr in reg A
            "AM=M-1\n" // SP addr--
            "D=M\n"    // RAM[SP] in reg D
            "@%s.%s\n"
            "M=D\n",
            filename, data);
    }
}

void emit_add(FILE *out, const char *segment, const char* data) {
    if (strcmp(segment, "") == 0) {
        fprintf(out,
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "@SP\n"
            "AM=M-1\n"
            "M=D+M\n"
            "@SP\n"
            "M=M+1\n");
    }
}

void emit_sub(FILE *out, const char *segment, const char* data) {
    if (strcmp(segment, "") == 0) {
        fprintf(out,
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "@SP\n"
            "AM=M-1\n"
            "M=M-D\n"
            "@SP\n"
            "M=M+1\n");
    }
}

void emit_eq(FILE *out, const char *segment, const char* data) {
    fprintf(out,
        "@RET_POINT_%ld\n"
        "D=A\n"
        "@R15\n"
        "M=D\n"
        "@SP\n"
        "AM=M-1\n"
        "D=M\n"
        "@SP\n"
        "AM=M-1\n"
        "MD=M-D\n"
        "@SP\n"
        "M=M+1\n"
        "@TRUE\n"
        "D;JEQ\n"
        "@FALSE\n"
        "0;JMP\n"
        "(RET_POINT_%ld)\n",
        return_id, return_id);
    return_id++;
}

void emit_lt(FILE *out, const char *segment, const char* data) {
    fprintf(out,
        "@RET_POINT_%ld\n"
        "D=A\n"
        "@R15\n"
        "M=D\n"
        "@SP\n"
        "AM=M-1\n"
        "D=M\n"
        "@SP\n"
        "AM=M-1\n"
        "MD=M-D\n"
        "@SP\n"
        "M=M+1\n"
        "@TRUE\n"
        "D;JLT\n"
        "@FALSE\n"
        "0;JMP\n"
        "(RET_POINT_%ld)\n",
        return_id, return_id);
    return_id++;
}

void emit_gt(FILE *out, const char *segment, const char* data) {
    fprintf(out,
        "@RET_POINT_%ld\n"
        "D=A\n"
        "@R15\n"
        "M=D\n"
        "@SP\n"
        "AM=M-1\n"
        "D=M\n"
        "@SP\n"
        "AM=M-1\n"
        "MD=M-D\n"
        "@SP\n"
        "M=M+1\n"
        "@TRUE\n"
        "D;JGT\n"
        "@FALSE\n"
        "0;JMP\n"
        "(RET_POINT_%ld)\n",
        return_id, return_id);
    return_id++;
}

void emit_neg(FILE *out, const char *segment, const char* data) {
    if (strcmp(segment, "") == 0) {
        fprintf(out,
        "@SP\n"
        "AM=M-1\n"
        "M=-M\n"
        "@SP\n"
        "M=M+1\n");
    }
}

// can improve by just overwirting SP-1
// @SP
// AM=M-1
// D=M
// A=A-1
// M=D&M

void emit_and(FILE *out, const char *segment, const char* data) {
    if (strcmp(segment, "") == 0) {
        fprintf(out,
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "@SP\n"
            "AM=M-1\n"
            "M=D&M\n"
            "@SP\n"
            "M=M+1\n");
    }
}

void emit_or(FILE *out, const char *segment, const char* data) {
    if (strcmp(segment, "") == 0) {
        fprintf(out,
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "@SP\n"
            "AM=M-1\n"
            "M=D|M\n"
            "@SP\n"
            "M=M+1\n");
    }
}

void emit_not(FILE *out, const char *segment, const char* data) {
    if (strcmp(segment, "") == 0) {
        fprintf(out,
            "@SP\n"
            "AM=M-1\n"
            "M=!M\n"
            "@SP\n"
            "M=M+1\n");
    }
}
