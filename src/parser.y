%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#ifdef _WIN32
#include <io.h>
extern int _fileno(FILE *);
#else
#include <unistd.h>
#endif

typedef struct {
    int codigo;
    int linea;
    int columna;
    char mensaje[256];
} ErrorSintactico;

static ErrorSintactico *errores = NULL;
static size_t cantidad_errores = 0;
static size_t capacidad_errores = 0;
static int error_memoria = 0;

static void registrar_error(int codigo, const char *mensaje) {
    if (cantidad_errores == capacidad_errores) {
        size_t nueva = capacidad_errores == 0 ? 8 : capacidad_errores * 2;
        ErrorSintactico *tmp = realloc(errores, nueva * sizeof(*errores));
        if (tmp == NULL) {
            error_memoria = 1;
            return;
        }
        errores = tmp;
        capacidad_errores = nueva;
    }
    errores[cantidad_errores].codigo = codigo;
    errores[cantidad_errores].linea = yy_linea;
    errores[cantidad_errores].columna = yy_col_ini;
    snprintf(errores[cantidad_errores].mensaje,
             sizeof(errores[cantidad_errores].mensaje), "%s", mensaje);
    cantidad_errores++;
}

static void informar_errores_sintacticos(void) {
    for (size_t i = 0; i < cantidad_errores; i++) {
        fprintf(stderr, "Error E%d en %d:%d: %s\n",
                errores[i].codigo, errores[i].linea, errores[i].columna,
                errores[i].mensaje);
    }
    if (error_memoria)
        fprintf(stderr, "No se pudo reservar memoria para errores sintacticos\n");
}

static int errores_sintacticos(void) {
    return cantidad_errores != 0 || error_memoria;
}

static void imprimir_traza(const char *nombre) {
    FILE *archivo = fopen(nombre, "r");
    char linea[1024];
    if (archivo == NULL) return;
    while (fgets(linea, sizeof(linea), archivo) != NULL)
        fputs(linea, stdout);
    fclose(archivo);
}

static void error_cantidad(const char *nombre, int cantidad) {
    char mensaje[256];
    if (cantidad == 0) {
        snprintf(mensaje, sizeof(mensaje),
                 "la funcion '%s' debe tener 1 o 2 parametros",
                 nombre[0] ? nombre : "<desconocida>");
        registrar_error(10, mensaje);
    } else {
        snprintf(mensaje, sizeof(mensaje),
                 "la funcion '%s' supera el maximo de 2 parametros/argumentos",
                 nombre[0] ? nombre : "<desconocida>");
        registrar_error(9, mensaje);
    }
}

void yyerror(const char *mensaje) {
    registrar_error(5, mensaje);
}
%}

%start programa
%debug

%union {
    int cantidad;
    char texto[8192];
}

%token <texto> ID    100
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

%type <cantidad> parametros argumentos

%nonassoc LOWER_THAN_SINO
%nonassoc SINO
%nonassoc DECLARACION_ID
%left COMA
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
      { if ($4 == 0 || $4 > 2) error_cantidad($2, $4); }
    ;

parametros
    : /* vacio */ { $$ = 0; }
    | tipo ID { $$ = 1; }
    | tipo ID COMA tipo ID { $$ = 2; }
    | tipo ID COMA tipo ID COMA tipo ID { $$ = 3; }
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
    | sentencias_i error P_COMA
    | error P_COMA
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
    | sentencias error P_COMA
    | error P_COMA
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
      { if ($3 > 2) error_cantidad($1, $3); }
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
    : expresion %prec COMA { $$ = 1; }
    | argumentos COMA expresion { $$ = $1 + 1; }
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
    yydebug = 1;

    FILE *traza = fopen("parser_debug.log", "w");
    if (traza == NULL) {
        fprintf(stderr, "No se pudo crear parser_debug.log\n");
        fclose(archivo);
        return 1;
    }

#ifdef _WIN32
    int stderr_copia = _dup(_fileno(stderr));
    if (stderr_copia < 0 || _dup2(_fileno(traza), _fileno(stderr)) < 0) {
        fprintf(stderr, "No se pudo redirigir la traza del parser\n");
        fclose(traza);
        fclose(archivo);
        return 1;
    }
#else
    int stderr_copia = dup(fileno(stderr));
    if (stderr_copia < 0 || dup2(fileno(traza), fileno(stderr)) < 0) {
        fprintf(stderr, "No se pudo redirigir la traza del parser\n");
        fclose(traza);
        fclose(archivo);
        return 1;
    }
#endif

    int resultado_sintactico = yyparse();
    fflush(stderr);
#ifdef _WIN32
    _dup2(stderr_copia, _fileno(stderr));
    _close(stderr_copia);
#else
    dup2(stderr_copia, fileno(stderr));
    close(stderr_copia);
#endif
    fclose(traza);

    imprimir_traza("parser_debug.log");
    informar_errores_sintacticos();
    int errores_lexicos = lexer_hubo_errores();
    int errores_informados = informar_errores();
    fclose(archivo);

    if (resultado_sintactico != 0 || errores_sintacticos() ||
        errores_lexicos != 0 || errores_informados != 0) {
        fprintf(stderr, "Compilacion completa con errores\n");
        free(errores);
        return 1;
    }

    printf("Compilacion completa sin errores\n");
    free(errores);
    return 0;
}