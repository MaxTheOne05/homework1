#include "progetto.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

int fai_tutto(FILE *fi){

    //inizialmente leggiamo tutto il testo in input. Ignorando la risoluzione di include e rimozione commenti.
    //(quello lo faremo piu avanti e salveremo il risultato in un'altra stringa)
    
    size_t lenTesto = 1;                //lunghezza "dinamica" del file in input. Partiamo da 1 per includere '\0'
    char *testo = malloc(lenTesto);     //iniziamo a creare la stringa dinamica.  
    testo[0] = '\0';                    //Inizialmente ha solo il terminatore di stringa. Serve principalmente a strstr 
                                        //per capire dove concatenare la riga

    //fgets legge una riga di fi e la inserisce dentro "riga"
    char riga[2048];
    while (fgets(riga, sizeof(riga), fi) != NULL) {
        size_t lenRiga = strlen(riga);
        testo = realloc(testo, lenTesto + lenRiga); //aumentiamo lo spazio allocato
        strcat(testo, riga);                        //e concateniamo la riga alla fine di testo.
        lenTesto += lenRiga;                        //aumentiamo la lunghezza del testo

        //Gestiamo l'eventuale caso in cui realloc dovesse fallire liberando la memoria e ritornando 1 (errore)
        if (testo == NULL){
            free(testo);
            return 1;
        }
    }

    printf("%s", testo);
    free(testo);
    return 0;
}

void fai_verbose(){
    
}

//conta il numero di occorrenze di "parola" in "testo"
int conta(char *parola, char *testo)  {
    int count = 0;
    char *pos = testo;          //è un puntatore all'inizio del testo

    //strstr(pos, parola) restituisce un puntatore alla prima occorrenza di "parola" nel testo puntato da "pos"
    //(se facessimo strstr(pos, testo) restituirebbe sempre la prima occorrenza. Con pos rendiamo il tutto "dinamico")
    while ((pos = strstr(pos, parola)) != NULL) {
        count++;                //ho trovato la parola => aumento il conteggio
        pos += strlen(parola);  //con l'artimetica dei puntatori andiamo a puntare 
                                //al carattere successivo alla partola appena vista
    }
    return count;
}
