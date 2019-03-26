#include "config.h"
#include "index.h"
#include "util.h"
#include <glib.h>
#include <stdio.h>

static GTree* tags = NULL; // HashTable<string, HashSet<int>>

static void destroy_wrapper(void* v) { g_hash_table_destroy((GHashTable*)v); }

static int vstrcmp(const void* a, const void* b, void* _data)
{
    (void)_data;
    return strcmp((char*)a, (char*)b);
}

void tags_init()
{
    tags = g_tree_new_full(vstrcmp, NULL, free, destroy_wrapper);
}

void tags_add(int post_id, const char* tag)
{
    GHashTable* post_table = g_tree_lookup(tags, tag);
    if (!post_table) {
        post_table = g_hash_table_new_full(g_int_hash, g_int_equal, free, NULL);
        g_tree_insert(tags, g_strdup(tag), post_table);
    }
    int* post_id_ptr = malloc(sizeof(int));
    *post_id_ptr = post_id;
    g_hash_table_add(post_table, post_id_ptr);
}

// FLUSHING A TAGS POSTS
static void flush_post(void* /*int*/ post, void* _value, void* /*FILE**/ tag_posts)
{
    (void)_value;
    FILE* f = (FILE*)tag_posts;
    int post_id = *(int*)post;
    char* title = index_lookup(post_id);
    fprintf(
        f,
        "      <li><a href='post-%d.html'>%s</a></li>\n",
        post_id,
        title ? title : "Sem Titulo");
}

static void flush_posts(GHashTable* posts, char* tag)
{
    str_replace(tag, '/', '-');
    char filename[1024];
    sprintf(filename, NEWS_FOLDER TAGS_FOLDER "%s.html", (char*)tag);
    FILE* tag_file = fopen(filename, "w");
    if (!tag_file) {
        fprintf(stderr, "Failed to open file: %s\n", filename);
        return;
    }
    fprintf(tag_file,
        "<html>\n"
        "<head>\n"
        "  <meta charset='UTF-8'/>\n"
        "</head>\n"
        "<body>\n"
        "  <ul>\n");
    g_hash_table_foreach(posts, flush_post, tag_file);
    fprintf(tag_file,
        "  </ul>\n"
        "</body>\n"
        "</html>");
    fclose(tag_file);
}

// FLUSHING THE TAG FILES
static int flush_tag(void* /*string*/ tag, void* /*GHashTable*/ posts, void* file)
{
    FILE* f = (FILE*)file;
    flush_posts((GHashTable*)posts, (char*)tag);
    fprintf(f, "      <li><a href='" TAGS_FOLDER "%s.html'>%s</a></li>\n", (char*)tag, (char*)tag);
    return 0;
}

void tags_flush()
{
    FILE* tags_file = fopen(NEWS_FOLDER "tags.html", "w");
    fprintf(tags_file,
        "<html>\n"
        "<head>\n"
        "  <meta charset='UTF-8'/>\n"
        "</head>\n"
        "<body>\n"
        "  <ul>\n");
    g_tree_foreach(tags, flush_tag, tags_file);
    fprintf(tags_file,
        "  </ul>\n"
        "</body>\n"
        "</html>");
    fclose(tags_file);
    g_tree_destroy(tags);
}
