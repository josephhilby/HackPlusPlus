#include "Lexer.h"

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
    size_t len = strlen(line) - start;
    if (len == 0) {
        return NULL;
    }

    // create blank instruction
    Instruction* inst = create_instruction(line_num);

    // '@xxx\0'  -> xxx, type = A
    if (line[start] == '@') {
        return lex_inst_a(line, inst, start, len);
    }

    // '(xxx)\0' -> xxx, type = L
    if (line[start] == '(') {
        return lex_inst_l(line, inst, start, len);
    }

    // else      -> xxx\0, type = C
    return lex_inst_c(line, inst, start, len);
}

Instruction* lex_inst_a(const char* line, Instruction* inst, const size_t start, const size_t len) {
    inst->type = INST_A;
    memcpy(inst->ltrl, line + start + 1, len);

    // regex inst->ltrl to see if an int (numerical address)
    regex_t regex;
    regcomp(&regex, "^[0-9]+$", REG_EXTENDED);
    if (regexec(&regex, inst->ltrl, 0, NULL, 0) == 0) {
        inst->is_const = true;
    } else {
        inst->is_const = false;
    }
}

Instruction* lex_inst_l(const char* line, Instruction* inst, const size_t start, const size_t len) {
    inst->type = INST_L;
    memcpy(inst->ltrl, line + start + 1, len - 2);
}

Instruction* lex_inst_c(const char* line, Instruction* inst, const size_t start, const size_t len) {
    inst->type = INST_C;
    memcpy(inst->ltrl, line + start, len);
}

void lex_comment(char* line) {
    line[strcspn(line, "\r\n")] = '\0';
    char* comment = strstr(line, "//");
    if (comment) {
        *comment = '\0';
    }
}

size_t find_start(const char* line) {
    size_t i = 0;
    while (isspace(line[i])) {
        i++;
    }

    return i;
}
