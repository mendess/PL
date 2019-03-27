#include "newspaper.h"
#include "config.h"
#include "util.h"
#include <glib.h>
#include <stdio.h>
#include <string.h>

// Index functions
static void index_init();
static void index_add_pub(Pub*);
static Pub* index_lookup(int post_id);
static void index_flush();
static void index_delete();
// Tags functions
static void tags_init();
static void tags_add_pub(Pub*);
static void tags_flush();
static void tags_delete();

// Post id <-> Post title
static GTree* index = NULL; //Tree<int, pub>

// Tag <-> [Post id]
static GTree* tags = NULL; // HashTable<string, HashSet<int>>

void newspaper_init()
{
    index_init();
    tags_init();
}

Pub newspaper_new_pub(int post_id, const char* title, char** tags, size_t tags_num)
{
    char** new_tags = malloc(sizeof(char*) * tags_num);
    for (size_t i = 0; i < tags_num; i++)
        new_tags[i] = str_replace(g_strdup(tags[i]), '/', '-');
    return (Pub){
        .post_id = post_id,
        .title = g_strdup(title),
        .tags = new_tags,
        .tags_num = tags_num
    };
}

void newspaper_add_publication(Pub pub)
{
    Pub* entry_p = malloc(sizeof(struct _pub));
    memcpy(entry_p, &pub, sizeof(struct _pub));
    index_add_pub(entry_p);
    tags_add_pub(entry_p);
}

void newspaper_flush()
{
    index_flush();
    tags_flush();
    tags_delete();
    index_delete();
}

static int vintcmp(const void* a, const void* b, void* data)
{
    (void)data;
    int av = *(int*)a;
    int bv = *(int*)b;
    return av- bv; // (av < bv) ? -1 : (av == bv) ? 0 : 1;
}

static void pub_delete(void* pub)
{
    Pub* p = (Pub*)pub;
    for (size_t i = 0; i < p->tags_num; i++)
        free(p->tags[i]);
    free(p->tags);
    free(p->title);
    free(p);
}

void index_init()
{
    index = g_tree_new_full(vintcmp, NULL, NULL, pub_delete);
}

void index_add_pub(Pub* pub)
{
    g_tree_replace(index, &(pub->post_id), pub);
}

static int flush_entry(void* key, void* value, void* file)
{
    FILE* f = (FILE*)file;
    Pub* pub = (Pub*)value;
    fprintf(
        f,
        "    <tr><td><a href='post-%d.html'>%s</a></td>",
        *(int*)key,
        pub->title ? pub->title : "Sem titulo");
    for (size_t i = 0; i < pub->tags_num; i++) {
        fprintf(
            f,
            "    <td><a href='" TAGS_FOLDER "%s.html'>%s</a></td>",
            pub->tags[i],
            pub->tags[i]);
    }
    fprintf(f, "</tr>\n");
    return 0;
}

Pub* index_lookup(int post_id)
{
    return g_tree_lookup(index, &post_id);
}

void index_flush()
{
    FILE* index_file = fopen(NEWS_FOLDER "index.html", "w");
    fprintf(index_file,
        "<html>\n"
        "<head>\n"
        "  <meta charset='UTF-8'/>\n"
        "</head>\n"
        "<body>\n"
        "  <table>\n"
        "    <tr><th>Titulo</th><th>Tags</th></tr>\n");
    g_tree_foreach(index, flush_entry, index_file);
    fprintf(index_file,
        "  </table>\n"
        "</body>\n"
        "</html>");
    fclose(index_file);
}

void index_delete()
{
    g_tree_destroy(index);
}

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

void tags_add_pub(Pub* pub)
{
    for (size_t i = 0; i < pub->tags_num; i++) {
        char* tag = pub->tags[i];
        GHashTable* post_table = g_tree_lookup(tags, tag);
        if (!post_table) {
            post_table = g_hash_table_new_full(g_int_hash, g_int_equal, free, NULL);
            g_tree_insert(tags, g_strdup(tag), post_table);
        }
        int * post_id = malloc(sizeof(int));
        *post_id = pub->post_id;
        g_hash_table_add(post_table, post_id);
    }
}

// FLUSHING A TAGS POSTS
static void flush_post(void* /*int*/ post, void* _value, void* /*FILE**/ tag_posts)
{
    (void)_value;
    FILE* f = (FILE*)tag_posts;
    int post_id = *(int*)post;
    Pub* pub = index_lookup(post_id);
    fprintf(
        f,
        "      <li><a href='post-%d.html'>%s</a></li>\n",
        post_id,
        pub->title ? pub->title : "Sem Titulo");
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
}

void tags_delete()
{
    g_tree_destroy(tags);
}
