#include "lexer.h"
#include "../../shared/line.h"

Operation* lex_line(char* line) {
    char* cleaned = clean_line(line);
    if (!cleaned) {
        return NULL;
    }

    Operation* op = create_operation();
    return lex_operation(cleaned, op, strlen(cleaned));
}

Operation* lex_operation(const char* line, Operation* op, size_t len) {
    memcpy(op->ltrl, line, len);
    op->ltrl[len] = '\0';

    return op;
}
