# Spec — 04-analizador-sintactico

**Grupo:** B · **Lenguaje de implementación:** C
**Depende de:** [`specs/01-diseno/spec.md`](../01-diseno/spec.md),
[`specs/02-tabla-simbolos/spec.md`](../02-tabla-simbolos/spec.md),
[`specs/03-analizador-lexico/spec.md`](../03-analizador-lexico/spec.md)
**Consume:** tokens producidos por el analizador lexico - `yylex()`.
**Produce:** verificación gramatical.
**Estado:** EN CURSO.

---

## 1. Objetivo

Diseñar e implementar el Analizador Sintáctico (Parser) para el lenguaje BETA
utilizando **Yacc / Bison** e integrado con el Analizador Léxico previo
(`yylex()`).
Su función es verificar que la secuencia de tokens cumpla con las reglas
gramaticales del lenguaje BETA y construir la representación intermedia basada
en un **Árbol Sintáctico Abstracto (AST)** con nodos anotados de conversión de
tipos (Entero/Real).

## 2. Alcance
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

## 3. Entrada / Salida

| Elemento | Contrato |
|---|---|
| Entrada | Secuencia de `int` devueltos por `yylex()`: 100-132 según `01-diseno/spec.md §4`. |
| Salida principal | `0` si el programa es gramaticalmente válido, `!= 0` si hay error sintáctico. |

El analizador sintactico se implementa de la siguiente manera (pseudocodigo):

```c

int main() {

    archivo = leer_archivo("archivo.beta", "r");

    mientras (archivo != vacio) {
        yyparse()
    }

    archivo.close();

    si (archivo con errores) { print("compilacion completa con errores"); informar_errores(); }
    else { print("compilacion completa sin errores"); }

}

```

Algunos resultados esperados:

1. El parser nunca pide un token que el lexer no define (100–132, EOF).
2. Blancos y comentarios nunca llegan al parser (los consume el lexer).

## 4. Reglas y decisiones de diseño

| # | Decisión | Valor / justificación |
|---|---|---|
| S1 | Gramática | Copia literal de `01-diseno/spec.md §6`, transcripta a sintaxis Bison (§6). Sin agregar ni quitar producciones. |
| S2 | Gramatica | Recursion y asociatividad a la izquierda |
| S3 | Funciones | 1 o 2 parametros. 0 parametros o 3+ parametros es E10/E9. `inicio` es `<principal> ::= INICIO bloque_i`, sin parametros ni retorno; solo puede aparecer una vez (§6.3). |
| S5 | `imprimir` | Lista vacía `imprimir()` es E5. |

## 5. Tokens

| Código | Token | Lexema |
|--------|--------|-------------|
| 100 | `ID` | identificador (máx. 20 caracteres) |
| 101 | `CTE_E` | constante entera |
| 102 | `CTE_R` | constante real |
| 103 | `INICIO` | `inicio` |
| 104 | `ENTERO` | `entero` |
| 105 | `REAL` | `real` |
| 106 | `SI` | `si` |
| 107 | `SINO` | `sino` |
| 108 | `PARA` | `para` |
| 109 | `IMPRIMIR` | `imprimir` |
| 110 | `RETORNAR` | `retornar` |
| 111 | `ASIG` | `=` |
| 112 | `IGUAL` | `==` |
| 113 | `DISTINTO` | `!=` |
| 114 | `MENOR` | `<` |
| 115 | `MAYOR` | `>` |
| 116 | `MENOR_IGUAL` | `<=` |
| 117 | `MAYOR_IGUAL` | `>=` |
| 118 | `AND` | `&` |
| 119 | `OR` | `\|` |
| 120 | `SUMA` | `+` |
| 121 | `RESTA` | `-` |
| 122 | `PRODUCTO` | `*` |
| 123 | `DIVISION` | `/` |
| 124 | `PAR_ABRE` | `(` |
| 125 | `PAR_CIERRA` | `)` |
| 126 | `LLAVE_ABRE` | `{` |
| 127 | `LLAVE_CIERRA` | `}` |
| 128 | `P_COMA` | `;` |
| 129 | `COMA` | `,` |
| 130 | `PUNTO` | `.` |
| 131 | `COMILLA` | `"` |
| 132 | `CADENA` | cadena de caracteres delimitada por comillas dentro de la funcion `IMPRIMIR` |

## 6. Gramática (copia de `01-diseno/spec.md §6`)

```
<programa>            ::= <unidades> <principal>
                        | <principal>

<principal>           ::= INICIO <bloque_i>

<unidades>            ::= <unidades> <unidad>
                        | <unidad>

<unidad>              ::= <tipo> ID '(' <parametros> ')' <bloque>

<parametros>          ::= <tipo> ID
                        | <tipo> ID ',' <tipo> ID

<bloque>              ::= '{' <sentencias> '}'

<bloque_i>            ::= '{' <sentencias_i> '}'

<sentencias_i>        ::= <sentencias_i> <sentencia_i>
                        | <sentencia_i>

<sentencia_i>         ::= <declaracion> | <asignacion> | <seleccion> | <bucle> | <salida>

<sentencias>          ::= <sentencias> <sentencia>
                        | <sentencia>

<sentencia>           ::= <declaracion> | <asignacion> | <seleccion> | <bucle> | <salida> | <retorno>

<declaracion>         ::= <tipo> ID '=' <expresion> ';'
                        | <tipo> <ids> ';'
                        | <tipo> ID ';'

<ids>                 ::= <ids> ',' ID
                        | ID

<asignacion>          ::= ID '=' <expresion> ';'
                        | ID '=' <invocacion> ';'

<invocacion>          ::= ID '(' <argumentos> ')'

<seleccion>           ::= SI '(' <condicional> ')' <bloque> 
                        | SI '(' <condicional> ')' <bloque> SINO <bloque>

<condicional>         ::= <condicional> OR <t_condicional>
                        | <t_condicional>

<t_condicional>       ::= <t_condicional> AND <f_condicional>
                        | <f_condicional>

<f_condicional>       ::= <condicion>
                        | '(' <condicional> ')'

<condicion>           ::= <expresion> <comparador> <expresion>

<comparador>          ::= IGUAL | DISTINTO | MENOR | MAYOR | MENOR_IGUAL | MAYOR_IGUAL

<bucle>               ::= PARA '(' <control> ';' <condicional> ';' CTE_E ')' <bloque>

<control>             ::= ENTERO ID '=' CTE_E
                        | ID '=' CTE_E

<salida>              ::= IMPRIMIR '(' <elementos_salida> ')' ';'

<elementos_salida>    ::= <elementos_salida> ',' <elemento_salida>
                        | <elemento_salida>

<elemento_salida>      ::= CADENA | <expresion> | <invocacion>
                        
<retorno>             ::= RETORNAR <expresion> ';'

<argumentos>          ::= <expresion> ',' <expresion>
                        | <expresion>

<expresion>           ::= <expresion> '+' <termino>
                        | <expresion> '-' <termino>
                        | <termino>

<termino>             ::= <termino> '*' <factor>
                        | <termino> '/' <factor>
                        | <factor>

<factor>              ::= ID | CTE_E | CTE_R | '(' <expresion> ')'

<tipo>                ::= ENTERO | REAL
```

### 6.1 Restricciones sintacticas

1. `principal` (`inicio`) aparece exactamente una vez y al final del archivo.
2. No hay funciones anidadas: `unidad` solo aparece en `unidades` a nivel
   programa.
3. `retorno` solo aparece en `sentencia` (funciones), nunca en `sentencia_i`
   (`inicio` no retorna, no tiene tipo).
4. `inicio` no lleva parámetros ni tipo de retorno.

## 7. Errores de esta fase

Solo E5, E9, E10 en esta etapa. Los léxicos (E1–E4, E12, E18–E20) ya
fueron emitidos antes.

| Código | Condición sintáctica | Mensaje |
|---|---|---|
| E5 | cualquier secuencia que no reduce según §6. Ejemplo: `entero = 5;`, `si x {}`, `para (i=0;i<5;) {}`, `imprimir();`, `retornar;` | `Error E5 en ...` |
| E9 | definición o llamada con más de 2 parámetros/argumentos | `Error E9: la funcion 'f' supera el maximo de 2 parametros/argumentos` |
| E10 | definición de función sin parámetros (`f()`) | `Error E10: la funcion 'f' debe tener 1 o 2 parametros` |

## 8. Criterios de Aceptación
El Analizador Sintáctico se considerará aceptado cuando:
1. El archivo `src/parser.y` compile limpiamente con Bison (`bison -d src/parser.y -o src/parser.tab.c`) generando las cabeceras requeridas sin conflictos.
2. Reconozca la totalidad de las reglas gramaticales BNF (§6) consumiendo iterativamente los tokens `100–132` desde `yylex()`.
3. Reporte de forma precisa los errores sintácticos descritos en la sección 7 (`E5`, `E9` y `E10`)
