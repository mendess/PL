#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static GHashTable* tags = NULL;

void tags_init()
{
    tags = g_hash_table_new_full(g_str_hash, g_str_equal, free, NULL);
}

void tags_add(char* tag)
{
    g_hash_table_add(tags, g_strdup(tag));
}

void tags_flush()
{
    FILE* f = fopen("noticias/tags.txt", "w");
    GHashTableIter iter;
    gpointer key;
    g_hash_table_iter_init(&iter, tags);
    while (g_hash_table_iter_next(&iter, &key, NULL)) {
        fprintf(f, "%s\n", (char*)key);
    }
    g_hash_table_destroy(tags);
}
