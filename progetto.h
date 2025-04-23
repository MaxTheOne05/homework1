#ifndef FUNZIONI_H
#define FUNZIONI_H

#include <stdio.h>

typedef struct {
    char **files;      	//lista dei file già inclusi
    int len;         	//lunghezza attuale (numero di elementi) dell'array
    int capacity;      	//capacità massima dell’array (estendibile)
} Inclusi;

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
