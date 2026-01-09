#include "Lexer.h"

Instruction* lex_line(char* buffer, size_t line_num) {
    if (!buffer) {
        return NULL;
    }

    // clear new lines and comments
    buffer[strcspn(buffer, "\r\n")] = '\0';
    char* comment = strstr(buffer, "//");
    if (comment) {
        *comment = '\0';
    }
    if (buffer[0] == '\0') {
        return NULL;
    }

    // clear tabs and spaces
    size_t start = find_start(buffer);
    size_t len = strlen(buffer) - start;
    if (len == 0) {
        return NULL;
    }

    Instruction* inst = malloc(sizeof(Instruction));
    if (inst == NULL) {
        return NULL;
    }
    inst->line = line_num;
    inst->next = NULL;

    // '@xxx\0'  -> xxx, type = A
    if (buffer[start] == '@') {
        inst->type = INST_A;
        memcpy(inst->ltrl, buffer + start + 1, len);
        // regex inst->ltrl to see if an int (numerical address)
        regex_t regex;
        regcomp(&regex, "^[0-9]+$", REG_EXTENDED);
        if (regexec(&regex, inst->ltrl, 0, NULL, 0) == 0) {
            inst->is_var = false;
        } else {
            inst->is_var = true;
        }
    // '(xxx)\0' -> xxx, type = L
    } else if (buffer[start] == '(') {
        inst->type = INST_L;
        memcpy(inst->ltrl, buffer + start + 1, len - 2);
    //   else  -> xxx\0, type = C
    } else {
        inst->type = INST_C;
        memcpy(inst->ltrl, buffer + start, len);
    }

    return inst;
}

size_t find_start(char* buffer) {
    size_t i = 0;
    while (isspace(buffer[i])) {
        i++;
    }
    return i;
}
