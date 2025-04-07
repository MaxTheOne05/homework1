#include <funzioni.c>
#include <progetto.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int main(int argc, char *argv[]){
	
	char *in_filename = NULL;		//conterrà il nome del file input
	char *out_filename = stdout;	//conterrà il nome del file output
	bool verbose = 1;				//False
	
	//leggiamo gli argomenti in argv e impostiamo  
	for (int i=1; i<argc; i++){
	
		if (strcmp(argv[i], "-i") || strcmp(argv[i], "--in")){
			in_filename = argv[++i];
			
		} else if (strcmp(argv[i], "-o") || strcmp(argv[i], "--out")){
			out_filename = argv[++i];
			
		} else if (strcmp(argv[i], "-v") || strcmp(argv[i], "--verbose")){
			verbose = 0;
		}
	}
	
	if (in_filename == NULL){
		fprintf(stderr, "Errore! Inserire il file input.\n"
		return 1			//segnala che il main non è terminato con successo
	}
	

	//apriamo direttamente in_filename perche possiamo assumere che sia nella CWD
	FILE *fi 				
	fi = fopen(in_filename, "r");
	if (fi == NULL){
		fprintf(stderr, "Errore in apertura! Impossibile aprire il file di input: %s\n", in_filename)
		exit(1)
		return 1
	}
	
	FILE *fout
	if (strcmp(out_filename, "stdout")){fout = stdout;} 
	else {fout = fopen(out_filename, "w");}
	
	fai_tutto(fi)
	
	if (verbose==0){
		fai_verbose();
	}
	
	return 0
}

int fai_tutto(FILE *fi){

}

inf fai_verbose(){

}
