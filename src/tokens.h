#ifndef TOKENS_H
#define TOKENS_H

/* Códigos de token — spec 01-diseno §4. Mismos valores para futuro Bison. */
#define ID          100
#define CTE_E       101
#define CTE_R       102
#define INICIO      103
#define ENTERO      104
#define REAL        105
#define SI          106
#define SINO        107
#define PARA        108
#define IMPRIMIR    109
#define RETORNAR    110
#define ASIG        111
#define IGUAL       112
#define DISTINTO    113
#define MENOR       114
#define MAYOR       115
#define MENOR_IGUAL 116
#define MAYOR_IGUAL 117
#define AND         118
#define OR          119
#define SUMA        120
#define RESTA       121
#define PRODUCTO    122
#define DIVISION    123
#define PAR_ABRE    124
#define PAR_CIERRA  125
#define LLAVE_ABRE  126
#define LLAVE_CIERRA 127
#define P_COMA      128
#define COMA        129
#define PUNTO       130
#define COMILLA     131
#define CADENA      132

#define YYEOF 0
#define ERROR_LEXICO -1

const char *token_nombre(int tok);

#endif
