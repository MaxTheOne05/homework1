#include "progetto.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

int fai_tutto(FILE *fi){
    
    printf("\nIl risultato di fai_tutto è:\n");

    char riga[2048];
    while (fgets(riga, sizeof(riga), fi) != NULL) {
        int i = 0;
        while (riga[i] != '\0') {
            char c = riga[i++];
            putchar(c);  
        }
    }
    return 0;
}

void fai_verbose(){
    
}
