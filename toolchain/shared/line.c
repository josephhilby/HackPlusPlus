#include "line.h"

void strip_newline(char* line) {
    if (!line) return;

    line[strcspn(line, "\r\n")] = '\0';
}

void strip_line_comment(char* line) {
    if (!line) return;

    char* comment = strstr(line, "//");
    if (comment) {
        *comment = '\0';
    }
}

char* trim_left(char* line) {
    if (!line) return NULL;

    while (isspace((unsigned char)*line)) {
        line++;
    }

    return line;
}

void trim_right(char* line) {
    if (!line) return;

    char* end = line + strlen(line);

    while (end > line && isspace((unsigned char)*(end - 1))) {
        end--;
    }

    *end = '\0';
}

char* clean_line(char* line) {
    if (!line) return NULL;

    strip_newline(line);
    strip_line_comment(line);

    char* start = trim_left(line);
    trim_right(start);

    return *start ? start : NULL;
}
