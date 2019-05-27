%{
#include "y.tab.h"
#define __GNU_SOURCE
#include <string.h>
char* strdup(const char*);
%}
%option yylineno
%x TEXTS
ID_SEP [^\n %;,"\[\]:|]
TEXT_SEP [^\n "]
%%
{ID_SEP}[^%;,"\[\]:|]*{ID_SEP}   { yylval.str = strdup(yytext); return ID; }
(%)+                             { BEGIN TEXTS; return yytext[0]; }
<TEXTS>{TEXT_SEP}[^"]*{TEXT_SEP} { yylval.str = strdup(yytext); return TEXT; }
<*>[%;,"\[\]:|]                  { return yytext[0]; }
<*>.|\n                          { ; }
%%

int yywrap() { return 1; }
