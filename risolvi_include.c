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

    const char* current_pos = input;
    while (*current_pos) {
        // Cerca la direttiva #include
        if (strncmp(current_pos, "#include", 8) == 0 && isspace(current_pos[8])) {
            const char* include_start = current_pos + 8;
            
            // Salta gli spazi bianchi
            while (isspace(*include_start)) include_start++;
            
            // Estrai il nome del file (supporta sia "file" che <file>)
            char filename[MAX_PATH_LENGTH];
            int i = 0;
            
            if (*include_start == '"' || *include_start == '<') {
                char quote_char = *include_start;
                include_start++; // salta il carattere di quotazione iniziale
                
                while (*include_start && *include_start != quote_char && i < MAX_PATH_LENGTH - 1) {
                    filename[i++] = *include_start++;
                }
                
                if (*include_start == quote_char) include_start++; // salta il carattere di quotazione finale
            } else {
                // Se non ci sono quotazioni, leggi fino al prossimo spazio/newline
                while (*include_start && !isspace(*include_start) && i < MAX_PATH_LENGTH - 1) {
                    filename[i++] = *include_start++;
                }
            }
            
            filename[i] = '\0';
            
            // Leggi il file incluso
            char* included_content = read_file(filename);
            if (included_content) {
                strcat(result, included_content);
                free(included_content);
            } else {
                fprintf(stderr, "Warning: could not include file '%s'\n", filename);
            }
            
            // Aggiorna la posizione corrente
            current_pos = include_start;
        } else {
            // Copia il carattere corrente nel risultato
            char temp[2] = {*current_pos, '\0'};
            strcat(result, temp);
            current_pos++;
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