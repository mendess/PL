#ifndef PUB_H
#define PUB_H

#include <stdio.h>

void pub_init();

void pub_tags_append(char*);

void pub_id_add(char*);

void pub_title_append(char*);

void pub_author_date_add(char*);

void pub_category_add(char*);

FILE* pub_header_print();

void pub_clear();

const char* pub_id();

#endif // PUB_H
