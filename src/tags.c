#include <glib.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static GHashTable* tags = NULL;

static int vstrcmp(const void* a, const void* b) {
    return strcmp((char*)a, (char*)b);
}

void tags_init() {
    tags = g_hash_table_new_full(g_str_hash, vstrcmp, free, NULL);
}

void tags_add(char* tag) {
    g_hash_table_add(tags, g_strdup(tag));
}

void tags_flush() {
    FILE* f = fopen("tags.txt", "w");
    GHashTableIter iter;
    gpointer key;
    g_hash_table_iter_init(&iter, tags);
    while(g_hash_table_iter_next(&iter, &key, NULL)) {
        fprintf(f, "%s\n", (char*)key);
    }
    g_hash_table_destroy(tags);
}
