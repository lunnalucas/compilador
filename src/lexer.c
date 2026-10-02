#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"
#include "lexer_tables.h"

// Búfer estático de 256 posiciones para almacenar el lexema
static char buffer_lexema[256];
static int pos_buffer = 0;
static int linea_actual = 1;
static int linea_inicio_token = 1;

int get_linea_actual(void) {
    return linea_inicio_token;
}
const char* get_lexema_actual(void) {
    return buffer_lexema;
}

static void vaciar_buffer(void) {
    memset(buffer_lexema, 0, sizeof(buffer_lexema));
    pos_buffer = 0;
}

static void agregar_al_buffer(int c) {
    if (c == EOF || c == '\0') return;
    if (pos_buffer < 255) {
        buffer_lexema[pos_buffer++] = (char)c;
        buffer_lexema[pos_buffer] = '\0';
    }
}

static int buscar_palabra_reservada(const char *lex) {
    for (int i = 0; TABLA_RESERVADAS[i].lexema != NULL; i++) {
        if (strcmp(lex, TABLA_RESERVADAS[i].lexema) == 0) {
            return TABLA_RESERVADAS[i].token;
        }
    }
    return TOKEN_ID;
}

static void ejecutar_accion(AccionSemantica act, int c, int estado, int columna) {
    switch (act) {
        case ACT_INICIAR_ID:
        case ACT_INICIAR_ENTERO:
            vaciar_buffer();
            agregar_al_buffer(c);
            break;

        case ACT_INICIAR_REAL:
            agregar_al_buffer(c);
            break;

        case ACT_AGREGAR_ID:
            if (pos_buffer < 20) {
                agregar_al_buffer(c);
            } else if (pos_buffer == 20) {
                printf("[E4] Advertencia: Identificador truncado a 20 caracteres\n");
                pos_buffer++;
            }
            break;

        case ACT_FIN_ID:
            if (strlen(buffer_lexema) > 20) {
                buffer_lexema[20] = '\0';
                pos_buffer = 20;
            }
            break;

        case ACT_AGREGAR_ENTERO:
        case ACT_AGREGAR_REAL:
            agregar_al_buffer(c);
            break;

        case ACT_INICIAR_SALIDA:
            vaciar_buffer();
            break;

        case ACT_AGREGAR_SALIDA:
            if (c != '"' && c != EOF) {
                agregar_al_buffer(c);
            }
            break;

        case ACT_FIN_ENTERO:
        case ACT_FIN_REAL:
        case ACT_FIN_SALIDA:
        case ACT_IGNORAR_COMENTARIO:
            break;

      case ACT_NADA:
            // Solo agregar al búfer si estamos en estado 0 (inicio del token)
            // y no es espacio/salto de línea (columna 20) ni EOF
            if (estado == 0 && c != EOF && c != '\0' && columna != 20) {
                agregar_al_buffer(c);
            }
            break;

        case ACT_ERROR:
            if (c != EOF && c != '\0') {
                printf("[E1] Error léxico: carácter no válido '%c'\n", c);
            }
            break;
    }
}

int yylex(FILE *archivo) {
    int estado = 0;
    int estado_anterior = 0;
    int columna_anterior = 0;
    int c = 0;
    int columna = 0;

    vaciar_buffer();

    // Bucle principal según el pseudocódigo oficial
    while (estado != -1 && estado != -2) {
        c = fgetc(archivo);

        if (c == EOF) {
            if (estado == 0) {
                return EOF;
            }
            columna = 21; // Columna EOF de las matrices
        } else {
            columna = get_evento(c);
        }

        if (c == '\n') {
            linea_actual++;
        }

        // Ignorar espacios y saltos de línea iniciales en estado 0
        if (estado == 0 && columna == 20) {
            continue;
        }

        if (estado == 0) {
            linea_inicio_token = linea_actual;
        }


        estado_anterior = estado;
        columna_anterior = columna;

        // 1. Ejecutar acción semántica de la matriz 'proceso'
        ejecutar_accion(proceso[estado][columna], c, estado, columna);

        // 2. Transición de estado mediante la matriz 'nuevo_estado'
        estado = nuevo_estado[estado][columna];
    }

    // 3. Des-lectura (unread) indicada por 'unread_matriz'
    if (columna_anterior >= 0 && columna_anterior < NUM_COLUMNAS) {
        if (unread_matriz[estado_anterior][columna_anterior] == 1) {
            if (c != EOF) {
                ungetc(c, archivo);
                if (c == '\n') {
                    linea_actual--;
                }
            }
        }
    }

    if (estado == -2) {
        return ERROR_LEXICO;
    }

    // 4. Retorno de token desde 'token_matriz'
    if (columna_anterior >= 0 && columna_anterior < NUM_COLUMNAS) {
        int tok = token_matriz[estado_anterior][columna_anterior];
        if (tok == TOKEN_ID) {
            return buscar_palabra_reservada(buffer_lexema);
        }
        return tok;
    }

    return ERROR_LEXICO;
}