%option noyywrap
%option stack

%{
#include <stdio.h>
#include <string.h>
#include "pub.h"
int has_category = 0;
FILE* target = NULL;
%}
SEP  [ \t]
D    [0-9]
AN   [0-9A-Za-zÀ-ÖØ-öø-ÿ\-]
ANS  [0-9A-Za-zÀ-ÖØ-öø-ÿ\- ]
%x HEADER
%x TEXT
%%
"<pub>"\n*                      { BEGIN HEADER; init_header(); has_category = 0; }
<HEADER>"tag:{"[^}]+"}"         { yytext[yyleng - 1] = '\0'; tags_append(yytext + 5); }
<HEADER>"#ID:"{SEP}*\{post-{D}+ {
                                    char* id = yytext;
                                    while(*(id++) != '{');
                                    id_add(id);
                                }
<HEADER>{AN}{ANS}+\n            {
                                    yytext[yyleng - 1] = '\0';
                                    has_category++ ? title_add(yytext) : category_add(yytext);
                                }
<HEADER>━+\n                    { BEGIN TEXT; }
<HEADER>.|\n                    { ; }
<TEXT>"#DATE:".*\n              {
                                    yytext[yyleng - 1] = '\0';
                                    char* date = yytext + strlen("#DATE:");
                                    while(*(date++) != ']');
                                    while(*(++date) == ' ');
                                    author_date_add(date);
                                    target = header_print();
                                    free_header();
                                }
<TEXT>.|\n                      { if(target) fprintf(target, "%s", yytext); }
<TEXT>"Etiquetas:".*\n          { ; }
<TEXT>"</pub>"\n                { fprintf(target, "  </text>\n</pub>\n"); fclose(target); BEGIN INITIAL; }
<*>"PARTILHE VIA:"{SEP}*        { ; }
.|\n                            { ; }
%%

int main() {
    /* printf("<html><body>\n"); */
    yylex();
    /* printf("</body></html>\n"); */
    return 0;
}
