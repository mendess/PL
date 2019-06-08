#include "sati.h"
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
static Sati* SATI = NULL;
static const char* USAGE = "Usage:\n"
                           "sati [ OPTION... ]\n"
                           "\n"
                           "OPTIONS\n"
                           "-i | --input filename  Read from filename instead of standard input.\n"
                           "-o | --output filename Write to filename instead of standard output.\n"
                           "-s | --split           Write each text to a diferent file. Usefull when dealing with a very large input.\n"
                           "-n | --no-header       Don't output the latex header and footer.\n";

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
        snprintf(msg, 1024, "'%s' already defined.", word);
        yyerror(msg);
        exit(1);
    }
    case MEANING_ALREADY_DEFINED:
        yyerror("Meaning already added.");
        exit(1);
    case ENGLISH_NAME_ALREADY_DEFINED:
        yyerror("English name already addded.");
        exit(1);
    case IO_ERROR: {
        char msg[1024];
        snprintf(msg, 1024, "Couldn't open '%s'.", word);
        yyerror(msg);
        exit(1);
    }
    default:
        return;
    }
}

void add_synonym(char* s)
{
    test_errors(sati_add_synonym(SATI, s), s);
    free(s);
}

void add_meaning(char* s)
{
    test_errors(sati_add_meaning(SATI, s), s);
    free(s);
}

void add_english_name(char* s)
{
    test_errors(sati_add_english_name(SATI, s), s);
    free(s);
}

void add_word(char* s)
{
    test_errors(sati_add_word(SATI, s), s);
    free(s);
}

void annotate(char* text)
{
    test_errors(sati_annotate(SATI, "", text), "untitled");
    free(text);
}

void annotate_with_title(char* title, char* text)
{
    test_errors(sati_annotate(SATI, title, text), title);
    free(title);
    free(text);
}

typedef enum outputmode {
    STDOUT,
    SINGLE_FILE,
    SPLIT
} OutputMode;

void redirect_input()
{
    int fd = open(optarg, O_RDONLY);
    if (fd < 1) {
        test_errors(IO_ERROR, optarg);
    }
    dup2(fd, 0);
    close(fd);
}

void load_args(int argc, char* const* argv)
{
    OutputMode out_mode = STDOUT;
    int header = 1;
    struct option long_options[] = {
        { "output", required_argument, (int*)&out_mode, SINGLE_FILE },
        { "input", required_argument, NULL, 'i' },
        { "split", no_argument, (int*)&out_mode, SPLIT },
        { "no-header", no_argument, &header, 0 },
        { "help", no_argument, NULL, 0 },
        { 0, 0, 0, 0 }
    };
    int opt;
    char* output_file = NULL;
    do {
        int option_index = -1;
        opt = getopt_long(argc, argv, "o:si:nh", long_options, &option_index);
        switch (opt) {
        case 'i':
            redirect_input();
            break;
        case 'o':
            out_mode = SINGLE_FILE;
            output_file = optarg;
            break;
        case 's':
            out_mode = SPLIT;
            break;
        case 'n':
            header = 0;
            break;
        case 'h':
            puts(USAGE);
            exit(1);
        case '?':
            exit(1);
        case 0:
            switch (option_index) {
            case 0:
                output_file = optarg;
                break;
            case 1:
                redirect_input();
                break;
            case 4:
                puts(USAGE);
                exit(1);
            }
        }
    } while (opt != -1);
    switch (out_mode) {
    case SINGLE_FILE:
        if (!(SATI = sati_start_with_output(output_file, header)))
            test_errors(IO_ERROR, output_file);
        break;
    case SPLIT:
        if (!(SATI = sati_start_split(header))) {
            test_errors(IO_ERROR, "split mode");
        }
        break;
    case STDOUT:
        if (!(SATI = sati_start(header))) {
            test_errors(IO_ERROR, "Standard output");
        }
        break;
    }
}

int main(int argc, char* const* argv)
{
    load_args(argc, argv);
    yyparse();
    test_errors(sati_end(SATI), "output_file");
    yylex_destroy();
    return 0;
}
