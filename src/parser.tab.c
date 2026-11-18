/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "src/parser.y"

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

#line 282 "src/parser.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "parser.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_ID = 3,                         /* ID  */
  YYSYMBOL_CTE_E = 4,                      /* CTE_E  */
  YYSYMBOL_CTE_R = 5,                      /* CTE_R  */
  YYSYMBOL_INICIO = 6,                     /* INICIO  */
  YYSYMBOL_ENTERO = 7,                     /* ENTERO  */
  YYSYMBOL_REAL = 8,                       /* REAL  */
  YYSYMBOL_SI = 9,                         /* SI  */
  YYSYMBOL_SINO = 10,                      /* SINO  */
  YYSYMBOL_PARA = 11,                      /* PARA  */
  YYSYMBOL_IMPRIMIR = 12,                  /* IMPRIMIR  */
  YYSYMBOL_RETORNAR = 13,                  /* RETORNAR  */
  YYSYMBOL_ASIG = 14,                      /* ASIG  */
  YYSYMBOL_IGUAL = 15,                     /* IGUAL  */
  YYSYMBOL_DISTINTO = 16,                  /* DISTINTO  */
  YYSYMBOL_MENOR = 17,                     /* MENOR  */
  YYSYMBOL_MAYOR = 18,                     /* MAYOR  */
  YYSYMBOL_MENOR_IGUAL = 19,               /* MENOR_IGUAL  */
  YYSYMBOL_MAYOR_IGUAL = 20,               /* MAYOR_IGUAL  */
  YYSYMBOL_AND = 21,                       /* AND  */
  YYSYMBOL_OR = 22,                        /* OR  */
  YYSYMBOL_SUMA = 23,                      /* SUMA  */
  YYSYMBOL_RESTA = 24,                     /* RESTA  */
  YYSYMBOL_PRODUCTO = 25,                  /* PRODUCTO  */
  YYSYMBOL_DIVISION = 26,                  /* DIVISION  */
  YYSYMBOL_PAR_ABRE = 27,                  /* PAR_ABRE  */
  YYSYMBOL_PAR_CIERRA = 28,                /* PAR_CIERRA  */
  YYSYMBOL_LLAVE_ABRE = 29,                /* LLAVE_ABRE  */
  YYSYMBOL_LLAVE_CIERRA = 30,              /* LLAVE_CIERRA  */
  YYSYMBOL_P_COMA = 31,                    /* P_COMA  */
  YYSYMBOL_COMA = 32,                      /* COMA  */
  YYSYMBOL_PUNTO = 33,                     /* PUNTO  */
  YYSYMBOL_COMILLA = 34,                   /* COMILLA  */
  YYSYMBOL_CADENA = 35,                    /* CADENA  */
  YYSYMBOL_LOWER_THAN_SINO = 36,           /* LOWER_THAN_SINO  */
  YYSYMBOL_DECLARACION_ID = 37,            /* DECLARACION_ID  */
  YYSYMBOL_YYACCEPT = 38,                  /* $accept  */
  YYSYMBOL_programa = 39,                  /* programa  */
  YYSYMBOL_principal = 40,                 /* principal  */
  YYSYMBOL_41_1 = 41,                      /* $@1  */
  YYSYMBOL_unidades = 42,                  /* unidades  */
  YYSYMBOL_unidad = 43,                    /* unidad  */
  YYSYMBOL_unidad_inicio = 44,             /* unidad_inicio  */
  YYSYMBOL_parametros = 45,                /* parametros  */
  YYSYMBOL_bloque = 46,                    /* bloque  */
  YYSYMBOL_bloque_i = 47,                  /* bloque_i  */
  YYSYMBOL_sentencias_i = 48,              /* sentencias_i  */
  YYSYMBOL_sentencia_i = 49,               /* sentencia_i  */
  YYSYMBOL_sentencias = 50,                /* sentencias  */
  YYSYMBOL_sentencia = 51,                 /* sentencia  */
  YYSYMBOL_declaracion = 52,               /* declaracion  */
  YYSYMBOL_ids = 53,                       /* ids  */
  YYSYMBOL_asignacion = 54,                /* asignacion  */
  YYSYMBOL_declaracion_inicio = 55,        /* declaracion_inicio  */
  YYSYMBOL_asignacion_inicio = 56,         /* asignacion_inicio  */
  YYSYMBOL_invocacion = 57,                /* invocacion  */
  YYSYMBOL_seleccion = 58,                 /* seleccion  */
  YYSYMBOL_seleccion_inicio = 59,          /* seleccion_inicio  */
  YYSYMBOL_fin_bloque_si = 60,             /* fin_bloque_si  */
  YYSYMBOL_condicional = 61,               /* condicional  */
  YYSYMBOL_t_condicional = 62,             /* t_condicional  */
  YYSYMBOL_f_condicional = 63,             /* f_condicional  */
  YYSYMBOL_condicion = 64,                 /* condicion  */
  YYSYMBOL_comparador = 65,                /* comparador  */
  YYSYMBOL_bucle = 66,                     /* bucle  */
  YYSYMBOL_control = 67,                   /* control  */
  YYSYMBOL_bucle_cabecera = 68,            /* bucle_cabecera  */
  YYSYMBOL_bucle_inicio = 69,              /* bucle_inicio  */
  YYSYMBOL_salida = 70,                    /* salida  */
  YYSYMBOL_elementos_salida = 71,          /* elementos_salida  */
  YYSYMBOL_elemento_salida = 72,           /* elemento_salida  */
  YYSYMBOL_retorno = 73,                   /* retorno  */
  YYSYMBOL_argumentos = 74,                /* argumentos  */
  YYSYMBOL_expresion = 75,                 /* expresion  */
  YYSYMBOL_termino = 76,                   /* termino  */
  YYSYMBOL_factor = 77,                    /* factor  */
  YYSYMBOL_tipo = 78                       /* tipo  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  11
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   180

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  38
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  41
/* YYNRULES -- Number of rules.  */
#define YYNRULES  87
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  159

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   259


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,    36,    37
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   282,   282,   283,   287,   287,   292,   293,   297,   302,
     312,   313,   318,   323,   332,   336,   340,   341,   342,   343,
     347,   348,   349,   350,   351,   355,   356,   357,   358,   362,
     363,   364,   365,   366,   367,   371,   373,   383,   393,   399,
     408,   410,   415,   426,   436,   450,   452,   457,   462,   471,
     473,   477,   479,   483,   484,   488,   493,   494,   495,   496,
     497,   498,   502,   518,   524,   533,   543,   551,   555,   556,
     560,   561,   562,   566,   571,   572,   576,   578,   580,   584,
     586,   588,   592,   593,   594,   595,   599,   600
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "ID", "CTE_E", "CTE_R",
  "INICIO", "ENTERO", "REAL", "SI", "SINO", "PARA", "IMPRIMIR", "RETORNAR",
  "ASIG", "IGUAL", "DISTINTO", "MENOR", "MAYOR", "MENOR_IGUAL",
  "MAYOR_IGUAL", "AND", "OR", "SUMA", "RESTA", "PRODUCTO", "DIVISION",
  "PAR_ABRE", "PAR_CIERRA", "LLAVE_ABRE", "LLAVE_CIERRA", "P_COMA", "COMA",
  "PUNTO", "COMILLA", "CADENA", "LOWER_THAN_SINO", "DECLARACION_ID",
  "$accept", "programa", "principal", "$@1", "unidades", "unidad",
  "unidad_inicio", "parametros", "bloque", "bloque_i", "sentencias_i",
  "sentencia_i", "sentencias", "sentencia", "declaracion", "ids",
  "asignacion", "declaracion_inicio", "asignacion_inicio", "invocacion",
  "seleccion", "seleccion_inicio", "fin_bloque_si", "condicional",
  "t_condicional", "f_condicional", "condicion", "comparador", "bucle",
  "control", "bucle_cabecera", "bucle_inicio", "salida",
  "elementos_salida", "elemento_salida", "retorno", "argumentos",
  "expresion", "termino", "factor", "tipo", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-39)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-49)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     142,   -39,   -39,   -39,    15,   -39,   142,   -39,    26,    17,
       2,   -39,   -39,   -39,    14,    44,    22,   104,   -39,    23,
      25,   -39,    31,    62,    57,    66,    69,    60,   -39,   -39,
     -39,    24,    61,   -39,    23,    96,   -39,    23,    70,   -39,
     121,    91,   -39,    26,   -39,   -39,    70,   130,    18,    77,
     -39,   -39,   -39,   -39,   -39,    24,    -7,   109,   -39,   100,
     101,   105,   145,    23,   -39,    70,   -12,   135,   -39,   -39,
     102,    -5,   110,   126,    24,    47,   -39,   -39,   -39,   -39,
     -39,   -39,   -39,   155,     8,   146,   156,   131,   -39,   -39,
     111,   -39,   122,   -39,    86,    24,    24,   -39,    24,    24,
      24,   -39,   -39,   -39,    67,    63,    70,   157,    70,   -39,
     -39,   -39,   -39,   -39,   -39,    24,   -39,   -39,   -39,   160,
     -39,   107,   133,   -39,   -39,   134,   -39,   161,   153,   -39,
     137,    18,   -39,   109,   109,   -39,   -39,   112,   122,   -39,
     135,   141,   -39,   122,   -39,   -39,   -39,    26,   -39,   166,
     -39,   -39,   -39,    24,   -39,   168,   -39,   122,   -39
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     4,    86,    87,     0,     3,     0,     7,    10,     0,
       0,     1,     2,     6,     0,     0,     0,     0,     5,     0,
      11,     9,     0,     0,     0,     0,     0,     0,    17,    20,
      21,     0,     0,    22,     0,     0,    23,     0,     0,    24,
       0,     0,     8,     0,    19,    43,     0,     0,     0,     0,
      15,    16,    82,    83,    84,     0,     0,    78,    81,    82,
       0,     0,    45,     0,    62,     0,     0,    50,    52,    53,
       0,    39,     0,     0,     0,     0,    26,    29,    30,    31,
      32,    33,    34,     0,     0,     0,     0,     0,    70,    72,
       0,    69,    71,    18,     0,     0,     0,    35,     0,     0,
       0,    41,    40,    46,     0,     0,     0,     0,     0,    56,
      57,    58,    59,    60,    61,     0,    42,    37,    36,     0,
      28,     0,     0,    14,    25,    12,    47,     0,     0,    66,
       0,     0,    85,    76,    77,    79,    80,     0,    74,    54,
      49,     0,    51,    55,    38,    73,    27,     0,    64,     0,
      67,    68,    44,     0,    65,     0,    63,    75,    13
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -39,   -39,   167,   -39,   -39,   169,   -39,   -39,   -23,   -39,
     -39,   147,   -39,    97,   -38,   -39,   -37,   -39,   -39,   144,
     -36,   -39,   -39,   -33,    71,    72,   -39,   -39,   -34,   -39,
     -39,   -39,   -29,   -39,    48,   -39,   -39,   -30,    56,    55,
       0
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,     4,     5,    10,     6,     7,     8,    14,    42,    18,
      27,    28,    75,    76,    29,    72,    30,    31,    32,    89,
      33,    34,    35,    66,    67,    68,    69,   115,    36,    87,
      37,    38,    39,    90,    91,    82,   137,    70,    57,    58,
      40
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
       9,    56,    61,    77,    78,    79,     9,    80,    15,   116,
     106,    62,    81,    84,    64,    11,    95,    96,    92,   107,
      16,    59,    53,    54,    97,    94,   117,    52,    53,    54,
     106,    17,   104,     2,     3,   105,   126,    77,    78,    79,
     103,    80,    19,    83,   121,    55,    81,    20,   122,    21,
      23,    55,    41,    88,     2,     3,    24,    43,    25,    26,
      74,    49,    44,    23,    59,    53,    54,     2,     3,    24,
     138,    25,    26,    52,    53,    54,    45,   123,   109,   110,
     111,   112,   113,   114,    46,   143,    95,    96,    55,   106,
      50,   132,    73,    47,    23,   139,    48,    65,     2,     3,
      24,    92,    25,    26,    74,    22,    63,    23,    93,    95,
      96,     2,     3,    24,   132,    25,    26,   109,   110,   111,
     112,   113,   114,   157,    71,    95,    96,   100,    95,    96,
      95,    96,   101,    85,    98,    99,   102,    86,   145,   130,
     152,   118,   119,   131,   153,    95,    96,   155,     1,     2,
       3,   133,   134,   135,   136,   -48,   108,   120,   125,   128,
     127,   141,   129,   144,   146,   148,   147,   149,   150,   154,
     156,   158,   124,    12,    51,    13,    60,   140,     0,   151,
     142
};

static const yytype_int16 yycheck[] =
{
       0,    31,    32,    41,    41,    41,     6,    41,     8,    14,
      22,    34,    41,    46,    37,     0,    23,    24,    48,    31,
       3,     3,     4,     5,    31,    55,    31,     3,     4,     5,
      22,    29,    65,     7,     8,    65,    28,    75,    75,    75,
      63,    75,    28,    43,    74,    27,    75,     3,     1,    27,
       3,    27,    29,    35,     7,     8,     9,    32,    11,    12,
      13,     1,    31,     3,     3,     4,     5,     7,     8,     9,
     100,    11,    12,     3,     4,     5,    14,    30,    15,    16,
      17,    18,    19,    20,    27,   115,    23,    24,    27,    22,
      30,    28,     1,    27,     3,    28,    27,    27,     7,     8,
       9,   131,    11,    12,    13,     1,    10,     3,    31,    23,
      24,     7,     8,     9,    28,    11,    12,    15,    16,    17,
      18,    19,    20,   153,     3,    23,    24,    27,    23,    24,
      23,    24,    31,     3,    25,    26,    31,     7,    31,    28,
      28,    31,    32,    32,    32,    23,    24,   147,     6,     7,
       8,    95,    96,    98,    99,    10,    21,    31,     3,     3,
      14,     4,    31,     3,    31,     4,    32,    14,    31,    28,
       4,     3,    75,     6,    27,     6,    32,   106,    -1,   131,
     108
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     6,     7,     8,    39,    40,    42,    43,    44,    78,
      41,     0,    40,    43,    45,    78,     3,    29,    47,    28,
       3,    27,     1,     3,     9,    11,    12,    48,    49,    52,
      54,    55,    56,    58,    59,    60,    66,    68,    69,    70,
      78,    29,    46,    32,    31,    14,    27,    27,    27,     1,
      30,    49,     3,     4,     5,    27,    75,    76,    77,     3,
      57,    75,    46,    10,    46,    27,    61,    62,    63,    64,
      75,     3,    53,     1,    13,    50,    51,    52,    54,    58,
      66,    70,    73,    78,    61,     3,     7,    67,    35,    57,
      71,    72,    75,    31,    75,    23,    24,    31,    25,    26,
      27,    31,    31,    46,    61,    75,    22,    31,    21,    15,
      16,    17,    18,    19,    20,    65,    14,    31,    31,    32,
      31,    75,     1,    30,    51,     3,    28,    14,     3,    31,
      28,    32,    28,    76,    76,    77,    77,    74,    75,    28,
      62,     4,    63,    75,     3,    31,    31,    32,     4,    14,
      31,    72,    28,    32,    28,    78,     4,    75,     3
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    38,    39,    39,    41,    40,    42,    42,    43,    44,
      45,    45,    45,    45,    46,    47,    48,    48,    48,    48,
      49,    49,    49,    49,    49,    50,    50,    50,    50,    51,
      51,    51,    51,    51,    51,    52,    52,    52,    53,    53,
      54,    54,    55,    56,    57,    58,    58,    59,    60,    61,
      61,    62,    62,    63,    63,    64,    65,    65,    65,    65,
      65,    65,    66,    67,    67,    68,    69,    70,    71,    71,
      72,    72,    72,    73,    74,    74,    75,    75,    75,    76,
      76,    76,    77,    77,    77,    77,    78,    78
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     0,     3,     2,     1,     4,     3,
       0,     2,     5,     8,     3,     3,     2,     1,     3,     2,
       1,     1,     1,     1,     1,     2,     1,     3,     2,     1,
       1,     1,     1,     1,     1,     3,     3,     3,     3,     1,
       3,     3,     3,     2,     4,     2,     3,     4,     2,     3,
       1,     3,     1,     1,     3,     3,     1,     1,     1,     1,
       1,     1,     2,     4,     3,     5,     4,     5,     3,     1,
       1,     1,     1,     3,     1,     3,     3,     3,     1,     3,
       3,     1,     1,     1,     1,     3,     1,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 4: /* $@1: %empty  */
#line 287 "src/parser.y"
             { emitir("INICIO_PROG", TIPO_VACIO); }
#line 1459 "src/parser.tab.c"
    break;

  case 5: /* principal: INICIO $@1 bloque_i  */
#line 288 "src/parser.y"
      { emitir("FIN_PROG", TIPO_VACIO); }
#line 1465 "src/parser.tab.c"
    break;

  case 8: /* unidad: unidad_inicio parametros PAR_CIERRA bloque  */
#line 298 "src/parser.y"
      { emitir("RET", TIPO_VACIO); }
#line 1471 "src/parser.tab.c"
    break;

  case 9: /* unidad_inicio: tipo ID PAR_ABRE  */
#line 303 "src/parser.y"
      {
          tipo_funcion_actual = (yyvsp[-2].tipo);
          registrar_funcion((yyvsp[-1].texto), (yyvsp[-2].tipo));
          emitir_formato("FUNC %s", (yyvsp[-1].texto), (yyvsp[-2].tipo));
          (yyval.tipo) = (yyvsp[-2].tipo);
      }
#line 1482 "src/parser.tab.c"
    break;

  case 10: /* parametros: %empty  */
#line 312 "src/parser.y"
                  { (yyval.cantidad) = 0; }
#line 1488 "src/parser.tab.c"
    break;

  case 11: /* parametros: tipo ID  */
#line 314 "src/parser.y"
      {
          registrar_simbolo((yyvsp[0].texto), (yyvsp[-1].tipo));
          emitir((yyvsp[0].texto), (yyvsp[-1].tipo)); emitir("PARAM", (yyvsp[-1].tipo)); (yyval.cantidad) = 1;
      }
#line 1497 "src/parser.tab.c"
    break;

  case 12: /* parametros: tipo ID COMA tipo ID  */
#line 319 "src/parser.y"
      {
          registrar_simbolo((yyvsp[-3].texto), (yyvsp[-4].tipo)); emitir((yyvsp[-3].texto), (yyvsp[-4].tipo)); emitir("PARAM", (yyvsp[-4].tipo));
          registrar_simbolo((yyvsp[0].texto), (yyvsp[-1].tipo)); emitir((yyvsp[0].texto), (yyvsp[-1].tipo)); emitir("PARAM", (yyvsp[-1].tipo)); (yyval.cantidad) = 2;
      }
#line 1506 "src/parser.tab.c"
    break;

  case 13: /* parametros: tipo ID COMA tipo ID COMA tipo ID  */
#line 324 "src/parser.y"
      {
          registrar_simbolo((yyvsp[-6].texto), (yyvsp[-7].tipo)); emitir((yyvsp[-6].texto), (yyvsp[-7].tipo)); emitir("PARAM", (yyvsp[-7].tipo));
          registrar_simbolo((yyvsp[-3].texto), (yyvsp[-4].tipo)); emitir((yyvsp[-3].texto), (yyvsp[-4].tipo)); emitir("PARAM", (yyvsp[-4].tipo));
          registrar_simbolo((yyvsp[0].texto), (yyvsp[-1].tipo)); emitir((yyvsp[0].texto), (yyvsp[-1].tipo)); emitir("PARAM", (yyvsp[-1].tipo)); (yyval.cantidad) = 3;
      }
#line 1516 "src/parser.tab.c"
    break;

  case 35: /* declaracion: declaracion_inicio expresion P_COMA  */
#line 372 "src/parser.y"
      { emitir_asignacion((yyvsp[-2].control).nombre, (yyvsp[-2].control).tipo, (yyvsp[-1].expresion).tipo); }
#line 1522 "src/parser.tab.c"
    break;

  case 36: /* declaracion: tipo ids P_COMA  */
#line 374 "src/parser.y"
      {
          for (int i = 0; i < (yyvsp[-1].ids).cantidad; i++) {
              int tipo = (yyvsp[-2].tipo);
              registrar_simbolo((yyvsp[-1].ids).nombres[i], tipo);
              emitir((yyvsp[-1].ids).nombres[i], tipo);
              emitir(tipo == TIPO_REAL ? "0.0" : "0", tipo == TIPO_REAL ? TIPO_REAL : TIPO_ENTERO);
              emitir("=", tipo);
          }
      }
#line 1536 "src/parser.tab.c"
    break;

  case 37: /* declaracion: tipo ID P_COMA  */
#line 384 "src/parser.y"
      {
          registrar_simbolo((yyvsp[-1].texto), (yyvsp[-2].tipo)); emitir((yyvsp[-1].texto), (yyvsp[-2].tipo));
          emitir((yyvsp[-2].tipo) == TIPO_REAL ? "0.0" : "0",
                 (yyvsp[-2].tipo) == TIPO_REAL ? TIPO_REAL : TIPO_ENTERO);
          emitir("=", (yyvsp[-2].tipo));
      }
#line 1547 "src/parser.tab.c"
    break;

  case 38: /* ids: ids COMA ID  */
#line 394 "src/parser.y"
      {
          (yyval.ids) = (yyvsp[-2].ids);
          if ((yyval.ids).cantidad < 16)
              snprintf((yyval.ids).nombres[(yyval.ids).cantidad++], sizeof((yyval.ids).nombres[0]), "%s", (yyvsp[0].texto));
      }
#line 1557 "src/parser.tab.c"
    break;

  case 39: /* ids: ID  */
#line 400 "src/parser.y"
      {
          memset(&(yyval.ids), 0, sizeof((yyval.ids)));
          snprintf((yyval.ids).nombres[0], sizeof((yyval.ids).nombres[0]), "%s", (yyvsp[0].texto));
          (yyval.ids).cantidad = 1;
      }
#line 1567 "src/parser.tab.c"
    break;

  case 40: /* asignacion: asignacion_inicio expresion P_COMA  */
#line 409 "src/parser.y"
      { emitir_asignacion((yyvsp[-2].control).nombre, (yyvsp[-2].control).tipo, (yyvsp[-1].expresion).tipo); }
#line 1573 "src/parser.tab.c"
    break;

  case 41: /* asignacion: asignacion_inicio invocacion P_COMA  */
#line 411 "src/parser.y"
      { emitir_asignacion((yyvsp[-2].control).nombre, (yyvsp[-2].control).tipo, (yyvsp[-1].expresion).tipo); }
#line 1579 "src/parser.tab.c"
    break;

  case 42: /* declaracion_inicio: tipo ID ASIG  */
#line 416 "src/parser.y"
      {
          (yyval.control) = (Control){0};
          (yyval.control).tipo = (yyvsp[-2].tipo);
          snprintf((yyval.control).nombre, sizeof((yyval.control).nombre), "%s", (yyvsp[-1].texto));
          registrar_simbolo((yyvsp[-1].texto), (yyvsp[-2].tipo));
          emitir((yyvsp[-1].texto), (yyvsp[-2].tipo));
      }
#line 1591 "src/parser.tab.c"
    break;

  case 43: /* asignacion_inicio: ID ASIG  */
#line 427 "src/parser.y"
      {
          (yyval.control) = (Control){0};
          (yyval.control).tipo = buscar_simbolo((yyvsp[-1].texto));
          snprintf((yyval.control).nombre, sizeof((yyval.control).nombre), "%s", (yyvsp[-1].texto));
          emitir((yyvsp[-1].texto), (yyval.control).tipo);
      }
#line 1602 "src/parser.tab.c"
    break;

  case 44: /* invocacion: ID PAR_ABRE argumentos PAR_CIERRA  */
#line 437 "src/parser.y"
      {
          if ((yyvsp[-1].cantidad) > 2) error_cantidad((yyvsp[-3].texto), (yyvsp[-1].cantidad));
          emitir((yyvsp[-3].texto), buscar_funcion((yyvsp[-3].texto)));
          {
              char llamada[64];
              snprintf(llamada, sizeof(llamada), "CALL %d", (yyvsp[-1].cantidad));
              emitir(llamada, buscar_funcion((yyvsp[-3].texto)));
          }
          (yyval.expresion).tipo = buscar_funcion((yyvsp[-3].texto));
      }
#line 1617 "src/parser.tab.c"
    break;

  case 45: /* seleccion: seleccion_inicio bloque  */
#line 451 "src/parser.y"
      { parchear((yyvsp[-1].cantidad), (int)cantidad_polaca); }
#line 1623 "src/parser.tab.c"
    break;

  case 46: /* seleccion: fin_bloque_si SINO bloque  */
#line 453 "src/parser.y"
      { parchear((yyvsp[-2].saltos).bi, (int)cantidad_polaca); }
#line 1629 "src/parser.tab.c"
    break;

  case 47: /* seleccion_inicio: SI PAR_ABRE condicional PAR_CIERRA  */
#line 458 "src/parser.y"
      { (yyval.cantidad) = emitir("BF ?", TIPO_VACIO); }
#line 1635 "src/parser.tab.c"
    break;

  case 48: /* fin_bloque_si: seleccion_inicio bloque  */
#line 463 "src/parser.y"
      {
          (yyval.saltos).bf = (yyvsp[-1].cantidad);
          (yyval.saltos).bi = emitir("BI ?", TIPO_VACIO);
          parchear((yyval.saltos).bf, (int)cantidad_polaca);
      }
#line 1645 "src/parser.tab.c"
    break;

  case 49: /* condicional: condicional OR t_condicional  */
#line 472 "src/parser.y"
      { emitir("|", TIPO_VACIO); (yyval.expresion).tipo = TIPO_ENTERO; }
#line 1651 "src/parser.tab.c"
    break;

  case 50: /* condicional: t_condicional  */
#line 473 "src/parser.y"
                    { (yyval.expresion) = (yyvsp[0].expresion); }
#line 1657 "src/parser.tab.c"
    break;

  case 51: /* t_condicional: t_condicional AND f_condicional  */
#line 478 "src/parser.y"
      { emitir("&", TIPO_VACIO); (yyval.expresion).tipo = TIPO_ENTERO; }
#line 1663 "src/parser.tab.c"
    break;

  case 52: /* t_condicional: f_condicional  */
#line 479 "src/parser.y"
                    { (yyval.expresion) = (yyvsp[0].expresion); }
#line 1669 "src/parser.tab.c"
    break;

  case 53: /* f_condicional: condicion  */
#line 483 "src/parser.y"
                { (yyval.expresion) = (yyvsp[0].expresion); }
#line 1675 "src/parser.tab.c"
    break;

  case 54: /* f_condicional: PAR_ABRE condicional PAR_CIERRA  */
#line 484 "src/parser.y"
                                      { (yyval.expresion) = (yyvsp[-1].expresion); }
#line 1681 "src/parser.tab.c"
    break;

  case 55: /* condicion: expresion comparador expresion  */
#line 489 "src/parser.y"
      { emitir((yyvsp[-1].texto), TIPO_VACIO); (yyval.expresion).tipo = TIPO_ENTERO; }
#line 1687 "src/parser.tab.c"
    break;

  case 56: /* comparador: IGUAL  */
#line 493 "src/parser.y"
                  { (yyval.texto) = "=="; }
#line 1693 "src/parser.tab.c"
    break;

  case 57: /* comparador: DISTINTO  */
#line 494 "src/parser.y"
                  { (yyval.texto) = "!="; }
#line 1699 "src/parser.tab.c"
    break;

  case 58: /* comparador: MENOR  */
#line 495 "src/parser.y"
                  { (yyval.texto) = "<"; }
#line 1705 "src/parser.tab.c"
    break;

  case 59: /* comparador: MAYOR  */
#line 496 "src/parser.y"
                  { (yyval.texto) = ">"; }
#line 1711 "src/parser.tab.c"
    break;

  case 60: /* comparador: MENOR_IGUAL  */
#line 497 "src/parser.y"
                  { (yyval.texto) = "<="; }
#line 1717 "src/parser.tab.c"
    break;

  case 61: /* comparador: MAYOR_IGUAL  */
#line 498 "src/parser.y"
                  { (yyval.texto) = ">="; }
#line 1723 "src/parser.tab.c"
    break;

  case 62: /* bucle: bucle_cabecera bloque  */
#line 503 "src/parser.y"
      {
          emitir((yyvsp[-1].control).nombre, (yyvsp[-1].control).tipo);
          emitir((yyvsp[-1].control).decremento, TIPO_ENTERO);
          emitir("-", (yyvsp[-1].control).tipo);
          emitir("=", (yyvsp[-1].control).tipo);
          {
              char salto[64];
              snprintf(salto, sizeof(salto), "BI %d", (yyvsp[-1].control).posicion);
              emitir(salto, TIPO_VACIO);
          }
          parchear((yyvsp[-1].control).bf, (int)cantidad_polaca);
      }
#line 1740 "src/parser.tab.c"
    break;

  case 63: /* control: ENTERO ID ASIG CTE_E  */
#line 519 "src/parser.y"
      {
          (yyval.control).tipo = TIPO_ENTERO; snprintf((yyval.control).nombre, sizeof((yyval.control).nombre), "%s", (yyvsp[-2].texto));
          (yyval.control).posicion = -1; (yyval.control).bf = -1;
          registrar_simbolo((yyvsp[-2].texto), TIPO_ENTERO); emitir((yyvsp[-2].texto), TIPO_ENTERO); emitir((yyvsp[0].texto), TIPO_ENTERO); emitir("=", TIPO_ENTERO);
      }
#line 1750 "src/parser.tab.c"
    break;

  case 64: /* control: ID ASIG CTE_E  */
#line 525 "src/parser.y"
      {
          (yyval.control).tipo = buscar_simbolo((yyvsp[-2].texto)); snprintf((yyval.control).nombre, sizeof((yyval.control).nombre), "%s", (yyvsp[-2].texto));
          (yyval.control).posicion = -1; (yyval.control).bf = -1;
          emitir((yyvsp[-2].texto), (yyval.control).tipo); emitir((yyvsp[0].texto), TIPO_ENTERO); emitir("=", (yyval.control).tipo);
      }
#line 1760 "src/parser.tab.c"
    break;

  case 65: /* bucle_cabecera: bucle_inicio condicional P_COMA CTE_E PAR_CIERRA  */
#line 534 "src/parser.y"
      {
          (yyval.control) = (yyvsp[-4].control);
          emitir("BF ?", TIPO_VACIO);
          (yyval.control).bf = (int)cantidad_polaca - 1;
          snprintf((yyval.control).decremento, sizeof((yyval.control).decremento), "%s", (yyvsp[-1].texto));
      }
#line 1771 "src/parser.tab.c"
    break;

  case 66: /* bucle_inicio: PARA PAR_ABRE control P_COMA  */
#line 544 "src/parser.y"
      {
          (yyval.control) = (yyvsp[-1].control);
          (yyval.control).posicion = (int)cantidad_polaca;
      }
#line 1780 "src/parser.tab.c"
    break;

  case 70: /* elemento_salida: CADENA  */
#line 560 "src/parser.y"
             { emitir((yyvsp[0].texto), TIPO_VACIO); emitir("IMP_CADENA", TIPO_VACIO); }
#line 1786 "src/parser.tab.c"
    break;

  case 71: /* elemento_salida: expresion  */
#line 561 "src/parser.y"
                { emitir((yyvsp[0].expresion).tipo == TIPO_REAL ? "IMP_REAL" : "IMP_ENTERO", (yyvsp[0].expresion).tipo); }
#line 1792 "src/parser.tab.c"
    break;

  case 72: /* elemento_salida: invocacion  */
#line 562 "src/parser.y"
                 { emitir((yyvsp[0].expresion).tipo == TIPO_REAL ? "IMP_REAL" : "IMP_ENTERO", (yyvsp[0].expresion).tipo); }
#line 1798 "src/parser.tab.c"
    break;

  case 73: /* retorno: RETORNAR expresion P_COMA  */
#line 567 "src/parser.y"
      { emitir("RET", tipo_funcion_actual); }
#line 1804 "src/parser.tab.c"
    break;

  case 74: /* argumentos: expresion  */
#line 571 "src/parser.y"
                           { (yyval.cantidad) = 1; }
#line 1810 "src/parser.tab.c"
    break;

  case 75: /* argumentos: argumentos COMA expresion  */
#line 572 "src/parser.y"
                                { (yyval.cantidad) = (yyvsp[-2].cantidad) + 1; }
#line 1816 "src/parser.tab.c"
    break;

  case 76: /* expresion: expresion SUMA termino  */
#line 577 "src/parser.y"
      { emitir("+", tipo_resultado((yyvsp[-2].expresion).tipo, (yyvsp[0].expresion).tipo)); (yyval.expresion).tipo = tipo_resultado((yyvsp[-2].expresion).tipo, (yyvsp[0].expresion).tipo); }
#line 1822 "src/parser.tab.c"
    break;

  case 77: /* expresion: expresion RESTA termino  */
#line 579 "src/parser.y"
      { emitir("-", tipo_resultado((yyvsp[-2].expresion).tipo, (yyvsp[0].expresion).tipo)); (yyval.expresion).tipo = tipo_resultado((yyvsp[-2].expresion).tipo, (yyvsp[0].expresion).tipo); }
#line 1828 "src/parser.tab.c"
    break;

  case 78: /* expresion: termino  */
#line 580 "src/parser.y"
              { (yyval.expresion) = (yyvsp[0].expresion); }
#line 1834 "src/parser.tab.c"
    break;

  case 79: /* termino: termino PRODUCTO factor  */
#line 585 "src/parser.y"
      { emitir("*", tipo_resultado((yyvsp[-2].expresion).tipo, (yyvsp[0].expresion).tipo)); (yyval.expresion).tipo = tipo_resultado((yyvsp[-2].expresion).tipo, (yyvsp[0].expresion).tipo); }
#line 1840 "src/parser.tab.c"
    break;

  case 80: /* termino: termino DIVISION factor  */
#line 587 "src/parser.y"
      { emitir("/", tipo_resultado((yyvsp[-2].expresion).tipo, (yyvsp[0].expresion).tipo)); (yyval.expresion).tipo = tipo_resultado((yyvsp[-2].expresion).tipo, (yyvsp[0].expresion).tipo); }
#line 1846 "src/parser.tab.c"
    break;

  case 81: /* termino: factor  */
#line 588 "src/parser.y"
             { (yyval.expresion) = (yyvsp[0].expresion); }
#line 1852 "src/parser.tab.c"
    break;

  case 82: /* factor: ID  */
#line 592 "src/parser.y"
         { emitir((yyvsp[0].texto), buscar_simbolo((yyvsp[0].texto))); (yyval.expresion).tipo = buscar_simbolo((yyvsp[0].texto)); }
#line 1858 "src/parser.tab.c"
    break;

  case 83: /* factor: CTE_E  */
#line 593 "src/parser.y"
            { emitir((yyvsp[0].texto), TIPO_ENTERO); (yyval.expresion).tipo = TIPO_ENTERO; }
#line 1864 "src/parser.tab.c"
    break;

  case 84: /* factor: CTE_R  */
#line 594 "src/parser.y"
            { emitir((yyvsp[0].texto), TIPO_REAL); (yyval.expresion).tipo = TIPO_REAL; }
#line 1870 "src/parser.tab.c"
    break;

  case 85: /* factor: PAR_ABRE expresion PAR_CIERRA  */
#line 595 "src/parser.y"
                                    { (yyval.expresion) = (yyvsp[-1].expresion); }
#line 1876 "src/parser.tab.c"
    break;

  case 86: /* tipo: ENTERO  */
#line 599 "src/parser.y"
             { (yyval.tipo) = TIPO_ENTERO; }
#line 1882 "src/parser.tab.c"
    break;

  case 87: /* tipo: REAL  */
#line 600 "src/parser.y"
           { (yyval.tipo) = TIPO_REAL; }
#line 1888 "src/parser.tab.c"
    break;


#line 1892 "src/parser.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 603 "src/parser.y"


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
