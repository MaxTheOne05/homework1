#include "progetto.h"

#include <libgen.h>
#include <stdio.h>    // Per funzioni standard input/output (es. printf, FILE)
#include <stdlib.h>   // Per funzioni come exit()
#include <string.h>   // Per operazioni su stringhe (es. strtok, strcmp)
#include <ctype.h>    // Per verifiche su caratteri (es. isdigit)
#include <stdbool.h>
#include <stddef.h>   // Per size_t 
/* 
    ---------------------------------------------------------GLOBALI---------------------------------------------------------
*/
// Struct globale utilizzata per tenere tutti i contatori e le informazioni che servono per il verbose. La dobbiamo solo inizializzare perche è dichiara nel file progetto.h
VarInfo out1234 = {
    .variables_num = 0,
    .errors_num = 0,
    .errors = NULL,
    .commenti = 0,
    .num_inclusi = 0
};

// Tipi per strutture, unioni ed enumerazioni (introducono nuovi tipi composti)
// "struct",
// "union",
// "enum",
// Parole chiave per i tipi di dato standard in C e typedef comuni dalle librerie standard
const char *c_data_types[] = {
    // Parole chiave standard C
    "void",     // Tipo vuoto
    "char",     // Carattere
    "short",    // Intero corto
    "int",      // Intero
    "long",     // Intero lungo
    "float",    // Numero in virgola mobile singola precisione
    "double",   // Numero in virgola mobile doppia precisione
    "_Bool",    // Parola chiave nativa per il tipo booleano (standard C99+)
    "bool",     // Typedef comune per _Bool (da <stdbool.h>, standard C99+)
    
    // _Complex e _Imaginary (per numeri complessi/immaginari, standard C99+)
    // "_Complex", // Meno comuni per liste base
    // "_Imaginary" // Meno comuni per liste base

    // Typedef comuni dalle librerie standard
    "size_t",       // Dimensione di oggetti (da <stddef.h>, <stdio.h>, <stdlib.h>, <string.h>...)
    "ptrdiff_t",    // Differenza tra puntatori (da <stddef.h>)
    "wchar_t",      // Carattere wide (da <stddef.h>, <wchar.h>)
    "max_align_t",  // Tipo con l'allineamento massimo (da <stddef.h>, C11+)
    "FILE",         // Struttura per flussi di file (da <stdio.h>)
    "fpos_t",       // Posizione nel flusso di file (da <stdio.h>)
    "va_list",      // Argomenti variabili (da <stdarg.h>, a volte <stdio.h>)
    "div_t",        // Risultato divisione int (da <stdlib.h>)
    "ldiv_t",       // Risultato divisione long (da <stdlib.h>)
    "lldiv_t",      // Risultato divisione long long (da <stdlib.h>, C99+)
    "clock_t",      // Tick di clock (da <time.h>)
    "time_t",       // Tempo in secondi (da <time.h>)
    "struct tm",    // Struttura per data e ora calendariale (da <time.h>) // Nota: è una struct, non un typedef diretto, ma si usa così.
    "intptr_t",     // Intero abbastanza grande da contenere un puntatore (da <stdint.h>, C99+)
    "uintptr_t",    // Intero senza segno abbastanza grande da contenere un puntatore (da <stdint.h>, C99+)

    // Esempi di tipi interi a larghezza fissa (da <stdint.h>, C99+)
    // Potresti voler includere tutti i vari intN_t e uintN_t se ti servono,
    // ma la lista diventerebbe molto lunga. Eccone alcuni:
    "int8_t",
    "int16_t",
    "int32_t",
    "int64_t",
    "uint8_t",
    "uint16_t",
    "uint32_t",
    "uint64_t",
    "int_least8_t", // Tipo minimo con almeno N bit
    "int_fast8_t",  // Tipo più veloce con almeno N bit
    "intmax_t",     // Intero signed massimo (da <stdint.h>, <inttypes.h>, C99+)
    "uintmax_t",    // Intero unsigned massimo (da <stdint.h>, <inttypes.h>, C99+)
};

// Parole chiave per i modificatori (qualificatori e specificatori) dei tipi in C
const char *c_type_modifiers[] = {
    "const",    // Qualificatore di costante
    "volatile", // Qualificatore volatile
    "restrict", // Qualificatore restrict (per puntatori, standard C99+)
    "signed",   // Specificatore di segno (per tipi interi, implicito per int)
    "unsigned", // Specificatore di segno (per tipi interi, senza segno)
    "short",    // Specificatore di dimensione (es. con int, come in "short int")
    "long",     // Specificatore di dimensione (es. con int o double, come in "long int", "long double")
    "_Atomic",  // Qualificatore atomico (standard C11+)
};

// Array: Tutte le parole chiave riservate del linguaggio C
const char *c_reserved_keywords[] = {
    "auto",
    "break",
    "case",
    "char",
    "const",
    "continue",
    "default",
    "do",
    "double",
    "else",
    "enum",
    "extern",
    "float",
    "for",
    "goto",
    "if",
    "inline",           // Standard C99+
    "int",
    "long",
    "register",
    "restrict",         // Standard C99+ (principalmente per puntatori)
    "return",
    "short",
    "signed",
    "sizeof",
    "static",
    "struct",
    "switch",
    "typedef",
    "union",
    "unsigned",
    "void",
    "volatile",
    "while",
    // Parole chiave aggiunte in C99
    "_Bool",
    "_Complex",
    "_Imaginary",
    // Parole chiave aggiunte in C11
    "_Alignas",
    "_Alignof",
    "_Atomic",
    "_Generic",
    "_Noreturn",
    "_Static_assert",
    "_Thread_local",
};

// Parole chiave che mi permettono di capire che la riga corrente non contiene una dichiarazione di variabili
const char *keywords_to_skip[] = {
    "struct",
    "union",
    "enum",
    "typedef",
    "return",
    "for",
    "while",
    "if",
    "else",
    "do",
};

// Dimensioni degli array
const size_t num_c_data_types = sizeof(c_data_types) / sizeof(c_data_types[0]);
const size_t num_c_type_modifiers = sizeof(c_type_modifiers) / sizeof(c_type_modifiers[0]);
const size_t num_c_reserved_keywords = sizeof(c_reserved_keywords) / sizeof(c_reserved_keywords[0]);
const size_t num_keywords_to_skip = sizeof(keywords_to_skip) / sizeof(keywords_to_skip[0]);

//Il numero di file "" inclusi è dato da inclusi.len quando il programma termina
/* 
    ---------------------------------------------------------FINE GLOBALI---------------------------------------------------------
*/ 

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

/* 
    ---------------------------------------------------------INCLUDES---------------------------------------------------------
*/
//Risolve tutti gli include con "" presenti in testo
char* risolvi_includes(char *testo, char *input_dir, Inclusi *inclusi) {

    Inclusi *inclusi2 = inizializza_inclusi("");    //esclusivamente per gli include con <>. 
    /*Siccome ogni volta restituiamo un nuovo testo (e NON modifichiamo quello prima), utilizzando lo stesso inclusi "globale" succedeva che un include <stdio.h>
      viene copiato solo alla prima invocazione della funzione. In tutte le successive invocazioni <stdio.h> è gia presente in inclusi e quindi verrà ignorato.
      Per risolvere il problema è sufficiente creare un inclusi2 "locale" che si resetta ad ogni invocazione. */

    size_t lenResult = 1;                   //inizialmente vale solo 1 perchè contiene solo il terminatore '\0'
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
                    out1234.num_inclusi++;
                    char* included_content = leggi_da_filename(filename);   //prendiamo il contenuto del file da includere (il caso di fallimento in apertura è gia gestito in leggi())
                    char* tmp = rimuovi_commenti(included_content);
                    count_variables(included_content, filename);

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
/* 
    ---------------------------------------------------------FINE INCLUDES---------------------------------------------------------
*/

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

/* 
    ---------------------------------------------------------VARIABILI---------------------------------------------------------
*/

// Funzione per dividere una stringa in base a delimitatori
StringSplit split_string(const char *input, const char *separators, bool keep_empty, bool keep_separator) {
    StringSplit result = {NULL, 0}; // inizializzazione vuota

    if (!input || !separators) {
        return result;
    }
    // Creiamo una copia dell'input perché strtok modifica la stringa
    char *input_copy = strdup(input);
    if (!input_copy) return result;

    size_t capacity = 10;
    result.string_list = malloc(capacity * sizeof(char *));
    if (!result.string_list) {
        free(input_copy);
        return result;
    }
    char *start = input_copy;
    char *token = strpbrk(start, separators);

    while (token != NULL) {
        if (keep_empty || token > start) {
            if (result.len >= capacity) {
                capacity *= 2;
                char **temp = realloc(result.string_list, capacity * sizeof(char *));
                if (!temp) {
                    // Libera memoria in caso di errore
                    for (size_t i = 0; i < result.len; i++) {
                        free(result.string_list[i]);
                    }
                    free(result.string_list);
                    free(input_copy);
                    result.string_list = NULL;
                    result.len = 0;
                    return result;
                }
                result.string_list = temp;
            }
            size_t length = token - start;
            if (keep_separator) {
                length++; // Include il separatore
            }
            result.string_list[result.len] = strndup(start, length);
            result.len++;
        }
        start = token + 1;
        token = strpbrk(start, separators);
    }
    // Aggiungi l'ultimo token (o stringa vuota se necessario)
    if (keep_empty || *start != '\0') {
        if (result.len >= capacity) {
            capacity *= 2;
            char **temp = realloc(result.string_list, capacity * sizeof(char *));
            if (!temp) {
                for (size_t i = 0; i < result.len; i++) {
                    free(result.string_list[i]);
                }
                free(result.string_list);
                free(input_copy);
                result.string_list = NULL;
                result.len = 0;
                return result;
            }
            result.string_list = temp;
        }
        result.string_list[result.len] = strdup(start);
        result.len++;
    }

    free(input_copy);
    return result;
}

// Funzione per liberare la memoria occupata da StringSplit
void free_string_split(StringSplit split) {
    for (size_t i = 0; i < split.len; i++) {
        free(split.string_list[i]);
    }
    free(split.string_list);
}

// Revised add_error function (safer allocation and counting)
void add_error(ErrorInfo** array, int* count, const char* filename, int line) {
    // 1. Allocate memory for the filename string first
    char* filename_copy = strdup(filename);
    if (filename_copy == NULL) {
        perror("Errore in strdup per l'errore filename");
        // Consider more robust error handling here if needed
        return; // Cannot add error without filename
    }

    // Calculate the index for the new element (current count)
    int new_index = *count;

    // 2. Reallocate the array for the new size (current count + 1)
    // Assuming safe_realloc is a wrapper around realloc that returns NULL on failure
    ErrorInfo* temp = safe_realloc(*array, (new_index + 1) * sizeof(ErrorInfo));
    if (temp == NULL) {
        perror("Errore nel realloc per l'array di errori");
        free(filename_copy); // Free the string copy if realloc fails
        // Consider more robust error handling here if needed
        return; // Cannot add error if realloc fails
    }
    *array = temp; // Update the array pointer *after* successful realloc

    // 3. Assign the allocated filename copy and line number to the new element
    (*array)[new_index].file = filename_copy; // Assign the already allocated string
    (*array)[new_index].line = line;

    // 4. Increment the count ONLY if the element was fully added
    (*count)++;
}

// Funzione per liberare la memoria occupata da VarInfo
void free_varinfo(VarInfo* info) {
    if (info) {
        for (int i = 0; i < info->errors_num; i++) {
            free(info->errors[i].file);
        }
        free(info->errors);
        info->errors = NULL;
        info->errors_num = 0;
    }
}

//Cancella tutto il contenuto tra l'open_char ed il close_char. Se keep_close_char true il carattere di chiusura non viene cancellato
char* remove_text_inside(const char* str, char* open_char, char* close_char, bool keep_close_char) {
    char *result = malloc(strlen(str) + 1);
    if (!result) return NULL; // Controllo errore allocazione
    int i = 0, j = 0;
    int count = 0;

    while (str[i] != '\0') {
        if (strchr(open_char, str[i]) != NULL) {
            count++; // Conta i delimitatori di apertura
        } else if (strchr(close_char, str[i]) != NULL && count > 0) {
            count--; // Conta i delimitatori di chiusura
            if (count == 0 && keep_close_char) {
                result[j++] = str[i]; // Copia il delimitatore di chiusura se richiesto
            }
        } else if (count == 0) {
            result[j++] = str[i]; // Copia solo i caratteri fuori dai delimitatori
        }
        i++;
    }
    result[j] = '\0'; // Terminazione della stringa
    return result;
}

//Cancella tutte le stringhe del testo. Lo utilizziamo per rimuovere le inizializzazioni delle stringhe.
//L'altra funzione per rimuovere le inizializzazioni leva tutto quello che è compreso '=' e ','. COn le stringhe non funzioerebbe
char* remove_strings(const char* str) {
    char* result = malloc(strlen(str) + 1);
    if (!result) return NULL; // Controllo errore allocazione
    int i = 0, j = 0;
    bool inside_str = false; // Flag per tenere traccia se siamo dentro una stringa
    
    while (str[i] != '\0') {
        if (str[i] == '\"') {
            // && (i == 0 || str[i-1] != '\\') devo capire se è necessario dentro l'if (non credo)
            inside_str = !inside_str; // Inverte il flag quando trova un "
        } else if (!inside_str) {
            result[j++] = str[i]; // Copia solo i caratteri fuori dalle stringhe
        }
        i++;
    }
    result[j] = '\0'; // Terminazione della stringa
    return result;
}

//Data una stringa ed un array di caratteri cancella tali caratteri dalla stringa, e ritorna la nuova stringa
char* remove_chars(const char* str, const char* chars_to_remove) {
    char* result = malloc(strlen(str) + 1);
    if (!result) return NULL; // Controllo errore allocazione
    int i = 0, j = 0;
    while (str[i] != '\0') {
        if (strchr(chars_to_remove, str[i]) == NULL) {
            result[j++] = str[i]; // Copia solo i caratteri non presenti in chars_to_remove
        }
        i++;
    }
    result[j] = '\0'; // Terminazione della stringa
    return result;
}

char* extract_variable_identifiers(const char* declaration) {
    if (declaration == NULL) {
        return NULL;
    }
    // Trova il primo ',' o ';' nella stringa
    const char* delimiter = strpbrk(declaration, ",;");
    if (delimiter == NULL) {
        return NULL; // Nessun delimitatore trovato
    }
    // Trova l'inizio della parola subito prima del delimitatore
    const char* start = delimiter - 1;
    while (start > declaration && isspace((unsigned char)*(start))) {
        start--; // Salta gli spazi
    }
    while (start > declaration && !isspace((unsigned char)*(start - 1)) && *(start - 1) != ',' && *(start - 1) != ';') {
        start--; // Trova l'inizio della parola
    }
    // Copia tutto da "start" fino alla fine della stringa
    char* result = strdup(start);
    if (result == NULL) {
        return NULL; // Errore di allocazione
    }
    return result;
}

//Pulisce la stringa sfruttando le funzioni sopra. Rimuove tutto quello che è tra parentesi, le stringhe e le inizializzazioni e delle variabili
char* clean_string(const char* str) {
    char* output = strdup(str);
    if (!output) return NULL;

    char* temp = remove_text_inside(output, "{[(", ")]}", false);  // Rimuove tutto ciò che si trova all'interno delle parentesi
    free(output);
    if (!temp) return NULL;
    output = temp;
    printf("Dopo aver rimosso tutto ciò che si trova all'interno di parentesi: %s\n", output);

    temp = remove_strings(output);                      //Rimuove tutte le stringhe dal testo
    free(output);
    if (!temp) return NULL;
    output = temp;
    printf("Dopo aver rimosso le stringhe: %s\n", output);

    temp = remove_text_inside(output, "=", ",;", true);  //Rimuove le inizializzazioni delle variabili
    free(output);
    if (!temp) return NULL;
    output = temp; 
    printf("Dopo aver rimosso le inizializzazioni : %s\n", output);

    temp = remove_chars(output, "*");                   //Rimuove gli * dai nomi delle variabili puntatore per evitare che vengano consideati come parte del nome di variabile
    free(output);
    if (!temp) return NULL;
    output = temp;
    printf("Dopo aver rimosso gli * dai nomi dei puntatori : %s\n", output);

    temp = extract_variable_identifiers(output);  // Estrae gli identificatori delle variabili
    free(output);
    if (!temp) return NULL;
    output = temp;
    printf("Dopo aver estratto i nomi delle variabili : %s\n", output);

    return output;
}

//Funzione per controllare se una parola è presente in un array di stringhe
bool in_array(const char *word, const char *array[], size_t array_size) {
    for (size_t i = 0; i < array_size; i++) {
        if (strcmp(word, array[i]) == 0) {
            return true;
        }
    }
    return false;
}

// Funzione per estrarre la prima parola da una stringa
char* extract_first_word(const char* str) {
    if (str == NULL || *str == '\0') {
        return NULL; // Ritorna NULL se la stringa è vuota o nulla
    }

    // Salta eventuali spazi iniziali
    while (isspace((unsigned char)*str)) {
        str++;
    }

    // Trova la fine della prima parola
    const char* end = str;
    while (*end != '\0' && !isspace((unsigned char)*end)) {
        end++;
    }

    // Calcola la lunghezza della parola
    size_t length = end - str;

    // Alloca memoria per la parola e copia il contenuto
    char* word = malloc(length + 1);
    if (word == NULL) {
        return NULL; // Ritorna NULL in caso di errore di allocazione
    }
    strncpy(word, str, length);
    word[length] = '\0'; // Aggiunge il terminatore di stringa

    return word;
}

// Funzione per contare il numero di parole fino a un carattere specifico
size_t count_words_until_char(const char* str, char stop_char) {
    if (str == NULL) {
        return 0; // Ritorna 0 se la stringa è nulla
    }

    size_t word_count = 0;
    bool in_word = false;

    // Salta eventuali spazi iniziali
    while (*str != '\0' && isspace((unsigned char)*str)) {
        str++;
    }

    // Itera sulla stringa
    while (*str != '\0') {
        if (*str == stop_char) {
            // Conta stop_char come parola se è preceduto da uno spazio
            if (isspace((unsigned char)*(str - 1))) {
                word_count++;
            }
            break; // Interrompi il ciclo quando raggiungi stop_char
        }

        if (!isspace((unsigned char)*str)) {
            if (!in_word) {
                in_word = true; // Inizia una nuova parola
                word_count++;
            }
        } else {
            in_word = false; // Fine della parola
        }
        str++;
    }
    return word_count;
}

// Funzione per contare il numero di variabili in una stringa
size_t count_words_in_string(const char* str) {
    if (str == NULL) {
        return 0; // Ritorna 0 se la stringa è nulla
    }
    size_t variable_count = 0;
    bool in_word = false;

    // Salta eventuali spazi iniziali
    while (*str != '\0' && isspace((unsigned char)*str)) {
        str++;
    }

    // Itera sulla stringa
    while (*str != '\0') {
        if (!isspace((unsigned char)*str) && *str != ',' && *str != ';') {
            if (!in_word) {
                in_word = true; // Inizia una nuova variabile
                variable_count++;
            }
        } else {
            in_word = false; // Fine della variabile
        }
        str++;
    }
    return variable_count;
}

bool ends_with(const char* line, char separator) {
    if (line[strlen(line) - 1] == separator) {
        return true; // La riga termina con il separatore
    }
    return false; // La riga non termina con il separatore
}

// Funzione per verificare se una stringa è una dichiarazione di variabile
bool is_variable_declaration(const char* instruction) {
    char* first_word = extract_first_word(instruction);
    
    if (first_word == NULL || in_array(first_word, keywords_to_skip, num_keywords_to_skip)) {
        return false; // Non è una dichiarazione di variabile
    }
    free(first_word); // Libera la memoria allocata per first_word

    char* open_parenthesis = strchr(instruction, '(');
    char* equals = strchr(instruction, '=');

    if ((open_parenthesis != NULL && equals == NULL) || (open_parenthesis != NULL && equals != NULL && open_parenthesis < equals)) {
        return false; // Non è una dichiarazione di variabile
    }

    if (equals != NULL && count_words_until_char(instruction, '=') <= 2) {
        return false; // Non è una dichiarazione di variabile
    }
    
    if (count_words_in_string(instruction) <= 1) {
        return false; // Non è una dichiarazione di variabile
    }
    return true; // È una dichiarazione di variabile
}

//Controlla se la "name" è un identificatore di variabile valido
bool is_valid_identifier(const char *name) {

    // Controllo lunghezza minima (almeno 1 carattere)
    if (name == NULL || name[0] == '\0') {
        printf("Nome non valido: %s\n", name);
        return false;
    }
    //controllo il primo carattere
    if (!isalpha(name[0]) && name[0] != '_') {      //se il primo carattere non è una lettera o underscore (es. inizia con un numero)
        printf("Nome non valido: %s\n", name);
        return false;                               //NON puo essere un nome valido
    }
    //controllo caratteri successivi
    for (size_t i = 1; name[i] != '\0'; i++) {
        if (!isalnum(name[i]) && name[i] != '_') {  //se i prossimi caratteri non sono alfanumeri o underscore 
            printf("Nome non valido: %s\n", name);
            return false;                           //NON puo essere un nome valido
        }
    }
    //controllo che non sia una parola chiave riservata in c
    for (size_t i = 0; i < num_c_reserved_keywords; i++) {
        if (strcmp(name, c_reserved_keywords[i]) == 0) {
            printf("Nome non valido: %s\n", name);
            return false;
        }
    }
    printf("Nome valido: %s\n", name);

    return true;
}

//Conta il numero di variabili TOTALI nel file di input, il numero di variabili con nome errato e la lista di coppie (filename, num riga) per ogni errore rilevato
void count_variables(const char* text, const char* filename) {

    const char *line_sep = "\n";
    const char *instr_sep = ";";
    const char *word_sep = " ,;\t\n";
    // char* var_names = NULL; // This variable is unused

    StringSplit lines = split_string(text, line_sep, true, false);

    for (size_t l = 0; l < lines.len; l++) {

        printf("\n\n\nRiga %ld: %s \n\n", l, lines.string_list[l]);

        if (strlen(lines.string_list[l]) > 0 && ends_with(lines.string_list[l], ';')) {

            StringSplit instructions = split_string(lines.string_list[l], instr_sep, false, true);

            for (size_t i = 0; i < instructions.len; i++){

                if (is_variable_declaration(instructions.string_list[i])) {
                    printf("sono una dichiarazione di variabile: %s\n", instructions.string_list[i]);
                    // *** FIX 3: Store and free clean_string result ***
                    char* cleaned_instruction = clean_string(instructions.string_list[i]);
                    // if (cleaned_instruction == NULL) {
                    //     // Handle potential allocation error from clean_string
                    //     perror("Errore in clean_string");
                    //     continue; // Skip this instruction
                    // }

                    StringSplit words = split_string(cleaned_instruction, word_sep, false, false);

                    free(cleaned_instruction); // *** FIX 3: Free the cleaned string ***

                    for (size_t w = 0; w < words.len; w++)
                    {
                        out1234.variables_num++;

                        if (!is_valid_identifier(words.string_list[w])) {
                            // *** FIX 1: Removed the erroneous out.errors_num++; here ***
                            // Add error if not a valid identifier
                            add_error(&out1234.errors, &out1234.errors_num, filename, l + 1);
                            // add_error is now responsible for incrementing out.errors_num
                        }
                    }
                    free_string_split(words);
                }
            }
            free_string_split(instructions);
        }
    }
    free_string_split(lines);
}

/* 
---------------------------------------------------------FINE VARIABILI---------------------------------------------------------
*/

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
            out1234.commenti++;                               //incremento il contatore delle righe con commenti
        } 

        //Se troviamo "/*" è l'inizio di un commento multi-linea
        else if (testo[r] == '/' && testo[r+1] == '*') {
            
            out1234.commenti++;                                           //la riga attuale contiene un commento (anche se contenesse solo /*)
            r += 2;                                                         //saltiamo /* iniziale
            while (r + 1 < len && !(testo[r] == '*' && testo[r+1] == '/')){ 
                if (testo[r] == '\n') out1234.commenti++;                 //ogni nuova riga che incontriamo è all'interno del commento multilinea. Quindi è una riga con commenti
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
    printf("<debug>: Righe di commento eliminate: %i\n", out1234.commenti);
    printf("<debug>: Numero di variabili non valide: %i\n", out1234.variables_num);
    printf("<debug>: Numero di errori rilevati: %i\n", out1234.errors_num);

    printf("<debug>: Errori Rilevati:\n");
    for (int i = 0; i<out1234.errors_num; i++){
        ErrorInfo err = out1234.errors[i];
        printf("Riga: %i, File: %s\n", err.line, err.file);
    }
    printf("Numero di file inclusi: %d \n", out1234.num_inclusi);
}