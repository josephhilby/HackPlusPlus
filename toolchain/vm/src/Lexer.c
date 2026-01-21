#include "Lexer.h"

Operation* lex_line(char* line) {
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

    Operation* operation = create_operation();
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