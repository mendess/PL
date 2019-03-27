#ifndef NEWSPAPER_H
#define NEWSPAPER_H

#include <stddef.h>

typedef struct _pub {
    int post_id;
    char* title;
    char** tags;
    size_t tags_num;
} Pub;

void newspaper_init();

Pub newspaper_new_pub(int post_id, const char*, char**, size_t);

void newspaper_add_publication(Pub pub);

void newspaper_flush();

#endif // NEWSPAPER_H
