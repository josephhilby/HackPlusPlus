#include "generator.h"

/*
  nand2tetris does not use return or frame pointers in the symbol table
  these are added until the entire project is complete then will be refactored
  to the assembler table
 */
#define FRAME "R13"
#define RET   "R14"

static char filename[256];
static long compare_id = 1;

// Memory
static void emit_push(FILE* out, const char* segment, const char* data);
static void emit_pop(FILE* out, const char* segment, const char* data);

// Arithmetic
static void emit_add(FILE* out, const char* segment, const char* data);
static void emit_sub(FILE* out, const char* segment, const char* data);

// Logic
static void emit_eq(FILE* out, const char* segment, const char* data);
static void emit_lt(FILE* out, const char* segment, const char* data);
static void emit_gt(FILE* out, const char* segment, const char* data);
static void emit_neg(FILE* out, const char* segment, const char* data);
static void emit_and(FILE* out, const char* segment, const char* data);
static void emit_or(FILE* out, const char* segment, const char* data);
static void emit_not(FILE* out, const char* segment, const char* data);
static void emit_function(FILE* out, const char* segment, const char* data);
static void emit_return(FILE* out, const char* segment, const char* data);
static void emit_label(FILE* out, const char* segment, const char* data);
static void emit_goto(FILE* out, const char* segment, const char* data);
static void emit_if_goto(FILE* out, const char* segment, const char* data);

static const VmCommand* find_vm_command(const char* mnemonic);

// Dispatch table
static const VmCommand vm_table[] = {
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
    {"function",  emit_function},
    {"return", emit_return},
    {"label",   emit_label},
	{"goto",    emit_goto},
	{"if-goto", emit_if_goto},
    {NULL,   NULL}
};

void set_filename(const char* path) {
    if (!path) {
        filename[0] = '\0';
        return;
    }

    const char* slash = strrchr(path, '/');
    const char* backslash = strrchr(path, '\\');

    const char* sep = slash > backslash ? slash : backslash;
    const char* name = sep ? sep + 1 : path;

    snprintf(filename, sizeof(filename), "%s", name);

    char* dot = strrchr(filename, '.');
    if (dot) {
        *dot = '\0';
    }
}

// Operation := Command Segment Data
void generate(FILE* dest, Operation* op) {
    if (!op) {
        return;
    }
    const VmCommand* command = find_vm_command(op->command);
    if (!command) {
        fprintf(dest, "error: command not found\n");
    }
    command->emit(dest, op->segment, op->data);
}

static const VmCommand* find_vm_command(const char* mnemonic) {
    for (size_t i = 0; vm_table[i].mnemonic; i++) {
        if (strcmp(vm_table[i].mnemonic, mnemonic) == 0) {
            return &vm_table[i];
        }
    }
    return NULL;
}

// push:
//   segment := argument, local, static, constant, this, that, pointer, temp
//   index   := non neg int
static void emit_push(FILE* out, const char* segment, const char* data) {
    // push constant i
    if (strcmp(segment, "constant") == 0) {
        fprintf(out,
            "@%s\n"
            "D=A\n"
            "@SP\n"
            "A=M\n"
            "M=D\n"
            "@SP\n"
            "M=M+1\n",
            data);
        return;
    }

    // push static i   (FileName.i)
    if (strcmp(segment, "static") == 0) {
        fprintf(out,
            "@%s.%s\n"
            "D=M\n"
            "@SP\n"
            "A=M\n"
            "M=D\n"
            "@SP\n"
            "M=M+1\n",
            filename, data);
        return;
    }

    // push pointer 0/1  -> THIS/THAT
    if (strcmp(segment, "pointer") == 0) {
        int idx = atoi(data);
        fprintf(out,
            "@%s\n"
            "D=M\n"
            "@SP\n"
            "A=M\n"
            "M=D\n"
            "@SP\n"
            "M=M+1\n",
            (idx == 0) ? "THIS" : "THAT");
        return;
    }

    // push temp i -> RAM[5+i]
    if (strcmp(segment, "temp") == 0) {
        int addr = 5 + atoi(data);
        fprintf(out,
            "@%d\n"
            "D=M\n"
            "@SP\n"
            "A=M\n"
            "M=D\n"
            "@SP\n"
            "M=M+1\n",
            addr);
        return;
    }

    // base-pointer segments: local/argument/this/that
    const char *base = NULL;
    if (strcmp(segment, "local") == 0) base = "LCL";
    else if (strcmp(segment, "argument") == 0) base = "ARG";
    else if (strcmp(segment, "this") == 0) base = "THIS";
    else if (strcmp(segment, "that") == 0) base = "THAT";

    if (base) {
        // R13 = base + index; D = *R13; push D
        fprintf(out,
            "@%s\n"
            "D=M\n"
            "@%s\n"
            "D=D+A\n"
            "@R13\n"
            "M=D\n"
            "@R13\n"
            "A=M\n"
            "D=M\n"
            "@SP\n"
            "A=M\n"
            "M=D\n"
            "@SP\n"
            "M=M+1\n",
            base, data);
    }
}

// pop:
//   segment := argument, local, static, this, that, pointer, temp
//   index   := non neg int
static void emit_pop(FILE* out, const char* segment, const char* data) {
    if (strcmp(segment, "static") == 0) {
        fprintf(out,
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "@%s.%s\n"
            "M=D\n",
            filename, data);
        return;
    }

    // pointer 0/1 -> THIS/THAT
    if (strcmp(segment, "pointer") == 0) {
        int idx = atoi(data);
        fprintf(out,
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "@%s\n"
            "M=D\n",
            (idx == 0) ? "THIS" : "THAT");
        return;
    }

    // temp i -> RAM[5+i]
    if (strcmp(segment, "temp") == 0) {
        int addr = 5 + atoi(data);
        fprintf(out,
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "@%d\n"
            "M=D\n",
            addr);
        return;
    }

    // base-pointer segments: local/argument/this/that
    const char* base = NULL;
    if (strcmp(segment, "local") == 0) base = "LCL";
    else if (strcmp(segment, "argument") == 0) base = "ARG";
    else if (strcmp(segment, "this") == 0) base = "THIS";
    else if (strcmp(segment, "that") == 0) base = "THAT";

    if (base) {
        fprintf(out,
            "@%s\n"
            "D=M\n"
            "@%s\n"
            "D=D+A\n"
            "@R13\n"
            "M=D\n"
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "@R13\n"
            "A=M\n"
            "M=D\n",
            base, data);
    }
}

static void emit_add(FILE* out, const char* segment, const char* data) {
    (void)segment;
    (void)data;

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

static void emit_sub(FILE* out, const char* segment, const char* data) {
    (void)segment;
    (void)data;

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

// true = -1, false = 0
static static void emit_compare(FILE* out, const char* jump) {
    const long id = compare_id++;

    fprintf(out,
        "@SP\n"
        "AM=M-1\n"
        "D=M\n"
        "A=A-1\n"
        "D=M-D\n"
        "@CMP_TRUE_%ld\n"
        "D;%s\n"
        "@SP\n"
        "A=M-1\n"
        "M=0\n"
        "@CMP_END_%ld\n"
        "0;JMP\n"
        "(CMP_TRUE_%ld)\n"
        "@SP\n"
        "A=M-1\n"
        "M=-1\n"
        "(CMP_END_%ld)\n",
        id, jump, id, id, id);
}

static void emit_eq(FILE* out, const char* segment, const char* data) {
    (void)segment;
    (void)data;
    emit_compare(out, "JEQ");
}

static void emit_lt(FILE* out, const char* segment, const char* data) {
    (void)segment;
    (void)data;
    emit_compare(out, "JLT");
}

static void emit_gt(FILE* out, const char* segment, const char* data) {
    (void)segment;
    (void)data;
    emit_compare(out, "JGT");
}

static void emit_neg(FILE* out, const char* segment, const char* data) {
    (void)segment;
    (void)data;

    fprintf(out,
        "@SP\n"
        "AM=M-1\n"
        "M=-M\n"
        "@SP\n"
        "M=M+1\n");
}


// todo: improve emit_and by overwirting SP-1
// @SP
// AM=M-1  // SP--, A=SP -> y
// D=M     // D=y
// A=A-1   // A=SP-1     -> x
// M=D&M   // x=x&y
static void emit_and(FILE* out, const char* segment, const char* data) {
    (void)segment;
    (void)data;

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

static void emit_or(FILE* out, const char* segment, const char* data) {
    (void)segment;
    (void)data;

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

static void emit_not(FILE* out, const char* segment, const char* data) {
    (void)segment;
    (void)data;

    fprintf(out,
        "@SP\n"
        "AM=M-1\n"
        "M=!M\n"
        "@SP\n"
        "M=M+1\n");
}

static void emit_function(FILE* out, const char* name, const char* n_vars) {
    int count = atoi(n_vars);

    fprintf(out, "(%s)\n", name);

    for (int i = 0; i < count; i++) {
        fprintf(out,
            "@SP\n"
            "A=M\n"
            "M=0\n"
            "@SP\n"
            "M=M+1\n");
    }
}

static void emit_return(FILE* out, const char* segment, const char* data) {
    (void)segment;
    (void)data;

    fprintf(out,

        // save current frame base (FRAME = LCL)
        "@LCL\n"
        "D=M\n"
        "@" FRAME "\n"
        "M=D\n"

        // get return address (RET = (FRAME - 5))
        "@5\n"
        "A=D-A\n"
        "D=M\n"
        "@" RET "\n"
        "M=D\n"

        // move return value to arg 0 (ARG = pop())
        "@SP\n"
        "AM=M-1\n"
        "D=M\n"
        "@ARG\n"
        "A=M\n"
        "M=D\n"

        // restore caller stack pointer (SP = ARG + 1)
        "@ARG\n"
        "D=M+1\n"
        "@SP\n"
        "M=D\n"

        // restore THAT = (FRAME - 1)
        "@" FRAME "\n"
        "AM=M-1\n"
        "D=M\n"
        "@THAT\n"
        "M=D\n"

        // restore THIS = (FRAME - 2)
        "@" FRAME "\n"
        "AM=M-1\n"
        "D=M\n"
        "@THIS\n"
        "M=D\n"

        // restore ARG = (FRAME - 3)
        "@" FRAME "\n"
        "AM=M-1\n"
        "D=M\n"
        "@ARG\n"
        "M=D\n"

        // restore LCL = (FRAME - 4)
        "@" FRAME "\n"
        "AM=M-1\n"
        "D=M\n"
        "@LCL\n"
        "M=D\n"

        // jump back to caller
        "@" RET "\n"
        "A=M\n"
        "0;JMP\n"
    );
}

static void emit_label(FILE* out, const char* segment, const char* label) {
    (void)segment;
    fprintf(out, "(%s)\n", label);
}

static void emit_goto(FILE* out, const char* segment, const char* label) {
    (void)segment;
    fprintf(out,
        "@%s\n"
        "0;JMP\n",
        label);
}

static void emit_if_goto(FILE* out, const char* segment, const char* label) {
    (void)segment;
    fprintf(out,
        "@SP\n"
        "AM=M-1\n"
        "D=M\n"
        "@%s\n"
        "D;JNE\n",
        label);
}
