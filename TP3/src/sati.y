%{
#include "sati.h"
#include "lex.yy.c"
%}
%union { char* str; }
%token <str> ID
%token <str> TEXT
%type <str> Wd Synonym Meaning EnglishName Dicl Word Synonyms Text Title
%%
Sati : Dicl '%' Texts
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

Wd : ID                              { add_word($1); }
   ;

Meaning : ID                         { add_meaning($1); }
        ;

EnglishName : ID                     { add_english_name($1); }
            ;

Synonym : ID                         { add_synonym($1); }
        ;


Texts : '"' Text '"'                 { parse_text($2); }
      | Title '"' Text '"'           { parse_text_with_title($1, $3); }
      | Texts '"' Text '"'           { parse_text($3); }
      | Texts Title '"' Text '"'     { parse_text_with_title($2, $4); }
      ;

Text : TEXT                          { $$=$1; }
     ;

Title : TEXT                         { $$=$1; }
      ;
%%
