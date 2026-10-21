#include <stdio.h>
#include "tokens.h"
#include "lexer.h"

#ifdef _WIN32
#include <direct.h>
#include <windows.h>
#define MKDIR(d) _mkdir(d)
#else
#include <sys/stat.h>
#include <sys/types.h>
#define MKDIR(d) mkdir(d, 0755)
#endif

/* Imprime los encabezados con anchos que coinciden con las filas de tokens. */
static void imprimir_encabezado(FILE *salida) {
    fprintf(salida, "%3s|%3s|%-11s|%-19s|%3s\n",
            "Lin", "ID", "Tipo", "Lexema", "Col");
}

/* Mantiene cada campo bajo su encabezado y alinea también lexemas cortos. */
static void imprimir_token(FILE *salida, int linea, int codigo,
                           const char *tipo, const char *lexema, int columna) {
    fprintf(salida, "%3d|%3d|%-11s|%-19s|%3d\n",
            linea, codigo, tipo, lexema, columna);
}

/* Recorre el archivo, muestra los tokens y resume diagnósticos al finalizar. */
int main(int argc, char *argv[]) {
#ifdef _WIN32
    /* Interpreta como UTF-8 los lexemas no ASCII al imprimir en la consola. */
    if (GetConsoleOutputCP() != 0 && !SetConsoleOutputCP(CP_UTF8)) {
        fprintf(stderr, "No se pudo configurar la consola para mostrar UTF-8\n");
        return 1;
    }
#endif

    if (argc < 2) {
        fprintf(stderr, "Error, debe modificar el archivo de entrada\n");
        fprintf(stderr, "Uso: %s <archivo.beta>\n", argv[0]);
        return 1;
    }
    FILE *archivo = fopen(argv[1], "r");
    if (archivo == NULL) {
        fprintf(stderr, "No se pudo abrir el archivo de entrada: %s\n", argv[1]);
        return 1;
    }
    lexer_init(archivo);

    MKDIR("out");
    FILE *fout = fopen("out/tokens.txt", "w");
    if (!fout) {
        fprintf(stderr, "No se pudo crear out/tokens.txt\n");
        fclose(archivo);
        return 1;
    }

    imprimir_encabezado(stdout);
    imprimir_encabezado(fout);
    int error_detectado = 0;
    int token;
    while ((token = yylex()) != 0) {
        if (token == ERROR_LEXICO) {
            error_detectado = 1;
        } else {
            imprimir_token(fout, yy_linea, token, token_nombre(token),
                           yy_lexema, yy_col_ini);
            imprimir_token(stdout, yy_linea, token, token_nombre(token),
                           yy_lexema, yy_col_ini);
        }
    }
    fclose(archivo);
    fclose(fout);

    fflush(stdout);
    if (informar_errores() != 0) return 1;

    if (error_detectado == 0) {
        printf("Compilacion exitosa\n");
        mostrarTS();
    } else {
        printf("Analisis lexico completo con errores\n");
    }
    return 0;
}
