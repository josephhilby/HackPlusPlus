#ifndef LINE_H
#define LINE_H

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

void strip_newline(char* line);
void strip_line_comment(char* line);
char* trim_left(char* line);
void trim_right(char* line);
char* clean_line(char* line);

#endif //LINE_H
