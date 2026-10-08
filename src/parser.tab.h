/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_SRC_PARSER_TAB_H_INCLUDED
# define YY_YY_SRC_PARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    ID = 100,                      /* ID  */
    CTE_E = 101,                   /* CTE_E  */
    CTE_R = 102,                   /* CTE_R  */
    INICIO = 103,                  /* INICIO  */
    ENTERO = 104,                  /* ENTERO  */
    REAL = 105,                    /* REAL  */
    SI = 106,                      /* SI  */
    SINO = 107,                    /* SINO  */
    PARA = 108,                    /* PARA  */
    IMPRIMIR = 109,                /* IMPRIMIR  */
    RETORNAR = 110,                /* RETORNAR  */
    ASIG = 111,                    /* ASIG  */
    IGUAL = 112,                   /* IGUAL  */
    DISTINTO = 113,                /* DISTINTO  */
    MENOR = 114,                   /* MENOR  */
    MAYOR = 115,                   /* MAYOR  */
    MENOR_IGUAL = 116,             /* MENOR_IGUAL  */
    MAYOR_IGUAL = 117,             /* MAYOR_IGUAL  */
    AND = 118,                     /* AND  */
    OR = 119,                      /* OR  */
    SUMA = 120,                    /* SUMA  */
    RESTA = 121,                   /* RESTA  */
    PRODUCTO = 122,                /* PRODUCTO  */
    DIVISION = 123,                /* DIVISION  */
    PAR_ABRE = 124,                /* PAR_ABRE  */
    PAR_CIERRA = 125,              /* PAR_CIERRA  */
    LLAVE_ABRE = 126,              /* LLAVE_ABRE  */
    LLAVE_CIERRA = 127,            /* LLAVE_CIERRA  */
    P_COMA = 128,                  /* P_COMA  */
    COMA = 129,                    /* COMA  */
    PUNTO = 130,                   /* PUNTO  */
    COMILLA = 131,                 /* COMILLA  */
    CADENA = 132                   /* CADENA  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 69 "src/parser.y"

    int val_int;
    double val_real;
    char cadena[64];
    struct NodoAST *nodo;

#line 106 "src/parser.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_SRC_PARSER_TAB_H_INCLUDED  */
