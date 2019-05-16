#ifndef SATI_H
#define SATI_H

#include "string_vec.h"

typedef enum sati_error {
    OK = 0,
    MEANING_ALREADY_DEFINED = 1,
    ENGLISH_NAME_ALREADY_DEFINED = 2,
} SATI_ERROR;

void sati_new();

void sati_add_word(const char* word);

SATI_ERROR sati_add_meaning(const char* meaning);

SATI_ERROR sati_add_english_name(const char* english_name);

void sati_add_synonym(const char* synonym);

void sati_dump();

void sati_parse_text(const char*);

#endif // SATI_H
