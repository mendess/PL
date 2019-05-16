%{
#include "y.tab.h"
#include <ctype.h>
#define __GNU_SOURCE
#include <string.h>
%}

%%
[^\n %;,"\[\]:|][^%;,"\[\]:|]*[^\n %;,"\[\]:|]  { yylval.str = strdup(yytext); return ID; }
[%;,"\[\]:|]                             { return yytext[0]; }
%%

int yywrap() { return 1; }
