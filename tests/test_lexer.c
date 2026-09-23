#include <stdio.h>
#include <stdlib.h>
#include "../src/lexico/lexer.h"

int main(void)
{
    const char *filename = "prueba.uno";
    FILE *f_in = fopen(filename, "r");

    if (!f_in) {
        fprintf(stderr, "Error: No se pudo abrir el archivo de prueba '%s'.\n", filename);
        return EXIT_FAILURE;
    }

    lexer_init(f_in);

    printf("=== INICIO DE ANÁLISIS LÉXICO ('%s') ===\n", filename);
    printf("%-10s %-10s %-20s\n", "LÍNEA", "TOKEN ID", "TEXTO (yytext)");
    printf("--------------------------------------------\n");

    int tok;
    while ((tok = yylex()) > 0) {
        printf("%-10d %-10d %-20s\n", yylineno, tok, yytext);
    }

    printf("--------------------------------------------\n");
    printf("=== FIN DE ANÁLISIS LÉXICO ===\n");

    fclose(f_in);
    return EXIT_SUCCESS;
}