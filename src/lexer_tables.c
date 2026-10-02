#include "lexer_tables.h"

// Arreglo privado para el mapeo directo de caracteres ASCII a columnas del AFD
static int tabla_eventos[256];
static int eventos_inicializados = 0;

// Inicializa la tabla de mapeo de caracteres
void init_tabla_eventos(void) {
    for (int i = 0; i < 256; i++) tabla_eventos[i] = -1;

    // Columna 0: Letras (a-z, A-Z)
    for (char c = 'a'; c <= 'z'; c++) tabla_eventos[(unsigned char)c] = 0;
    for (char c = 'A'; c <= 'Z'; c++) tabla_eventos[(unsigned char)c] = 0;

    // Columna 1: Dígitos (0-9)
    for (char c = '0'; c <= '9'; c++) tabla_eventos[(unsigned char)c] = 1;

    // Columnas 2 a 19: Caracteres especiales y operadores
    tabla_eventos[(unsigned char)'='] = 2;  
    tabla_eventos[(unsigned char)'!'] = 3;
    tabla_eventos[(unsigned char)'<'] = 4;  
    tabla_eventos[(unsigned char)'>'] = 5;
    tabla_eventos[(unsigned char)'&'] = 6;  
    tabla_eventos[(unsigned char)'|'] = 7;
    tabla_eventos[(unsigned char)'+'] = 8;  
    tabla_eventos[(unsigned char)'-'] = 9;
    tabla_eventos[(unsigned char)'*'] = 10; 
    tabla_eventos[(unsigned char)'/'] = 11;
    tabla_eventos[(unsigned char)'('] = 12; 
    tabla_eventos[(unsigned char)')'] = 13;
    tabla_eventos[(unsigned char)'{'] = 14; 
    tabla_eventos[(unsigned char)'}'] = 15;
    tabla_eventos[(unsigned char)';'] = 16; 
    tabla_eventos[(unsigned char)','] = 17;
    tabla_eventos[(unsigned char)'.'] = 18; 
    tabla_eventos[(unsigned char)'"'] = 19;

    // Columna 20: Delimitadores y espacios en blanco
    tabla_eventos[(unsigned char)' ']  = 20; 
    tabla_eventos[(unsigned char)'\t'] = 20;
    tabla_eventos[(unsigned char)'\n'] = 20; 
    tabla_eventos[(unsigned char)'\r'] = 20;

    eventos_inicializados = 1;
}

// Devuelve el índice de columna correspondiente al carácter recibido
int get_evento(int c) {
    if (!eventos_inicializados) {
        init_tabla_eventos();
    }
    
    // Si es EOF o un entero fuera del rango ASCII
    if (c == EOF || c < 0 || c >= 256) {
        return 21; // Columna 21: Error / EOF
    }

    int col = tabla_eventos[(unsigned char)c];
    
    // Si el carácter no fue registrado en init_tabla_eventos(), va a la columna de error (21)
    return (col == -1) ? 21 : col;
}

// Tabla con las Palabras Reservadas del lenguaje Beta
const PalabraReservada TABLA_RESERVADAS[] = {
    {"inicio",   TOKEN_INICIO},
    {"entero",   TOKEN_ENTERO},
    {"real",     TOKEN_REAL},
    {"si",       TOKEN_SI},
    {"sino",     TOKEN_SINO},
    {"para",     TOKEN_PARA},
    {"imprimir", TOKEN_IMPRIMIR},
    {"retornar", TOKEN_RETORNAR},
    {NULL, 0} // Marcador de final de lista
};

// Matriz de Transición de Estados
const int nuevo_estado[NUM_ESTADOS][NUM_COLUMNAS] = {
    { 1,  2,  4,  6,  8, 10, 12, 13, 14, 15, 16, 17, 20, 21, 22, 23, 24, 25, -2, 26,  0, -1},
    { 1,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  3, -1, -1, -1},
    {-1,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-2, -2,  7, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1,  9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 18, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 19, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 27, 26, -2},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
};

// Matriz de Unread (1 = devuelve el carácter al archivo, 0 = no devuelve)
const int unread_matriz[NUM_ESTADOS][NUM_COLUMNAS] = {
    /* 0 */ {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    /* 1 */ {0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    /* 2 */ {1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1},
    /* 3 */ {1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    /* 4 */ {1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    /* 5 */ {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    /* 6 */ {1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    /* 7 */ {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    /* 8 */ {1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    /* 9 */ {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    /* 10*/ {1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    /* 11*/ {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    /* 12*/ {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    /* 13*/ {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    /* 14*/ {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    /* 15*/ {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    /* 16*/ {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    /* 17*/ {1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1},
    /* 18*/ {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    /* 19*/ {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    /* 20*/ {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    /* 21*/ {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    /* 22*/ {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    /* 23*/ {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    /* 24*/ {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    /* 25*/ {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    /* 26*/ {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    /* 27*/ {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};


// Matriz de Tokens generados por cada transición

const int token_matriz[NUM_ESTADOS][NUM_COLUMNAS] = {
    /* Estado 0  */ { -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1 },
    /* Estado 1  */ { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100 },
    /* Estado 2  */ { 101, 101, 101, 101, 101, 101, 101, 101, 101, 101, 101, 101, 101, 101, 101, 101, 101, 101,  -1, 101, 101, 101 },
    /* Estado 3  */ { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    /* Estado 4  */ { 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111 },
    /* Estado 5  */ { 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112 },
    /* Estado 6  */ {  -2,  -2,  -1,  -2,  -2,  -2,  -2,  -2,  -2,  -2,  -2,  -2,  -2,  -2,  -2,  -2,  -2,  -2,  -2,  -2,  -2,  -2 },
    /* Estado 7  */ { 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113 },
    /* Estado 8  */ { 114, 114,  -1, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114 },
    /* Estado 9  */ { 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116 },
    /* Estado 10 */ { 115, 115,  -1, 115, 115, 115, 115, 115, 115, 115, 115, 115, 115, 115, 115, 115, 115, 115, 115, 115, 115, 115 },
    /* Estado 11 */ { 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117 },
    /* Estado 12 */ { 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118 },
    /* Estado 13 */ { 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119 },
    /* Estado 14 */ { 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120 },
    /* Estado 15 */ { 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121 },
    /* Estado 16 */ { 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122 },
    /* Estado 17 */ { 123, 123, 123, 123, 123, 123, 123, 123, 123, 123,  -1, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123 },
    /* Estado 18 */ {  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1 },
    /* Estado 19 */ {  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1 },
    /* Estado 20 */ { 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124 },
    /* Estado 21 */ { 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125 },
    /* Estado 22 */ { 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126 },
    /* Estado 23 */ { 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127 },
    /* Estado 24 */ { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
    /* Estado 25 */ { 129, 129, 129, 129, 129, 129, 129, 129, 129, 129, 129, 129, 129, 129, 129, 129, 129, 129, 129, 129, 129, 129 },
    /* Estado 26 */ {-1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  132,  -1,  -1 },
    /* Estado 27 */ { 132, 132, 132, 132, 132, 132, 132, 132, 132, 132, 132, 132, 132, 132, 132, 132, 132, 132, 132, 132, 132, 132 }
};
// Matriz de Acciones Semánticas a ejecutar en cada transición
const AccionSemantica proceso[NUM_ESTADOS][NUM_COLUMNAS] = {
    { ACT_INICIAR_ID, ACT_INICIAR_ENTERO, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_ERROR, ACT_INICIAR_SALIDA, ACT_NADA, ACT_NADA },
    { ACT_AGREGAR_ID, ACT_AGREGAR_ID, ACT_FIN_ID, ACT_FIN_ID, ACT_FIN_ID, ACT_FIN_ID, ACT_FIN_ID, ACT_FIN_ID, ACT_FIN_ID, ACT_FIN_ID, ACT_FIN_ID, ACT_FIN_ID, ACT_FIN_ID, ACT_FIN_ID, ACT_FIN_ID, ACT_FIN_ID, ACT_FIN_ID, ACT_FIN_ID, ACT_FIN_ID, ACT_FIN_ID, ACT_FIN_ID, ACT_FIN_ID },
    { ACT_FIN_ENTERO, ACT_AGREGAR_ENTERO, ACT_FIN_ENTERO, ACT_FIN_ENTERO, ACT_FIN_ENTERO, ACT_FIN_ENTERO, ACT_FIN_ENTERO, ACT_FIN_ENTERO, ACT_FIN_ENTERO, ACT_FIN_ENTERO, ACT_FIN_ENTERO, ACT_FIN_ENTERO, ACT_FIN_ENTERO, ACT_FIN_ENTERO, ACT_FIN_ENTERO, ACT_FIN_ENTERO, ACT_FIN_ENTERO, ACT_FIN_ENTERO, ACT_INICIAR_REAL, ACT_FIN_ENTERO, ACT_FIN_ENTERO, ACT_FIN_ENTERO },
    { ACT_FIN_REAL, ACT_AGREGAR_REAL, ACT_FIN_REAL, ACT_FIN_REAL, ACT_FIN_REAL, ACT_FIN_REAL, ACT_FIN_REAL, ACT_FIN_REAL, ACT_FIN_REAL, ACT_FIN_REAL, ACT_FIN_REAL, ACT_FIN_REAL, ACT_FIN_REAL, ACT_FIN_REAL, ACT_FIN_REAL, ACT_FIN_REAL, ACT_FIN_REAL, ACT_FIN_REAL, ACT_FIN_REAL, ACT_FIN_REAL, ACT_FIN_REAL, ACT_FIN_REAL },
    { ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA },
    { ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA },
    { ACT_ERROR, ACT_ERROR, ACT_NADA, ACT_ERROR, ACT_ERROR, ACT_ERROR, ACT_ERROR, ACT_ERROR, ACT_ERROR, ACT_ERROR, ACT_ERROR, ACT_ERROR, ACT_ERROR, ACT_ERROR, ACT_ERROR, ACT_ERROR, ACT_ERROR, ACT_ERROR, ACT_ERROR, ACT_ERROR, ACT_ERROR, ACT_ERROR },
    { ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA },
    { ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA },
    { ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA },
    { ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA },
    { ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA },
    { ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA },
    { ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA },
    { ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA },
    { ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA },
    { ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA },
    { ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_IGNORAR_COMENTARIO, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA },
    { ACT_IGNORAR_COMENTARIO, ACT_IGNORAR_COMENTARIO, ACT_IGNORAR_COMENTARIO, ACT_IGNORAR_COMENTARIO, ACT_IGNORAR_COMENTARIO, ACT_IGNORAR_COMENTARIO, ACT_IGNORAR_COMENTARIO, ACT_IGNORAR_COMENTARIO, ACT_IGNORAR_COMENTARIO, ACT_IGNORAR_COMENTARIO, ACT_IGNORAR_COMENTARIO, ACT_IGNORAR_COMENTARIO, ACT_IGNORAR_COMENTARIO, ACT_IGNORAR_COMENTARIO, ACT_IGNORAR_COMENTARIO, ACT_IGNORAR_COMENTARIO, ACT_IGNORAR_COMENTARIO, ACT_IGNORAR_COMENTARIO, ACT_IGNORAR_COMENTARIO, ACT_IGNORAR_COMENTARIO, ACT_IGNORAR_COMENTARIO, ACT_IGNORAR_COMENTARIO },
    { ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA },
    { ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA },
    { ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA },
    { ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA },
    { ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA },
    { ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA },
    { ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA },
    { ACT_AGREGAR_SALIDA, ACT_AGREGAR_SALIDA, ACT_AGREGAR_SALIDA, ACT_AGREGAR_SALIDA, ACT_AGREGAR_SALIDA, ACT_AGREGAR_SALIDA, ACT_AGREGAR_SALIDA, ACT_AGREGAR_SALIDA, ACT_AGREGAR_SALIDA, ACT_AGREGAR_SALIDA, ACT_AGREGAR_SALIDA, ACT_AGREGAR_SALIDA, ACT_AGREGAR_SALIDA, ACT_AGREGAR_SALIDA, ACT_AGREGAR_SALIDA, ACT_AGREGAR_SALIDA, ACT_AGREGAR_SALIDA, ACT_AGREGAR_SALIDA, ACT_AGREGAR_SALIDA, ACT_FIN_SALIDA, ACT_AGREGAR_SALIDA, ACT_ERROR },
    { ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA, ACT_NADA }
};