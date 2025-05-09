#include "progetto.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <libgen.h>

//Var Globale che ci tiene il conto di righe di commento eliminate
int righe_con_commento = 0;

//Il numero di file "" inclusi è dato da inclusi.len quando il programma termina

//Var Globale che mantiene le informazioni sui nomi di variabili errati. La dobbiamo solo inizializzare perche è dichiara nel file progetto.h
VarInfo out1234 = {
    .variables_num = 0,
    .errors_num = 0,
    .errors = NULL
};

int fai_tutto(char *in_filename, char *out_filename){

    //apriamo direttamente in_filename perche possiamo assumere che sia nella CWD
	FILE *fi = apri(in_filename);

    //leggiamo il contenuto del file di input. La variabile conterrà il puntatore al primo carattere del file letto
    char *testo = leggi(fi);

    //ci prendiamo la directory in cui sono salvati tutti i file .h e .c da includere
    char *input_dir = prendi_nomedir(in_filename);

    //inizializziamo la struct inclusi che mantiene i nomi dei file gia inclusi (per evitare include ciclici)
    Inclusi *inclusi = inizializza_inclusi(in_filename);    //passiamo il path assoluto al file di input (in_filename) per evitare che venga incluso una 2a volta

    //Rimuoviamo i commenti presenti nel file iniziale
    char *tmp = rimuovi_commenti(testo);        //per evitare di risolvere include commentati andiamo prima a rimuovere i commenti. Tuttavia rimuovi_commenti alloca nuova memoria
    free(testo);                                //e la assegniamo a testo. Questo significa che il valore precedente di testo non sarà più accessibile e dobbiamo liberarlo
    testo = tmp;

    //finche ci sono include VALIDI nel testo li risolviamo
    while (conta_include(testo)>0){
        tmp = risolvi_includes(testo, input_dir, inclusi);      //tmp conterrà il testo aggiornato con gli include risolti
        free(testo);                                            //Analogo a prima. L'area di memoria puntata da tmp non sarà piu accessibile e quindi dobbiamo liberarla
        testo = tmp;                                            //ci salviamo il testo con gli include risolti
    }
    
    //scriviamo il risultato nel file di output o nello stdout
    scrivi(out_filename, testo);

    stampaDiDebug();

    //liberiamo lo spazio allocato e ritorniamo 0 (esecuzione terminata correttamente)
    free(testo);
    return 0;
}

void stampaDiDebug(){
    printf("<debug>: Righe di commento eliminate: %i\n", righe_con_commento);
    printf("<debug>: Numero di variabili non valide: %i\n", out1234.variables_num);
    printf("<debug>: Numero di errori rilevati: %i\n", out1234.errors_num);

    printf("<debug>: Errori Rilevati:\n");
    for (int i = 0; i<out1234.errors_num; i++){
        ErrorInfo err = out1234.errors[i];
        printf("Riga: %i, File: %s\n", err.line, err.file);
    }
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
        testo = safe_realloc(testo, lenTesto + lenRiga);    //aumentiamo lo spazio allocato
        strcat(testo, riga);                                //e concateniamo la riga alla fine di testo.
        lenTesto += lenRiga;                                //aumentiamo la lunghezza del testo

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
char* risolvi_includes(char *testo, char *input_dir, Inclusi *inclusi) {

    Inclusi *inclusi2 = inizializza_inclusi("");    //esclusivamente per gli include con <>. 
    /*Siccome ogni volta restituiamo un nuovo testo (e NON modifichiamo quello prima), utilizzando lo stesso inclusi "globale" succedeva che un include <stdio.h>
      viene copiato solo alla prima invocazione della funzione. In tutte le successive invocazioni <stdio.h> è gia presente in inclusi e quindi verrà ignorato.
      Per risolvere il problema è sufficiente creare un inclusi2 "locale" che si resetta ad ogni invocazione. */

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

            //CASO 2: è un #include. Andiamo cercare "" oppure <>
            char *p = read_pos + 8;     //saltiamo la parola #include
            while (isspace(*p)){        //saltiamo tutti gli spazi
                p++;                            
            }
            
            if (*p == '"'){
                //CASO 2.1: è un #include "filename". Continuiamo a leggere per individuare il nome del file 
                char filename[2048];                    //conterrà il nome del file
                strcpy(filename, input_dir);            //inizializziamo filename con il percorso alla directory
                int i = strlen(filename);               //ci posizioniamo alla fine e serve per scorrere filename
                filename[i++] = '/';            

                //aggiungiamo il nome del file al percorso della directory (per formare il path assoluto)
                read_pos = p + 1;                       //salta la virgoletta iniziale 
                while (*read_pos != '"') {
                    filename[i++] = *read_pos++;        //copia carattere per carattere il nome del file
                }
                read_pos++;                             //salta la virgoletta finale
                filename[i] = '\0';                     //termina il nome del file

                //Se il file è gia stato incluso và ignorato per evitare include ciclici
                if (gia_incluso(filename, inclusi) == 0){
                    aggiungi(filename, inclusi);                            //lo aggiungiamo alla lista di file gia inclusi
                    char* included_content = leggi_da_filename(filename);   //prendiamo il contenuto del file da includere (il caso di fallimento in apertura è gia gestito in leggi())
                    
                    char* tmp = rimuovi_commenti(included_content);
                    free(included_content);
                    included_content = tmp;
                    count_variables(included_content, filename);

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
                }

            } else if (*p == '<') {
                //CASO 2.2: è un #include <filename>. Andiamo ad estrarre il filename e lo aggiungiamo ad inclusi SENZA però risolverlo.
                //In questo modo evitiamo di avere piu #include <filename> duplicati (es. avremo un unico #include <stdio.h> nel file finale)
                char filename[2048];     //conterrà il nome del file
                int i = 0;               //ci serve per scorrere filename

                read_pos = p + 1;                       //salta la < iniziale 
                while (*read_pos != '>') {
                    filename[i++] = *read_pos++;        //copia carattere per carattere il nome del file
                }
                read_pos++;                             //salta la > finale
                filename[i] = '\0';                     //termina il nome del file 

                if (!gia_incluso(filename, inclusi2)) {
                    aggiungi(filename, inclusi2);
                    
                    int len = snprintf(NULL, 0, "#include <%s>\n", filename);   //calcola quanto spazio servirebbe per stampare la stringa #include <filename>\n, senza effettivamente stamparla.
                    char *temp = malloc(len + 1);                               //creiamo un'area di memoria abbastanza grande da contenere la stringa + '\0'
                    snprintf(temp, len + 1, "#include <%s>\n", filename);       //inserisce la stringa #include <filename>\n nell'area di memoria puntata da temp

                    size_t spazio_necessario = write_pos+len+1;         //dobbiamo scrivere: testo fino ad ora + #include<filename> + \0
                    while (spazio_necessario > lenResult) {
                        lenResult *= 2;                                 //se non c'è spazio lo creiamo (aumentando esponenzialmente, come prima)
                        result = safe_realloc(result, lenResult);
                    }
                    memcpy(result + write_pos, temp, len);          //aggiungiamo il contenuto a result    
                    write_pos += len;                               //spostiamo il prossimo punto in cui scrivere
                    result[write_pos] = '\0';                       //inseriamo il terminatore di stringa
                    free(temp);
                }
            }
        }
    }
    return result;
}

Inclusi *inizializza_inclusi(char *in_filename) {
    Inclusi *inclusi = malloc(sizeof(Inclusi));
    inclusi->files = malloc(10 * sizeof(char *));   //inizializziamo files come array di 10 stringhe
    inclusi->len = 0;                               //inizialmente contiene 0 elementi
    inclusi->capacity = 10;                         //ed ha capacità massima 10 elementi

    //Aggiungiamo subito il file iniziale ad inclusi, per evitare che venga incluso una 2a volta
    aggiungi(in_filename, inclusi);

    return inclusi; 
}

void aggiungi(char *filename, Inclusi *inclusi) {
    //controlliamo se c'è bisogno di allocare altro spazio. Eventulmente aumentiamo la capacita esponenzialmente (maggiore efficienza)
    if (inclusi->len >= inclusi->capacity) {
        inclusi->capacity *= 2;            
        inclusi->files = safe_realloc(inclusi->files, inclusi->capacity * sizeof(char *));
    }

    int i = inclusi->len++;                 //aumentiamo la lunghezza dell'array
    inclusi->files[i] = strdup(filename);   //aggiungiamo il filename all'array
}

//Controlla se un file è gia stato incluso (cioè se è presente nell'array inclusi).
int gia_incluso(char *filename, Inclusi *inclusi) {
    for (int i = 0; i < inclusi->len; i++) {
        if (strcmp(inclusi->files[i], filename) == 0) {
            return 1;                       //stringa trovata
        }
    }
    return 0;                               //stringa non trovata
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

//Effettua comunque un realloc ma gestendo meglio i casi in cui realloc fallisca
//Edit: meglio prendere un puntatore generico void come parametro. Almeno possiamo gestire qualsiasi riallocazione e non solo stringhe
void* safe_realloc(void *testo, size_t new_size) {
    void* tmp = realloc(testo, new_size);
    if (!tmp) {
        free(testo);
        return NULL;
    }
    return tmp;
}

//Scrive "testo" nel file "out_filename" oppure in stdout (se out_filename == NULL)
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

//Rimuove TUTTI i commenti da un testo
char *rimuovi_commenti(char *testo) {
    //Come tutte le altre funzioni utilizziamo l'allocazione dinamica della memoria. 
    size_t len = strlen(testo);
    char *result = malloc(len+1);  
    //Siccome non aggiungiamo mai nulla ma leviamo solo i commenti si avrà che, nel peggiore dei casi,
    //result ha la stessa dimensione del testo originale (len) + 1 per il terminatore    

    size_t r = 0;               //indice per scorrere il testo iniziale (in lettura)
    size_t w = 0;               //indice per scorrere il testo result (in scrittura)

    //Iniziamo a scorrere per individuare i punti in cui iniziano i commenti. L'idea non è tanto di eliminare i commenti
    //quanto più di "Ignorarli"; cioè NON copiarli nel risultato finale.
    while (r < len) {

        //Gestiamo il caso in cui ci siano "commenti" all'interno della dichiarazione di una stringa
        if (testo[r] == '\"') {
            result[w++] = testo[r++];                                       //scriviamo la virgoletta iniziale
            while (r < len && testo[r] != '\"' && testo[r-1] != '\\') {     //andiamo a cercare la virgoletta di chiusura, ignorando quelle con backslash davanti
                result[w++] = testo[r++];                                   //scriviamo tutto fino a fine stringa
            }
            result[w++] = testo[r++];                                       //scriviamo la virgoletta finale
        } 

        //Se troviamo "//" è l'inizio di un commento su singola linea
        else if (testo[r] == '/' && testo[r+1] == '/') {
            while (r < len && testo[r] != '\n') r++;            //ignoriamo tutto fino a fine riga
            righe_con_commento++;                               //incremento il contatore delle righe con commenti
        } 

        //Se troviamo "/*" è l'inizio di un commento multi-linea
        else if (testo[r] == '/' && testo[r+1] == '*') {
            
            righe_con_commento++;                                           //la riga attuale contiene un commento (anche se contenesse solo /*)
            r += 2;                                                         //saltiamo /* iniziale
            while (r + 1 < len && !(testo[r] == '*' && testo[r+1] == '/')){ 
                if (testo[r] == '\n') righe_con_commento++;                 //ogni nuova riga che incontriamo è all'interno del commento multilinea. Quindi è una riga con commenti
                r++;                                                        //ignoriamo tutto il testo fino a fine commento
            }
            if (r+1 < len) r += 2;                                          //saltiamo */ finale
        } 

        //Se siamo rrivati qui significa che non siamo in nessun commento
        else {
            result[w++] = testo[r++];       //copiamo il testo cosi com'è
        }
    }

    result[w] = '\0';
    return result;
}


void fai_verbose(){
    
    printf("               VERBOSE\n\n");
    printf("Righe di commento eliminate: %d \n", righe_con_commento); 
    //printf("Numero di file inclusi: %d \n", inclusi->len);
}


