#include <stdio.h>    // Per funzioni standard input/output (es. printf, FILE)
#include <stdlib.h>   // Per funzioni come exit()
#include <string.h>   // Per operazioni su stringhe (es. strtok, strcmp)
#include <ctype.h>    // Per verifiche su caratteri (es. isdigit)

#define MAX_LINE 1024 // Dimensione massima di una riga di codice letta

// Lista di tipi di variabili base che vogliamo rilevare
const char *types[] = {
    "int", "float", "double", "char", "bool", "long", "short", "unsigned", "signed"
};

const int num_types = sizeof(types) / sizeof(types[0]); // Conta quanti tipi ci sono nella lista

// Funzione che verifica se una parola è uno dei tipi riconosciuti
int is_data_type(const char *word) {
    for (int i = 0; i < num_types; i++) {
        if (strcmp(word, types[i]) == 0) {
            return 1; // È un tipo valido
        }
    }
    return 0; // Non è un tipo valido
}

// Funzione che conta le variabili dichiarate in una riga di codice
int count_vars_in_line(const char *line) {
    char buffer[MAX_LINE];
    strcpy(buffer, line); // Copia la riga in un buffer modificabile

    // Divide la riga in "pezzi" (token), separati da spazi, virgole, punti e virgola, tabulazioni, newline
    char *token = strtok(buffer, " ;,\t\n");
    int count = 0;            // Contatore delle variabili
    int in_declaration = 0;   // Flag che indica se siamo all'interno di una dichiarazione

    while (token) {
        if (is_data_type(token)) {
            in_declaration = 1; // Trovato un tipo → inizia una dichiarazione
        } else if (in_declaration) {
            // Ignora simboli di assegnazione o numeri
            if (strcmp(token, "=") == 0 || isdigit(token[0])) {
                // niente da fare
            }
            // Se troviamo una parentesi, probabilmente è una funzione → interrompi dichiarazione
            else if (strchr(token, '(')) {
                in_declaration = 0;
            } else {
                count++; // È una variabile!
            }
        }

        // Passa al token successivo
        token = strtok(NULL, " ;,\t\n");
    }

    return count; // Numero di variabili trovate nella riga
}

// Funzione che apre un file, lo legge riga per riga e conta tutte le variabili dichiarate
int count_variables(const char *filename) {
    FILE *file = fopen(filename, "r"); // Apre il file in sola lettura
    if (!file) {
        perror("Errore apertura file"); // Stampa errore se il file non si apre
        return -1;
    }

    char line[MAX_LINE];
    int total = 0; // Contatore totale

    while (fgets(line, MAX_LINE, file)) { // Legge una riga alla volta
        total += count_vars_in_line(line); // Somma le variabili trovate nella riga
    }

    fclose(file); // Chiude il file
    return total;
}

// Funzione principale
int main() {
    const char *filename = "prova.c"; // Nome del file sorgente da analizzare
    int count = count_variables(filename); // Chiama la funzione per contare le variabili

    if (count >= 0) {
        printf("Numero di variabili dichiarate: %d\n", count); // Stampa il risultato
    }

    return 0; // Fine programma
}
