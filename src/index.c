#include <glib.h>
#include <stdio.h>

GTree* index = NULL;

static int vintcmp(const void* a, const void* b, void* data)
{
    (void)data;
    int av = *(int*)a;
    int bv = *(int*)b;
    return av - bv;
}

void index_init()
{
    index = g_tree_new_full(vintcmp, NULL, free, free);
}

void index_add_entry(int entry, char* title)
{
    int* entry_p = malloc(sizeof(int));
    *entry_p = entry;
    g_tree_insert(index, entry_p, g_strdup(title));
}

static int add_entry(void* key, void* value, void* file)
{
    FILE* f = (FILE*)file;
    fprintf(f, "      <li><a href='post-%d.html'>%s</a></li>\n", *(int*)key, (char*)value);
    return 0;
}

void index_flush()
{
    FILE* index_file = fopen("noticias/index.html", "w");
    fprintf(index_file,
            "<html>\n"
            "<head>\n"
            "  <meta charset='UTF-8'/>\n"
            "</head>\n"
            "<body>\n"
            "  <ul>\n"
            );
    g_tree_foreach(index, add_entry, index_file);
    fprintf(index_file,
            "  </ul>\n"
            "</body>\n"
            "</html>"
            );
    fclose(index_file);
}
