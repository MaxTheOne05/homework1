// Funzione per Homework1 che ignora i commenti in un file e scrive il contenuto restante in un file di output


#include <stdio.h>    // Libreria standard per input/output
#include <stdlib.h>   // Libreria per funzioni di sistema come exit()
#include <string.h>   // Libreria per funzioni sulle stringhe

#define MAX_LINE 1024 // Dimensione massima di una riga da leggere

// Funzione per rimuovere i commenti da una singola riga
int remove_comment_from_line(char *line, int in_comment_block) {
    char clean_line[MAX_LINE]; // Buffer temporaneo per costruire la riga senza commenti
    int i = 0, j = 0;          // i legge da line, j scrive su clean_line

    while (line[i] != '\0') {  // Finché non si arriva alla fine della riga
        // Rileva l'inizio di un commento su singola linea (//)
        if (!(in_comment_block) && line[i] == '/' && line[i+1] == '/') {
            break; // ignora il resto della riga
        }
        // Rileva l'inizio di un commento multilinea (/*)
        else if (!(in_comment_block) && line[i] == '/' && line[i+1] == '*') {
            in_comment_block = 1; // attiva il flag di commento attivo
            i += 2;
            continue;
        }
        // Rileva la fine di un commento multilinea (*/)
        else if (in_comment_block && line[i] == '*' && line[i+1] == '/') {
            in_comment_block = 0; // disattiva il flag
            i += 2;
            continue;
        }

        // Copia il carattere solo se non siamo all'interno di un commento
        if (!(in_comment_block)) {
            clean_line[j++] = line[i];
        }
        i++;
    }

    clean_line[j] = '\0'; // Termina la stringa

    // Rimuove eventuale newline se è alla fine e inutile
    size_t len = strlen(clean_line);
    if (len > 0 && clean_line[len - 1] == '\n') {
        clean_line[len - 1] = '\0';
    }

    strcpy(line, clean_line); // Copia il risultato pulito nella stringa originale
    return in_comment_block;
}

// Funzione principale per processare un file ed eliminare i commenti
void remove_comments(const char *input_file, const char *output_file) {
    FILE *fin = fopen(input_file, "r"); // Apre il file sorgente in lettura
    if (!fin) {
        perror("Errore apertura file in input");
        exit(EXIT_FAILURE); // termina se errore
    }

    FILE *fout = fopen(output_file, "w"); // Apre il file di destinazione in scrittura
    if (!fout) {
        fclose(fin);
        perror("Errore apertura file in output");
        exit(EXIT_FAILURE); // termina se errore
    }

    char line[MAX_LINE];         // buffer per ogni riga letta
    int in_comment_block = 0;    // flag per sapere se siamo in un commento multilinea

    while (fgets(line, MAX_LINE, fin)) { // legge riga per riga finché ce ne sono
        // Se la riga contiene commenti o siamo dentro un blocco, la processiamo
        if (strstr(line, "//") || strstr(line, "/*") || in_comment_block) {
            in_comment_block = remove_comment_from_line(line, in_comment_block); // rimuove i commenti
        }

        fprintf(fout, "%s", line); // scrive la riga risultante

        // Se fgets non legge newline, lo aggiungiamo noi (caso di riga lunga)
        if (!strchr(line, '\n')) {
            fprintf(fout, "\n");
        }
    }

    fclose(fin);  // chiude il file di input
    fclose(fout); // chiude il file di output
}

// Funzione principale del programma
int main() {
    remove_comments("prova.c", "risultato.c"); // chiama la funzione con i file indicati
    printf("File senza commenti salvato in output.c\n"); // messaggio di conferma
    return 0;
}
