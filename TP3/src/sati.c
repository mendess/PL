#include "sati.h"

#define __GNU_SOURCE
#include <string.h>

#include <glib.h>

typedef struct word {
    char* wd;
    char* meaning;
    char* english_name;
    StringVec synonyms;
} Word;

typedef struct sati {
    GHashTable* database;
} SATI;

char* strdup(const char*);

static SATI INSTANCE;
static Word* last_word;

static void word_free(Word* word);

void sati_new()
{
    INSTANCE = (SATI) {
        .database = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, (void (*)(void*))word_free), // TODO: Free funcs
    };
}

void sati_add_word(const char* word)
{
    last_word = calloc(sizeof(Word), 1);
    last_word->wd = strdup(word);
    g_hash_table_insert(INSTANCE.database, last_word->wd, last_word);
}

SATI_ERROR sati_add_meaning(const char* meaning)
{
    if (last_word->meaning != NULL) {
        return MEANING_ALREADY_DEFINED;
    }
    last_word->meaning = strdup(meaning);
    return OK;
}

SATI_ERROR sati_add_english_name(const char* english_name)
{
    if (last_word->english_name != NULL) {
        return ENGLISH_NAME_ALREADY_DEFINED;
    }
    last_word->english_name = strdup(english_name);
    return OK;
}

void sati_add_synonym(const char* synonym)
{
    string_vec_add(&last_word->synonyms, synonym);
}

#include <stdio.h>
void sati_dump()
{
    GHashTableIter iter;
    g_hash_table_iter_init(&iter, INSTANCE.database);
    char* word_key;
    Word* word;
    while (g_hash_table_iter_next(&iter, (void*)&word_key, (void*)&word)) {
        printf("%s : %s\n", word_key, word->wd);
        printf("\t| %s\n", word->meaning);
        printf("\t| %s\n", word->english_name);
        printf("\t| [");
        for (size_t i = 0; i < word->synonyms.len; i++) {
            printf(" %s, ", word->synonyms.strings[i]);
        }
        printf("]\n");
        printf("\t;\n");
    }
}

void word_free(Word* word)
{
    free(word->wd);
    free(word->meaning);
    free(word->english_name);
    string_vec_free(word->synonyms);
}
