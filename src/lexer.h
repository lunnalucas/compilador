#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>

/* Valor semántico y ubicación del último token / error. */
extern char yy_lexema[8192];
extern int yy_linea, yy_col_ini, yy_col_fin;
extern long yy_ival;
extern double yy_fval;

/* Diagnóstico del último error léxico (códigos E1,E2,E3,E12,E18,E19,E20). */
extern int lex_error_code;
extern char lex_error_msg[256];

/* Advertencia E4 del último identificador truncado. */
extern int lex_warning_code;
extern int lex_warning_line, lex_warning_col;
extern char lex_warning_msg[256];
extern char lex_warning_lexema[21];

/* Listo para Bison: misma firma que usará yyparse(). */
void lexer_init(FILE *f);
int yylex(void);
/* Indica si se registraron errores léxicos durante el análisis actual. */
int lexer_hubo_errores(void);
/* Imprime al final los diagnósticos acumulados por las llamadas a yylex(). */
int informar_errores(void);

/* Tabla de símbolos mínima de esta fase: dump de identificadores. */
void mostrarTS(void);

#endif
