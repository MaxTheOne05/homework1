#include "progetto.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <libgen.h>

int fai_tutto(char *in_filename, char *out_filename){

    //apriamo direttamente in_filename perche possiamo assumere che sia nella CWD
	FILE *fi = apri(in_filename);

    //leggiamo il contenuto del file di input. La variabile conterrà il puntatore al primo carattere del file letto
    char *testo = leggi(fi);

    //ci prendiamo la directory in cui sono salvati tutti i file .h e .c da includere
    char *input_dir = prendi_nomedir(in_filename);

    //finche ci sono include VALIDI nel testo li rimuoviamo
    while (conta_include(testo)>0){
        //inserire la funzione che rimuove i commenti       //per evitare di risolvere include commentati andiamo prima a rimuovere i commenti
        char *tmp = risolvi_includes(testo, input_dir);     //tmp conterrà il testo aggiornato con gli include risolti
        free(testo);                                        //boh da vedere :)
        testo = tmp;
    }
    
    //scriviamo il risultato nel file di output o nello stdout
    scrivi(out_filename, testo);
    
    //liberiamo lo spazio allocato e ritorniamo 0 (esecuzione terminata correttamente)
    free(testo);
    return 0;
}

//Dato il nome del file lo apre in sola lettura
FILE *apri (char *filename){
	FILE *fi = fopen(filename, "r");
	if (fi == NULL){
		fprintf(stderr, "Errore in apertura! Impossibile aprire il file: %s\n", filename);
		exit(1);
	}
    return fi;
}

//Dato il nome di un file lo apre, richiama la funzione leggi e ne restituisce il contenuto letto
char *leggi_da_filename(char *filename){

    //QUI UTILIZZARE LA FUNZIONE apri. Per ora la lascio cosi perche è piu facile per debug.
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        fprintf(stderr, "<leggi_da_filename>: Errore apertura file: %s\n", filename);
		exit(1);
    }
    char* testo = leggi(f);
    fclose(f);
    return testo;
}

//Dato il puntatore ad un file lo apre e ne restituisce TUTTO il contenuto in una stringa
char *leggi(FILE *fi){
    
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

//Dato il filename estrae il nome della directory (serve per formare il path assoluto dei file da includere)
char *prendi_nomedir(char *filename) {
    char copia[2048];
    strncpy(copia, filename, 2048);     //è importante fare la copia perche dirname() modifica la stringa
    return strdup(dirname(copia));      //estrae il path della cartella dal path del file. 
                                        //Senza strdup il puntatore a "copia" verrebbe cancellato dopo il return. Quindi la stringa ritornata sarebbe ""
}

//Risolve tutti gli include con "" presenti in testo
char* risolvi_includes(char *testo, char *input_dir) {
    size_t lenResult = 1;                   //inizialmente vale solo 1 perche contiene solo il terminatore '\0'
    size_t write_pos = 0;                   //ci dice il prossimo punto in cui andremo a scrivere
    char *result = malloc(lenResult);       
    result[0] = '\0';

    char* read_pos = testo;                             //iniziamo a leggere dall'inizio del file di input 
    while (*read_pos != '\0') {                         //iteriamo finche non raggiungiamo fine stringa

        if (strncmp(read_pos, "#include", 8) != 0) {     //Controlla se i prossimi 8 char sono un #include

            //CASO 1: i prossimi char NON sono "#include" copiamo carattere per carattere. Tuttavia NON possiamo usare strcat
            //perche richiede di unire due stringhe, noi invece dobbiamo aggiungere un singolo carattere a result.

            if (write_pos + 1 > lenResult) {            //se non c'è abbastanza spazio per aggiungere un nuovo char
                lenResult *= 2;                         //aumentiamo lo spazio allocato esponenzialmente (per maggior efficienza)
                result = safe_realloc(result, lenResult);
            }      
            result[write_pos++] = *read_pos;            //aggiungiamo il carattere in penultima posizione
            result[write_pos] = '\0';                   //aggiungiamo \0 in ultima posizione (senza strcat dobbiamo farlo manualmente noi)
            read_pos++;                                 //andiamo a leggere il char successivo

        } else {

            //Andiamo cercare "" oppure <>
            char *p = read_pos + 8;     //saltiamo la parola #include
            while (isspace(*p)){        //saltiamo tutti gli spazi
                p++;                            
            }
            
            if (*p == '"'){
                //CASO 2: è un #include "filename". Apriamo il file e ne compia l'intero contenuto in result
                char filename[1024];                    //conterrà il nome del file
                int i = 0;                              //ci serve per scorrere filename

                read_pos = p + 1;                       //salta la virgoletta iniziale (DA DE-COMMENTARE DOPO)
                while (*read_pos != '"') {
                    filename[i++] = *read_pos++;        //copia carattere per carattere il nome del file
                }
                read_pos++;                             //salta la virgoletta finale
                filename[i] = '\0';                     
                
                //concateniamo input_dir e filename 
                char fullpath[2048];
                snprintf(fullpath, sizeof(fullpath), "%s/%s", input_dir, filename);
                //NON possiamo usare strcat perche andrebbe a modificare direttamente dir_name, che invece va lasciata intatta 
                //(poiche ci servirà per gli altri file da includere)

                //Apriamo il file e ne prendiamo il contenuto
                char* included_content = leggi_da_filename(fullpath);   //prendiamo il contenuto del file da includere (il caso di fallimento è gia gestito in leggi())
                size_t lenTesto = strlen(included_content);             //verifichiamo che ci sia abbastanza spazio per scrivere

                size_t spazio_necessario = write_pos+lenTesto+1;        //dobbiamo scrivere: testo fino ad ora + testo dell'include + \0
                while (spazio_necessario > lenResult) {
                    lenResult *= 2;                                     //se non c'è spazio lo creiamo (aumentando esponenzialmente, come prima)
                }
                result = safe_realloc(result, lenResult);
                memcpy(result + write_pos, included_content, lenTesto); //aggiungiamo il contenuto a result
                write_pos += lenTesto;                                  //spostiamo il prossimo punto in cui scrivere
                result[write_pos] = '\0';                               //inseriamo il terminatore di stringa
                free(included_content);                                 //liberiamo lo spazio di included_content poiche abbiamo copiato in result   

            } else {
                //CASO 3: è un #include <filename>. Copiamo il primo carattere e leggiamo il prossimo carattere
                if (write_pos + 1 > lenResult) {            //se non c'è abbastanza spazio per aggiungere un nuovo char
                    lenResult *= 2;                         //aumentiamo lo spazio allocato esponenzialmente (per maggior efficienza)
                    result = safe_realloc(result, lenResult);
                }      
                result[write_pos++] = *read_pos;            //aggiungiamo il carattere in penultima posizione
                result[write_pos] = '\0';                   //aggiungiamo \0 in ultima posizione (senza strcat dobbiamo farlo manualmente noi)
                read_pos++;                                 //andiamo a leggere il char successivo
            }      
        }
    }
    
    return result;
}

//Conta il numero di include VALIDI ancora da risolvere
int conta_include(char *testo) {
    int count = 0;          //conterra il numero di include da risolvere
    char *pos = testo;      //iniziamo a scorrere il file dall'inizio

    //strstr trova la prima occorrenza di #include nel testo (pos puntera al primo carattere di #include, quindi a #)
    while ((pos = strstr(pos, "#include ")) != NULL) {
        pos += 8;                   //saltiamo la parola #include
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
char* safe_realloc(char* testo, size_t new_size) {
    char* tmp = realloc(testo, new_size);
    if (!tmp) {
        free(testo);
        return NULL;
    }
    return tmp;
}

//Scrive "testo" nel file "out_filename" (se presente) oppure in stdout (se out_filename == NULL)
int scrivi(char *out_filename, char *testo) {
    //se out_filename è NULL, scriviamo su stdout con printf
    if (out_filename == NULL) {
        printf("%s", testo);
        return 0;
    }

    //se invece abbiamo un file di output lo apriamo
    FILE *fout = fopen(out_filename, "w");
    if (fout == NULL) {
        fprintf(stderr, "Errore apertura file output: %s\n", out_filename);
        return 1;
    }

    size_t len = strlen(testo);                     //numero di elementi da scrivere
    size_t written = fwrite(testo, 1, len, fout);   //fwrite restituisce il numero di elementi effettivamente scritti
    fclose(fout);                                   //chiudiamo il file

    //Se il numero di elementi scritti != elementi da scrivere abbiamo un errore
    if (written != len) {
        fprintf(stderr, "Errore scrittura file output: %s\n", out_filename);
        return 1;
    }

    //se invece abbiamo scritto tutto correttamente possiamo ritornare 0
    return 0;
}



void fai_verbose(){
    
}


