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

typedef struct sati Sati;

Sati* sati_start_with_output(const char*);

Sati* sati_start_split();

Sati* sati_start();

SATI_ERROR sati_add_word(Sati*, const char* word);

SATI_ERROR sati_add_meaning(Sati*, const char* meaning);

SATI_ERROR sati_add_english_name(Sati*, const char* english_name);

SATI_ERROR sati_add_synonym(Sati*, const char* synonym);

void sati_dump(Sati*);

SATI_ERROR sati_parse_text(Sati*, const char*, const char*);

SATI_ERROR sati_end(Sati*);

#endif // SATI_RS_H
