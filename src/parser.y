%{
/* =========================================================================
   SECCIÓN 1: CABECERA Y DECLARACIONES C
   Aquí se incluyen las librerías necesarias, se declaran las estructuras
   del Árbol Sintáctico Abstracto (AST) y los prototipos de funciones.
   ========================================================================= */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Declaración de variables globales que vienen de lexer.c
extern int yy_linea;
extern int yy_col_ini;
extern char yy_lexema[8192];

/* Prototipos de funciones externas del Léxico y la Tabla de Símbolos */
int yylex(void);
void yyerror(const char *s);

/* Simulación/Prototipos de la Tabla de Símbolos (TS) */
int obtener_tipo_ts(const char* lexema); /* Devuelve 104 (ENTERO) o 105 (REAL) */
int es_tipo_real_ts(const char* lexema);
int es_tipo_entero_ts(const char* lexema);

/* -------------------------------------------------------------------------
   ESTRUCTURA DEL ÁRBOL SINTÁCTICO ABSTRACTO (AST)
   ------------------------------------------------------------------------- */
typedef enum {
    N_PROGRAMA, N_BLOQUE, N_DECLARACION, N_ASIGNACION,
    N_SELECCION, N_BUCLE, N_SALIDA, N_RETORNO, N_INVOCACION,
    N_OP_SUMA, N_OP_RESTA, N_OP_PROD, N_OP_DIV,
    N_COMP_IGUAL, N_COMP_DISTINTO, N_COMP_MENOR, N_COMP_MAYOR, 
    N_COMP_MENOR_IGUAL, N_COMP_MAYOR_IGUAL,
    N_LOG_AND, N_LOG_OR,
    N_CONV_ENTERO_A_REAL, N_CONV_REAL_A_ENTERO, /* Nodos de Conversión */
    N_ID, N_CTE_E, N_CTE_R, N_CADENA
} TipoNodo;

typedef struct NodoAST {
    TipoNodo tipo;              /* Categórica del nodo (Suma, Asignación, etc.) */
    char lexema[64];            /* Texto asociado (ej: "+", "=", "x") */
    int val_int;                /* Valor si es constante entera (104) */
    double val_real;            /* Valor si es constante real (105) */
    int tipo_datos;             /* Tipo del resultado: 104 (ENTERO) / 105 (REAL) */
    struct NodoAST *izq;        /* Subárbol izquierdo */
    struct NodoAST *der;        /* Subárbol derecho */
    struct NodoAST *sig;        /* Puntero para listas/secuencias de sentencias */
} NodoAST;

/* Prototipos de funciones auxiliares del AST */
NodoAST* crear_nodo(TipoNodo tipo, const char* lexema, NodoAST* izq, NodoAST* der);
NodoAST* envolver_conversion(NodoAST* nodo_original, TipoNodo tipo_conversion);
int es_expresion_real(NodoAST* nodo);
int es_expresion_entera(NodoAST* nodo);

/* Puntero a la raíz del Árbol Sintáctico Abstracto */
NodoAST *raiz_ast = NULL;

%}

/* -------------------------------------------------------------------------
   DECLARACIONES DE BISON (%union, %token, %type)
   ------------------------------------------------------------------------- */

%define parse.error verbose

/* Define la unión de tipos que puede transportar yylval entre el Léxico y el Parser */
%union {
    int val_int;
    double val_real;
    char cadena[64];
    struct NodoAST *nodo;
}

/* Declaración de Tokens y sus códigos oficiales del Lenguaje Beta (100–132) */
%token <cadena> ID 100
%token <val_int> CTE_E 101
%token <val_real> CTE_R 102
%token INICIO 103 ENTERO 104 REAL 105 SI 106 SINO 107 PARA 108
%token IMPRIMIR 109 RETORNAR 110 ASIG 111 IGUAL 112 DISTINTO 113
%token MENOR 114 MAYOR 115 MENOR_IGUAL 116 MAYOR_IGUAL 117
%token AND 118 OR 119 SUMA 120 RESTA 121 PRODUCTO 122 DIVISION 123
%token PAR_ABRE 124 PAR_CIERRA 125 LLAVE_ABRE 126 LLAVE_CIERRA 127
%token P_COMA 128 COMA 129 PUNTO 130 COMILLA 131
%token <cadena> CADENA 132

/* Mapeo de Símbolos No-Terminales que producen subárboles (NodoAST*) */
%type <nodo> programa principal unidades unidad parametros bloque bloque_i
%type <nodo> sentencias sentencias_i sentencia sentencia_i declaracion asignacion
%type <nodo> seleccion bucle salida retorno invocacion expresion termino factor
%type <nodo> condicional t_condicional f_condicional condicion comparador
%type <nodo> elementos_salida elemento_salida argumentos control ids tipo

/* Símbolo inicial de la gramática */
%start programa

%%
/* =========================================================================
   SECCIÓN 2: REGLAS GRAMATICALES Y ACCIONES EN C
   Aquí se definen las producciones BNF del lenguaje Beta.
   En las acciones { ... }, se construyen los nodos del AST y se aplican
   las envolulturas (wrappers) de conversión de tipos cuando corresponde.
   ========================================================================= */

programa
    : unidades principal {
        $$ = crear_nodo(N_PROGRAMA, "PROGRAMA", $1, $2);
        raiz_ast = $$;
    }
    | principal {
        $$ = crear_nodo(N_PROGRAMA, "PROGRAMA", $1, NULL);
        raiz_ast = $$;
    }
    ;

principal
    : INICIO bloque_i {
        $$ = crear_nodo(N_PROGRAMA, "INICIO", $2, NULL);
    }
    ;

unidades
    : unidades unidad {
        NodoAST* aux = $1;
        while (aux->sig != NULL) aux = aux->sig;
        aux->sig = $2;
        $$ = $1;
    }
    | unidad { $$ = $1; }
    ;

unidad
    : tipo ID PAR_ABRE parametros PAR_CIERRA bloque {
        $$ = crear_nodo(N_PROGRAMA, $2, $4, $6);
    }
    | tipo ID PAR_ABRE PAR_CIERRA bloque {
        /* Error E10: Definición de función sin parámetros */
        fprintf(stderr, "Error E10: La funcion '%s' debe tener entre 1 y 2 parametros.\n", $2);
        YYERROR;
    }
    ;

parametros
    : tipo ID { 
        $$ = crear_nodo(N_DECLARACION, $2, $1, NULL); 
    }
    | tipo ID COMA tipo ID {
        NodoAST* p1 = crear_nodo(N_DECLARACION, $2, $1, NULL);
        NodoAST* p2 = crear_nodo(N_DECLARACION, $5, $4, NULL);
        p1->sig = p2;
        $$ = p1;
    }
    | tipo ID COMA tipo ID COMA tipo ID {
        /* Error E9: Definición de función con más de 2 parámetros */
        fprintf(stderr, "Error E9: La funcion supera el maximo permitido de 2 parametros.\n");
        YYERROR;
    }
    ;

bloque
    : LLAVE_ABRE sentencias LLAVE_CIERRA { $$ = crear_nodo(N_BLOQUE, "BLOQUE", $2, NULL); }
    ;

bloque_i
    : LLAVE_ABRE sentencias_i LLAVE_CIERRA { $$ = crear_nodo(N_BLOQUE, "BLOQUE_INICIO", $2, NULL); }
    ;

sentencias_i
    : sentencias_i sentencia_i {
        NodoAST* aux = $1;
        while (aux->sig != NULL) aux = aux->sig;
        aux->sig = $2;
        $$ = $1;
    }
    | sentencia_i { $$ = $1; }
    ;

sentencia_i
    : declaracion { $$ = $1; }
    | asignacion  { $$ = $1; }
    | seleccion   { $$ = $1; }
    | bucle       { $$ = $1; }
    | salida      { $$ = $1; }
    ;

sentencias
    : sentencias sentencia {
        NodoAST* aux = $1;
        while (aux->sig != NULL) aux = aux->sig;
        aux->sig = $2;
        $$ = $1;
    }
    | sentencia { $$ = $1; }
    ;

sentencia
    : declaracion { $$ = $1; }
    | asignacion  { $$ = $1; }
    | seleccion   { $$ = $1; }
    | bucle       { $$ = $1; }
    | salida      { $$ = $1; }
    | retorno     { $$ = $1; }
    ;

declaracion
    : tipo ID ASIG expresion P_COMA {
        NodoAST* id = crear_nodo(N_ID, $2, NULL, NULL);
        NodoAST* exp = $4;

        /* CONVERSIÓN EN DECLARACIÓN CON ASIGNACIÓN:
           Si la variable es REAL (105) y la expresión es ENTERA (104), envolvemos la expresión */
        if ($1->tipo_datos == 105 && es_expresion_entera(exp)) {
            exp = envolver_conversion(exp, N_CONV_ENTERO_A_REAL);
        }
        /* Si la variable es ENTERA (104) y la expresión es REAL (105), envolvemos la expresión */
        else if ($1->tipo_datos == 104 && es_expresion_real(exp)) {
            exp = envolver_conversion(exp, N_CONV_REAL_A_ENTERO);
        }

        $$ = crear_nodo(N_DECLARACION, "DECLARACION_ASIG", id, exp);
    }
    | tipo ids P_COMA { $$ = crear_nodo(N_DECLARACION, "DECLARACION_MULTIPLES", $1, $2); }
    | tipo ID P_COMA {
        NodoAST* id = crear_nodo(N_ID, $2, NULL, NULL);
        $$ = crear_nodo(N_DECLARACION, "DECLARACION_SIMPLE", $1, id);
    }
    ;

ids
    : ids COMA ID {
        NodoAST* aux = $1;
        while (aux->sig != NULL) aux = aux->sig;
        aux->sig = crear_nodo(N_ID, $3, NULL, NULL);
        $$ = $1;
    }
    | ID { $$ = crear_nodo(N_ID, $1, NULL, NULL); }
    ;

asignacion
    : ID ASIG expresion P_COMA {
        NodoAST* id = crear_nodo(N_ID, $1, NULL, NULL);
        NodoAST* exp = $3;

        /* CONVERSIÓN EN ASIGNACIÓN SIMPLE:
           Consultamos el tipo de la variable en la Tabla de Símbolos y envolvemos la expresión */
        if (es_tipo_real_ts($1) && es_expresion_entera(exp)) {
            exp = envolver_conversion(exp, N_CONV_ENTERO_A_REAL);
        } 
        else if (es_tipo_entero_ts($1) && es_expresion_real(exp)) {
            exp = envolver_conversion(exp, N_CONV_REAL_A_ENTERO);
        }

        $$ = crear_nodo(N_ASIGNACION, "=", id, exp);
    }
    | ID ASIG invocacion P_COMA {
        NodoAST* id = crear_nodo(N_ID, $1, NULL, NULL);
        $$ = crear_nodo(N_ASIGNACION, "=", id, $3);
    }
    ;

invocacion
    : ID PAR_ABRE argumentos PAR_CIERRA {
        $$ = crear_nodo(N_INVOCACION, $1, $3, NULL);
    }
    ;

argumentos
    : expresion COMA expresion {
        $1->sig = $3;
        $$ = $1;
    }
    | expresion { $$ = $1; }
    | expresion COMA expresion COMA expresion {
        /* Error E9: Invocación a función con más de 2 argumentos */
        fprintf(stderr, "Error E9: La llamada a la funcion '%s' supera el maximo de 2 argumentos.\n", "funcion");
        YYERROR;
    }
    ;

seleccion
    : SI PAR_ABRE condicional PAR_CIERRA bloque {
        $$ = crear_nodo(N_SELECCION, "SI", $3, $5);
    }
    | SI PAR_ABRE condicional PAR_CIERRA bloque SINO bloque {
        NodoAST* cuerpo = crear_nodo(N_SELECCION, "SI_SINO_BLOQUES", $5, $7);
        $$ = crear_nodo(N_SELECCION, "SI_SINO", $3, cuerpo);
    }
    ;

condicional
    : condicional OR t_condicional { $$ = crear_nodo(N_LOG_OR, "OR", $1, $3); }
    | t_condicional { $$ = $1; }
    ;

t_condicional
    : t_condicional AND f_condicional { $$ = crear_nodo(N_LOG_AND, "AND", $1, $3); }
    | f_condicional { $$ = $1; }
    ;

f_condicional
    : condicion { $$ = $1; }
    | PAR_ABRE condicional PAR_CIERRA { $$ = $2; }
    ;

condicion
    : expresion comparador expresion {
        $2->izq = $1;
        $2->der = $3;
        $$ = $2;
    }
    ;

comparador
    : IGUAL        { $$ = crear_nodo(N_COMP_IGUAL, "==", NULL, NULL); }
    | DISTINTO     { $$ = crear_nodo(N_COMP_DISTINTO, "!=", NULL, NULL); }
    | MENOR        { $$ = crear_nodo(N_COMP_MENOR, "<", NULL, NULL); }
    | MAYOR        { $$ = crear_nodo(N_COMP_MAYOR, ">", NULL, NULL); }
    | MENOR_IGUAL  { $$ = crear_nodo(N_COMP_MENOR_IGUAL, "<=", NULL, NULL); }
    | MAYOR_IGUAL  { $$ = crear_nodo(N_COMP_MAYOR_IGUAL, ">=", NULL, NULL); }
    ;

bucle
    : PARA PAR_ABRE control P_COMA condicional P_COMA CTE_E PAR_CIERRA bloque {
        NodoAST* paso = crear_nodo(N_CTE_E, "PASO", NULL, NULL);
        paso->val_int = $7;
        NodoAST* ctrl_paso = crear_nodo(N_BUCLE, "CTRL_PASO", $3, paso);
        NodoAST* cond_bloque = crear_nodo(N_BUCLE, "COND_BLOQUE", $5, $9);
        $$ = crear_nodo(N_BUCLE, "PARA", ctrl_paso, cond_bloque);
    }
    ;

control
    : ENTERO ID ASIG CTE_E {
        NodoAST* id = crear_nodo(N_ID, $2, NULL, NULL);
        NodoAST* cte = crear_nodo(N_CTE_E, "", NULL, NULL);
        cte->val_int = $4;
        $$ = crear_nodo(N_ASIGNACION, "=", id, cte);
    }
    | ID ASIG CTE_E {
        NodoAST* id = crear_nodo(N_ID, $1, NULL, NULL);
        NodoAST* cte = crear_nodo(N_CTE_E, "", NULL, NULL);
        cte->val_int = $3;
        $$ = crear_nodo(N_ASIGNACION, "=", id, cte);
    }
    ;

salida
    : IMPRIMIR PAR_ABRE elementos_salida PAR_CIERRA P_COMA {
        $$ = crear_nodo(N_SALIDA, "IMPRIMIR", $3, NULL);
    }
    | IMPRIMIR PAR_ABRE PAR_CIERRA P_COMA {
        /* Error E5: La función imprimir() requiere parámetros */
        fprintf(stderr, "Error sintactico E5: La funcion 'imprimir' requiere parametros.\n");
        YYERROR;
    }
    ;

elementos_salida
    : elementos_salida COMA elemento_salida {
        NodoAST* aux = $1;
        while (aux->sig != NULL) aux = aux->sig;
        aux->sig = $3;
        $$ = $1;
    }
    | elemento_salida { $$ = $1; }
    ;

elemento_salida
    : CADENA { $$ = crear_nodo(N_CADENA, $1, NULL, NULL); }
    | expresion { $$ = $1; }
    | invocacion { $$ = $1; }
    ;

retorno
    : RETORNAR expresion P_COMA { $$ = crear_nodo(N_RETORNO, "RETORNAR", $2, NULL); }
    ;

expresion
    : expresion SUMA termino {
        NodoAST* izq = $1;
        NodoAST* der = $3;

        /* CONVERSIÓN EN SUMA ARITMÉTICA:
           Si mezclamos entero y real, envolvemos el lado entero con ENTERO_A_REAL */
        if (es_expresion_real(izq) && es_expresion_entera(der)) {
            der = envolver_conversion(der, N_CONV_ENTERO_A_REAL);
        } else if (es_expresion_entera(izq) && es_expresion_real(der)) {
            izq = envolver_conversion(izq, N_CONV_ENTERO_A_REAL);
        }

        $$ = crear_nodo(N_OP_SUMA, "+", izq, der);
        $$->tipo_datos = (es_expresion_real(izq) || es_expresion_real(der)) ? 105 : 104;
    }
    | expresion RESTA termino {
        NodoAST* izq = $1;
        NodoAST* der = $3;

        if (es_expresion_real(izq) && es_expresion_entera(der)) {
            der = envolver_conversion(der, N_CONV_ENTERO_A_REAL);
        } else if (es_expresion_entera(izq) && es_expresion_real(der)) {
            izq = envolver_conversion(izq, N_CONV_ENTERO_A_REAL);
        }

        $$ = crear_nodo(N_OP_RESTA, "-", izq, der);
        $$->tipo_datos = (es_expresion_real(izq) || es_expresion_real(der)) ? 105 : 104;
    }
    | termino { $$ = $1; }
    ;

termino
    : termino PRODUCTO factor {
        NodoAST* izq = $1;
        NodoAST* der = $3;

        if (es_expresion_real(izq) && es_expresion_entera(der)) {
            der = envolver_conversion(der, N_CONV_ENTERO_A_REAL);
        } else if (es_expresion_entera(izq) && es_expresion_real(der)) {
            izq = envolver_conversion(izq, N_CONV_ENTERO_A_REAL);
        }

        $$ = crear_nodo(N_OP_PROD, "*", izq, der);
        $$->tipo_datos = (es_expresion_real(izq) || es_expresion_real(der)) ? 105 : 104;
    }
    | termino DIVISION factor {
        NodoAST* izq = $1;
        NodoAST* der = $3;

        if (es_expresion_real(izq) && es_expresion_entera(der)) {
            der = envolver_conversion(der, N_CONV_ENTERO_A_REAL);
        } else if (es_expresion_entera(izq) && es_expresion_real(der)) {
            izq = envolver_conversion(izq, N_CONV_ENTERO_A_REAL);
        }

        $$ = crear_nodo(N_OP_DIV, "/", izq, der);
        $$->tipo_datos = (es_expresion_real(izq) || es_expresion_real(der)) ? 105 : 104;
    }
    | factor { $$ = $1; }
    ;

factor
    : ID { 
        $$ = crear_nodo(N_ID, $1, NULL, NULL);
        $$->tipo_datos = obtener_tipo_ts($1);
    }
    | CTE_E {
        NodoAST* n = crear_nodo(N_CTE_E, "CTE_ENTERA", NULL, NULL);
        n->val_int = $1;
        n->tipo_datos = 104; /* 104 = ENTERO */
        $$ = n;
    }
    | CTE_R {
        NodoAST* n = crear_nodo(N_CTE_R, "CTE_REAL", NULL, NULL);
        n->val_real = $1;
        n->tipo_datos = 105; /* 105 = REAL */
        $$ = n;
    }
    | PAR_ABRE expresion PAR_CIERRA { $$ = $2; }
    ;

tipo
    : ENTERO { 
        $$ = crear_nodo(N_DECLARACION, "ENTERO", NULL, NULL); 
        $$->tipo_datos = 104;
    }
    | REAL   { 
        $$ = crear_nodo(N_DECLARACION, "REAL", NULL, NULL); 
        $$->tipo_datos = 105;
    }
    ;

%%
/* =========================================================================
   SECCIÓN 3: CÓDIGO C FINAL Y FUNCIONES AUXILIARES
   Aquí se implementan las funciones para crear nodos, realizar la envoltura
   de conversiones, chequear tipos y emitir mensajes de error.
   ========================================================================= */

/* Función básica para instanciar un nodo estándar en el AST */
NodoAST* crear_nodo(TipoNodo tipo, const char* lexema, NodoAST* izq, NodoAST* der) {
    NodoAST* n = (NodoAST*)malloc(sizeof(NodoAST));
    if (!n) {
        fprintf(stderr, "Error grave: Memoria insuficiente para crear nodo AST.\n");
        exit(1);
    }
    n->tipo = tipo;
    if (lexema) strncpy(n->lexema, lexema, 63);
    else n->lexema[0] = '\0';
    n->izq = izq;
    n->der = der;
    n->sig = NULL;
    n->tipo_datos = 0;
    return n;
}

/* -------------------------------------------------------------------------
   FUNCIÓN DE ENVOLTURA (WRAPPING) PARA CONVERSIÓN DE TIPOS
   Crea un nodo de conversión que envuelve al nodo original colocándolo
   como su hijo izquierdo.
   ------------------------------------------------------------------------- */
NodoAST* envolver_conversion(NodoAST* nodo_original, TipoNodo tipo_conversion) {
    if (nodo_original == NULL) return NULL;

    NodoAST* nodo_envuelto = (NodoAST*)malloc(sizeof(NodoAST));
    if (!nodo_envuelto) {
        fprintf(stderr, "Error grave: Memoria insuficiente para nodo de conversion.\n");
        exit(1);
    }

    nodo_envuelto->tipo = tipo_conversion;
    if (tipo_conversion == N_CONV_ENTERO_A_REAL) {
        strcpy(nodo_envuelto->lexema, "ENTERO_A_REAL");
        nodo_envuelto->tipo_datos = 105; /* Se transforma en REAL */
    } else {
        strcpy(nodo_envuelto->lexema, "REAL_A_ENTERO");
        nodo_envuelto->tipo_datos = 104; /* Se transforma en ENTERO */
    }

    /* Envoltura: el nodo original pasa a ser el subárbol izquierdo */
    nodo_envuelto->izq = nodo_original;
    nodo_envuelto->der = NULL;
    nodo_envuelto->sig = NULL;

    return nodo_envuelto;
}

/* Helpers para verificar si la expresión/subárbol resulta en ENTERO o REAL */
int es_expresion_real(NodoAST* nodo) {
    if (!nodo) return 0;
    return (nodo->tipo_datos == 105);
}

int es_expresion_entera(NodoAST* nodo) {
    if (!nodo) return 0;
    return (nodo->tipo_datos == 104);
}

/* Funciones auxiliares de simulación para la Tabla de Símbolos */
int obtener_tipo_ts(const char* lexema) {
    /* Retorna 104 (ENTERO) o 105 (REAL) según corresponda en la TS */
    return 104; 
}

int es_tipo_real_ts(const char* lexema) {
    return (obtener_tipo_ts(lexema) == 105);
}

int es_tipo_entero_ts(const char* lexema) {
    return (obtener_tipo_ts(lexema) == 104);
}

/* Manejador de errores por defecto de Bison */

void yyerror(const char *s) {
    // Como yy_lexema es un arreglo, verificamos si el primer carácter es nulo
    const char *lex = (yy_lexema[0] != '\0') ? yy_lexema : "DESCONOCIDO";

    printf("\n[ERROR SINTÁCTICO] Línea %d, Columna %d: %s (cerca de '%s')\n", 
           yy_linea, yy_col_ini, s, lex);
    fflush(stdout);
}

