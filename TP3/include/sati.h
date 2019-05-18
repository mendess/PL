#ifndef SATI_H
#define SATI_H

int yylex();

void yyerror(char* s);

void add_synonym(const char*);

void add_meaning(const char*);

void add_english_name(const char*);

void add_word(const char*);

void parse_text(const char*, const char*);

#endif // SATI_H
