#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

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

char* clean_string(const char* str) {
    char* output = strdup(str);
    if (!output) return NULL;

    char* temp = remove_text_inside(output, "{[(", ")]}", false);  // Rimuove tutto ciò che si trova all'interno delle parentesi
    free(output);
    if (!temp) return NULL;
    output = temp;

    printf("Dopo aver tutto ciò che si trova all'interno di parentesi: %s\n", output);


    temp = remove_strings(output);  
    free(output);
    if (!temp) return NULL;
    output = temp;

    printf("Dopo aver rimosso le stringhe: %s\n", output);


    temp = remove_text_inside(output, "=", ",;", true);  // Rimuove le inizializzazioni delle variabili
    free(output);
    if (!temp) return NULL;
    output = temp;
    
    printf("Dopo aver rimosso le inizializzazioni : %s\n", output);


    temp = remove_chars(output, "*");  // Rimuove gli * dai nomi delle variabili puntatore
    free(output);
    if (!temp) return NULL;
    output = temp;
    
    printf("Dopo aver rimosso gli * dai nomi dei puntatori : %s\n", output);

    return output;
}

int main() {
    const char* input = 
        "int d = main(), ***c = 3;";

    char* cleaned_output = clean_string(input);

    if (cleaned_output) {
        printf("Output:\n%s\n", cleaned_output);
        free(cleaned_output); // Libera la memoria allocata
    } else {
        printf("Errore nell'elaborazione della stringa.\n");
    }

    return 0;
}