#ifndef PUB_H
#define PUB_H

#include <stdio.h>

void pub_init();

void pub_tags_append(const char*);

void pub_id_add(const char*);

void pub_title_append(const char*);

void pub_author_date_add(const char*);

void pub_category_add(const char*);

FILE* pub_header_print();

void pub_clear();

const char* pub_id();

#endif // PUB_H
