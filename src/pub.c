#include "pub.h"
#include "config.h"
#include "newspaper.h"
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
    FILE* target;
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
    header.target = NULL;
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
#define MAYBE(s) (s ? s : "")

int pub_header_print()
{
    if (!pub_ok()) {
        return 0;
    }
    char filename[1024];
    sprintf(filename, "noticias/%s.html", header.id);
    header.target = fopen(filename, "w");
    fprintf(header.target,
        "<html>\n"
        "<head>\n"
        "  <meta charset='UTF-8' />\n"
        "</head>\n"
        "<div id=\"%s\">\n"
        "  <title>%s</title>\n"
        "  <h1 align='center'>%s</h1>\n"
        "  <h2 align='center'>%s</h2>\n",
        header.id,
        MAYBE(header.title),
        MAYBE(header.title),
        MAYBE(header.author_date));
    fprintf(header.target,
        "  <h4 align='center'>Categoria: %s</h3>\n"
        "  <p>\n",
        MAYBE(header.category));
    return 1;
}

void pub_append_text(const char* text)
{
    if (header.target)
        fprintf(header.target, "%s", text);
}

void pub_footer_print()
{
    if (!header.target)
        return;
    fprintf(header.target,
        "  </p>\n"
        " </div>\n"
        " <div align='center'><a href='index.html'>De volta para o indice</a></div>\n"
        "</body>\n");
    if (header.tags_num != 0) {
        fprintf(header.target, "<footer><p align='center'><font color='grey'><i>\n");
        for (size_t i = 0; i < header.tags_num - 1; i++) {
            fprintf(
                header.target,
                "<a href='" TAGS_FOLDER "%s.html'>%s</a> | ",
                str_replace(header.tags[i], '/', '-'),
                header.tags[i]);
        }
        fprintf(
            header.target,
            "<a href='" TAGS_FOLDER "%s.html'>%s</a>\n",
            str_replace(header.tags[header.tags_num - 1], '/', '-'),
            header.tags[header.tags_num - 1]);
        fprintf(header.target, "</font></i></p></footer>\n");
    }
    fprintf(header.target, "</html>");
    fclose(header.target);
    newspaper_add_publication(
        newspaper_new_pub(
            atoi(header.id + 5),
            header.title,
            header.tags,
            header.tags_num));
}

int pub_ok()
{
    return header.id != NULL;
}
