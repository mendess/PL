#ifndef SATI_RS_H
#define SATI_RS_H

/**
 * An enum representing an error code.
 */
typedef enum sati_error {
    OK = 0,                           // No errors
    NO_CURRENT_WORD = 1,              // Tried to add an atribute without setting the current word.
    WORD_ALREADY_DEFINED = 2,         // Tried to add a repeated word.
    MEANING_ALREADY_DEFINED = 3,      // Tried to define a second meaning to the same word.
    ENGLISH_NAME_ALREADY_DEFINED = 4, // Tried to define an English name to the same word.
    IO_ERROR = 5                      // Reading or writing failed.
} SATI_ERROR;

/**
 * The Sati
 */
typedef struct sati Sati;

/**
 * Creates an instance of `Sati`
 *
 * @param wrapped Whether to wrap the output with the latex preamble
 */
Sati* sati_start(int);

/**
 * Creates an instance of `Sati` with output redirected to the passed file
 *
 * @param file The file to redirect the output to. (Non-null)
 * @param wrapped Whether to wrap the output with the latex preamble
 * @returns A instance of `Sati`.
 */
Sati* sati_start_with_output(const char*, int);

/**
 * Creates an instance of `Sati` splitting the output in individual files
 *
 * @param wrapped Whether to wrap the output with the latex preamble
 */
Sati* sati_start_split(int);

/**
 * Adds a word to a `Sati`, this method sets the current word, and add methods
 * called later will affect this same word.
 *
 * @param sati The `Sati` instance.
 * @param word The word to add.
 * @returns An error code.
 */
SATI_ERROR sati_add_word(Sati*, const char* word);

/** Adds a meaning to the current word (see `sati_add_word`)
 *
 * @param sati The `Sati` instance.
 * @param meaning The meaning to add.
 * @returns An error code.
 */
SATI_ERROR sati_add_meaning(Sati*, const char* meaning);

/** Adds an English name to the current word (see `sati_add_word`)
 *
 * @param sati The `Sati` instance.
 * @param english_name The English Name to add.
 * @returns An error code.
 */
SATI_ERROR sati_add_english_name(Sati*, const char* english_name);

/** Adds a synonym to the current word (see `sati_add_word`)
 *
 * @param sati The `Sati` instance.
 * @param synonym The synonym to add.
 * @returns An error code.
 */
SATI_ERROR sati_add_synonym(Sati*, const char* synonym);

/** Prints to stderr the contents of a `Sati` instance.
 *
 * This is meant to be used as a debug mechanism.
 *
 * @param sati The `Sati` instance.
 */
void sati_dump(Sati*);

/** Annotates a text using the passed `Sati`.
 *
 * @param sati The `Sati` instance.
 * @param title The texts title (Empty string or null for untitled).
 * @param text The text to annotate. (Non-null).
 * @returns An error code.
 */
SATI_ERROR sati_annotate(Sati*, const char*, const char*);

/** Free the memory used by the `Sati` instance. Printing any extra latex
 * preamble in case the instance was built with `wrapped` == true.
 *
 * @param sati The `Sati` instance.
 * @returns An error code.
 */
SATI_ERROR sati_end(Sati*);

#endif // SATI_RS_H
