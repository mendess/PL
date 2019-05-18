%{
#include "y.tab.h"
#include <ctype.h>
#define __GNU_SOURCE
#include <string.h>
char* strdup(const char*);
%}
%option yylineno
%x TEXTS
%%
[^\n %;,"\[\]:|][^%;,"\[\]:|]*[^\n %;,"\[\]:|]  { yylval.str = strdup(yytext); return ID; }
(%)+                                            { BEGIN TEXTS; return yytext[0]; }
<TEXTS>[^\n "][^"]*[^\n "]                      { yylval.str = strdup(yytext); return TEXT; }
<*>[%;,"\[\]:|]                                 { return yytext[0]; }
<*>.|\n                                         { ; }
%%

int yywrap() { return 1; }
