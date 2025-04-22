#include "progetto.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <libgen.h>

int fai_tutto(char *in_filename, char *out_filename){

	//apriamo direttamente in_filename perche possiamo assumere che sia nella CWD
	FILE *fi = fopen(in_filename, "r");
	if (fi == NULL){
		fprintf(stderr, "Errore in apertura! Impossibile aprire il file di input: %s\n", in_filename);
		exit(1);
		return 1;
	}

    //apriamo il file di output se necessario, altrimenti è stdout
	FILE *fout = stdout;
	if (out_filename != NULL){
		fout = fopen(out_filename, "w");
		if (fout == NULL){
			fprintf(stderr, "Errore apertura file output: %s\n", out_filename);
			exit(1);
    	}
	}

    //leggiamo il contenuto del file di input
    char *testo = leggi(fi);

    //ci prendiamo la directory in cui sono salvati tutti i file .h e .c da includere
    char copia[2048];
    strncpy(copia, in_filename, 2048);  //è importante fare la ocpia perche dirname() modifica la stringa
    char *input_dir = dirname(copia);   //estrae il path della cartella dal path del file

    while (conta_include(testo)>0){
        //inserire la funzione che rimuove i commenti
        char *tmp = risolvi_includes(testo, input_dir);     //tmp conterrà il testo aggiornato con gli include risolti
        free(testo);                                        //boh da vedere :)
        testo = tmp;
    }
    scrivi(out_filename, testo);         //ritorna 0 se va tutto bene, 1 se c'è errore in scrittura
    free(testo);

    return 0;
}



//Dato il nome di un file lo apre, richiama la funzione leggi e ne restituisce il contenuto letto
char *leggi_da_filename(char *filename){
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        fprintf(stderr, "<leggi_da_filename>: Errore apertura file: %s\n", filename);
		exit(1);
    }
    char* testo = leggi(f);
    fclose(f);
    return testo;
}


char *leggi(FILE *fi){
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
        testo = safe_realloc(testo, lenTesto + lenRiga); //aumentiamo lo spazio allocato
        strcat(testo, riga);                            //e concateniamo la riga alla fine di testo.
        lenTesto += lenRiga;                            //aumentiamo la lunghezza del testo

        //Gestiamo l'eventuale caso in cui realloc dovesse fallire liberando la memoria e ritornando NULL (errore)
        if (testo == NULL){
            free(testo);
            return NULL;
        }
    }

    return testo;
}

//Risolve tutti gli include con "" presenti in testo
char* risolvi_includes(char *testo, char *input_dir) {
                                            //similmente a quanto fatto con leggi allochiamo dinamicamente la memoria
    size_t lenResult = 1;                   //inizialmente vale solo 1 perche contiene solo il terminatore '\0'
    size_t posizione = 0;                   //ci dice il prossimo punto in cui andremo a scrivere
    char *result = malloc(lenResult);       
    result[0] = '\0';

    const char* current_pos = testo;                        //ci posizioniamo all'inizio del file di input per iniziare la ricerca della direttiva #include.
    while (*current_pos) {                                  //finche non raggiungiamo fine stringa

        if (strncmp(current_pos, "#include", 8) != 0) {     //Controlla se i prossimi 8 char sono "#include". 

            //se i prossimi char NON sono "#include" copiamo carattere per carattere. Tuttavia NON possiamo usare strcat
            //perche richiede di unire due stringhe, noi invece dobbiamo aggiungere un singolo carattere a result.

            if (posizione + 1 > lenResult) {            //se non c'è abbastanza spazio per aggiungere un nuovo char
                lenResult *= 2;                         //aumentiamo lo spazio allocato esponenzialmente (per maggior efficienza)
                result = safe_realloc(result, lenResult);
            }      
            result[posizione++] = *current_pos;         //aggiungiamo il carattere in penultima posizione
            result[posizione] = '\0';                   //aggiungiamo \0 in ultima posizione (senza strcat dobbiamo farlo manualmente noi)
            current_pos++;   

        } else {

            //se i prossimi char sono "#include" saltiamo la parola "#include" e gli spazi per andare a cercare il nome del file
            current_pos += 8;
            while (isspace(*current_pos)){
                current_pos++;                            
            }
            
            char filename[1024];                    //conterrà il nome del file
            int i = 0;                              //ci serve per scorrere filename

            current_pos++;                          //salta la virgoletta iniziale
            while (*current_pos != '"') {
                filename[i++] = *current_pos++;     //copia carattere per carattere il nome del file
            }
            current_pos++;                          //salta la virgoletta finale
            filename[i] = '\0';
            
            //concateniamo input_dir e filename 
            char fullpath[2048];
            snprintf(fullpath, sizeof(fullpath), "%s/%s", input_dir, filename);
            //NON possiamo usare strcat perche andrebbe a modificare direttamente dir_name, che invece va lasciata intatta (poiche ci servirà per gli altri file da includere)

            char* included_content = leggi_da_filename(fullpath);   //prendiamo il contenuto del file da includere (il caso di fallimento è gia gestito in leggi())
            size_t lenTesto = strlen(included_content);             //verifichiamo che ci sia abbastanza spazio per scrivere

            size_t spazio_necessario = posizione+lenTesto+1;        //dobbiamo scrivere: tutto il testo fino ad ora + il testo dell'include + \0
            while (spazio_necessario > lenResult) {
                lenResult *= 2;                                     //se non c'è spazio lo creiamo (aumentando esponenzialmente, come prima)
            }
            result = safe_realloc(result, lenResult);
            memcpy(result + posizione, included_content, lenTesto); //aggiungiamo il contenuto a result
            posizione += lenTesto;
            result[posizione] = '\0';
            free(included_content);
            //liberiamo lo spazio di included_content poiche abbiamo copiato in result e terminato         
        }
    }
    
    return result;
}

void fai_verbose(){
    
}

int conta_include(char *testo) {
    int count = 0;
    char *pos = testo;

    //trova la prima occorrenza di #include nel testo (pos puntera al primo carattere di #include, quindi a #)
    while ((pos = strstr(pos, "#include ")) != NULL) {
        pos += 9;                   //saltiamo la parola #include
        while (isspace(*pos)) {     //salta tutti gli spazi dopo la parola #include
            pos++;
        }

        if (*pos == '"') {      //Se il prossimo carattere è una virgoletta
            count++;            //aumentiamo il conteggio perche abbiamo trovato un #include "filename"
        }
    }
    return count;
}

//effettua comunque un realloc ma gestendo meglio i casi in cui realloc fallisca
char* safe_realloc(char* ptr, size_t new_size) {
    char* tmp = realloc(ptr, new_size);
    if (!tmp) {
        free(ptr);
        return NULL;
    }
    return tmp;
}

int scrivi(char *out_filename, char *testo) {
    // Se out_filename è NULL, scriviamo su stdout con printf
    if (out_filename == NULL) {
        printf("%s", testo);
        return 0;
    }

    //Se abbiamo un file di utput lo apriam0
    FILE *fout = fopen(out_filename, "w");
    if (fout == NULL) {
        fprintf(stderr, "Errore apertura file output: %s\n", out_filename);
        return 1;
    }

    size_t len = strlen(testo);                     //numero di elementi da scrivere
    size_t written = fwrite(testo, 1, len, fout);   //fwrite restituisce il numero di elementi effettivamente scritti
    fclose(fout);                                   //chiudiamo il file

    //Se il numero di elementi scritti != elementi da scrivere (ne abbiamo persi alcuni) abbiamo un errore
    if (written != len) {
        fprintf(stderr, "Errore scrittura file output: %s\n", out_filename);
        return 1;
    }

    //se invece abbiamo sicrtto tutto correttamente possiamo ritornare 0
    return 0;
}
