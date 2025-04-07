
#include "progetto.h"
#include "funzioni.c"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>


int main(int argc, char *argv[]){
	
	char *in_filename = NULL;		//conterrà il nome del file input
	char *out_filename = stdout;	//conterrà il nome del file output (o stdout se non viene dato)
	bool verbose = false;			
	
	//leggiamo gli argomenti in argv e impostiamo  
	for (int i=1; i<argc; i++){
	
		if (strcmp(argv[i], "-i") || strcmp(argv[i], "--in")){
			in_filename = argv[++i];
			
		} else if (strcmp(argv[i], "-o") || strcmp(argv[i], "--out")){
			out_filename = argv[++i];
				
		} else if (strcmp(argv[i], "-v") || strcmp(argv[i], "--verbose")){
			verbose = true;
		}
	}
	
	if (in_filename == NULL){
		fprintf(stderr, "Errore! Inserire il file input.\n");
		return 1;				
	}
	

	//apriamo direttamente in_filename perche possiamo assumere che sia nella CWD
	FILE *fi = fopen(in_filename, "r");
	if (fi == NULL){
		fprintf(stderr, "Errore in apertura! Impossibile aprire il file di input: %s\n", in_filename);
		exit(1);
		return 1;
	}

	FILE *fout = stdout;
	if (!strcmp(out_filename, "stdout")){
		fout = fopen(out_filename, "w");
	}
	
	fai_tutto(fi);
	
	if (verbose==0){
		fai_verbose();
	}
	
	return 0;
}

