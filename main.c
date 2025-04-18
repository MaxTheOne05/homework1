
#include "progetto.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>


int main(int argc, char *argv[]){
	
	char *in_filename = NULL;		//conterrà il nome del file input
	char *out_filename = NULL;		//conterrà il nome del file output (o stdout se non viene dato)
	bool verbose = false;			

	//leggiamo gli argomenti in argv e impostiamo  
	printf("\n\nGli argomenti letti sono:\n");
	for (int i=1; i<argc; i++){
		printf("%s\n", argv[i]);
		if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--in") == 0){
			in_filename = argv[i+1];
			
		} else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--out") == 0){
			out_filename = argv[i+1];
				
		} else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0){
			verbose = true;
		}
	}

	printf("\n\n\n");
	
	if (in_filename == NULL){
		fprintf(stderr, "Errore! Inserire il file input.\n");
		return 1;				
	}
	
	fai_tutto(in_filename, out_filename);
	
	if (verbose){
		fai_verbose();
	}
	
	return 0;
}

