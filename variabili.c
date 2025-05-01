#include <stdio.h>    // Per funzioni standard input/output (es. printf, FILE)
#include <stdlib.h>   // Per funzioni come exit()
#include <string.h>   // Per operazioni su stringhe (es. strtok, strcmp)
#include <ctype.h>    // Per verifiche su caratteri (es. isdigit)
#include <stdbool.h>

#include "progetto.h"
#include "funzioni.c"





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
    // Lista di parole chiave standard (C11/C17)
    const char *c_keywords[] = {
        "auto", "break", "case", "char", "const", "continue", "const", "default", "do",
        "double", "else", "enum", "extern", "float", "for", "goto", "if",
        "int", "long", "register", "return", "short", "signed", "sizeof", "static",
        "struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while",
        "_Bool", "_Complex", "_Imaginary", "inline", "restrict",
        "_Alignas", "_Alignof", "_Atomic", "_Generic", "_Noreturn",
        "_Static_assert", "_Thread_local"
    };
    
    size_t num_keywords = (sizeof(c_keywords) / sizeof(c_keywords[0]));   //numero di elementi presenti nell'array c_keywords

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
    for (size_t i = 0; i < num_keywords; i++) {
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

int is_type_keyword(const char *word) {
    const char *keywords[] = {"const", "unsigned", "signed", "short", "long", "int", "char", "float", "double", "void", "struct", "enum", "union", NULL};
    for (int i = 0; keywords[i] != NULL; i++) {
        if (strcmp(word, keywords[i]) == 0) return 1;
    }
    return 0;
}


StringSplit split_string(const char *input, const char *separators, bool keep_empty) {
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

void free_string_split(StringSplit split) {
    for (size_t i = 0; i < split.len; i++) {
        free(split.string_list[i]);
    }
    free(split.string_list);
}



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
    
    const char *line_sep = "\n";
    const char *instr_sep = ";";
    const char *word_sep = " ,\t\n";
    char* var_names = NULL;

    StringSplit lines = split_string(text, line_sep, false);
    
    for (size_t l = 0; l < lines.len; l++) {
        printf("Riga %ld: %s \n\n", l, lines.string_list[l]);
        
        
        StringSplit instructions = split_string(lines.string_list[l], instr_sep, false);
        
        for (size_t i = 0; i < instructions.len; i++){
            if (contains_parenthesis(instructions.string_list[i]))
                continue;
    
            var_names= extract_variable_names(instructions.string_list[i]);
            StringSplit words = split_string(var_names, word_sep, false);
    
    
            if (words.len > 0) {  // Controlla words.len > 0
                for (size_t w = 1; w < words.len; w++) {
    
                    printf("Parola %ld: %s \n\n", w, words.string_list[w]);
                    out.variables_num++;
                    if (!is_valid_c_identifier(words.string_list[w])) {
                        add_error(&out.errors, &out.errors_num, filename, l+1);
                    }
                }
            }
            free_string_split(words);  // Usa la versione migliorato

        }

        free_string_split(instructions);
    }

    free_string_split(lines);  // Usa la versione migliorata
    return out;
}



// Funzione principale modificata
int main() {

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
