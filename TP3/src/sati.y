%{
int yylex();
void yyerror(char *s);
#define __GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include "sati.h"
extern int yylineno;
void add_synonym(const char*);
void add_meaning(const char*);
void add_english_name(const char*);
void add_word(const char*);
%}
%union { char* str; }
%token <str> ID
%type <str> Wd Synonym Meaning EnglishName Dicl Word Synonyms Text
%%
Sati : Dicl '%' '%' Texts
     ;

Dicl : Word
     | Dicl Word
     ;

Word : Wd ':' Meaning '|' EnglishName '|' '[' Synonyms ']' ';'
     | Wd ':' Meaning '|' EnglishName '|' Synonym ';'
     ;

Synonyms : Synonym ','
         | Synonyms Synonym ','
         | Synonyms Synonym
         ;

Synonym : ID                       { add_synonym($1); }
        ;

Meaning : ID                       { add_meaning($1); }
        ;

EnglishName : ID                   { add_english_name($1); }
            ;

Wd : ID                            { add_word($1); }
   ;

Texts : '"' Text '"'
      | Texts '"' Text '"'
      ;

Text : ID                          { sati_parse_text($1); }
     ;
%%

void test_errors(SATI_ERROR e, const char * word) {
    switch(e) {
        case NO_CURRENT_WORD:
            printf("No current word: line %d\n", yylineno);
            exit(1);
        case WORD_ALREADY_DEFINED:
            printf("'%s' already defined: line %d\n", word, yylineno);
            exit(1);
        case MEANING_ALREADY_DEFINED:
            printf("Meaning already added: line %d\n", yylineno);
            exit(1);
        case ENGLISH_NAME_ALREADY_DEFINED:
            printf("English name already addded: line %d\n", yylineno);
            exit(1);
        default: return;
    }
}

void add_synonym(const char* s) {
    test_errors(sati_add_synonym(s), s);
}

void add_meaning(const char* s) {
    test_errors(sati_add_meaning(s), s);
}

void add_english_name(const char* s) {
    test_errors(sati_add_english_name(s), s);
}

void add_word(const char* s) {
    test_errors(sati_add_word(s), s);
}

#include "lex.yy.c"
void yyerror(char* s) {
    printf("Error: %s\n", s);
}

int main() {
    yyparse();
    sati_dump();
    return 0;
}

