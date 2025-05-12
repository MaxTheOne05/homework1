#include <stdio.h>    // Per funzioni standard input/output (es. printf, FILE)
#include <stdlib.h>   // Per funzioni come exit()
#include <string.h>   // Per operazioni su stringhe (es. strtok, strcmp)
#include <ctype.h>    // Per verifiche su caratteri (es. isdigit)
#include <stdbool.h>
#include <stddef.h>   // Per size_t 

#include "progetto.h"
#include "funzioni.c"


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

    printf("Dopo aver tutto ciò che si trova all'interno di parentesi: %s\n", output);


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

    stampaVarInfo(&out1234);

    free_string_split(lines);
}


void stampaVarInfo(VarInfo *vi){
    printf("OUTPUT:\n\n");

    printf("Numero variabili: %d \n\n", vi->variables_num);

    printf("Numero errori: %d \n\n", vi->errors_num);

    for (int i = 0; i < vi->errors_num; i++) {
        // With the fixed add_error, vi.errors[i].file should not be NULL here
        // unless strdup failed for that specific error, which add_error now handles by not incrementing count.
        // So this check inside the loop is less critical for crashing, but good defensive programming.
        if (vi->errors[i].file == NULL) {
           fprintf(stderr, "Warning: file non inizializzato per l'errore %d (index %d)\n", i+1, i);
           continue; // Skip printing this invalid error entry
        }
       printf("(file: %s, line: %d)\n", vi->errors[i].file, vi->errors[i].line);
    }
}

//Funzione principale modificata
int main() {

    char *filename = "test_variabili2.c";
    char *file_content = leggi_da_filename(filename); // Assuming leggi_da_filename returns a heap-allocated string

    if (file_content == NULL) {
        fprintf(stderr, "Errore: Impossibile leggere il file %s\n", filename);
        return 1;
    }


    count_variables(file_content, filename);

    printf("--------------------------------------------------------\n");

     // Free the file content read from the file
     free(file_content);


    
    return 0;
}
