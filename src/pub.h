#ifndef PUB_H
#define PUB_H

#include <stdio.h>

void init_header();

void tags_append(char*);

void id_add(char*);

void title_add(char*);

void author_date_add(char*);

void category_add(char*);

FILE* header_print();

void free_header();

#endif // PUB_H
