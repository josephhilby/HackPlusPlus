#include "lexer.h"
#include "../../shared/line.h"

// assumptions: correct code, max line 128 chars
Instruction* lex_line(char* line, const size_t line_num) {
    char* cleaned = clean_line(line);
    if (!cleaned) {
        return NULL;
    }

    const size_t len = strlen(cleaned);

    Instruction* inst = create_instruction(line_num);

    // '@xxx' -> xxx, type = A
    if (cleaned[0] == '@') {
        return lex_a_inst(cleaned, inst, len);
    }

    // '(xxx)' -> xxx, type = L
    if (cleaned[0] == '(') {
        return lex_l_inst(cleaned, inst, len);
    }

    // 'xxx' -> xxx, type = C
    return lex_c_inst(cleaned, inst, len);
}

Instruction* lex_a_inst(const char* line, Instruction* inst, const size_t len) {
    inst->type = INST_A;

    memcpy(inst->ltrl, line + 1, len - 1);
    inst->ltrl[len - 1] = '\0';

    regex_t regex;
    regcomp(&regex, "^[0-9]+$", REG_EXTENDED);

    inst->is_const = regexec(&regex, inst->ltrl, 0, NULL, 0) == 0;

    regfree(&regex);
    return inst;
}

Instruction* lex_c_inst(const char* line, Instruction* inst, const size_t len) {
    inst->type = INST_C;

    memcpy(inst->ltrl, line, len);
    inst->ltrl[len] = '\0';

    return inst;
}

Instruction* lex_l_inst(const char* line, Instruction* inst, const size_t len) {
    inst->type = INST_L;

    memcpy(inst->ltrl, line + 1, len - 2);
    inst->ltrl[len - 2] = '\0';

    return inst;
}
