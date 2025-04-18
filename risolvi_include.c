#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_FILE_SIZE 1048576  // 1MB max file size
#define MAX_PATH_LENGTH 1024

// Funzione per leggere un intero file in una stringa
char* read_file(const char* filename) {
    // crea un puntatore al file
    FILE* file = fopen(filename, "r");
    
    // se file == None == False stampa errore su stderr 
    if (!file) {
        perror("Error opening file");
        return NULL;
    }

    
    // NB: in C non esiste una funzione standard in per ottenere la lunghezza del file
    // sposta il putatore alla fine del file
    fseek(file, 0, SEEK_END);
    // salva la posizione del puntatore alla fine del file == lunghezza del file
    long length = ftell(file);
    // riporta il puntatore all'inzio del file
    fseek(file, 0, SEEK_SET);

    
    
    if (length > MAX_FILE_SIZE) {
        fprintf(stderr, "File too large\n");
        fclose(file);
        return NULL;
    }

    char* buffer = malloc(length + 1);
    if (!buffer) {
        fclose(file);
        return NULL;
    }

    fread(buffer, 1, length, file);
    buffer[length] = '\0';
    fclose(file);

    return buffer;
}

// Funzione principale che processa gli include
char* process_includes(const char* input) {
    char* result = malloc(MAX_FILE_SIZE);
    if (!result) return NULL;
    result[0] = '\0';

    const char* current_pos = input;                        //ci posizioniamo all'inizio del file di input per iniziare la ricerca della direttiva #include.
    while (*current_pos) {                                  //finche non raggiungiamo fine stringa

        if (strncmp(current_pos, "#include", 8) != 0) {     //Controlla se i prossimi 8 char sono "#include". 

            //se i prossimi char NON sono "#include" copiamo carattere per carattere. Tuttavia NON possiamo usare strcat
            //perche richiede di unire due stringhe, noi invece dobbiamo aggiungere un singolo carattere a result,
            result[strlen(result)] = *current_pos;  
            result[strlen(result) + 1] = '\0';      //senza strcat dobbiamo gestire manualmente il terminatore di stringa
            current_pos++;   

        } else {

            //se i prossimi char sono "#include" saltiamo la parola "#include" e gli spazi per andare a cercare il nome del file
            current_pos += 8;
            while (isspace(*current_pos)){
                current_pos++;                            
            }
            
            char filename[MAX_PATH_LENGTH];     //conterrà il nome del file
            int i = 0;                          //ci serve per scorrere filename

            current_pos++;                        //salta il carattere di quotazione iniziale
            while (*current_pos != '"' && *current_pos != '>' ) {
                filename[i++] = *current_pos++;   //copia carattere per carattere il nome del file
            }
            current_pos++;                        //salta il carattere di quotazione finale
            filename[i] = '\0';
            
            //Prende il contenuto del file da includere e lo incolla nella stringa result
            char* included_content = read_file(filename);
            strcat(result, included_content);
            free(included_content);
            //liberiamo lo spazio di included_content poiche abbiamo copiato in result e terminato         
        }
    }
    
    return result;
}

// Funzione wrapper che legge un file e processa gli include
char* read_file_with_includes(const char* filename) {
    char* file_content = read_file(filename);
    if (!file_content) return NULL;
    
    char* processed_content = process_includes(file_content);
    free(file_content);
    
    return processed_content;
}


int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }
    
    char* content = read_file_with_includes(argv[1]);
    if (content) {
        printf("Processed content:\n%s\n", content);
        free(content);
    }
    
    return 0;
}