#include "util.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static GHashTable* tags = NULL;

void tags_init()
{
    tags = g_hash_table_new_full(g_str_hash, g_str_equal, free, free);
}

void tags_add(const char* tag)
{
    char* new_tag = str_replace(g_strdup(tag), '\n', ' ');
    int* counter;
    if ((counter = g_hash_table_lookup(tags, new_tag))) {
        (*counter)++;
    } else {
        counter = malloc(sizeof(int));
        *counter = 1;
        g_hash_table_insert(tags, new_tag, counter);
    }
}

void tags_flush()
{
    FILE* f = fopen("noticias/tags.txt", "w");
    GHashTableIter iter;
    void *key, *value;
    g_hash_table_iter_init(&iter, tags);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        fprintf(f, "%d\t%s\n", *(int*)value, (char*)key);
    }
    g_hash_table_destroy(tags);
}
