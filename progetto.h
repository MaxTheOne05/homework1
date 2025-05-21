#ifndef FUNZIONI_H
#define FUNZIONI_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

//Utilizzata per capire quali file sono gia stati inclusi e quali no. Evita include ciclici
typedef struct {
    char **files;      	//lista dei file già inclusi
    size_t len;         //lunghezza attuale (numero di elementi) dell'array
    int capacity;      	//capacità massima dell’array (estendibile)
} Inclusi;

// ???
typedef struct {
    char* file;			//nome file in cui si verifica l'errore
    int line;			//linea dell'errore
} ErrorInfo;

// ???
typedef struct {
    char** string_list; //lista di stringhe (che stringhe? cosa fanno?)
    size_t len;			//lunghezza attuale (numero di elementi) dell'array
} StringSplit;

// ???
typedef struct {
    int variables_num;	//
    int errors_num;		//
    ErrorInfo* errors;	//
    int commenti;       //
    int num_inclusi;

} VarInfo;

extern VarInfo out1234;

void stampaDiDebug();

int fai_tutto(char *in_filename, char *out_filename);
FILE *apri (char *filename);
char *leggi_da_filename(char *filename);
char *leggi(FILE *fi);
char *prendi_nomedir(char *filename);
char* risolvi_includes(char *testo, char *input_dir, Inclusi *inclusi);
Inclusi *inizializza_inclusi(char *in_filename);
void aggiungi(char *filename, Inclusi *inclusi);
int gia_incluso(char *filename, Inclusi *inclusi);
int conta_include(char *testo);
char *rimuovi_commenti(char *testo);
void* safe_realloc(void* ptr, size_t new_size);
int scrivi(char *out_filename, char *testo);
void fai_verbose();

// Array di keyword C riservate
extern const char* c_reserved_keywords[];
extern const size_t num_c_reserved_keywords;

// Array di keyword da saltare
extern const char* keywords_to_skip[];
extern const size_t num_keywords_to_skip;

// Funzioni per la manipolazione di stringhe
StringSplit split_string(const char* input, const char* separators, bool keep_empty, bool keep_separator);
void free_string_split(StringSplit split);
char* remove_text_inside(const char* str, char* open_char, char* close_char, bool keep_close_char);
char* remove_strings(const char* str);
char* remove_chars(const char* str, const char* chars_to_remove);
char* extract_variable_identifiers(const char* declaration);
char* clean_string(const char* str);
char* extract_first_word(const char* str);

// Funzioni di utilità
bool in_array(const char* word, const char* array[], size_t array_size);
size_t count_words_until_char(const char* str, char stop_char);
size_t count_words_in_string(const char* str);
bool ends_with(const char* line, char separator);
bool is_variable_declaration(const char* instruction);
bool is_valid_identifier(const char* name);

// Funzioni per la gestione degli errori e variabili
void add_error(ErrorInfo** array, int* count, const char* filename, int line);
void free_varinfo(VarInfo* info);
void count_variables(const char* text, const char* filename);
void stampaVarInfo(VarInfo* vi);

char* rimuovi_commenti_alla_fine(char* line);

#endif