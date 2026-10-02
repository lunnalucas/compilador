#ifndef LEXER_TABLES_H
#define LEXER_TABLES_H

#include <stdio.h>

// ============================================================================
// 1. DEFINICIÓN DE CÓDIGOS DE TOKENS
// ============================================================================
#define TOKEN_ID           100
#define TOKEN_CTE_E        101
#define TOKEN_CTE_R        102
#define TOKEN_INICIO       103
#define TOKEN_ENTERO       104
#define TOKEN_REAL         105
#define TOKEN_SI           106
#define TOKEN_SINO         107
#define TOKEN_PARA         108
#define TOKEN_IMPRIMIR     109
#define TOKEN_RETORNAR     110
#define TOKEN_ASIG         111
#define TOKEN_IGUAL        112
#define TOKEN_DISTINTO     113
#define TOKEN_MENOR        114
#define TOKEN_MAYOR        115
#define TOKEN_MENOR_IGUAL  116
#define TOKEN_MAYOR_IGUAL  117
#define TOKEN_AND          118
#define TOKEN_OR           119
#define TOKEN_SUMA         120
#define TOKEN_RESTA        121
#define TOKEN_PRODUCTO     122
#define TOKEN_DIVISION     123
#define TOKEN_PAR_ABRE     124
#define TOKEN_PAR_CIERRA   125
#define TOKEN_LLAVE_ABRE   126
#define TOKEN_LLAVE_CIERRA 127
#define TOKEN_P_COMA       128
#define TOKEN_COMA         129
#define TOKEN_PUNTO        130
#define TOKEN_COMILLA      131
#define TOKEN_CADENA       132

// Código especial asignado cuando se detecta un error léxico
#define ERROR_LEXICO       -2

// Dimensiones de la matriz del Autómata Finito Determinista (AFD)
#define NUM_ESTADOS  28
#define NUM_COLUMNAS 22

// ============================================================================
// 2. ENUMERACIÓN DE ACCIONES SEMÁNTICAS (AS)
// ============================================================================
typedef enum {
    ACT_NADA = 0,            // No realiza acción sobre el buffer
    ACT_INICIAR_ID,          // Vacía buffer y guarda primer carácter del ID
    ACT_AGREGAR_ID,          // Agrega un carácter al ID (máx 20)
    ACT_FIN_ID,              // Cierra ID y verifica si es palabra reservada
    ACT_INICIAR_ENTERO,      // Vacía buffer e inicia constante entera
    ACT_AGREGAR_ENTERO,      // Agrega dígitos al entero
    ACT_FIN_ENTERO,          // Finaliza constante entera
    ACT_INICIAR_REAL,        // Vacía buffer e inicia constante real
    ACT_AGREGAR_REAL,        // Agrega dígitos o punto al real
    ACT_FIN_REAL,            // Finaliza constante real
    ACT_INICIAR_SALIDA,      // Inicia captura de cadena de texto entre comillas
    ACT_AGREGAR_SALIDA,      // Acumula caracteres dentro de la cadena
    ACT_FIN_SALIDA,          // Finaliza la cadena de texto
    ACT_IGNORAR_COMENTARIO,  // Descarta caracteres pertenecientes a comentarios
    ACT_ERROR                // Emite mensaje de error léxico
} AccionSemantica;

// ============================================================================
// 3. ESTRUCTURA Y TABLAS DEL AUTÓMATA
// ============================================================================
typedef struct {
    const char *lexema;
    int token;
} PalabraReservada;

// Declaración de variables globales compartidas con el lexer
extern const int nuevo_estado[NUM_ESTADOS][NUM_COLUMNAS];
extern const int unread_matriz[NUM_ESTADOS][NUM_COLUMNAS];
extern const int token_matriz[NUM_ESTADOS][NUM_COLUMNAS];
extern const AccionSemantica proceso[NUM_ESTADOS][NUM_COLUMNAS];
extern const PalabraReservada TABLA_RESERVADAS[];

// Prototipos de funciones auxiliares para mapear caracteres a columnas del AFD
void init_tabla_eventos(void);
int get_evento(int c);
static int buscar_palabra_reservada(const char *lexema);

#endif // LEXER_TABLES_H