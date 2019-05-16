%{
#include "y.tab.h"
#include <ctype.h>
#define __GNU_SOURCE
#include <string.h>
char* strdup(const char*);
%}

%%
[^\n %;,"\[\]:|][^%;,"\[\]:|]*[^\n %;,"\[\]:|]  { yylval.str = strdup(yytext); return ID; }
[%;,"\[\]:|]                                    { return yytext[0]; }
.|\n                                            { ; }
%%

int yywrap() { return 1; }
