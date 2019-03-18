#include "pub.h"
#include "tags.h"
#include "util.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Pub {
    char* id;
    char* title;
    char* author_date;
    char** tags;
    size_t tags_capacity;
    size_t tags_num;
    char* category;
};

static struct Pub header;

static char* str_dup(const char* s)
{
    char* new_s = malloc(sizeof(char) * (strlen(s) + 1));
    strcpy(new_s, s);
    return new_s;
}

void pub_tags_append(const char* tag)
{
    if (header.tags_capacity == header.tags_num) {
        header.tags_capacity *= 2;
        header.tags = realloc(header.tags, sizeof(char*) * header.tags_capacity);
    }
    header.tags[header.tags_num++] = str_replace(str_dup(tag), '\n', ' ');
}

void pub_id_add(const char* id)
{
    header.id = str_dup(id);
}

void pub_title_append(const char* title)
{
    if (strlen(title) == 0) {
        return;
    }
    if (header.title == NULL) {
        header.title = malloc(sizeof(char) * strlen(title) + 1);
        header.title[0] = '\0';
    } else {
        header.title = realloc(header.title, sizeof(char) * (strlen(header.title) + strlen(title)) + 1);
    }
    strcat(header.title, title);
}

void pub_author_date_add(const char* author_date)
{
    header.author_date = str_dup(author_date);
}

void pub_category_add(const char* category)
{
    header.category = str_dup(category);
}

void pub_init()
{
    header.id = NULL;
    header.title = NULL;
    header.author_date = NULL;
    header.tags = malloc(sizeof(char*));
    header.tags_capacity = 1;
    header.tags_num = 0;
    header.category = NULL;
}

void pub_clear()
{
    free(header.id);
    free(header.title);
    free(header.author_date);
    for (size_t i = 0; i < header.tags_num; i++) {
        free(header.tags[i]);
    }
    free(header.tags);
    free(header.category);
}

#define BEGIN_TAG_LEN (strlen("<tag>"))
#define END_TAG_LEN (strlen("</tag> ") + 1)
#define TAG_LEN(s) (BEGIN_TAG_LEN + strlen(s) + END_TAG_LEN)

char* tags_to_string()
{
    char* tags = NULL;
    size_t tags_len = 0;
    const int post_id = atoi(header.id + 5);
    for (size_t i = 0; i < header.tags_num; i++) {
        tags_len += sizeof(char) * TAG_LEN(header.tags[i]);
        tags = realloc(tags, tags_len);
        tags[tags_len - sizeof(char) * TAG_LEN(header.tags[i])] = '\0';
        strcat(strcat(strcat(tags, "<tag>"), header.tags[i]), "</tag> ");
        tags_add(post_id, header.tags[i]);
    }
    if (tags) {
        size_t i;
        for (i = strlen(tags) - 1; tags[i] != '>'; --i)
            ;
        tags[i + 1] = '\0';
    }
    return tags;
}

#define MAYBE(s) (s ? s : "")

FILE* pub_header_print()
{
    if (!header.id) {
        return NULL;
    }
    char* tags = tags_to_string();
    char filename[1024];
    sprintf(filename, "noticias/%s.html", header.id);
    FILE* target = fopen(filename, "w");
    fprintf(target,
        "<pub id=\"%s\">\n"
        "  <title>%s</title>\n"
        "  <author_date>%s</author_date>\n",
        header.id,
        MAYBE(header.title),
        MAYBE(header.author_date));
    if (tags) {
        fprintf(target,
            "  <tags>\n"
            "    %s\n"
            "  </tags>\n",
            tags);
    }
    fprintf(target,
        "  <category>%s</category>\n"
        "  <text>\n",
        MAYBE(header.category));
    free(tags);
    return target;
}

const char* pub_id()
{
    return header.id;
}
