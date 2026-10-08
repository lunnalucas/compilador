#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.tab.h"  // Reemplaza a tokens.h (generado por Bison)
#include "lexer.h"

// Declaración externa de la función del parser generada por Bison
extern int yyparse(void);

// Declaración externa de yyerror (su código está en parser.y)
void yyerror(const char *s);

#ifdef _WIN32
#include <direct.h>
#define MKDIR(d) _mkdir(d)
#else
#include <sys/stat.h>
#include <sys/types.h>
#define MKDIR(d) mkdir(d, 0755)
#endif

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Error, debe modificar el archivo de entrada\n");
        printf("Uso: %s <archivo.beta>\n", argv[0]);
        return 1;
    }
    FILE *archivo = fopen(argv[1], "r");
    if (archivo == NULL) {
        printf("Archivo vacio\n");
        return 1;
    }
    lexer_init(archivo);

    // 1. Imprimir encabezado de columnas en la consola
    printf("%-6s | %-6s | %-12s | %-20s | %-10s\n", "LINEA", "CODIGO", "TOKEN", "LEXEMA", "COL_INICIO");
    printf("------------------------------------------------------------------------\n");

    // 2. Iniciar el Analizador Sintáctico (Bison llamará a yylex() internamente)
    int resultado = yyparse();

    fclose(archivo);

    // 3. Evaluar resultado de la compilación sintáctica
    if (resultado == 0) {
        printf("\n Compilacion Sintactica Exitosa \n");
        mostrarTS();
    } else {
        printf("\n Error Sintactico en la compilacion \n");
    }

    return 0;
}