#include "string_vec.h"
#define __GNU_SOURCE
#include <string.h>
char* strdup(const char*);

void string_vec_add(StringVec* vec, const char* str)
{
    if (vec->len >= vec->size) {
        vec->size = vec->size == 0 ? 1 : vec->size * 2;
        vec->strings = realloc(vec->strings, sizeof(char*) * vec->size);
    }
    vec->strings[vec->len++] = strdup(str);
}

void string_vec_free(StringVec vec)
{
    for (size_t i = 0; i < vec.len; i++) {
        free(vec.strings[i]);
    }
    free(vec.strings);
}
