#include "util.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static GHashTable* tags = NULL; // HashTable<int, HashSet<string>>

static void g_hash_table_destroy_wrapper(void* v) { g_hash_table_destroy((GHashTable*)v); }

void tags_init()
{
    tags = g_hash_table_new_full(g_int_hash, g_int_equal, free, g_hash_table_destroy_wrapper);
}

void tags_add(int post_id, const char* tag)
{
    char * new_tag = g_strdup(tag);
    GHashTable* post_table = g_hash_table_lookup(tags, &post_id);
    if (!post_table) {
        post_table = g_hash_table_new_full(g_str_hash, g_str_equal, free, NULL);
        int* id = malloc(sizeof(int));
        *id = post_id;
        g_hash_table_insert(tags, id, post_table);
    }
    g_hash_table_add(post_table, new_tag);
}

void tags_flush()
{
    FILE* f = fopen("noticias/tags.txt", "w");
    GHashTable* tag_counts = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, free); // HashTable<&str, int>

    GHashTableIter tags_iter;
    GHashTable* post_tags;
    g_hash_table_iter_init(&tags_iter, tags);
    while (g_hash_table_iter_next(&tags_iter, NULL, (void**)&post_tags)) { // for post_tags in tags
        GHashTableIter post_tags_iter;
        g_hash_table_iter_init(&post_tags_iter, post_tags);
        char* tag;
        while (g_hash_table_iter_next(&post_tags_iter, (void*)&tag, NULL)) { // for tag in post_tags
            int* counter;
            if ((counter = g_hash_table_lookup(tag_counts, tag))) { // tags_count.entry(tag).and_modify(++).or_insert(1)
                (*counter)++;
            } else {
                counter = malloc(sizeof(int));
                *counter = 1;
                g_hash_table_insert(tag_counts, tag, counter);
            }
        }
    }
    GHashTableIter tag_counts_iter;
    char* tag;
    int* count;
    g_hash_table_iter_init(&tag_counts_iter, tag_counts);
    while (g_hash_table_iter_next(&tag_counts_iter, (void**)&tag, (void**)&count)) {
        fprintf(f, "%d\t%s\n", *count, tag);
    }
    fclose(f);
    g_hash_table_destroy(tag_counts);
    g_hash_table_destroy(tags);
}
