#include "generator.h"

/*
  nand2tetris 'registers':
    R0-12  reserved
    R13-15 free

  To keep things readable I am adding def for the free 'registers'. When able refactor
  to the assembler table so the names can be used. Also consider TMP_A for 13 and alias TMP_B
  for 14. To help distinguish its use
 */
#define TMP   "R13"
#define FRAME "R14"
#define RET   "R15"

/*
  Generator state:
    filename         Current VM file stem. Used for static symbols and generated labels.
    current_function Current VM function scope. Used for label/goto/if-goto scoping.
    compare_id       Unique suffix for generated comparison labels.
    function_id      Unique suffix for generated return-address labels.
*/
static char filename[256];
static char current_function[256];
static long compare_id = 0;
static long function_id = 0;

static const char* label_scope(void) {
    return current_function[0] ? current_function : filename;
}

// Emitters
//   Memory
static void emit_push(FILE* out, const char* segment, const char* index);
static void emit_pop(FILE* out, const char* segment, const char* index);

//   Functions
static void emit_function(FILE* out, const char* name, const char* n_vars);
static void emit_call(FILE* out, const char* name, const char* n_args);

//   Branching
static void emit_label(FILE* out, const char* _seg, const char* label);
static void emit_goto(FILE* out, const char* _seg, const char* label);
static void emit_if_goto(FILE* out, const char* _seg, const char* label);

//   Arithmetic and Logic
static void emit_add(FILE* out, const char* _seg, const char* _dat);
static void emit_sub(FILE* out, const char* _seg, const char* _dat);
static void emit_eq(FILE* out, const char* _seg, const char* _dat);
static void emit_lt(FILE* out, const char* _seg, const char* _dat);
static void emit_gt(FILE* out, const char* _seg, const char* _dat);
static void emit_neg(FILE* out, const char* _seg, const char* _dat);
static void emit_and(FILE* out, const char* _seg, const char* _dat);
static void emit_or(FILE* out, const char* _seg, const char* _dat);
static void emit_not(FILE* out, const char* _seg, const char* _dat);

//   Return
static void emit_return(FILE* out, const char* _seg, const char* _dat);

// Dispatch Table
static const VmCommand vm_table[] = {
    {"push", emit_push},
    {"pop", emit_pop},
    {"function", emit_function},
    {"call", emit_call},
    {"label", emit_label},
	{"goto", emit_goto},
	{"if-goto", emit_if_goto},
    {"add", emit_add},
    {"sub", emit_sub},
    {"eq", emit_eq},
    {"lt", emit_lt},
    {"gt", emit_gt},
    {"neg", emit_neg},
    {"and", emit_and},
    {"or", emit_or},
    {"not", emit_not},
    {"return", emit_return},
    {NULL,   NULL}
};

// Dispatcher
static const VmCommand* find_vm_command(const char* mnemonic) {
    for (size_t i = 0; vm_table[i].mnemonic; i++) {
        if (strcmp(vm_table[i].mnemonic, mnemonic) == 0) {
            return &vm_table[i];
        }
    }
    return NULL;
}

void set_filename(const char* path) {
    current_function[0] = '\0';

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

void generate(FILE* dest, Operation* op) {
    if (!dest || !op) return;

    const VmCommand* command = find_vm_command(op->command);
    if (!command) {
        fprintf(dest, "error: command not found: %s\n", op->command);
        return;
    }

    command->emit(dest, op->segment, op->data);
}

void generate_bootstrap(FILE* out) {
    // SP = 256
    fprintf(out,
        "@256\n"
        "D=A\n"
        "@SP\n"
        "M=D\n");

    // call Sys.init 0
    emit_call(out, "Sys.init", "0");
}

// push: segment[index]
//   segment := argument, local, static, constant, this, that, pointer, temp
//   index   := non neg int
static void emit_push(FILE* out, const char* segment, const char* index) {
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
            index);
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
            filename, index);
        return;
    }

    // push pointer 0/1  -> THIS/THAT
    if (strcmp(segment, "pointer") == 0) {
        int idx = atoi(index);
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
        int addr = 5 + atoi(index);
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
        // TMP = base + index; D = *TMP; push D
        fprintf(out,
            "@%s\n"
            "D=M\n"
            "@%s\n"
            "D=D+A\n"
            "@" TMP "\n"
            "M=D\n"
            "@" TMP "\n"
            "A=M\n"
            "D=M\n"
            "@SP\n"
            "A=M\n"
            "M=D\n"
            "@SP\n"
            "M=M+1\n",
            base, index);
    }
}

// pop: segment[index]
//   segment := argument, local, static, this, that, pointer, temp
//   index   := non neg int
static void emit_pop(FILE* out, const char* segment, const char* index) {
    if (strcmp(segment, "static") == 0) {
        fprintf(out,
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "@%s.%s\n"
            "M=D\n",
            filename, index);
        return;
    }

    // pointer 0/1 -> THIS/THAT
    if (strcmp(segment, "pointer") == 0) {
        int idx = atoi(index);
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
        int addr = 5 + atoi(index);
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
            "@" TMP "\n"
            "M=D\n"
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "@" TMP "\n"
            "A=M\n"
            "M=D\n",
            base, index);
    }
}

static void emit_function(FILE* out, const char* name, const char* n_vars) {
    snprintf(current_function, sizeof(current_function), "%s", name);

    fprintf(out, "(%s)\n", name);

    int count = atoi(n_vars);
    for (int i = 0; i < count; i++) {
        fprintf(out,
            "@SP\n"
            "A=M\n"
            "M=0\n"
            "@SP\n"
            "M=M+1\n");
    }
}

static void emit_call(FILE* out, const char* name, const char* n_args) {
    const long id = function_id++;
    char ret_label[256];
	snprintf(ret_label, sizeof(ret_label), "%s$RET.%ld", filename, id);

    fprintf(out,
        // push return address
        "@%s\n"
        "D=A\n"
        "@SP\n"
        "A=M\n"
        "M=D\n"
        "@SP\n"
        "M=M+1\n"

        // push LCL
        "@LCL\n"
        "D=M\n"
        "@SP\n"
        "A=M\n"
        "M=D\n"
        "@SP\n"
        "M=M+1\n"

        // push ARG
        "@ARG\n"
        "D=M\n"
        "@SP\n"
        "A=M\n"
        "M=D\n"
        "@SP\n"
        "M=M+1\n"

        // push THIS
        "@THIS\n"
        "D=M\n"
        "@SP\n"
        "A=M\n"
        "M=D\n"
        "@SP\n"
        "M=M+1\n"

        // push THAT
        "@THAT\n"
        "D=M\n"
        "@SP\n"
        "A=M\n"
        "M=D\n"
        "@SP\n"
        "M=M+1\n"

        // ARG = SP - nArgs - 5
        "@SP\n"
        "D=M\n"
        "@%s\n"
        "D=D-A\n"
        "@5\n"
        "D=D-A\n"
        "@ARG\n"
        "M=D\n"

        // LCL = SP
        "@SP\n"
        "D=M\n"
        "@LCL\n"
        "M=D\n"

        // goto function
        "@%s\n"
        "0;JMP\n"

        // return address label
        "(%s)\n",
        ret_label, n_args, name, ret_label);
}

static void emit_label(FILE* out, const char* _seg, const char* label) {
    (void)_seg;
    fprintf(out, "(%s$%s)\n", label_scope(), label);
}

static void emit_goto(FILE* out, const char* _seg, const char* label) {
    (void)_seg;

    fprintf(out,
    	"@%s$%s\n"
    	"0;JMP\n",
    	label_scope(), label);
}

static void emit_if_goto(FILE* out, const char* _seg, const char* label) {
    (void)_seg;

    fprintf(out,
    	"@SP\n"
    	"AM=M-1\n"
    	"D=M\n"
    	"@%s$%s\n"
    	"D;JNE\n",
    	label_scope(), label);
}

static void emit_add(FILE* out, const char* _seg, const char* _dat) {
    (void)_seg;
    (void)_dat;

    fprintf(out,
        "@SP\n"
        "AM=M-1\n"
        "D=M\n"
        "A=A-1\n"
        "M=D+M\n");
}

static void emit_sub(FILE* out, const char* _seg, const char* _dat) {
    (void)_seg;
    (void)_dat;

    fprintf(out,
        "@SP\n"
        "AM=M-1\n"
        "D=M\n"
        "A=A-1\n"
        "M=M-D\n");
}

// true = -1, false = 0
static void compare(FILE* out, const char* jump) {
    const long id = compare_id++;

    char true_label[256];
    char end_label[256];

    snprintf(true_label, sizeof(true_label), "%s$CMP_TRUE.%ld", filename, id);
    snprintf(end_label, sizeof(end_label), "%s$CMP_END.%ld", filename, id);

    fprintf(out,
        "@SP\n"
        "AM=M-1\n"
        "D=M\n"
        "A=A-1\n"
        "D=M-D\n"
        "@%s\n"
        "D;%s\n"
        "@SP\n"
        "A=M-1\n"
        "M=0\n"
        "@%s\n"
        "0;JMP\n"
        "(%s)\n"
        "@SP\n"
        "A=M-1\n"
        "M=-1\n"
        "(%s)\n",
        true_label,
        jump,
        end_label,
        true_label,
        end_label);
}

static void emit_eq(FILE* out, const char* _seg, const char* _dat) {
    (void)_seg;
    (void)_dat;

    compare(out, "JEQ");
}

static void emit_lt(FILE* out, const char* _seg, const char* _dat) {
    (void)_seg;
    (void)_dat;

    compare(out, "JLT");
}

static void emit_gt(FILE* out, const char* _seg, const char* _dat) {
    (void)_seg;
    (void)_dat;

    compare(out, "JGT");
}

static void emit_neg(FILE* out, const char* _seg, const char* _dat) {
    (void)_seg;
    (void)_dat;

    fprintf(out,
        "@SP\n"
        "A=M-1\n"
        "M=-M\n");
}

static void emit_and(FILE* out, const char* _seg, const char* _dat) {
    (void)_seg;
    (void)_dat;

    fprintf(out,
        "@SP\n"
        "AM=M-1\n"
        "D=M\n"
        "A=A-1\n"
        "M=D&M\n");
}

static void emit_or(FILE* out, const char* _seg, const char* _dat) {
    (void)_seg;
    (void)_dat;

    fprintf(out,
        "@SP\n"
        "AM=M-1\n"
        "D=M\n"
        "A=A-1\n"
        "M=D|M\n");
}

static void emit_not(FILE* out, const char* _seg, const char* _dat) {
    (void)_seg;
    (void)_dat;

    fprintf(out,
        "@SP\n"
        "A=M-1\n"
        "M=!M\n");
}

static void emit_return(FILE* out, const char* _seg, const char* _dat) {
    (void)_seg;
    (void)_dat;

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
