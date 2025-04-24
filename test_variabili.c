int   global_var,   1bad_global;   // 1 non corretta (1bad_global)
float temp,   valid_float;         // Tutte corrette

int main() {
    int local1 = 1, local-2, local3;  // 1 non corretta (local-2)
    char *str1 = "ciaociao", *str2, *@str3;    // 1 non corretta (@str3)
    double d1, d2, d3;            // Tutte corrette
    
    /* Variabili con spaziatura irregolare */
    short   s1,   s2,   s3;   int   i1,   i-2,   i3;   // 1 non corretta (i-2)
    
    return 0;
}