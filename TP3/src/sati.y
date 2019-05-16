%{
int yylex();
void yyerror(char *s);
#define __GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include "sati.h"
%}
%union { char* str; }
%token <str> ID
%type <str> Wd Synonym Meaning EnglishName Dicl Word Synonyms
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

Synonym : ID               { sati_add_synonym($1); }
        ;

Meaning : ID               { sati_add_meaning($1); }
        ;

EnglishName : ID           { sati_add_english_name($1); }
            ;

Wd : ID                    { sati_add_word($1); }
   ;

Texts : '"' Text '"'
      | Texts '"' Text '"'
      ;

Text : ID
     ;
%%


#include "lex.yy.c"
void yyerror(char* s) {
    printf("Error: %s\n", s);
}

int main() {
    sati_new();
    yyparse();
    sati_dump();
    return 0;
}

