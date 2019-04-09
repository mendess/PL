#include "util.h"

char* str_replace(char* s, char f, char r)
{
    char * start = s;
    while (*s) {
        if (*s == f)
            *s = r;
        ++s;
    }
    return start;
}

