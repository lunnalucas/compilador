#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <stddef.h>

/* Valor semántico y ubicación del último token / error. */
extern char yy_lexema[8192];
extern int yy_linea, yy_col_ini, yy_col_fin;
extern long yy_ival;
extern double yy_fval;

/* Diagnóstico del último error léxico (códigos E1,E2,E3,E12,E18,E19,E20). */
extern int lex_error_code;
extern char lex_error_msg[256];

typedef struct {
    char categoria[16];
    char codigo[24];
    int linea;
    int columna;
    char lexema[256];
    char mensaje[256];
} ErrorAnalisis;

extern int yy_prev_token_line;
extern int yy_prev_token_col_fin;

/* Listo para Bison: misma firma que usará yyparse(). */
void lexer_init(FILE *f);
int yylex(void);
void registrar_error_analisis(const char *categoria, const char *codigo,
                              int linea, int columna, const char *lexema,
                              const char *mensaje);
size_t cantidad_errores_analisis(void);
const ErrorAnalisis *obtener_error_analisis(size_t indice);
void limpiar_errores_analisis(void);
const char *token_nombre(int tok);

/* Tabla de símbolos mínima de esta fase: dump de identificadores. */
void mostrarTS(void);

#endif
