#ifndef FUNZIONI_H
#define FUNZIONI_H

#include <stdio.h>

int fai_tutto(char *in_filename, char *out_filename);
char *leggi_da_filename(char *filename);
char *leggi(FILE *fi);
char* risolvi_includes(char *input, char *input_dir);
void fai_verbose();
int conta(char *parola, char *testo);
char* safe_realloc(char* ptr, size_t new_size);

#endif
