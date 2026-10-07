#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokens.h"
#include "lexer.h"

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

    MKDIR("out");
    FILE *fout = fopen("out/tokens.txt", "w");
    if (!fout) { printf("No se pudo crear out/tokens.txt\n"); fclose(archivo); return 1; }

    fprintf(fout, "LINEA | CODIGO | TOKEN | LEXEMA | COL_INICIO\n");
    int error_detectado = 0;
    int token;
    while ((token = yylex()) != 0) {
        if (token == ERROR_LEXICO) {
            error_detectado = 1;
            fprintf(stderr, "Error lexico E%d en %d:%d: %s (lexema: %s)\n",
                    lex_error_code, yy_linea, yy_col_ini, lex_error_msg, yy_lexema);
        } else {
            fprintf(fout, "%d | %d | %s | %s | %d\n",
                    yy_linea, token, token_nombre(token), yy_lexema, yy_col_ini);
            printf("%d | %d | %s | %s | %d\n",
                   yy_linea, token, token_nombre(token), yy_lexema, yy_col_ini);
        }
    }
    fclose(archivo);
    fclose(fout);

    if (error_detectado == 0) {
        printf("Compilacion exitosa\n");
        mostrarTS();
    } else {
        printf("Analisis lexico completo con errores\n");
    }
    return 0;
}
