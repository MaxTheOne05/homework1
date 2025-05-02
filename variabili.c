#include <stdio.h>    // Per funzioni standard input/output (es. printf, FILE)
#include <stdlib.h>   // Per funzioni come exit()
#include <string.h>   // Per operazioni su stringhe (es. strtok, strcmp)
#include <ctype.h>    // Per verifiche su caratteri (es. isdigit)
#include <stdbool.h>
#include <stddef.h>   // Per size_t 

#include "progetto.h"



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
    // Tipi per strutture, unioni ed enumerazioni (introducono nuovi tipi composti)
    "struct",
    "union",
    "enum",
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

// Dimensioni degli array
const size_t num_c_data_types = sizeof(c_data_types) / sizeof(c_data_types[0]);
const size_t num_c_type_modifiers = sizeof(c_type_modifiers) / sizeof(c_type_modifiers[0]);
const size_t num_c_reserved_keywords = sizeof(c_reserved_keywords) / sizeof(c_reserved_keywords[0]);





//Funzione per dividere una stringa in base a delimitatori. keep_empty se true mantiene gli \n anche nelle righe vuote
StringSplit split_string(const char *input, const char *separators, bool keep_empty) {
    StringSplit result = {NULL, 0}; //inizializzazione vuota

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



// Furnzione per aggiungere un errore all'array di errori
void add_error(ErrorInfo** array, int* count, const char* filename, int line) {
    ErrorInfo* temp = safe_realloc(*array, (*count + 1) * sizeof(ErrorInfo));
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
        } else if (strchr(close_char, str[i]) != NULL) {
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

//Controlla se la "name" è un identificatore di variabile valido
bool is_valid_identifier(const char *name) {

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
    for (size_t i = 0; i < num_c_reserved_keywords; i++) {
        if (strcmp(name, c_reserved_keywords[i]) == 0) {
            return false;
        }
    }

    return true;
}

//Conta il numero di variabili TOTALI nel file di input, il numero di variabili con nome errato e la lista di coppie (filename, num riga) per ogni errore rilevato
VarInfo count_variables(const char* text, const char* filename) {
    VarInfo out = {
        .variables_num = 0,
        .errors_num = 0,
        .errors = NULL
    };
    
    const char *line_sep = "\n";
    const char *instr_sep = ";";
    const char *word_sep = " ,\t\n";
    char* var_names = NULL;

    StringSplit lines = split_string(text, line_sep, true);
    
    for (size_t l = 0; l < lines.len; l++) {
        
        printf("Riga %ld: %s \n\n", l, lines.string_list[l]);
        
        if (strlen(lines.string_list[l]) > 0) {
          
            StringSplit instructions = split_string(lines.string_list[l], instr_sep, false);
            
            for (size_t i = 0; i < instructions.len; i++){
        
                StringSplit words = split_string(clean_string(instructions.string_list[i]), word_sep, false);
        

                
                if (words.len > 0) {  // Controlla words.len > 0
                    for (size_t w = 1; w < words.len; w++) {
        
                        printf("Parola %ld: %s \n\n", w, words.string_list[w]);
                        out.variables_num++;
                        if (!is_valid_identifier(words.string_list[w])) {
                            add_error(&out.errors, &out.errors_num, filename, l+1);
                        }
                    }
                }
                free_string_split(words);  // Usa la versione migliorato

            }


            free_string_split(instructions);
        }
    }

    free_string_split(lines);  // Usa la versione migliorata
    return out;
}



// Funzione principale modificata
/*int main() {

    char *filename = "test_variabili.c";
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
*/