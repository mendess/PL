#include "pub.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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

static char* str_dup(const char* s) {
    char * new_s = malloc(sizeof(char) * (strlen(s) + 1));
    strcpy(new_s, s);
    return new_s;
}

void tags_append(char* tag) {
    if (header.tags_capacity == header.tags_num) {
        header.tags_capacity *= 2;
        header.tags = realloc(header.tags, sizeof(char*) * header.tags_capacity);
    }
    header.tags[header.tags_num++] = str_dup(tag);
}

void id_add(char* id) {
    header.id = str_dup(id);
}

void title_add(char* title){
    if(strlen(title) == 0) { return; }
    if(header.title == NULL) {
        header.title = malloc(sizeof(char) * strlen(title) + 1);
        header.title[0] = '\0';
    } else {
        header.title = realloc(header.title, sizeof(char) * (strlen(header.title) + strlen(title)) + 1);
    }
    strcat(header.title, title);
}

void author_date_add(char* author_date){
    header.author_date = str_dup(author_date);
}

void category_add(char* category) {
    header.category = str_dup(category);
}

void init_header() {
    header.id = NULL;
    header.title = NULL;
    header.author_date = NULL;
    header.tags = malloc(sizeof(char*));
    header.tags_capacity = 1;
    header.tags_num = 0;
    header.category = NULL;
}

void free_header() {
    free(header.id);
    free(header.title);
    free(header.author_date);
    for(size_t i = 0; i < header.tags_num; i++) {
        free(header.tags[i]);
    }
    free(header.tags);
    free(header.category);
}

#define BEGIN_TAG_LEN (strlen("<tag>") + 1)
#define END_TAG_LEN (strlen("</tag> ") + 1)
#define TAG_LEN(s) (BEGIN_TAG_LEN + strlen(s) + END_TAG_LEN)

char* tags_to_string() {
    char* tags = NULL;
    size_t tags_len = 0;
    for(size_t i = 0; i < header.tags_num; i++) {
        tags_len += sizeof(char) * TAG_LEN(header.tags[i]);
        tags = realloc(tags, tags_len);
        tags[tags_len - sizeof(char) * TAG_LEN(header.tags[i])] = '\0';
        strcat(strcat(strcat(tags, "<tag>"), header.tags[i]), "</tag> ");
    }
    return tags;
}

#define MAYBE(s) (s ? s : "")

FILE* header_print() {
    if(!header.id) { return NULL; }
    char* tags = tags_to_string();
    char filename[1024];
    sprintf(filename, "noticias/%s.html", header.id);
    FILE* target = fopen(filename, "w");
    fprintf(target,
            "<pub id=\"%s\">\n"
            "  <title>%s</title>\n"
            "  <author_date>%s</author_date>\n"
            "  <tags>\n"
            "    %s\n"
            "  </tags>\n"
            "  <category>%s</category>\n"
            "  <text>\n",
            header.id,
            MAYBE(header.title),
            MAYBE(header.author_date),
            MAYBE(tags),
            MAYBE(header.category)
          );
    free(tags);
    return target;
}

char* header_id() {
    return header.id;
}
