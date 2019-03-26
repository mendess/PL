#ifndef INDEX_H
#define INDEX_H
void index_init();
void index_add_entry(int entry, char* title);
char* index_lookup(int post_id);
void index_flush();
#endif // INDEX_H
