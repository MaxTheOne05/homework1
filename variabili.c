#include <stdio.h>    // Per funzioni standard input/output (es. printf, FILE)
#include <stdlib.h>   // Per funzioni come exit()
#include <string.h>   // Per operazioni su stringhe (es. strtok, strcmp)
#include <ctype.h>    // Per verifiche su caratteri (es. isdigit)
#include <stdbool.h>

#include "progetto.h"
#include "funzioni.c"


// Lista di parole chiave standard (C11/C17)
static const char *c_keywords[] = {
    "auto", "break", "case", "char", "const", "continue", "default", "do",
    "double", "else", "enum", "extern", "float", "for", "goto", "if",
    "int", "long", "register", "return", "short", "signed", "sizeof", "static",
    "struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while",
    "_Bool", "_Complex", "_Imaginary", "inline", "restrict",
    "_Alignas", "_Alignof", "_Atomic", "_Generic", "_Noreturn",
    "_Static_assert", "_Thread_local"
};
#define NUM_KEYWORDS (sizeof(c_keywords) / sizeof(c_keywords[0]))   //numero di elementi presenti nell'array c_keywords


//Lista dei tipi principali c
static const char *c_types[] = {
    // Tipi fondamentali
    "char", "short", "int", "long", "float", "double", 
    "void", "_Bool", "_Complex", "_Imaginary",
    
    // Modificatori di tipo
    "signed", "unsigned",
    
    // Tipi derivati (che possono essere usati in dichiarazioni)
    "struct", "union", "enum", "typedef",
};
#define NUM_C_TYPES (sizeof(c_types) / sizeof(c_types[0]))          //numero di elementi presenti nell'array c_types


//Controlla se la "name" è un identificatore di variabile valido
bool is_valid_c_identifier(const char *name) {
    // Controllo lunghezza minima (almeno 1 carattere)
    if (name == NULL || name[0] == '\0') {
        return false;
    }

    //controllo il primo carattere
    if (!isalpha(name[0]) && name[0] != '_') {      //se il primo carattere non è una lettera o underscore (es. inizia con un numero)
        return false;                               //NON puo essere un nome valido
    }

    //controllo caratteri successivi
    for (size_t i = 1; name[i] != '\0'; i++) {
        if (!isalnum(name[i]) && name[i] != '_') {  //se i prossimi caratteri non sono alfanumeri o underscore 
            return false;                           //NON puo essere un nome valido
        }
    }

    //controllo che non sia una parola chiave riservata in c
    for (size_t i = 0; i < NUM_KEYWORDS; i++) {
        if (strcmp(name, c_keywords[i]) == 0) {
            return false;
        }
    }

    return true;
}

//Controlla se "word" è un tipo di dato
bool is_data_type(const char *word) {

    for (int i = 0; i < NUM_C_TYPES; i++) {
        if (strcmp(word, c_types[i]) == 0) {
            return true;
        }
    }
    return false;
}

//Rimuove le assegnazioni da una riga dove sono state dichiarate variabili
char* remove_assignments(const char *line) {
    if (line == NULL) return NULL;
    
    size_t length = strlen(line);           //prendiamo la lunghezza della riga
    char* new_line = malloc(length + 1);    //e allochiamo spazio per una stringa dinamica (+1 per il terminatore \0)
    
    bool is_assignment = false;             //ci dice se siamo dopo un = (cioè in una parte da ignorare)
    bool in_quotes = false;                 //ci dice se siamo dentro delle virgolette (altra parte da ignorare)
    int j = 0;                              //indice per la nuova stringa

    for (int i = 0; i<length; i++) {
        
        //Se ci troviamo tra virgolette..
        if (line[i] == '"' && (i == 0 || line[i-1] != '\\')) {  //Ogni volta che troviamo una virgoletta che non è preceduta da "\"
            in_quotes = !in_quotes;                             //entriamo o usciamo da una stringa
        }
        
        //Se non ci troviamo tra virgolette..
        if (!in_quotes){
            //..non siamo in un assegnamento e troviamo un "="  
            if (!is_assignment && line[i] == '=') {   
                //..e questo NON è un "=="      
                if ((i == 0 || line[i-1] != '=') && (i == length-1 || line[i+1] != '=')) {
                    //allora siamo in un assegnamento
                    is_assignment = true;
                    continue;
                }
            }
            
            //quando troviamo una virgola o punto e virgola, vuol dire che l’assegnazione è finita
            if (line[i] == ',' || line[i] == ';') {
                is_assignment = false;
            }
        }
        
        if (!is_assignment) {           //se NON siamo in un assegnamento
            new_line[j++] = line[i];    //copiamo il carattere
        }
    }
    new_line[j] = '\0';                 //aggiugniamo il terminatore di stringa
    
    return new_line;
}

//Dato un testo restituisce un array di stringhe, in cui ogni elemento è una riga del testo originale
StringSplit split_into_lines(const char* text) {
    //Inizializziamo la struttura StringSplit (definita in progetto.h) che andremo restituire
    StringSplit result;         //creiamo un istanza della struct
    result.string_list = NULL;  //inizialmente non abbiamo nessuna riga
    result.len = 0;             //e quindi la lunghezza dell'array è 0
        
    //Alloca spazio per le righe. Iniziamo con 1024, se poi non dovesse bastare facciamo realloc
    size_t capacity = 1024;
    char** lines = malloc(capacity * sizeof(char*));
    
    char* current = text;       //serve per scorrere il testo 
    char* line_start = text;    //punta all'inizio di una riga
    int line_count = 0;         //mantiene il numero di righe trovate
    
    //finche non arriviamo a fine testo
    while (*current) {
        current = strstr(current, "\n");            //trova il primo \n del testo da current in poi (serve per capire dove finisce la riga)
        size_t line_length = current - line_start;  //ora possiamo calcolare la lunghezza della riga
   
        char* line = malloc(line_length + 1);       //allochiamo spazio per la riga + terminatore '\0'
        strncpy(line, line_start, line_length);     //e ci copiamo la riga
        line[line_length] = '\0';                   //+ terminatore
            
        //Rimuoviamo \r se presente (per compatibilità Windows)
        if (line_length > 0 && line[line_length-1] == '\r') {
            line[line_length-1] = '\0';
        }

        //se non c'è abbastanza spazio
        if (line_count >= capacity) {
            capacity *= 2;                                                      //aumentiamo lo spazio allocato esponenzialmente (per maggior efficienza)
            char** lines = safe_realloc(lines, capacity * sizeof(char*));       //reallochiamo la memoria
            
            lines[line_count++] = line;     //Inseriamo la riga nell'array
            line_start = current + 1;       //Sposta il puntatore all'inizio della prossima riga
        }
        current++;
    }
    
    //Gestiamo l'ultima riga, siccome potrebbe non terminare con \n. 
    if (line_start < current) {
        size_t line_length = current - line_start;
        char* line = malloc(line_length + 1);
        if (line) {
            strncpy(line, line_start, line_length);
            line[line_length] = '\0';
            
            // Rimuove \r finale se presente
            if (line_length > 0 && line[line_length-1] == '\r') {
                line[line_length-1] = '\0';
            }
            
            lines[line_count++] = line;
        }
    }
    
    result.string_list = lines;
    result.len = line_count;
    return result;
}

// Funzione per trasformare un testo in una lista di righe
StringSplit split_into_instructions(const char* text) {
    StringSplit result = {NULL, 0};  // Inizializza a NULL e 0
    
    if (!text) return result;

    // Calcola dimensione massima (1 riga per carattere nel caso peggiore)
    size_t max_possible_lines = strlen(text) + 1;
    
    // Alloca spazio per le righe
    char** lines = malloc(max_possible_lines * sizeof(char*));
    if (!lines) return result;
    
    int line_count = 0;  // Variabile locale invece di puntatore
    char* copy = strdup(text);
    if (!copy) {
        free(lines);
        return result;
    }

    char* line;
    char* rest = copy;
    
    // Usa strtok_r con \n come delimitatore
    while ((line = strtok_r(rest, ";{}", &rest))) {
        // Controllo superamento dimensione allocata
        if (line_count >= max_possible_lines) {
            fprintf(stderr, "Attenzione: superato il limite di righe\n");
            break;
        }
        
        // Duplica la riga rimuovendo eventuali \r rimanenti (per Windows)
        char* line_copy = strdup(line);
        if (!line_copy) {
            for (int i = 0; i < line_count; i++) free(lines[i]);
            free(lines);
            free(copy);
            return result;
        }
        
        // Rimuove \r finale se presente (per compatibilità Windows)
        size_t len = strlen(line_copy);
        if (len > 0 && line_copy[len-1] == '\r') {
            line_copy[len-1] = '\0';
        }
        
        lines[line_count++] = line_copy;
    }

    free(copy);
    
    // Ridimensiona l'array al numero effettivo di righe
    char** tmp = realloc(lines, line_count * sizeof(char*));
    if (tmp || line_count == 0) {
        lines = tmp;
    }
    
    result.string_list = lines;
    result.len = line_count;
    return result;
}

// Funzione per trasformare una riga in una lista di parole
StringSplit split_into_words(const char* line) {
    StringSplit result = {NULL, 0};  // Inizializza a NULL e 0
    
    if (!line) return result;
    
    // Calcola la dimensione massima possibile
    size_t max_possible_words = strlen(line) + 1;
    
    // Alloca spazio per le parole
    char** words = malloc(max_possible_words * sizeof(char*));
    if (!words) return result;
    
    int word_count = 0;  // Variabile locale invece di puntatore
    char* copy = strdup(line);
    if (!copy) {
        free(words);
        return result;
    }

    char* word;
    char* rest = copy;
    
    while ((word = strtok_r(rest, " \t\n\r,;*[]=&", &rest))) {
        if (strlen(word) == 0) continue;
        
        if (word_count >= max_possible_words) {
            fprintf(stderr, "Attenzione: superato il limite di parole\n");
            break;
        }
        
        words[word_count] = strdup(word);
        if (!words[word_count]) {
            for (int i = 0; i < word_count; i++) free(words[i]);
            free(words);
            free(copy);
            return result;
        }
        word_count++;
    }

    free(copy);
    
    // Riduci l'array alla dimensione effettiva
    char** tmp = realloc(words, word_count * sizeof(char*));
    if (tmp || word_count == 0) {
        words = tmp;
    }
    
    result.string_list = words;
    result.len = word_count;
    return result;
}


// Versione migliorata di free_memory
void free_string_split(StringSplit* split) {
    if (split) {
        for (int i = 0; i < split->len; i++) {
            free(split->string_list[i]);
        }
        free(split->string_list);
        split->string_list = NULL;
        split->len = 0;
    }
}


// Versione più robusta di add_error
void add_error(ErrorInfo** array, int* count, const char* filename, int line) {
    ErrorInfo* temp = realloc(*array, (*count + 1) * sizeof(ErrorInfo));
    if (temp == NULL) {
        perror("Errore nel realloc");
        return;
    }
    *array = temp;

    // Aggiungi il nuovo elemento
    (*array)[*count].file = strdup(filename);
    if ((*array)[*count].file == NULL) {
        perror("Errore in strdup");
        return;
    }
    (*array)[*count].line = line;
    (*count)++;  // Incrementa solo se tutto va bene
}


// Funzione per pulire VarInfo
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


bool contains_parenthesis(char* str){
    int p1 = '(', p2 = ')';

    return strchr(str, p1)!= NULL || strchr(str, p2)!= NULL;
}


VarInfo count_variables(const char* text, const char* filename) {
    VarInfo out = {
        .variables_num = 0,
        .errors_num = 0,
        .errors = NULL
    };
    
    StringSplit lines = split_into_lines(text);
    char* cleaned_line;
    
    for (size_t l = 0; l < lines.len; l++) {
        printf("Riga %ld: %s \n\n", l, lines.string_list[l]);
        
        
        StringSplit instructions = split_into_instructions(lines.string_list[l]);
        
        for (size_t i = 0; i < instructions.len; i++){
            if (contains_parenthesis(instructions.string_list[i]))
                continue;
    
            cleaned_line = remove_assignments(instructions.string_list[i]);
            StringSplit words = split_into_words(cleaned_line);
    
    
            if (words.len > 0 && is_data_type(words.string_list[0])) {  // Controlla words.len > 0
                for (size_t w = 1; w < words.len; w++) {
    
                    printf("Parola %ld: %s \n\n", w, words.string_list[w]);
                    out.variables_num++;
                    if (!is_valid_c_identifier(words.string_list[w])) {
                        add_error(&out.errors, &out.errors_num, filename, l+1);
                    }
                }
            }
            free_string_split(&words);  // Usa la versione migliorato

        }

        free_string_split(&instructions);
    }

    free_string_split(&lines);  // Usa la versione migliorata
    return out;
}



// Funzione principale modificata
int main() {

    char *filename = "prova.c";
    char *file_content = leggi_da_filename(filename);

    VarInfo vi = count_variables(file_content, filename);

    printf("--------------------------------------------------------\n");
    
    printf("OUTPUT:\n\n");
    
    printf("Numero variabili: %d \n\n", vi.variables_num);

    printf("Numero errori: %d \n\n", vi.errors_num);
    
    // Scorri tutti gli elementi
    for (int i = 0; i < vi.errors_num; i++) {
        // Verifica se il puntatore al file è valido (se usi un marcatore di fine)
        if (vi.errors[i].file == NULL) {
            break;
        }
        
        // Stampa la stringa nel formato richiesto
        printf("(file: %s, line: %d)\n", vi.errors[i].file, vi.errors[i].line);
    }


    return 0;
}
