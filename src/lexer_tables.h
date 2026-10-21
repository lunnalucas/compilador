#ifndef LEXER_TABLES_H
#define LEXER_TABLES_H

#define LEXER_ESTADOS 28
#define LEXER_EVENTOS 22

/* Acciones semánticas que la matriz de procesos ejecuta por estado y evento. */
void hacer_nada(int c);
void ignorar_comentario(int c);
void iniciar_id(int c);
void agregar_id(int c);
void fin_id(int c);
void iniciar_entero(int c);
void agregar_entero(int c);
void agregar_real(int c);
void fin_entero(int c);
void fin_real(int c);
void iniciar_salida(int c);
void agregar_salida(int c);
void fin_salida(int c);
void ERROR(int c);

/* Tablas que definen las transiciones y acciones del autómata léxico. */
extern const int nuevo_estado[LEXER_ESTADOS][LEXER_EVENTOS];
extern void (*const proceso[LEXER_ESTADOS][LEXER_EVENTOS])(int);
extern const int unread_mat[LEXER_ESTADOS][LEXER_EVENTOS];
extern const int token_matriz[LEXER_ESTADOS][LEXER_EVENTOS];

/* Clasifica un carácter en la columna correspondiente de las matrices. */
int get_evento(int c);

#endif
