%{
int yylex();
void yyerror(char *s);
#define __GNU_SOURCE
#include "sati.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define BOLD "\033[1m"
#define RED "\033[31m"
#define CLEAR "\033[0m"
extern int yylineno;
void add_synonym(const char*);
void add_meaning(const char*);
void add_english_name(const char*);
void add_word(const char*);
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

Synonym : ID                       { add_synonym($1); }
        ;

Meaning : ID                       { add_meaning($1); }
        ;

EnglishName : ID                   { add_english_name($1); }
            ;

Wd : ID                            { add_word($1); }
   ;

Texts : '"' Text '"'               { sati_parse_text("", $2); }
      | Title '"' Text '"'         { sati_parse_text($1, $3); }
      | Texts '"' Text '"'         { sati_parse_text("", $3); }
      | Texts Title '"' Text '"'   { sati_parse_text($2, $4); }
      ;

Text : TEXT                          { $$=$1; }
     ;

Title : TEXT                         { $$=$1; }
      ;
%%

#include "lex.yy.c"
void yyerror(char* s) {
    fprintf(stderr, BOLD RED"error" CLEAR BOLD" on line %d:"CLEAR" %s\n", yylineno, s);
}


void test_errors(SATI_ERROR e, const char * word) {
    switch(e) {
        case NO_CURRENT_WORD:
            yyerror("No current word");
            exit(1);
        case WORD_ALREADY_DEFINED: {
            char msg[1024];
            snprintf(msg, 1024, "'%s' already defined", word);
            yyerror(msg);
            exit(1);
        }
        case MEANING_ALREADY_DEFINED:
            yyerror("Meaning already added");
            exit(1);
        case ENGLISH_NAME_ALREADY_DEFINED:
            yyerror("English name already addded");
            exit(1);
        case FILE_NOT_FOUND: {
            char msg[1024];
            snprintf(msg, 1024, "Couldn't open '%s' for writing", word);
            yyerror(msg);
            exit(1);
        }

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

int main(int argc, char** argv) {
    if(argc > 1) {
        test_errors(sati_set_output(argv[1]), argv[1]);
    }
    sati_start();
    yyparse();
    sati_end();
    return 0;
}

