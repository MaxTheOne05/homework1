#include <stdio.h>    // Per funzioni standard input/output (es. printf, FILE)
#include <stdlib.h>   // Per funzioni come exit()
#include <string.h>   // Per operazioni su stringhe (es. strtok, strcmp)
#include <ctype.h>    // Per verifiche su caratteri (es. isdigit)
#include <stdbool.h>

// (Nome file in cui si verifica l'errore, Linea dell'errore)
typedef struct {
    char* file;
    int line;
} ErrorInfo;

// (Lista di stringhe, Numero di elementi)
typedef struct {
    char** string_list;
    size_t len;
} StringSplit;


typedef struct {
    int variables_num;
    int errors_num;
    ErrorInfo* errors;
} VarInfo;


#define NUM_KEYWORDS (sizeof(c_keywords) / sizeof(c_keywords[0]))

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

#define NUM_C_TYPES (sizeof(c_types) / sizeof(c_types[0]))

// Lista dei tipi principali c
static const char *c_types[] = {
    // Tipi fondamentali
    "char", "short", "int", "long", "float", "double", 
    "void", "_Bool", "_Complex", "_Imaginary",
    
    // Modificatori di tipo
    "signed", "unsigned",
    
    // Tipi derivati (che possono essere usati in dichiarazioni)
    "struct", "union", "enum", "typedef",
};


// Funzione che effettua una realloc gestendo meglio i casi in cui realloc fallisce
char* safe_realloc(char* ptr, size_t new_size) {
    char* tmp = realloc(ptr, new_size);
    if (!tmp) {
        free(ptr);
        return NULL;
    }
    return tmp;
}

// Funzione che legge il contenuto di un file
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

// Funzione che dato il nome di un file lo apre, richiama la funzione leggi e ne restituisce il contenuto letto
char *leggi_da_filename(const char *filename){
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        fprintf(stderr, "<leggi_da_filename>: Errore apertura file: %s\n", filename);
		exit(1);
    }
    char* testo = leggi(f);
    fclose(f);
    return testo;
}

// Funzione per controllare se la parola corrente è un identificatore di variabile valido
bool is_valid_c_identifier(const char *name) {
    // Controllo lunghezza minima (almeno 1 carattere)
    if (name == NULL || name[0] == '\0') {
        return false;
    }

    // Controllo primo carattere: deve essere lettera o underscore
    if (!isalpha(name[0]) && name[0] != '_') {
        return false;
    }

    // Controllo caratteri successivi
    for (size_t i = 1; name[i] != '\0'; i++) {
        if (!isalnum(name[i]) && name[i] != '_') {
            return false;
        }
    }

    // Controllo che non sia una parola chiave
    for (size_t i = 0; i < NUM_KEYWORDS; i++) {
        if (strcmp(name, c_keywords[i]) == 0) {
            return false;
        }
    }

    return true;
}

// Funzione per controllare se la parola corrente è un tipo c
bool is_data_type(const char *word) {
    for (int i = 0; i < NUM_C_TYPES; i++) {
        if (strcmp(word, c_types[i]) == 0) {
            return true;
        }
    }
    return false;
}

// Rimuove le assegnazioni da una riga dove sono state dichiarate variabili
char* remove_assignments(const char *line) {
    if (line == NULL) return NULL;
    
    size_t length = strlen(line);
    char* new_line = malloc(length + 1);
    if (new_line == NULL) {
        return NULL;
    }
    
    bool is_assignment = false;
    bool in_quotes = false;
    size_t new_i = 0;

    for (size_t i = 0; i < length; i++) {
        // Gestione delle virgolette
        if (line[i] == '"' && (i == 0 || line[i-1] != '\\')) {
            in_quotes = !in_quotes;
        }
        
        if (!in_quotes) {
            if (line[i] == '=' && !is_assignment) {
                // Controlla che non sia ==
                if (i == 0 || line[i-1] != '=') {
                    if (i == length-1 || line[i+1] != '=') {
                        is_assignment = true;
                        continue;
                    }
                }
            }
            
            if (line[i] == ',' || line[i] == ';') {
                is_assignment = false;
            }
        }
        
        if (!is_assignment) {
            new_line[new_i++] = line[i];
        }
    }
    
    new_line[new_i] = '\0'; // Terminatore nullo
    
    // Riduci la memoria allocata alla dimensione effettiva
    char* tmp = realloc(new_line, new_i + 1);
    if (tmp != NULL) {
        new_line = tmp;
    }
    
    return new_line;
}


StringSplit split_into_lines(const char* text) {
    StringSplit result = {NULL, 0};
    
    if (!text) return result;

    // Calcola dimensione massima (1 riga per carattere nel caso peggiore)
    size_t max_possible_lines = strlen(text) + 1;
    
    // Alloca spazio per le righe
    char** lines = malloc(max_possible_lines * sizeof(char*));
    if (!lines) return result;
    
    int line_count = 0;
    char* copy = strdup(text);
    if (!copy) {
        free(lines);
        return result;
    }

    char* current = copy;
    char* line_start = copy;
    
    while (*current) {
        if (*current == '\n') {
            // Calcola la lunghezza della riga corrente
            size_t line_length = current - line_start;
            
            // Alloca spazio per la riga (includendo il terminatore nullo)
            char* line = malloc(line_length + 1);
            if (!line) {
                // In caso di errore, libera la memoria allocata finora
                for (int i = 0; i < line_count; i++) free(lines[i]);
                free(lines);
                free(copy);
                return result;
            }
            
            // Copia la riga
            strncpy(line, line_start, line_length);
            line[line_length] = '\0';
            
            // Rimuove \r finale se presente (per compatibilità Windows)
            if (line_length > 0 && line[line_length-1] == '\r') {
                line[line_length-1] = '\0';
            }
            
            lines[line_count++] = line;
            
            // Sposta il puntatore all'inizio della prossima riga
            line_start = current + 1;
        }
        current++;
    }
    
    // Aggiungi l'ultima riga se non termina con \n
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
    
    // Aggiungi stringhe vuote per ogni \n extra alla fine
    if (copy && strlen(copy) > 0 && text[strlen(text)-1] == '\n') {
        char* last_char = copy + strlen(copy) - 1;
        while (last_char >= copy && *last_char == '\n') {
            char* empty = strdup("");
            if (empty) {
                lines[line_count++] = empty;
                last_char--;
            } else {
                break;
            }
        }
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
