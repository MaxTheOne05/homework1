#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "funzioni.c"
#include "variabili.c"


/*IDEA: Facciamo un while. finhe c'è un typedef nel testo troviamo l'indice in cui inizia typedef e sostituisce ogni carattere con uno spazio (tranne gli \n). 
  In questo modo preserviamo gli indici associati ad ogni parola. Poi prendiamo la nuova stringa e */

int find_first_separator_index(const char* input_string, const char* terminators, size_t index) {
    if (input_string == NULL) {
        return -1; // Gestisce il caso di stringa di input NULL
    }

    //Verifica che l'indice sia valido
    size_t input_length = strlen(input_string);
    if (index >= input_length) {
        fprintf(stderr, "Indice fuori dai limiti della stringa.\n");
        return -1;
    }

    // Trova la prima occorrenza di un qualsiasi carattere terminatore a partire da index
    const char *current_char = input_string + index;

    while (*current_char != '\0') {
        if (strchr(terminators, *current_char) != NULL) {
            return (int)(current_char - input_string); // Restituisce l'indice del separatore
        }
        current_char++;
    }

    return -1; // Nessun separatore trovato
}


char* remove_type_declarations(const char* str) {
    char* output = strdup(str);
    if (!output) return NULL;


    char *temp = remove_text_inside(output, "{", "}", false);  // Rimuove tutto ciò che si trova all'interno delle parentesi
    free(output);
    if (!temp) return NULL;
    output = temp;

    printf("Dopo aver rimosso tutto quello che si trova tra {...}: %s\n", output);

    temp = extract_substring(output, ";", 0);
    free(output);
    if (!temp) return NULL;
    output = temp;

    printf("Dopo aver estratto la prima dichiarazione di tipo: %s\n", output);

    return output;
}


void* add_custom_types(const char* str) {
    
    split_string(str, " ,\t\n", false);
    
    
}


int main() {
    char *filename = "test_new_types.c";
    char *file_content = leggi_da_filename(filename);

    if (file_content == NULL) {
        fprintf(stderr, "Errore nella lettura del file.\n");
        return 1;
    }

    char* result = remove_type_declarations(file_content);
    free(file_content);

    if (result != NULL) {
        printf("Contenuto del file: %s\n", result);
        free(result);
    } else {
        fprintf(stderr, "Errore nella rimozione delle dichiarazioni di tipo.\n");
    }

    return 0;
}