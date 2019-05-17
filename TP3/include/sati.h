#ifndef SATI_H
#define SATI_H

typedef enum sati_error {
    OK = 0,
    NO_CURRENT_WORD = 1,
    WORD_ALREADY_DEFINED = 2,
    MEANING_ALREADY_DEFINED = 3,
    ENGLISH_NAME_ALREADY_DEFINED = 4,
} SATI_ERROR;

SATI_ERROR sati_add_word(const char* word);

SATI_ERROR sati_add_meaning(const char* meaning);

SATI_ERROR sati_add_english_name(const char* english_name);

SATI_ERROR sati_add_synonym(const char* synonym);

void sati_dump();

void sati_parse_text(const char*);

#endif // SATI_H
