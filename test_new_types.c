// Enum senza typedef
enum Colore {
    ROSSO,   // avrà valore 0 di default
    VERDE,   // avrà valore 1
    BLU      // avrà valore 2
};

// Enum con typedef (anonimo)
typedef enum {
    TIPO_INT,
    TIPO_FLOAT,
    TIPO_STRINGA,
    TIPO_SCONOSCIUTO
} TipoMessaggio;

// Union con typedef (anonima)
typedef union {
    int valoreInt;
    float valoreFloat;
    char valoreStringa[50]; // Una stringa (con dimensione massima)
} ContenutoMessaggio;

// Struct con typedef (anonima) che usa l'enum e la union
typedef struct {
    TipoMessaggio tipo;      // Indica quale membro della union è valido
    ContenutoMessaggio contenuto; // La union che contiene il dato effettivo
} Messaggio;

// Crea un alias "Anno" per il tipo int
typedef int Anno;

// Crea un alias "Quantita" per il tipo int
typedef int Quantita;
