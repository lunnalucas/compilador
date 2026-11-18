%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "intermedio_types.h"
#include "assembler.h"
#ifdef _WIN32
#include <direct.h>
#define MKDIR(d) _mkdir(d)
#else
#include <sys/stat.h>
#define MKDIR(d) mkdir(d, 0755)
#endif
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

enum { TIPO_VACIO, TIPO_ENTERO, TIPO_REAL };

typedef struct {
    char nombre[8192];
    int tipo;
} Simbolo;

typedef struct {
    char elemento[8192];
    int tipo;
} CeldaPolaca;

typedef struct {
    char nombre[8192];
    int tipo;
} Funcion;

static Simbolo simbolos[2048];
static int cantidad_simbolos = 0;
static Funcion funciones[256];
static int cantidad_funciones = 0;
static CeldaPolaca *polaca = NULL;
static size_t cantidad_polaca = 0;
static size_t capacidad_polaca = 0;
static int tipo_funcion_actual = TIPO_VACIO;

static const char *nombre_tipo(int tipo) {
    if (tipo == TIPO_ENTERO) return "entero";
    if (tipo == TIPO_REAL) return "real";
    return "";
}

static int buscar_simbolo(const char *nombre) {
    for (int i = cantidad_simbolos - 1; i >= 0; i--)
        if (strcmp(simbolos[i].nombre, nombre) == 0) return simbolos[i].tipo;
    return TIPO_ENTERO;
}

static void registrar_simbolo(const char *nombre, int tipo) {
    if (cantidad_simbolos < (int)(sizeof(simbolos) / sizeof(simbolos[0]))) {
        snprintf(simbolos[cantidad_simbolos].nombre,
                 sizeof(simbolos[cantidad_simbolos].nombre), "%s", nombre);
        simbolos[cantidad_simbolos++].tipo = tipo;
    }
}

static int buscar_funcion(const char *nombre) {
    for (int i = cantidad_funciones - 1; i >= 0; i--)
        if (strcmp(funciones[i].nombre, nombre) == 0) return funciones[i].tipo;
    return TIPO_ENTERO;
}

static void registrar_funcion(const char *nombre, int tipo) {
    if (cantidad_funciones < (int)(sizeof(funciones) / sizeof(funciones[0]))) {
        snprintf(funciones[cantidad_funciones].nombre,
                 sizeof(funciones[cantidad_funciones].nombre), "%s", nombre);
        funciones[cantidad_funciones++].tipo = tipo;
    }
}

static int emitir(const char *elemento, int tipo) {
    if (cantidad_polaca == capacidad_polaca) {
        size_t nueva = capacidad_polaca == 0 ? 64 : capacidad_polaca * 2;
        CeldaPolaca *tmp = realloc(polaca, nueva * sizeof(*polaca));
        if (tmp == NULL) {
            error_memoria = 1;
            return -1;
        }
        polaca = tmp;
        capacidad_polaca = nueva;
    }
    snprintf(polaca[cantidad_polaca].elemento,
             sizeof(polaca[cantidad_polaca].elemento), "%s", elemento);
    polaca[cantidad_polaca].tipo = tipo;
    return (int)cantidad_polaca++;
}

static int emitir_formato(const char *formato, const char *texto, int tipo) {
    char elemento[8192];
    snprintf(elemento, sizeof(elemento), formato, texto);
    return emitir(elemento, tipo);
}

static void parchear(int posicion, int destino) {
    if (posicion >= 0 && (size_t)posicion < cantidad_polaca)
        snprintf(polaca[posicion].elemento, sizeof(polaca[posicion].elemento),
                 "%s%d", strncmp(polaca[posicion].elemento, "BF", 2) == 0 ? "BF " : "BI ", destino);
}

static void escribir_polaca(void) {
    FILE *salida;
    MKDIR("out");
    salida = fopen("out/polaca.txt", "w");
    if (salida == NULL) {
        fprintf(stderr, "No se pudo crear out/polaca.txt\n");
        error_memoria = 1;
        return;
    }
    for (size_t i = 0; i < cantidad_polaca; i++)
        fprintf(salida, "%zu|%s|%s\n", i, polaca[i].elemento,
                nombre_tipo(polaca[i].tipo));
    fclose(salida);
}

static int tipo_resultado(int izquierdo, int derecho) {
    return izquierdo == TIPO_REAL || derecho == TIPO_REAL
               ? TIPO_REAL : TIPO_ENTERO;
}

static void emitir_asignacion(const char *nombre, int tipo_destino,
                              int tipo_expresion) {
    (void)tipo_expresion;
    emitir("=", tipo_destino);
    registrar_simbolo(nombre, tipo_destino);
}

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

%code requires {
#include "intermedio_types.h"
}

%union {
    int cantidad;
    int tipo;
    char *texto;
    Expresion expresion;
    Control control;
    ListaIds ids;
    SaltosSeleccion saltos;
}

%token <texto> ID    100
%token <texto> CTE_E 101
%token <texto> CTE_R 102
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
%token <texto> CADENA 132

%type <cantidad> parametros argumentos
%type <tipo> tipo
%type <expresion> expresion termino factor condicional t_condicional f_condicional condicion invocacion
%type <control> control bucle_inicio bucle_cabecera declaracion_inicio asignacion_inicio
%type <ids> ids
%type <saltos> fin_bloque_si
%type <cantidad> seleccion_inicio
%type <texto> comparador
%type <tipo> unidad_inicio

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
    : INICIO { emitir("INICIO_PROG", TIPO_VACIO); } bloque_i
      { emitir("FIN_PROG", TIPO_VACIO); }
    ;

unidades
    : unidades unidad
    | unidad
    ;

unidad
    : unidad_inicio parametros PAR_CIERRA bloque
      { emitir("RET", TIPO_VACIO); }
    ;

unidad_inicio
    : tipo ID PAR_ABRE
      {
          tipo_funcion_actual = $1;
          registrar_funcion($2, $1);
          emitir_formato("FUNC %s", $2, $1);
          $$ = $1;
      }
    ;

parametros
    : /* vacio */ { $$ = 0; }
    | tipo ID
      {
          registrar_simbolo($2, $1);
          emitir($2, $1); emitir("PARAM", $1); $$ = 1;
      }
    | tipo ID COMA tipo ID
      {
          registrar_simbolo($2, $1); emitir($2, $1); emitir("PARAM", $1);
          registrar_simbolo($5, $4); emitir($5, $4); emitir("PARAM", $4); $$ = 2;
      }
    | tipo ID COMA tipo ID COMA tipo ID
      {
          registrar_simbolo($2, $1); emitir($2, $1); emitir("PARAM", $1);
          registrar_simbolo($5, $4); emitir($5, $4); emitir("PARAM", $4);
          registrar_simbolo($8, $7); emitir($8, $7); emitir("PARAM", $7); $$ = 3;
      }
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
    : declaracion_inicio expresion P_COMA
      { emitir_asignacion($1.nombre, $1.tipo, $2.tipo); }
    | tipo ids P_COMA
      {
          for (int i = 0; i < $2.cantidad; i++) {
              int tipo = $1;
              registrar_simbolo($2.nombres[i], tipo);
              emitir($2.nombres[i], tipo);
              emitir(tipo == TIPO_REAL ? "0.0" : "0", tipo == TIPO_REAL ? TIPO_REAL : TIPO_ENTERO);
              emitir("=", tipo);
          }
      }
    | tipo ID P_COMA
      {
          registrar_simbolo($2, $1); emitir($2, $1);
          emitir($1 == TIPO_REAL ? "0.0" : "0",
                 $1 == TIPO_REAL ? TIPO_REAL : TIPO_ENTERO);
          emitir("=", $1);
      }
    ;

ids
    : ids COMA ID
      {
          $$ = $1;
          if ($$.cantidad < 16)
              snprintf($$.nombres[$$.cantidad++], sizeof($$.nombres[0]), "%s", $3);
      }
    | ID %prec DECLARACION_ID
      {
          memset(&$$, 0, sizeof($$));
          snprintf($$.nombres[0], sizeof($$.nombres[0]), "%s", $1);
          $$.cantidad = 1;
      }
    ;

asignacion
    : asignacion_inicio expresion P_COMA
      { emitir_asignacion($1.nombre, $1.tipo, $2.tipo); }
    | asignacion_inicio invocacion P_COMA
      { emitir_asignacion($1.nombre, $1.tipo, $2.tipo); }
    ;

declaracion_inicio
    : tipo ID ASIG
      {
          $$ = (Control){0};
          $$.tipo = $1;
          snprintf($$.nombre, sizeof($$.nombre), "%s", $2);
          registrar_simbolo($2, $1);
          emitir($2, $1);
      }
    ;

asignacion_inicio
    : ID ASIG
      {
          $$ = (Control){0};
          $$.tipo = buscar_simbolo($1);
          snprintf($$.nombre, sizeof($$.nombre), "%s", $1);
          emitir($1, $$.tipo);
      }
    ;

invocacion
    : ID PAR_ABRE argumentos PAR_CIERRA
      {
          if ($3 > 2) error_cantidad($1, $3);
          emitir($1, buscar_funcion($1));
          {
              char llamada[64];
              snprintf(llamada, sizeof(llamada), "CALL %d", $3);
              emitir(llamada, buscar_funcion($1));
          }
          $$.tipo = buscar_funcion($1);
      }
    ;

seleccion
    : seleccion_inicio bloque %prec LOWER_THAN_SINO
      { parchear($1, (int)cantidad_polaca); }
    | fin_bloque_si SINO bloque
      { parchear($1.bi, (int)cantidad_polaca); }
    ;

seleccion_inicio
    : SI PAR_ABRE condicional PAR_CIERRA
      { $$ = emitir("BF ?", TIPO_VACIO); }
    ;

fin_bloque_si
    : seleccion_inicio bloque
      {
          $$.bf = $1;
          $$.bi = emitir("BI ?", TIPO_VACIO);
          parchear($$.bf, (int)cantidad_polaca);
      }
    ;

condicional
    : condicional OR t_condicional
      { emitir("|", TIPO_VACIO); $$.tipo = TIPO_ENTERO; }
    | t_condicional { $$ = $1; }
    ;

t_condicional
    : t_condicional AND f_condicional
      { emitir("&", TIPO_VACIO); $$.tipo = TIPO_ENTERO; }
    | f_condicional { $$ = $1; }
    ;

f_condicional
    : condicion { $$ = $1; }
    | PAR_ABRE condicional PAR_CIERRA { $$ = $2; }
    ;

condicion
    : expresion comparador expresion
      { emitir($2, TIPO_VACIO); $$.tipo = TIPO_ENTERO; }
    ;

comparador
    : IGUAL       { $$ = "=="; }
    | DISTINTO    { $$ = "!="; }
    | MENOR       { $$ = "<"; }
    | MAYOR       { $$ = ">"; }
    | MENOR_IGUAL { $$ = "<="; }
    | MAYOR_IGUAL { $$ = ">="; }
    ;

bucle
    : bucle_cabecera bloque
      {
          emitir($1.nombre, $1.tipo);
          emitir($1.decremento, TIPO_ENTERO);
          emitir("-", $1.tipo);
          emitir("=", $1.tipo);
          {
              char salto[64];
              snprintf(salto, sizeof(salto), "BI %d", $1.posicion);
              emitir(salto, TIPO_VACIO);
          }
          parchear($1.bf, (int)cantidad_polaca);
      }
    ;

control
    : ENTERO ID ASIG CTE_E
      {
          $$.tipo = TIPO_ENTERO; snprintf($$.nombre, sizeof($$.nombre), "%s", $2);
          $$.posicion = -1; $$.bf = -1;
          registrar_simbolo($2, TIPO_ENTERO); emitir($2, TIPO_ENTERO); emitir($4, TIPO_ENTERO); emitir("=", TIPO_ENTERO);
      }
    | ID ASIG CTE_E
      {
          $$.tipo = buscar_simbolo($1); snprintf($$.nombre, sizeof($$.nombre), "%s", $1);
          $$.posicion = -1; $$.bf = -1;
          emitir($1, $$.tipo); emitir($3, TIPO_ENTERO); emitir("=", $$.tipo);
      }
    ;

bucle_cabecera
    : bucle_inicio condicional P_COMA CTE_E PAR_CIERRA
      {
          $$ = $1;
          emitir("BF ?", TIPO_VACIO);
          $$.bf = (int)cantidad_polaca - 1;
          snprintf($$.decremento, sizeof($$.decremento), "%s", $4);
      }
    ;

bucle_inicio
    : PARA PAR_ABRE control P_COMA
      {
          $$ = $3;
          $$.posicion = (int)cantidad_polaca;
      }
    ;

salida
    : IMPRIMIR PAR_ABRE elementos_salida PAR_CIERRA P_COMA
    ;

elementos_salida
    : elementos_salida COMA elemento_salida
    | elemento_salida
    ;

elemento_salida
    : CADENA { emitir($1, TIPO_VACIO); emitir("IMP_CADENA", TIPO_VACIO); }
    | expresion { emitir($1.tipo == TIPO_REAL ? "IMP_REAL" : "IMP_ENTERO", $1.tipo); }
    | invocacion { emitir($1.tipo == TIPO_REAL ? "IMP_REAL" : "IMP_ENTERO", $1.tipo); }
    ;

retorno
    : RETORNAR expresion P_COMA
      { emitir("RET", tipo_funcion_actual); }
    ;

argumentos
    : expresion %prec COMA { $$ = 1; }
    | argumentos COMA expresion { $$ = $1 + 1; }
    ;

expresion
    : expresion SUMA termino
      { emitir("+", tipo_resultado($1.tipo, $3.tipo)); $$.tipo = tipo_resultado($1.tipo, $3.tipo); }
    | expresion RESTA termino
      { emitir("-", tipo_resultado($1.tipo, $3.tipo)); $$.tipo = tipo_resultado($1.tipo, $3.tipo); }
    | termino { $$ = $1; }
    ;

termino
    : termino PRODUCTO factor
      { emitir("*", tipo_resultado($1.tipo, $3.tipo)); $$.tipo = tipo_resultado($1.tipo, $3.tipo); }
    | termino DIVISION factor
      { emitir("/", tipo_resultado($1.tipo, $3.tipo)); $$.tipo = tipo_resultado($1.tipo, $3.tipo); }
    | factor { $$ = $1; }
    ;

factor
    : ID { emitir($1, buscar_simbolo($1)); $$.tipo = buscar_simbolo($1); }
    | CTE_E { emitir($1, TIPO_ENTERO); $$.tipo = TIPO_ENTERO; }
    | CTE_R { emitir($1, TIPO_REAL); $$.tipo = TIPO_REAL; }
    | PAR_ABRE expresion PAR_CIERRA { $$ = $2; }
    ;

tipo
    : ENTERO { $$ = TIPO_ENTERO; }
    | REAL { $$ = TIPO_REAL; }
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
    cantidad_polaca = 0;
    capacidad_polaca = 0;
    free(polaca);
    polaca = NULL;
    cantidad_simbolos = 0;
    cantidad_funciones = 0;
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

    escribir_polaca();
    if (error_memoria ||
        assembler_generar("out/polaca.txt", "out/programa.asm",
                          "out/traza_assembler.txt") != 0) {
        free(errores);
        free(polaca);
        return 1;
    }
    printf("Compilacion completa sin errores\n");
    free(errores);
    free(polaca);
    return 0;
}