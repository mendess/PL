#ifndef SATI_RS_H
#define SATI_RS_H

typedef enum sati_error {
    OK = 0,
    NO_CURRENT_WORD = 1,
    WORD_ALREADY_DEFINED = 2,
    MEANING_ALREADY_DEFINED = 3,
    ENGLISH_NAME_ALREADY_DEFINED = 4,
    IO_ERROR = 5
} SATI_ERROR;

SATI_ERROR sati_set_output(const char*);

void sati_set_split();

SATI_ERROR sati_start();

SATI_ERROR sati_add_word(const char* word);

SATI_ERROR sati_add_meaning(const char* meaning);

SATI_ERROR sati_add_english_name(const char* english_name);

SATI_ERROR sati_add_synonym(const char* synonym);

void sati_dump();

SATI_ERROR sati_parse_text(const char*, const char*);

SATI_ERROR sati_end();

#endif // SATI_RS_H
