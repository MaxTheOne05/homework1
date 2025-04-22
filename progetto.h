#ifndef FUNZIONI_H
#define FUNZIONI_H

#include <stdio.h>

int fai_tutto(char *in_filename, char *out_filename);
FILE *apri (char *filename);
char *leggi_da_filename(char *filename);
char *leggi(FILE *fi);
char *prendi_nomedir(char *filename);
char* risolvi_includes(char *testo, char *input_dir);
int conta_include(char *testo);
char* safe_realloc(char* testo, size_t new_size);
int scrivi(char *out_filename, char *testo);
char *rimuovi_commenti(const char *testo);
void fai_verbose();

#endif
