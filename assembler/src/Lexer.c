#include "Lexer.h"

// assumptions: correct code, no trailing whitespace, max line 128 chars
Instruction* lex_line(char* line, const size_t line_num) {
    if (!line) {
        return NULL;
    }

    // clear new lines and comment lines
    lex_comment(line);
    if (line[0] == '\0') {
        return NULL;
    }

    // remove leading whitespace (trailing whitespace will cause issues)
    const size_t start = find_start(line);
    const size_t len = strlen(line) - start;
    if (len == 0) {
        return NULL;
    }

    // create blank instruction
    Instruction* inst = create_instruction(line_num);

    // '@xxx'  -> xxx, type = A
    if (line[start] == '@') {
        return lex_a_inst(line, inst, start, len);
    }

    // '(xxx)' -> xxx, type = L
    if (line[start] == '(') {
        return lex_l_inst(line, inst, start, len);
    }

    //   xxx   -> xxx, type = C
    return lex_c_inst(line, inst, start, len);
}

Instruction* lex_a_inst(const char* line, Instruction* inst, const size_t start, const size_t len) {
    inst->type = INST_A;
    memcpy(inst->ltrl, line + start + 1, len);
    inst->ltrl[len] = '\0';

    // regex inst->ltrl to see if a const (numerical memory address)
    regex_t regex;
    regcomp(&regex, "^[0-9]+$", REG_EXTENDED);
    if (regexec(&regex, inst->ltrl, 0, NULL, 0) == 0) {
        inst->is_const = true;
    } else {
        inst->is_const = false;
    }
    regfree(&regex);
    return inst;
}

Instruction* lex_c_inst(const char* line, Instruction* inst, const size_t start, const size_t len) {
    inst->type = INST_C;
    memcpy(inst->ltrl, line + start, len);
    inst->ltrl[len] = '\0';
    return inst;
}

Instruction* lex_l_inst(const char* line, Instruction* inst, const size_t start, const size_t len) {
    inst->type = INST_L;
    memcpy(inst->ltrl, line + start + 1, len - 2);
    inst->ltrl[len - 2] = '\0';
    return inst;
}

void lex_comment(char* line) {
    line[strcspn(line, "\r\n")] = '\0';
    char* comment = strstr(line, "//");
    if (comment) {
        *comment = '\0';
    }
}

size_t find_start(const char* buffer) {
    size_t i = 0;
    while (isspace(buffer[i])) {
        i++;
    }

    return i;
}
