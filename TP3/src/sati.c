#include "sati_rs.h"
#include "y.tab.h"

#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

extern int yylineno;

void yyerror(char* s)
{
#define BOLD "\033[1m"
#define RED "\033[31m"
#define CLEAR "\033[0m"
    fprintf(stderr, BOLD RED "error" CLEAR BOLD " on line %d:" CLEAR " %s\n", yylineno, s);
}

void test_errors(SATI_ERROR e, const char* word)
{
    switch (e) {
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
    case IO_ERROR: {
        char msg[1024];
        snprintf(msg, 1024, "Couldn't open '%s' for writing", word);
        yyerror(msg);
        exit(1);
    }
    default:
        return;
    }
}

void add_synonym(char* s)
{
    test_errors(sati_add_synonym(s), s);
    free(s);
}

void add_meaning(char* s)
{
    test_errors(sati_add_meaning(s), s);
    free(s);
}

void add_english_name(char* s)
{
    test_errors(sati_add_english_name(s), s);
    free(s);
}

void add_word(char* s)
{
    test_errors(sati_add_word(s), s);
    free(s);
}

void parse_text(char* title, char* text)
{
    test_errors(sati_parse_text(title, text), title);
    free(title);
    free(text);
}

typedef enum outputmode {
    STDOUT,
    SINGLE_FILE,
    SPLIT
} OutputMode;

void load_args(int argc, char* const* argv)
{
    OutputMode out_mode = STDOUT;
    struct option long_options[] = {
        { "output", required_argument, (int*)&out_mode, SINGLE_FILE },
        { "input", required_argument, NULL, 'i' },
        { "split", no_argument, (int*)&out_mode, SPLIT },
        { 0, 0, 0, 0 }
    };

    int option_index = 0;
    int opt;
    char* output_file = NULL;
    do {
        opt = getopt_long(argc, argv, "o:si:", long_options, &option_index);
        switch (opt) {
        case 'i': {
            int fd = open(optarg, O_RDONLY);
            if (fd < 1) {
                test_errors(IO_ERROR, optarg);
            }
            dup2(fd, 0);
            close(fd);
            break;
        }
        case 0:
        case 'o':
            out_mode = SINGLE_FILE;
            output_file = optarg;
            break;
        case 's':
            out_mode = SPLIT;
            break;
        case '?':
            exit(1);
            break;
        }
    } while (opt != -1);
    switch (out_mode) {
    case SINGLE_FILE:
        test_errors(sati_set_output(output_file), output_file);
        break;
    case SPLIT:
        sati_set_split();
        break;
    default:
        break;
    }
}

int main(int argc, char* const* argv)
{
    load_args(argc, argv);
    test_errors(sati_start(), "output_file");
    yyparse();
    test_errors(sati_end(), "output_file");
    yylex_destroy();
    return 0;
}
