%{
#include <stdio.h>
#include "lexer.h"

#define yyerror(mensaje) fprintf(stderr, "Error E5: %s\n", (mensaje))
%}

%start programa

%token ID            100
%token CTE_E         101
%token CTE_R         102
%token INICIO        103
%token ENTERO        104
%token REAL          105
%token SI            106
%token SINO          107
%token PARA          108
%token IMPRIMIR      109
%token RETORNAR      110
%token ASIG          111
%token IGUAL         112
%token DISTINTO      113
%token MENOR         114
%token MAYOR         115
%token MENOR_IGUAL   116
%token MAYOR_IGUAL   117
%token AND           118
%token OR            119
%token SUMA          120
%token RESTA         121
%token PRODUCTO      122
%token DIVISION      123
%token PAR_ABRE      124
%token PAR_CIERRA    125
%token LLAVE_ABRE    126
%token LLAVE_CIERRA  127
%token P_COMA        128
%token COMA          129
%token PUNTO         130
%token COMILLA       131
%token CADENA        132

%nonassoc LOWER_THAN_SINO
%nonassoc SINO
%nonassoc DECLARACION_ID
%nonassoc P_COMA

%%

programa
    : unidades principal
    | principal
    ;

principal
    : INICIO bloque_i
    ;

unidades
    : unidades unidad
    | unidad
    ;

unidad
    : tipo ID PAR_ABRE parametros PAR_CIERRA bloque
    ;

parametros
    : tipo ID
    | tipo ID COMA tipo ID
    ;

bloque
    : LLAVE_ABRE sentencias LLAVE_CIERRA
    ;

bloque_i
    : LLAVE_ABRE sentencias_i LLAVE_CIERRA
    ;

sentencias_i
    : sentencias_i sentencia_i
    | sentencia_i
    ;

sentencia_i
    : declaracion
    | asignacion
    | seleccion
    | bucle
    | salida
    ;

sentencias
    : sentencias sentencia
    | sentencia
    ;

sentencia
    : declaracion
    | asignacion
    | seleccion
    | bucle
    | salida
    | retorno
    ;

declaracion
    : tipo ID ASIG expresion P_COMA
    | tipo ids P_COMA
    | tipo ID P_COMA
    ;

ids
    : ids COMA ID
    | ID %prec DECLARACION_ID
    ;

asignacion
    : ID ASIG expresion P_COMA
    | ID ASIG invocacion P_COMA
    ;

invocacion
    : ID PAR_ABRE argumentos PAR_CIERRA
    ;

seleccion
    : SI PAR_ABRE condicional PAR_CIERRA bloque %prec LOWER_THAN_SINO
    | SI PAR_ABRE condicional PAR_CIERRA bloque SINO bloque
    ;

condicional
    : condicional OR t_condicional
    | t_condicional
    ;

t_condicional
    : t_condicional AND f_condicional
    | f_condicional
    ;

f_condicional
    : condicion
    | PAR_ABRE condicional PAR_CIERRA
    ;

condicion
    : expresion comparador expresion
    ;

comparador
    : IGUAL
    | DISTINTO
    | MENOR
    | MAYOR
    | MENOR_IGUAL
    | MAYOR_IGUAL
    ;

bucle
    : PARA PAR_ABRE control P_COMA condicional P_COMA CTE_E PAR_CIERRA bloque
    ;

control
    : ENTERO ID ASIG CTE_E
    | ID ASIG CTE_E
    ;

salida
    : IMPRIMIR PAR_ABRE elementos_salida PAR_CIERRA P_COMA
    ;

elementos_salida
    : elementos_salida COMA elemento_salida
    | elemento_salida
    ;

elemento_salida
    : CADENA
    | expresion
    | invocacion
    ;

retorno
    : RETORNAR expresion P_COMA
    ;

argumentos
    : expresion COMA expresion
    | expresion
    ;

expresion
    : expresion SUMA termino
    | expresion RESTA termino
    | termino
    ;

termino
    : termino PRODUCTO factor
    | termino DIVISION factor
    | factor
    ;

factor
    : ID
    | CTE_E
    | CTE_R
    | PAR_ABRE expresion PAR_CIERRA
    ;

tipo
    : ENTERO
    | REAL
    ;

%%

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <archivo.beta>\n", argv[0]);
        return 1;
    }

    FILE *archivo = fopen(argv[1], "r");
    if (archivo == NULL) {
        fprintf(stderr, "No se pudo abrir el archivo de entrada: %s\n", argv[1]);
        return 1;
    }

    lexer_init(archivo);
    int resultado_sintactico = yyparse();
    int errores_lexicos = lexer_hubo_errores();
    int errores_informados = informar_errores();

    fclose(archivo);

    if (resultado_sintactico != 0 || errores_lexicos != 0 ||
        errores_informados != 0) {
        fprintf(stderr, "Compilacion completa con errores\n");
        return 1;
    }

    printf("Compilacion completa sin errores\n");
    return 0;
}