%option noyywrap

%{
#include <stdio.h>
#include <string.h>
#include "pub.h"
#include "tags.h"
#include "index.h"
int has_category = 0;
FILE* target = NULL;
#define END_PUB (BEGIN INITIAL, pub_clear())
%}
SEP  [ \t]
D    [0-9]
AN   [0-9A-Za-zÀ-ÖØ-öø-ÿ\-]
ANS  [0-9A-Za-zÀ-ÖØ-öø-ÿ\- ]
%x HEADER
%x TEXT
%%
"<pub>"\n*                      { BEGIN HEADER; pub_init(); has_category = 0; target = NULL; }
<HEADER>"tag:{"[^}]+"}"         { yytext[yyleng - 1] = '\0'; pub_tags_append(yytext + 5); }
<HEADER>"#ID:"{SEP}*\{post-{D}+ {
                                    char* id = yytext;
                                    while(*(id++) != '{'); // skip to 'post'
                                    pub_id_add(id);
                                }
<HEADER>{AN}{ANS}+\n            {
                                    yytext[yyleng - 1] = '\0';
                                    has_category++ ? pub_title_append(yytext) : pub_category_add(yytext);
                                }
<HEADER>━+\n                    { if(pub_id()) { BEGIN TEXT; } else { END_PUB; }}
<HEADER>.|\n                    { ; }
<TEXT>"#DATE:".*\n              {
                                    yytext[yyleng - 1] = '\0';
                                    char* date = yytext + strlen("#DATE:");
                                    while(*(date++) != ']'); // skip to after ']'
                                    while(*(++date) == ' '); // skip spaces
                                    pub_author_date_add(date);
                                    target = pub_header_print();
                                    if(!target) { END_PUB; }
                                }
<TEXT>.|\n                      { if(target) { fprintf(target, "%s", yytext); } }
<TEXT>"Etiquetas:".*\n          { ; }
<TEXT>"</pub>"\n                {
                                    if(target) {
                                        fprintf(target, "  </text>\n</pub>\n");
                                        fclose(target);
                                    }
                                    END_PUB;
                                }
<*>"PARTILHE VIA:"{SEP}*        { ; }
.|\n                            { ; }
%%

int main() {
    tags_init();
    index_init();
    yylex();
    yylex_destroy();
    index_flush();
    tags_flush();
    return 0;
}
