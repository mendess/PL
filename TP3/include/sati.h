#ifndef SATI_H
#define SATI_H

int yylex();

int yylex_destroy();

void yyerror(char* s);

void add_synonym(char*);

void add_meaning(char*);

void add_english_name(char*);

void add_word(char*);

void annotate(char*);

void annotate_with_title(char*, char*);

#endif // SATI_H
