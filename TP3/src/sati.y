%{
int yylex();
void yyerror(char *s);
#define __GNU_SOURCE
#include <stdio.h>
#include <string.h>
%}
%union { char* str; }
%token <str> ID
%type <str> Wd Synonym Meaning EnglishName Text Dicl Texts Word
%%
Sati : Dicl '%' '%' Texts { printf("%%%%\n"); }
     ;

Dicl : Word          { $$=$1; printf("Dicl: Word (%s)\n", $$); }
     | Dicl Word     { asprintf(&$$, "%s%s", $1, $2); printf("Dicl | Dicl Word: (%s)\n", $$); }
     ;

Word : Wd ':' Meaning '|' EnglishName '|' '[' Synonyms ']' ';'
     | Wd ':' Meaning '|' EnglishName '|' Synonym ';'  { asprintf(&$$, "Word: %s | %s | %s | %s ;", $1, $3, $5, $7); printf("%s\n", $$); }
     ;

Synonyms : Synonym ','
         | Synonyms Synonym ','
         | Synonyms Synonym
         ;

Synonym : ID                  { printf("Synonym: (%s)\n", $1); $$=$1; }
        ;

Meaning : ID                  { printf("Meaning: (%s)\n", $1); $$=$1; }
        ;

EnglishName : ID              { printf("EnglishName: (%s)\n", $1); $$=$1; }
            ;

Wd : ID                       { printf("Wd: (%s)\n", $1); $$=$1; }
   ;


Texts : '"' Text '"'          { $$=$2; }
      | Texts '"' Text '"'    { asprintf(&$$, "%s%s", $1, $3); }
      ;

Text : ID                     { printf("Text: (%s)\n", $1); $$=$1; }
     ;
%%


#include "lex.yy.c"
void yyerror(char* s) {
    printf("Error: %s\n", s);
}

int main() {
    yyparse();
    return 0;
}

