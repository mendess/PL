%option noyywrap

%{
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "pub.h"
#include "newspaper.h"
int has_category = 0;
#define END_PUB (BEGIN INITIAL, pub_clear())
%}
SEP  [ \t]
D    [0-9]
%x HEADER
%x DATE
%x TEXT
%x TEXTHEADER
%%
"<pub>"\n*                      { BEGIN HEADER; pub_init(); has_category = 0; }
<HEADER>"tag:{"[^}]+"}"         { yytext[yyleng - 1] = '\0'; pub_tags_append(yytext + 5); }
<HEADER>"#ID:"{SEP}*\{post-{D}+ {
                                    char* id = yytext;
                                    while(*(id++) != '{'); // skip to 'post'
                                    pub_id_add(id);
                                }
<HEADER>^[^\n#<][^{\n#]+\n      {
                                    yytext[yyleng - 1] = '\0';
                                    has_category++ ?
                                    pub_title_append(yytext)
                                    : pub_category_add(yytext);
                                }
<HEADER>##+\n                   { if(pub_ok()) { BEGIN DATE; } else { END_PUB; }}
<HEADER>.|\n                    { ; }
<DATE>"#DATE:".*\n              {
                                    yytext[yyleng - 1] = '\0';
                                    char* date = yytext + strlen("#DATE:");
                                    while(*(date++) != ']'); // skip to after ']'
                                    while(*(++date) == ' '); // skip spaces
                                    pub_author_date_add(date);
                                    if(!pub_header_print()) { END_PUB; }
                                    BEGIN TEXTHEADER;
                                }
<TEXTHEADER>"["[^]]*"]"         { BEGIN TEXT; }
<TEXTHEADER>.|\n                { pub_append_text(yytext); BEGIN TEXT; }
<TEXT>.|\n                      { pub_append_text(yytext); }
<TEXT>"Etiquetas:".*\n          { ; }
<TEXT>"</pub>"\n                {
                                    pub_footer_print();
                                    END_PUB;
                                }
<*>"PARTILHE VIA:"{SEP}*        { ; }
<INITIAL,DATE>.|\n              { ; }
%%

int main(int argc, const char** argv) {
    newspaper_init();
    if(argc == 1) {
        yylex();
        yylex_destroy();
    } else {
        for(int i = 1; i < argc; i++) {
            dup2(open(argv[i], O_RDONLY), 0);
            yylex();
        }
        yylex_destroy();
    }
    newspaper_flush();
    return 0;
}
