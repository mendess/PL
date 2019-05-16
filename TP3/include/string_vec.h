#ifndef STRING_VEC_H
#define STRING_VEC_H

#include <stdlib.h>

typedef struct string_vec {
    char** strings;
    size_t len;
    size_t size;
} StringVec;

void string_vec_add(StringVec*, const char*);
void string_vec_free(StringVec vec);

#endif //STRING_VEC_H
