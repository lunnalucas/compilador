#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include "lexer_tables.h"

// Función principal que lee el archivo y devuelve el siguiente token reconocido
int yylex(FILE *archivo);

// Función para obtener el texto (lexema) asociado al último token analizado
const char* get_lexema_actual(void);

// Función para obtener el número de línea actual donde se encuentra el token
int get_linea_actual(void);

#endif // LEXER_H