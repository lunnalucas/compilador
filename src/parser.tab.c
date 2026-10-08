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


#line 132 "src/parser.tab.c"

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
  YYSYMBOL_YYACCEPT = 36,                  /* $accept  */
  YYSYMBOL_programa = 37,                  /* programa  */
  YYSYMBOL_principal = 38,                 /* principal  */
  YYSYMBOL_unidades = 39,                  /* unidades  */
  YYSYMBOL_unidad = 40,                    /* unidad  */
  YYSYMBOL_parametros = 41,                /* parametros  */
  YYSYMBOL_bloque = 42,                    /* bloque  */
  YYSYMBOL_bloque_i = 43,                  /* bloque_i  */
  YYSYMBOL_sentencias_i = 44,              /* sentencias_i  */
  YYSYMBOL_sentencia_i = 45,               /* sentencia_i  */
  YYSYMBOL_sentencias = 46,                /* sentencias  */
  YYSYMBOL_sentencia = 47,                 /* sentencia  */
  YYSYMBOL_declaracion = 48,               /* declaracion  */
  YYSYMBOL_ids = 49,                       /* ids  */
  YYSYMBOL_asignacion = 50,                /* asignacion  */
  YYSYMBOL_invocacion = 51,                /* invocacion  */
  YYSYMBOL_argumentos = 52,                /* argumentos  */
  YYSYMBOL_seleccion = 53,                 /* seleccion  */
  YYSYMBOL_condicional = 54,               /* condicional  */
  YYSYMBOL_t_condicional = 55,             /* t_condicional  */
  YYSYMBOL_f_condicional = 56,             /* f_condicional  */
  YYSYMBOL_condicion = 57,                 /* condicion  */
  YYSYMBOL_comparador = 58,                /* comparador  */
  YYSYMBOL_bucle = 59,                     /* bucle  */
  YYSYMBOL_control = 60,                   /* control  */
  YYSYMBOL_salida = 61,                    /* salida  */
  YYSYMBOL_elementos_salida = 62,          /* elementos_salida  */
  YYSYMBOL_elemento_salida = 63,           /* elemento_salida  */
  YYSYMBOL_retorno = 64,                   /* retorno  */
  YYSYMBOL_expresion = 65,                 /* expresion  */
  YYSYMBOL_termino = 66,                   /* termino  */
  YYSYMBOL_factor = 67,                    /* factor  */
  YYSYMBOL_tipo = 68                       /* tipo  */
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

#if 1

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
#endif /* 1 */

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
#define YYLAST   189

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  36
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  33
/* YYNRULES -- Number of rules.  */
#define YYNRULES  77
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  149

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   257


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
       2,     2,     2,     2,     2,     2,     1,     2
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   107,   107,   111,   118,   124,   130,   134,   137,   145,
     148,   154,   162,   166,   170,   176,   180,   181,   182,   183,
     184,   188,   194,   198,   199,   200,   201,   202,   203,   207,
     223,   224,   231,   237,   241,   256,   263,   269,   273,   274,
     282,   285,   292,   293,   297,   298,   302,   303,   307,   315,
     316,   317,   318,   319,   320,   324,   334,   340,   349,   352,
     360,   366,   370,   371,   372,   376,   380,   395,   408,   412,
     425,   438,   442,   446,   452,   458,   462,   466
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if 1
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
  "PUNTO", "COMILLA", "CADENA", "$accept", "programa", "principal",
  "unidades", "unidad", "parametros", "bloque", "bloque_i", "sentencias_i",
  "sentencia_i", "sentencias", "sentencia", "declaracion", "ids",
  "asignacion", "invocacion", "argumentos", "seleccion", "condicional",
  "t_condicional", "f_condicional", "condicion", "comparador", "bucle",
  "control", "salida", "elementos_salida", "elemento_salida", "retorno",
  "expresion", "termino", "factor", "tipo", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-67)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     140,    -9,   -67,   -67,    37,   -67,   140,   -67,    67,    50,
     -67,   -67,   -67,   -67,    -5,   113,    55,   129,   130,    80,
     -67,   -67,   -67,   -67,   -67,   -67,   126,    11,     8,    28,
      87,    21,   -67,   -67,     3,    19,   112,   131,   155,   133,
     -67,   -67,    81,   132,    43,    86,   -67,   -67,    28,    74,
     141,   -67,   -67,   116,   147,   161,   134,   135,   -67,   -67,
      65,   -67,    91,    81,   -67,   -67,   164,    92,   -67,   112,
     136,    81,   100,   -67,    81,    81,   -67,    81,    81,    85,
     102,    28,   112,    28,   -67,   -67,   -67,   -67,   -67,   -67,
      81,   165,   156,    28,   -67,   143,    25,   114,   -67,    81,
      68,   -67,   -67,   -67,   -67,   -67,   -67,   -67,   -67,   142,
     144,    22,   -67,    86,    86,   -67,   -67,   -67,   141,   163,
     -67,    91,   -67,   167,    -8,   -67,   -67,   -67,   120,   -67,
     -67,   172,   -67,    81,   112,   -67,   173,   -67,   146,    41,
     -67,   148,   142,    81,   112,   176,    91,   -67,   -67
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,    76,    77,     0,     3,     0,     6,     0,     0,
       4,     1,     2,     5,     0,     0,     0,     0,     0,     0,
      15,    16,    17,    18,    19,    20,     0,     0,     0,     0,
       0,     0,    13,    14,    33,     0,     0,     0,     0,    72,
      73,    74,     0,     0,     0,    68,    71,    72,     0,     0,
      43,    45,    46,     0,     0,     0,     0,     0,    62,    64,
       0,    61,    63,     0,    31,    30,     0,     0,     8,     0,
       9,     0,     0,    35,     0,     0,    34,     0,     0,     0,
       0,     0,     0,     0,    49,    50,    51,    52,    53,    54,
       0,     0,     0,     0,    59,     0,     0,     0,    32,     0,
       0,    22,    23,    24,    25,    26,    27,    28,     7,     0,
       0,    38,    75,    66,    67,    69,    70,    47,    42,    40,
      44,    48,    57,     0,     0,    58,    60,    29,     0,    12,
      21,     0,    36,     0,     0,    56,     0,    65,    10,    37,
      41,     0,     0,     0,     0,     0,    39,    55,    11
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -67,   -67,   174,   -67,   175,   -67,   -66,   -67,   -67,   166,
     -67,    82,   -62,   -67,   -60,   158,   -67,   -59,   -46,   103,
     104,   -67,   -67,   -58,   -67,   -57,   -67,    93,   -67,   -27,
      78,    77,     0
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     4,     5,     6,     7,    37,    68,    10,    19,    20,
     100,   101,    21,    35,    22,    59,   110,    23,    49,    50,
      51,    52,    90,    24,    56,    25,    60,    61,   107,    53,
      45,    46,    26
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
       8,    44,    79,   108,    62,   102,     8,   103,   104,   105,
     106,    39,    40,    41,    81,    72,   119,    63,     2,     3,
       9,    80,    27,   136,    39,    40,    41,    38,    39,    40,
      41,    47,    40,    41,    64,    42,    97,    11,   102,    36,
     103,   104,   105,   106,   111,    74,    75,   124,    42,    57,
      65,    66,    42,    15,   133,    48,    58,     2,     3,    16,
      58,    17,    18,   121,    74,    75,    74,    75,   140,    62,
      14,    15,   128,   143,    76,     2,     3,    16,   147,    17,
      18,    99,    29,    15,    47,    40,    41,     2,     3,    16,
      54,    17,    18,    95,    55,    15,    81,    96,   129,     2,
       3,    16,    82,    17,    18,    99,   139,    81,    42,   131,
      32,    77,    78,   117,    74,    75,   146,    84,    85,    86,
      87,    88,    89,    74,    75,    74,    75,    28,   112,    34,
     112,    84,    85,    86,    87,    88,    89,    74,    75,    74,
      75,    67,   145,    74,    75,   127,     1,     2,     3,     2,
       3,   137,   113,   114,   115,   116,    30,    31,    70,    69,
      71,    91,    83,    73,    92,    93,    94,    98,   109,   122,
     123,   135,   132,   134,   125,   138,   144,   141,   142,   148,
      12,    13,   130,     0,   118,    33,    43,   120,     0,   126
};

static const yytype_int16 yycheck[] =
{
       0,    28,    48,    69,    31,    67,     6,    67,    67,    67,
      67,     3,     4,     5,    22,    42,    82,    14,     7,     8,
      29,    48,    27,    31,     3,     4,     5,    27,     3,     4,
       5,     3,     4,     5,    31,    27,    63,     0,   100,    28,
     100,   100,   100,   100,    71,    23,    24,    93,    27,    28,
      31,    32,    27,     3,    32,    27,    35,     7,     8,     9,
      35,    11,    12,    90,    23,    24,    23,    24,   134,    96,
       3,     3,    99,    32,    31,     7,     8,     9,   144,    11,
      12,    13,    27,     3,     3,     4,     5,     7,     8,     9,
       3,    11,    12,    28,     7,     3,    22,    32,    30,     7,
       8,     9,    28,    11,    12,    13,   133,    22,    27,   109,
      30,    25,    26,    28,    23,    24,   143,    15,    16,    17,
      18,    19,    20,    23,    24,    23,    24,    14,    28,     3,
      28,    15,    16,    17,    18,    19,    20,    23,    24,    23,
      24,    29,   142,    23,    24,    31,     6,     7,     8,     7,
       8,    31,    74,    75,    77,    78,    27,    27,     3,    28,
      27,    14,    21,    31,     3,    31,    31,     3,    32,     4,
      14,     4,    28,    10,    31,     3,    28,     4,    32,     3,
       6,     6,   100,    -1,    81,    19,    28,    83,    -1,    96
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     6,     7,     8,    37,    38,    39,    40,    68,    29,
      43,     0,    38,    40,     3,     3,     9,    11,    12,    44,
      45,    48,    50,    53,    59,    61,    68,    27,    14,    27,
      27,    27,    30,    45,     3,    49,    28,    41,    68,     3,
       4,     5,    27,    51,    65,    66,    67,     3,    27,    54,
      55,    56,    57,    65,     3,     7,    60,    28,    35,    51,
      62,    63,    65,    14,    31,    31,    32,    29,    42,    28,
       3,    27,    65,    31,    23,    24,    31,    25,    26,    54,
      65,    22,    28,    21,    15,    16,    17,    18,    19,    20,
      58,    14,     3,    31,    31,    28,    32,    65,     3,    13,
      46,    47,    48,    50,    53,    59,    61,    64,    42,    32,
      52,    65,    28,    66,    66,    67,    67,    28,    55,    42,
      56,    65,     4,    14,    54,    31,    63,    31,    65,    30,
      47,    68,    28,    32,    10,     4,    31,    31,     3,    65,
      42,     4,    32,    32,    28,    68,    65,    42,     3
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    36,    37,    37,    38,    39,    39,    40,    40,    41,
      41,    41,    42,    43,    44,    44,    45,    45,    45,    45,
      45,    46,    46,    47,    47,    47,    47,    47,    47,    48,
      48,    48,    49,    49,    50,    50,    51,    52,    52,    52,
      53,    53,    54,    54,    55,    55,    56,    56,    57,    58,
      58,    58,    58,    58,    58,    59,    60,    60,    61,    61,
      62,    62,    63,    63,    63,    64,    65,    65,    65,    66,
      66,    66,    67,    67,    67,    67,    68,    68
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     2,     2,     1,     6,     5,     2,
       5,     8,     3,     3,     2,     1,     1,     1,     1,     1,
       1,     2,     1,     1,     1,     1,     1,     1,     1,     5,
       3,     3,     3,     1,     4,     4,     4,     3,     1,     5,
       5,     7,     3,     1,     3,     1,     1,     3,     3,     1,
       1,     1,     1,     1,     1,     9,     4,     3,     5,     4,
       3,     1,     1,     1,     1,     3,     3,     3,     1,     3,
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


/* Context of a parse error.  */
typedef struct
{
  yy_state_t *yyssp;
  yysymbol_kind_t yytoken;
} yypcontext_t;

/* Put in YYARG at most YYARGN of the expected tokens given the
   current YYCTX, and return the number of tokens stored in YYARG.  If
   YYARG is null, return the number of expected tokens (guaranteed to
   be less than YYNTOKENS).  Return YYENOMEM on memory exhaustion.
   Return 0 if there are more than YYARGN expected tokens, yet fill
   YYARG up to YYARGN. */
static int
yypcontext_expected_tokens (const yypcontext_t *yyctx,
                            yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  int yyn = yypact[+*yyctx->yyssp];
  if (!yypact_value_is_default (yyn))
    {
      /* Start YYX at -YYN if negative to avoid negative indexes in
         YYCHECK.  In other words, skip the first -YYN actions for
         this state because they are default actions.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;
      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yyx;
      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
        if (yycheck[yyx + yyn] == yyx && yyx != YYSYMBOL_YYerror
            && !yytable_value_is_error (yytable[yyx + yyn]))
          {
            if (!yyarg)
              ++yycount;
            else if (yycount == yyargn)
              return 0;
            else
              yyarg[yycount++] = YY_CAST (yysymbol_kind_t, yyx);
          }
    }
  if (yyarg && yycount == 0 && 0 < yyargn)
    yyarg[0] = YYSYMBOL_YYEMPTY;
  return yycount;
}




#ifndef yystrlen
# if defined __GLIBC__ && defined _STRING_H
#  define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
# else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
# endif
#endif

#ifndef yystpcpy
# if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#  define yystpcpy stpcpy
# else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
# endif
#endif

#ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
      char const *yyp = yystr;
      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
#endif


static int
yy_syntax_error_arguments (const yypcontext_t *yyctx,
                           yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yyctx->yytoken != YYSYMBOL_YYEMPTY)
    {
      int yyn;
      if (yyarg)
        yyarg[yycount] = yyctx->yytoken;
      ++yycount;
      yyn = yypcontext_expected_tokens (yyctx,
                                        yyarg ? yyarg + 1 : yyarg, yyargn - 1);
      if (yyn == YYENOMEM)
        return YYENOMEM;
      else
        yycount += yyn;
    }
  return yycount;
}

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return -1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return YYENOMEM if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                const yypcontext_t *yyctx)
{
  enum { YYARGS_MAX = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  yysymbol_kind_t yyarg[YYARGS_MAX];
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* Actual size of YYARG. */
  int yycount = yy_syntax_error_arguments (yyctx, yyarg, YYARGS_MAX);
  if (yycount == YYENOMEM)
    return YYENOMEM;

  switch (yycount)
    {
#define YYCASE_(N, S)                       \
      case N:                               \
        yyformat = S;                       \
        break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
    }

  /* Compute error message size.  Don't count the "%s"s, but reserve
     room for the terminator.  */
  yysize = yystrlen (yyformat) - 2 * yycount + 1;
  {
    int yyi;
    for (yyi = 0; yyi < yycount; ++yyi)
      {
        YYPTRDIFF_T yysize1
          = yysize + yytnamerr (YY_NULLPTR, yytname[yyarg[yyi]]);
        if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
          yysize = yysize1;
        else
          return YYENOMEM;
      }
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return -1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yytname[yyarg[yyi++]]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}


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

  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;

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
  case 2: /* programa: unidades principal  */
#line 107 "src/parser.y"
                         {
        (yyval.nodo) = crear_nodo(N_PROGRAMA, "PROGRAMA", (yyvsp[-1].nodo), (yyvsp[0].nodo));
        raiz_ast = (yyval.nodo);
    }
#line 1561 "src/parser.tab.c"
    break;

  case 3: /* programa: principal  */
#line 111 "src/parser.y"
                {
        (yyval.nodo) = crear_nodo(N_PROGRAMA, "PROGRAMA", (yyvsp[0].nodo), NULL);
        raiz_ast = (yyval.nodo);
    }
#line 1570 "src/parser.tab.c"
    break;

  case 4: /* principal: INICIO bloque_i  */
#line 118 "src/parser.y"
                      {
        (yyval.nodo) = crear_nodo(N_PROGRAMA, "INICIO", (yyvsp[0].nodo), NULL);
    }
#line 1578 "src/parser.tab.c"
    break;

  case 5: /* unidades: unidades unidad  */
#line 124 "src/parser.y"
                      {
        NodoAST* aux = (yyvsp[-1].nodo);
        while (aux->sig != NULL) aux = aux->sig;
        aux->sig = (yyvsp[0].nodo);
        (yyval.nodo) = (yyvsp[-1].nodo);
    }
#line 1589 "src/parser.tab.c"
    break;

  case 6: /* unidades: unidad  */
#line 130 "src/parser.y"
             { (yyval.nodo) = (yyvsp[0].nodo); }
#line 1595 "src/parser.tab.c"
    break;

  case 7: /* unidad: tipo ID PAR_ABRE parametros PAR_CIERRA bloque  */
#line 134 "src/parser.y"
                                                    {
        (yyval.nodo) = crear_nodo(N_PROGRAMA, (yyvsp[-4].cadena), (yyvsp[-2].nodo), (yyvsp[0].nodo));
    }
#line 1603 "src/parser.tab.c"
    break;

  case 8: /* unidad: tipo ID PAR_ABRE PAR_CIERRA bloque  */
#line 137 "src/parser.y"
                                         {
        /* Error E10: Definición de función sin parámetros */
        fprintf(stderr, "Error E10: La funcion '%s' debe tener entre 1 y 2 parametros.\n", (yyvsp[-3].cadena));
        YYERROR;
    }
#line 1613 "src/parser.tab.c"
    break;

  case 9: /* parametros: tipo ID  */
#line 145 "src/parser.y"
              { 
        (yyval.nodo) = crear_nodo(N_DECLARACION, (yyvsp[0].cadena), (yyvsp[-1].nodo), NULL); 
    }
#line 1621 "src/parser.tab.c"
    break;

  case 10: /* parametros: tipo ID COMA tipo ID  */
#line 148 "src/parser.y"
                           {
        NodoAST* p1 = crear_nodo(N_DECLARACION, (yyvsp[-3].cadena), (yyvsp[-4].nodo), NULL);
        NodoAST* p2 = crear_nodo(N_DECLARACION, (yyvsp[0].cadena), (yyvsp[-1].nodo), NULL);
        p1->sig = p2;
        (yyval.nodo) = p1;
    }
#line 1632 "src/parser.tab.c"
    break;

  case 11: /* parametros: tipo ID COMA tipo ID COMA tipo ID  */
#line 154 "src/parser.y"
                                        {
        /* Error E9: Definición de función con más de 2 parámetros */
        fprintf(stderr, "Error E9: La funcion supera el maximo permitido de 2 parametros.\n");
        YYERROR;
    }
#line 1642 "src/parser.tab.c"
    break;

  case 12: /* bloque: LLAVE_ABRE sentencias LLAVE_CIERRA  */
#line 162 "src/parser.y"
                                         { (yyval.nodo) = crear_nodo(N_BLOQUE, "BLOQUE", (yyvsp[-1].nodo), NULL); }
#line 1648 "src/parser.tab.c"
    break;

  case 13: /* bloque_i: LLAVE_ABRE sentencias_i LLAVE_CIERRA  */
#line 166 "src/parser.y"
                                           { (yyval.nodo) = crear_nodo(N_BLOQUE, "BLOQUE_INICIO", (yyvsp[-1].nodo), NULL); }
#line 1654 "src/parser.tab.c"
    break;

  case 14: /* sentencias_i: sentencias_i sentencia_i  */
#line 170 "src/parser.y"
                               {
        NodoAST* aux = (yyvsp[-1].nodo);
        while (aux->sig != NULL) aux = aux->sig;
        aux->sig = (yyvsp[0].nodo);
        (yyval.nodo) = (yyvsp[-1].nodo);
    }
#line 1665 "src/parser.tab.c"
    break;

  case 15: /* sentencias_i: sentencia_i  */
#line 176 "src/parser.y"
                  { (yyval.nodo) = (yyvsp[0].nodo); }
#line 1671 "src/parser.tab.c"
    break;

  case 16: /* sentencia_i: declaracion  */
#line 180 "src/parser.y"
                  { (yyval.nodo) = (yyvsp[0].nodo); }
#line 1677 "src/parser.tab.c"
    break;

  case 17: /* sentencia_i: asignacion  */
#line 181 "src/parser.y"
                  { (yyval.nodo) = (yyvsp[0].nodo); }
#line 1683 "src/parser.tab.c"
    break;

  case 18: /* sentencia_i: seleccion  */
#line 182 "src/parser.y"
                  { (yyval.nodo) = (yyvsp[0].nodo); }
#line 1689 "src/parser.tab.c"
    break;

  case 19: /* sentencia_i: bucle  */
#line 183 "src/parser.y"
                  { (yyval.nodo) = (yyvsp[0].nodo); }
#line 1695 "src/parser.tab.c"
    break;

  case 20: /* sentencia_i: salida  */
#line 184 "src/parser.y"
                  { (yyval.nodo) = (yyvsp[0].nodo); }
#line 1701 "src/parser.tab.c"
    break;

  case 21: /* sentencias: sentencias sentencia  */
#line 188 "src/parser.y"
                           {
        NodoAST* aux = (yyvsp[-1].nodo);
        while (aux->sig != NULL) aux = aux->sig;
        aux->sig = (yyvsp[0].nodo);
        (yyval.nodo) = (yyvsp[-1].nodo);
    }
#line 1712 "src/parser.tab.c"
    break;

  case 22: /* sentencias: sentencia  */
#line 194 "src/parser.y"
                { (yyval.nodo) = (yyvsp[0].nodo); }
#line 1718 "src/parser.tab.c"
    break;

  case 23: /* sentencia: declaracion  */
#line 198 "src/parser.y"
                  { (yyval.nodo) = (yyvsp[0].nodo); }
#line 1724 "src/parser.tab.c"
    break;

  case 24: /* sentencia: asignacion  */
#line 199 "src/parser.y"
                  { (yyval.nodo) = (yyvsp[0].nodo); }
#line 1730 "src/parser.tab.c"
    break;

  case 25: /* sentencia: seleccion  */
#line 200 "src/parser.y"
                  { (yyval.nodo) = (yyvsp[0].nodo); }
#line 1736 "src/parser.tab.c"
    break;

  case 26: /* sentencia: bucle  */
#line 201 "src/parser.y"
                  { (yyval.nodo) = (yyvsp[0].nodo); }
#line 1742 "src/parser.tab.c"
    break;

  case 27: /* sentencia: salida  */
#line 202 "src/parser.y"
                  { (yyval.nodo) = (yyvsp[0].nodo); }
#line 1748 "src/parser.tab.c"
    break;

  case 28: /* sentencia: retorno  */
#line 203 "src/parser.y"
                  { (yyval.nodo) = (yyvsp[0].nodo); }
#line 1754 "src/parser.tab.c"
    break;

  case 29: /* declaracion: tipo ID ASIG expresion P_COMA  */
#line 207 "src/parser.y"
                                    {
        NodoAST* id = crear_nodo(N_ID, (yyvsp[-3].cadena), NULL, NULL);
        NodoAST* exp = (yyvsp[-1].nodo);

        /* CONVERSIÓN EN DECLARACIÓN CON ASIGNACIÓN:
           Si la variable es REAL (105) y la expresión es ENTERA (104), envolvemos la expresión */
        if ((yyvsp[-4].nodo)->tipo_datos == 105 && es_expresion_entera(exp)) {
            exp = envolver_conversion(exp, N_CONV_ENTERO_A_REAL);
        }
        /* Si la variable es ENTERA (104) y la expresión es REAL (105), envolvemos la expresión */
        else if ((yyvsp[-4].nodo)->tipo_datos == 104 && es_expresion_real(exp)) {
            exp = envolver_conversion(exp, N_CONV_REAL_A_ENTERO);
        }

        (yyval.nodo) = crear_nodo(N_DECLARACION, "DECLARACION_ASIG", id, exp);
    }
#line 1775 "src/parser.tab.c"
    break;

  case 30: /* declaracion: tipo ids P_COMA  */
#line 223 "src/parser.y"
                      { (yyval.nodo) = crear_nodo(N_DECLARACION, "DECLARACION_MULTIPLES", (yyvsp[-2].nodo), (yyvsp[-1].nodo)); }
#line 1781 "src/parser.tab.c"
    break;

  case 31: /* declaracion: tipo ID P_COMA  */
#line 224 "src/parser.y"
                     {
        NodoAST* id = crear_nodo(N_ID, (yyvsp[-1].cadena), NULL, NULL);
        (yyval.nodo) = crear_nodo(N_DECLARACION, "DECLARACION_SIMPLE", (yyvsp[-2].nodo), id);
    }
#line 1790 "src/parser.tab.c"
    break;

  case 32: /* ids: ids COMA ID  */
#line 231 "src/parser.y"
                  {
        NodoAST* aux = (yyvsp[-2].nodo);
        while (aux->sig != NULL) aux = aux->sig;
        aux->sig = crear_nodo(N_ID, (yyvsp[0].cadena), NULL, NULL);
        (yyval.nodo) = (yyvsp[-2].nodo);
    }
#line 1801 "src/parser.tab.c"
    break;

  case 33: /* ids: ID  */
#line 237 "src/parser.y"
         { (yyval.nodo) = crear_nodo(N_ID, (yyvsp[0].cadena), NULL, NULL); }
#line 1807 "src/parser.tab.c"
    break;

  case 34: /* asignacion: ID ASIG expresion P_COMA  */
#line 241 "src/parser.y"
                               {
        NodoAST* id = crear_nodo(N_ID, (yyvsp[-3].cadena), NULL, NULL);
        NodoAST* exp = (yyvsp[-1].nodo);

        /* CONVERSIÓN EN ASIGNACIÓN SIMPLE:
           Consultamos el tipo de la variable en la Tabla de Símbolos y envolvemos la expresión */
        if (es_tipo_real_ts((yyvsp[-3].cadena)) && es_expresion_entera(exp)) {
            exp = envolver_conversion(exp, N_CONV_ENTERO_A_REAL);
        } 
        else if (es_tipo_entero_ts((yyvsp[-3].cadena)) && es_expresion_real(exp)) {
            exp = envolver_conversion(exp, N_CONV_REAL_A_ENTERO);
        }

        (yyval.nodo) = crear_nodo(N_ASIGNACION, "=", id, exp);
    }
#line 1827 "src/parser.tab.c"
    break;

  case 35: /* asignacion: ID ASIG invocacion P_COMA  */
#line 256 "src/parser.y"
                                {
        NodoAST* id = crear_nodo(N_ID, (yyvsp[-3].cadena), NULL, NULL);
        (yyval.nodo) = crear_nodo(N_ASIGNACION, "=", id, (yyvsp[-1].nodo));
    }
#line 1836 "src/parser.tab.c"
    break;

  case 36: /* invocacion: ID PAR_ABRE argumentos PAR_CIERRA  */
#line 263 "src/parser.y"
                                        {
        (yyval.nodo) = crear_nodo(N_INVOCACION, (yyvsp[-3].cadena), (yyvsp[-1].nodo), NULL);
    }
#line 1844 "src/parser.tab.c"
    break;

  case 37: /* argumentos: expresion COMA expresion  */
#line 269 "src/parser.y"
                               {
        (yyvsp[-2].nodo)->sig = (yyvsp[0].nodo);
        (yyval.nodo) = (yyvsp[-2].nodo);
    }
#line 1853 "src/parser.tab.c"
    break;

  case 38: /* argumentos: expresion  */
#line 273 "src/parser.y"
                { (yyval.nodo) = (yyvsp[0].nodo); }
#line 1859 "src/parser.tab.c"
    break;

  case 39: /* argumentos: expresion COMA expresion COMA expresion  */
#line 274 "src/parser.y"
                                              {
        /* Error E9: Invocación a función con más de 2 argumentos */
        fprintf(stderr, "Error E9: La llamada a la funcion '%s' supera el maximo de 2 argumentos.\n", "funcion");
        YYERROR;
    }
#line 1869 "src/parser.tab.c"
    break;

  case 40: /* seleccion: SI PAR_ABRE condicional PAR_CIERRA bloque  */
#line 282 "src/parser.y"
                                                {
        (yyval.nodo) = crear_nodo(N_SELECCION, "SI", (yyvsp[-2].nodo), (yyvsp[0].nodo));
    }
#line 1877 "src/parser.tab.c"
    break;

  case 41: /* seleccion: SI PAR_ABRE condicional PAR_CIERRA bloque SINO bloque  */
#line 285 "src/parser.y"
                                                            {
        NodoAST* cuerpo = crear_nodo(N_SELECCION, "SI_SINO_BLOQUES", (yyvsp[-2].nodo), (yyvsp[0].nodo));
        (yyval.nodo) = crear_nodo(N_SELECCION, "SI_SINO", (yyvsp[-4].nodo), cuerpo);
    }
#line 1886 "src/parser.tab.c"
    break;

  case 42: /* condicional: condicional OR t_condicional  */
#line 292 "src/parser.y"
                                   { (yyval.nodo) = crear_nodo(N_LOG_OR, "OR", (yyvsp[-2].nodo), (yyvsp[0].nodo)); }
#line 1892 "src/parser.tab.c"
    break;

  case 43: /* condicional: t_condicional  */
#line 293 "src/parser.y"
                    { (yyval.nodo) = (yyvsp[0].nodo); }
#line 1898 "src/parser.tab.c"
    break;

  case 44: /* t_condicional: t_condicional AND f_condicional  */
#line 297 "src/parser.y"
                                      { (yyval.nodo) = crear_nodo(N_LOG_AND, "AND", (yyvsp[-2].nodo), (yyvsp[0].nodo)); }
#line 1904 "src/parser.tab.c"
    break;

  case 45: /* t_condicional: f_condicional  */
#line 298 "src/parser.y"
                    { (yyval.nodo) = (yyvsp[0].nodo); }
#line 1910 "src/parser.tab.c"
    break;

  case 46: /* f_condicional: condicion  */
#line 302 "src/parser.y"
                { (yyval.nodo) = (yyvsp[0].nodo); }
#line 1916 "src/parser.tab.c"
    break;

  case 47: /* f_condicional: PAR_ABRE condicional PAR_CIERRA  */
#line 303 "src/parser.y"
                                      { (yyval.nodo) = (yyvsp[-1].nodo); }
#line 1922 "src/parser.tab.c"
    break;

  case 48: /* condicion: expresion comparador expresion  */
#line 307 "src/parser.y"
                                     {
        (yyvsp[-1].nodo)->izq = (yyvsp[-2].nodo);
        (yyvsp[-1].nodo)->der = (yyvsp[0].nodo);
        (yyval.nodo) = (yyvsp[-1].nodo);
    }
#line 1932 "src/parser.tab.c"
    break;

  case 49: /* comparador: IGUAL  */
#line 315 "src/parser.y"
                   { (yyval.nodo) = crear_nodo(N_COMP_IGUAL, "==", NULL, NULL); }
#line 1938 "src/parser.tab.c"
    break;

  case 50: /* comparador: DISTINTO  */
#line 316 "src/parser.y"
                   { (yyval.nodo) = crear_nodo(N_COMP_DISTINTO, "!=", NULL, NULL); }
#line 1944 "src/parser.tab.c"
    break;

  case 51: /* comparador: MENOR  */
#line 317 "src/parser.y"
                   { (yyval.nodo) = crear_nodo(N_COMP_MENOR, "<", NULL, NULL); }
#line 1950 "src/parser.tab.c"
    break;

  case 52: /* comparador: MAYOR  */
#line 318 "src/parser.y"
                   { (yyval.nodo) = crear_nodo(N_COMP_MAYOR, ">", NULL, NULL); }
#line 1956 "src/parser.tab.c"
    break;

  case 53: /* comparador: MENOR_IGUAL  */
#line 319 "src/parser.y"
                   { (yyval.nodo) = crear_nodo(N_COMP_MENOR_IGUAL, "<=", NULL, NULL); }
#line 1962 "src/parser.tab.c"
    break;

  case 54: /* comparador: MAYOR_IGUAL  */
#line 320 "src/parser.y"
                   { (yyval.nodo) = crear_nodo(N_COMP_MAYOR_IGUAL, ">=", NULL, NULL); }
#line 1968 "src/parser.tab.c"
    break;

  case 55: /* bucle: PARA PAR_ABRE control P_COMA condicional P_COMA CTE_E PAR_CIERRA bloque  */
#line 324 "src/parser.y"
                                                                              {
        NodoAST* paso = crear_nodo(N_CTE_E, "PASO", NULL, NULL);
        paso->val_int = (yyvsp[-2].val_int);
        NodoAST* ctrl_paso = crear_nodo(N_BUCLE, "CTRL_PASO", (yyvsp[-6].nodo), paso);
        NodoAST* cond_bloque = crear_nodo(N_BUCLE, "COND_BLOQUE", (yyvsp[-4].nodo), (yyvsp[0].nodo));
        (yyval.nodo) = crear_nodo(N_BUCLE, "PARA", ctrl_paso, cond_bloque);
    }
#line 1980 "src/parser.tab.c"
    break;

  case 56: /* control: ENTERO ID ASIG CTE_E  */
#line 334 "src/parser.y"
                           {
        NodoAST* id = crear_nodo(N_ID, (yyvsp[-2].cadena), NULL, NULL);
        NodoAST* cte = crear_nodo(N_CTE_E, "", NULL, NULL);
        cte->val_int = (yyvsp[0].val_int);
        (yyval.nodo) = crear_nodo(N_ASIGNACION, "=", id, cte);
    }
#line 1991 "src/parser.tab.c"
    break;

  case 57: /* control: ID ASIG CTE_E  */
#line 340 "src/parser.y"
                    {
        NodoAST* id = crear_nodo(N_ID, (yyvsp[-2].cadena), NULL, NULL);
        NodoAST* cte = crear_nodo(N_CTE_E, "", NULL, NULL);
        cte->val_int = (yyvsp[0].val_int);
        (yyval.nodo) = crear_nodo(N_ASIGNACION, "=", id, cte);
    }
#line 2002 "src/parser.tab.c"
    break;

  case 58: /* salida: IMPRIMIR PAR_ABRE elementos_salida PAR_CIERRA P_COMA  */
#line 349 "src/parser.y"
                                                           {
        (yyval.nodo) = crear_nodo(N_SALIDA, "IMPRIMIR", (yyvsp[-2].nodo), NULL);
    }
#line 2010 "src/parser.tab.c"
    break;

  case 59: /* salida: IMPRIMIR PAR_ABRE PAR_CIERRA P_COMA  */
#line 352 "src/parser.y"
                                          {
        /* Error E5: La función imprimir() requiere parámetros */
        fprintf(stderr, "Error sintactico E5: La funcion 'imprimir' requiere parametros.\n");
        YYERROR;
    }
#line 2020 "src/parser.tab.c"
    break;

  case 60: /* elementos_salida: elementos_salida COMA elemento_salida  */
#line 360 "src/parser.y"
                                            {
        NodoAST* aux = (yyvsp[-2].nodo);
        while (aux->sig != NULL) aux = aux->sig;
        aux->sig = (yyvsp[0].nodo);
        (yyval.nodo) = (yyvsp[-2].nodo);
    }
#line 2031 "src/parser.tab.c"
    break;

  case 61: /* elementos_salida: elemento_salida  */
#line 366 "src/parser.y"
                      { (yyval.nodo) = (yyvsp[0].nodo); }
#line 2037 "src/parser.tab.c"
    break;

  case 62: /* elemento_salida: CADENA  */
#line 370 "src/parser.y"
             { (yyval.nodo) = crear_nodo(N_CADENA, (yyvsp[0].cadena), NULL, NULL); }
#line 2043 "src/parser.tab.c"
    break;

  case 63: /* elemento_salida: expresion  */
#line 371 "src/parser.y"
                { (yyval.nodo) = (yyvsp[0].nodo); }
#line 2049 "src/parser.tab.c"
    break;

  case 64: /* elemento_salida: invocacion  */
#line 372 "src/parser.y"
                 { (yyval.nodo) = (yyvsp[0].nodo); }
#line 2055 "src/parser.tab.c"
    break;

  case 65: /* retorno: RETORNAR expresion P_COMA  */
#line 376 "src/parser.y"
                                { (yyval.nodo) = crear_nodo(N_RETORNO, "RETORNAR", (yyvsp[-1].nodo), NULL); }
#line 2061 "src/parser.tab.c"
    break;

  case 66: /* expresion: expresion SUMA termino  */
#line 380 "src/parser.y"
                             {
        NodoAST* izq = (yyvsp[-2].nodo);
        NodoAST* der = (yyvsp[0].nodo);

        /* CONVERSIÓN EN SUMA ARITMÉTICA:
           Si mezclamos entero y real, envolvemos el lado entero con ENTERO_A_REAL */
        if (es_expresion_real(izq) && es_expresion_entera(der)) {
            der = envolver_conversion(der, N_CONV_ENTERO_A_REAL);
        } else if (es_expresion_entera(izq) && es_expresion_real(der)) {
            izq = envolver_conversion(izq, N_CONV_ENTERO_A_REAL);
        }

        (yyval.nodo) = crear_nodo(N_OP_SUMA, "+", izq, der);
        (yyval.nodo)->tipo_datos = (es_expresion_real(izq) || es_expresion_real(der)) ? 105 : 104;
    }
#line 2081 "src/parser.tab.c"
    break;

  case 67: /* expresion: expresion RESTA termino  */
#line 395 "src/parser.y"
                              {
        NodoAST* izq = (yyvsp[-2].nodo);
        NodoAST* der = (yyvsp[0].nodo);

        if (es_expresion_real(izq) && es_expresion_entera(der)) {
            der = envolver_conversion(der, N_CONV_ENTERO_A_REAL);
        } else if (es_expresion_entera(izq) && es_expresion_real(der)) {
            izq = envolver_conversion(izq, N_CONV_ENTERO_A_REAL);
        }

        (yyval.nodo) = crear_nodo(N_OP_RESTA, "-", izq, der);
        (yyval.nodo)->tipo_datos = (es_expresion_real(izq) || es_expresion_real(der)) ? 105 : 104;
    }
#line 2099 "src/parser.tab.c"
    break;

  case 68: /* expresion: termino  */
#line 408 "src/parser.y"
              { (yyval.nodo) = (yyvsp[0].nodo); }
#line 2105 "src/parser.tab.c"
    break;

  case 69: /* termino: termino PRODUCTO factor  */
#line 412 "src/parser.y"
                              {
        NodoAST* izq = (yyvsp[-2].nodo);
        NodoAST* der = (yyvsp[0].nodo);

        if (es_expresion_real(izq) && es_expresion_entera(der)) {
            der = envolver_conversion(der, N_CONV_ENTERO_A_REAL);
        } else if (es_expresion_entera(izq) && es_expresion_real(der)) {
            izq = envolver_conversion(izq, N_CONV_ENTERO_A_REAL);
        }

        (yyval.nodo) = crear_nodo(N_OP_PROD, "*", izq, der);
        (yyval.nodo)->tipo_datos = (es_expresion_real(izq) || es_expresion_real(der)) ? 105 : 104;
    }
#line 2123 "src/parser.tab.c"
    break;

  case 70: /* termino: termino DIVISION factor  */
#line 425 "src/parser.y"
                              {
        NodoAST* izq = (yyvsp[-2].nodo);
        NodoAST* der = (yyvsp[0].nodo);

        if (es_expresion_real(izq) && es_expresion_entera(der)) {
            der = envolver_conversion(der, N_CONV_ENTERO_A_REAL);
        } else if (es_expresion_entera(izq) && es_expresion_real(der)) {
            izq = envolver_conversion(izq, N_CONV_ENTERO_A_REAL);
        }

        (yyval.nodo) = crear_nodo(N_OP_DIV, "/", izq, der);
        (yyval.nodo)->tipo_datos = (es_expresion_real(izq) || es_expresion_real(der)) ? 105 : 104;
    }
#line 2141 "src/parser.tab.c"
    break;

  case 71: /* termino: factor  */
#line 438 "src/parser.y"
             { (yyval.nodo) = (yyvsp[0].nodo); }
#line 2147 "src/parser.tab.c"
    break;

  case 72: /* factor: ID  */
#line 442 "src/parser.y"
         { 
        (yyval.nodo) = crear_nodo(N_ID, (yyvsp[0].cadena), NULL, NULL);
        (yyval.nodo)->tipo_datos = obtener_tipo_ts((yyvsp[0].cadena));
    }
#line 2156 "src/parser.tab.c"
    break;

  case 73: /* factor: CTE_E  */
#line 446 "src/parser.y"
            {
        NodoAST* n = crear_nodo(N_CTE_E, "CTE_ENTERA", NULL, NULL);
        n->val_int = (yyvsp[0].val_int);
        n->tipo_datos = 104; /* 104 = ENTERO */
        (yyval.nodo) = n;
    }
#line 2167 "src/parser.tab.c"
    break;

  case 74: /* factor: CTE_R  */
#line 452 "src/parser.y"
            {
        NodoAST* n = crear_nodo(N_CTE_R, "CTE_REAL", NULL, NULL);
        n->val_real = (yyvsp[0].val_real);
        n->tipo_datos = 105; /* 105 = REAL */
        (yyval.nodo) = n;
    }
#line 2178 "src/parser.tab.c"
    break;

  case 75: /* factor: PAR_ABRE expresion PAR_CIERRA  */
#line 458 "src/parser.y"
                                    { (yyval.nodo) = (yyvsp[-1].nodo); }
#line 2184 "src/parser.tab.c"
    break;

  case 76: /* tipo: ENTERO  */
#line 462 "src/parser.y"
             { 
        (yyval.nodo) = crear_nodo(N_DECLARACION, "ENTERO", NULL, NULL); 
        (yyval.nodo)->tipo_datos = 104;
    }
#line 2193 "src/parser.tab.c"
    break;

  case 77: /* tipo: REAL  */
#line 466 "src/parser.y"
             { 
        (yyval.nodo) = crear_nodo(N_DECLARACION, "REAL", NULL, NULL); 
        (yyval.nodo)->tipo_datos = 105;
    }
#line 2202 "src/parser.tab.c"
    break;


#line 2206 "src/parser.tab.c"

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
      {
        yypcontext_t yyctx
          = {yyssp, yytoken};
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == -1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *,
                             YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (yymsg)
              {
                yysyntax_error_status
                  = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
                yymsgp = yymsg;
              }
            else
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = YYENOMEM;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == YYENOMEM)
          YYNOMEM;
      }
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
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
  return yyresult;
}

#line 472 "src/parser.y"

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

