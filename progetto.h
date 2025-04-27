#ifndef FUNZIONI_H
#define FUNZIONI_H

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
} VarInfo;


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

#endif
