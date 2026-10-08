# Spec — 04-analizador-sintactico

**Grupo:** B · **Lenguaje de implementación:** C
**Depende de:** [`specs/01-diseno/spec.md`](../01-diseno/spec.md),
[`specs/02-tabla-simbolos/spec.md`](../02-tabla-simbolos/spec.md),
[`specs/03-analizador-lexico/spec.md`](../03-analizador-lexico/spec.md)
**Consume:** tokens y ubicación producidos por `yylex()` (`src/lexer.c`).
**Produce:** verificación gramatical + Árbol Sintáctico Abstracto (AST), listo para análisis semántico y código intermedio.
**Estado:** EN CURSO.

---

## 1- Objetivo

Diseñar e implementar el Analizador Sintáctico (Parser) para el lenguaje BETA
utilizando **Yacc / Bison** e integrado con el Analizador Léxico previo
(`yylex()`).
Su función es verificar que la secuencia de tokens cumpla con las reglas
gramaticales del lenguaje BETA y construir la representación intermedia basada
en un **Árbol Sintáctico Abstracto (AST)** con nodos anotados de conversión de
tipos (Entero/Real).

## 2- Alcance
* **Estructura Gramatical:** Reconocimiento de todas las reglas BNF del lenguaje BETA:
  * Declaraciones de variables (`entero`, `real`).
  * Estructuras de control (`si`, `sino`, `para`).
  * Asignaciones y expresiones aritmético-lógicas.
  * Definición e invocación de funciones (máximo 2 parámetros, sin llamadas recursivas).
  * Salida por pantalla (`imprimir`).
* **Representación Intermedia (AST):** Construcción dinámica de nodos del Árbol Sintáctico Abstracto en C durante el análisis.
* **Manejo de Conversiones:** Inserción de nodos de conversión implícita/explícita en el AST (`ENTERO_A_REAL` y `REAL_A_ENTERO`) según las reglas de compatibilidad de tipos (R2–R5).
* **Integración:** Interfaz directa entre `yyparse()` y el consumidor del lexer (`yylex()`).
* **Fuera del alcance:** La generación final de código Assembler y la ejecución en tiempo de ejecución (contemplado en etapas posteriores).

## 3- Entradas / Salidas

| Elemento | Contrato |
|---|---|
| Entrada | Secuencia de `int` devueltos por `yylex()`: 100–132 según `01-diseno/spec.md §4` y `src/tokens.h`; `0` = EOF; `-1` = error léxico (el parser no lo consume como token: aborta o sincroniza según `§10`). |
| Salida principal | `0` si el programa es gramaticalmente válido, `!= 0` si hay error sintáctico. |
| Salida secundaria | Puntero a raíz AST (`N_PROGRAMA`) con conversiones anotadas; tabla de símbolos poblada (funciones + ámbitos). |
| Ubicación | Cada nodo AST guarda `linea, col_ini, col_fin` copiadas del token que lo origina (ID, CTE, palabra reservada u operador). `yyerror()` informa línea/col del token actual. |

Algunos resultados esperados:

1. El parser nunca pide un token que el lexer no define (100–132, EOF).
2. Blancos y comentarios nunca llegan al parser (los consume el lexer).
3. `PUNTO` (130) y `COMILLA` (131) están definidos en diseño pero el lexer no
   los emite; el parser los declara por compatibilidad pero ninguna producción
   válida los exige.
4. Tras EOF, `yylex()` retorna `0` sin avanzar; `yyparse()` termina.

## 4- Reglas y decisiones de diseño

| # | Decisión | Valor / justificación |
|---|---|---|
| S1 | Gramática | Copia literal de `01-diseno/spec.md §6`, transcripta a sintaxis Bison (§6). Sin agregar ni quitar producciones. |
| S2 | Gramatica | Recursion y asociatividad a la izquierda |
| S3 | `para` | `<control>` solo `ENTERO ID ASIG CTE_E` o `ID ASIG CTE_E`; condición es `<condicional>`; actualizacion es `CTE_E` (`PAR_ABRE control P_COMA condicional P_COMA CTE_E PAR_CIERRA bloque`). |
| S4 | Funciones | 1 o 2 parametros. 0 parametros o 3+ parametros es E10/E9. `inicio` es `<principal> ::= INICIO bloque_i`, sin parametros ni retorno; solo puede aparecer una vez (§6.3). |
| S5 | `imprimir` | Lista vacía `imprimir()` es E5. |

## 5- Tokens en Bison

```yacc
%token ID 100 CTE_E 101 CTE_R 102
%token INICIO 103 ENTERO 104 REAL 105 SI 106 SINO 107 PARA 108
%token IMPRIMIR 109 RETORNAR 110
%token ASIG 111 IGUAL 112 DISTINTO 113 MENOR 114 MAYOR 115
%token MENOR_IGUAL 116 MAYOR_IGUAL 117 AND 118 OR 119
%token SUMA 120 RESTA 121 PRODUCTO 122 DIVISION 123
%token PAR_ABRE 124 PAR_CIERRA 125 LLAVE_ABRE 126 LLAVE_CIERRA 127
%token P_COMA 128 COMA 129 PUNTO 130 COMILLA 131 CADENA 132
```

## 6- Gramática Bison (copia de `01-diseno/spec.md §6`)

Nota: terminales en MAYÚSCULAS, `|` alternativas.

```
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
    | ID
    ;

asignacion
    : ID ASIG expresion P_COMA
    | ID ASIG invocacion P_COMA
    ;

invocacion
    : ID PAR_ABRE argumentos PAR_CIERRA
    ;

seleccion
    : SI PAR_ABRE condicional PAR_CIERRA bloque
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
    : IGUAL | DISTINTO | MENOR | MAYOR | MENOR_IGUAL | MAYOR_IGUAL
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
```

### 6.1 Restricciones sintacticas

1. `principal` (`inicio`) aparece exactamente una vez y al final del archivo.
2. No hay funciones anidadas: `unidad` solo aparece en `unidades` a nivel
   programa.
3. `retorno` solo aparece en `sentencia` (funciones), nunca en `sentencia_i`
   (`inicio` no retorna, no tiene tipo).
4. `inicio` no lleva parámetros ni tipo de retorno.

## 7- Errores de esta fase

Solo E5, E9, E10 en esta etapa. Los léxicos (E1–E4, E12, E18–E20) ya
fueron emitidos antes.

| Código | Condición sintáctica | Mensaje |
|---|---|---|
| E5 | cualquier secuencia que no reduce según §6. Ejemplo: `entero = 5;`, `si x {}`, `para (i=0;i<5;) {}`, `imprimir();`, `retornar;` | `Error sintactico E5 en L:C: se esperaba ...` |
| E9 | definición o llamada con más de 2 parámetros/argumentos | `Error E9: la funcion 'f' supera el maximo de 2 parametros/argumentos` |
| E10 | definición de función sin parámetros (`f()`) | `Error E10: la funcion 'f' debe tener 1 o 2 parametros` |

## 8. Criterios de Aceptación
El Analizador Sintáctico se considerará aceptado cuando:
1. El archivo `src/parser.y` compile limpiamente con Bison (`bison -d src/parser.y -o src/parser.tab.c`) generando las cabeceras requeridas sin conflictos.
2. Reconozca la totalidad de las reglas gramaticales BNF (§6) consumiendo iterativamente los tokens `100–132` desde `yylex()`.
3. Construya dinámicamente el Árbol Sintáctico Abstracto (AST) anotando las conversiones de tipos necesarias (`ENTERO_A_REAL` y `REAL_A_ENTERO`).
4. Gestione correctamente la creación y cierre de ámbitos en la Tabla de Símbolos.
5. Reporte de forma precisa los errores sintácticos descritos en la sección 7 (`E5`, `E9` y `E10`)
