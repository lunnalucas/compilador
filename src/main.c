#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

typedef struct {
    int codigo;
    int linea;
    int columna;
    int es_advertencia;
    char mensaje[256];
    char lexema[8192];
} Diagnostico;

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

/* Guarda un diagnóstico y aumenta el arreglo cuando se queda sin espacio. */
static int guardar_diagnostico(Diagnostico **diagnosticos, size_t *cantidad,
                               size_t *capacidad, int codigo, int linea,
                               int columna, int es_advertencia,
                               const char *mensaje, const char *lexema) {
    if (*cantidad == *capacidad) {
        size_t nueva_capacidad = *capacidad == 0 ? 8 : *capacidad * 2;
        if (nueva_capacidad < *capacidad ||
            nueva_capacidad > (size_t)-1 / sizeof(**diagnosticos)) {
            return 0;
        }

        Diagnostico *nuevo = realloc(*diagnosticos,
                                     nueva_capacidad * sizeof(**diagnosticos));
        if (nuevo == NULL) {
            return 0;
        }
        *diagnosticos = nuevo;
        *capacidad = nueva_capacidad;
    }

    Diagnostico *diagnostico = &(*diagnosticos)[(*cantidad)++];
    diagnostico->codigo = codigo;
    diagnostico->linea = linea;
    diagnostico->columna = columna;
    diagnostico->es_advertencia = es_advertencia;
    snprintf(diagnostico->mensaje, sizeof(diagnostico->mensaje), "%s", mensaje);
    snprintf(diagnostico->lexema, sizeof(diagnostico->lexema), "%s", lexema);
    return 1;
}

/* Lista juntos los errores y advertencias después de terminar la pasada léxica. */
static void imprimir_diagnosticos(const Diagnostico *diagnosticos,
                                  size_t cantidad) {
    if (cantidad == 0) {
        return;
    }

    fprintf(stderr, "Diagnosticos lexicos:\n");
    for (size_t i = 0; i < cantidad; i++) {
        const Diagnostico *diagnostico = &diagnosticos[i];
        fprintf(stderr, "%s E%d en %d:%d: %s (lexema: %s)\n",
                diagnostico->es_advertencia ? "Advertencia" : "Error",
                diagnostico->codigo, diagnostico->linea, diagnostico->columna,
                diagnostico->mensaje,
                diagnostico->lexema[0] == '\0' ? "<vacio>" : diagnostico->lexema);
    }
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
    Diagnostico *diagnosticos = NULL;
    size_t cantidad_diagnosticos = 0;
    size_t capacidad_diagnosticos = 0;
    int error_detectado = 0;
    int error_memoria = 0;
    int token;
    while ((token = yylex()) != 0) {
        if (lex_warning_code != 0 &&
            !guardar_diagnostico(&diagnosticos, &cantidad_diagnosticos,
                                 &capacidad_diagnosticos, lex_warning_code,
                                 lex_warning_line, lex_warning_col, 1,
                                 lex_warning_msg, lex_warning_lexema)) {
            error_memoria = 1;
            break;
        }

        if (token == ERROR_LEXICO) {
            error_detectado = 1;
            if (!guardar_diagnostico(&diagnosticos, &cantidad_diagnosticos,
                                     &capacidad_diagnosticos, lex_error_code,
                                     yy_linea, yy_col_ini, 0,
                                     lex_error_msg, yy_lexema)) {
                error_memoria = 1;
                break;
            }
        } else {
            imprimir_token(fout, yy_linea, token, token_nombre(token),
                           yy_lexema, yy_col_ini);
            imprimir_token(stdout, yy_linea, token, token_nombre(token),
                           yy_lexema, yy_col_ini);
        }
    }
    fclose(archivo);
    fclose(fout);

    if (error_memoria) {
        fprintf(stderr, "No se pudo reservar memoria para guardar diagnosticos lexicos\n");
        free(diagnosticos);
        return 1;
    }

    fflush(stdout);
    imprimir_diagnosticos(diagnosticos, cantidad_diagnosticos);
    free(diagnosticos);

    if (error_detectado == 0) {
        printf("Compilacion exitosa\n");
        mostrarTS();
    } else {
        printf("Analisis lexico completo con errores\n");
    }
    return 0;
}
