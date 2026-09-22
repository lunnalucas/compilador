# Spec — Diseño del lenguaje: ...

**Grupo:** B · **Lenguaje de implementación:** C  
**Estado:** En desarrollo.

---

## 1. Decisiones globales

| #  | Decisión | Valor |
|----|---|---|
| D1 | Tipo de datos | dos tipos: Entero y Real |
| D2 | Rango | Entero: 32 bits y Real: 64 bits  |
| D3 | Declaración | todas las variables deben ser declaradas con su tipo. |
| D4 | Alcance y Gestión de memoria | variables globales y locales en memoria estatica. |
| D5 | Sensibilidad a mayúsculas | `Total` y `total` son variables distintas |
| D6 | Longitud máxima de identificador | 20 caracteres; más largo se trunca |
| D7 | Comentarios | de bloque, delimitados por `/*` y `*/` |
| D8 | Compatibilidad de tipos | compatibilidad entre enteros y reales. |
| D9 | Bucle for | el campo de control debe ser una declaracion de variable entera o una asignacion entera, el campo de actualización debe contener una variable entera positiva que indica el valor que se decrementara en cada iteración. |
| D10 | Funciones | se admiten como máximo 2 parámetros |
| D11 | Pasaje de parámetros | copia-valor |
| D12 | Imprimir | los datos de salida pueden ser cadenas o expresiones (incluye funciones) no pueden ser ambos.  |
| D13 | Funcion | las funciones pueden tener 1 o 2 parámetros |
| D14 | División por cero | Error en ejecución |
| D15 | Plataforma destino | Por definir |

---

## 2. Alfabeto

| Clase | Caracteres |
|---|---|
| `L` | `a`–`z`, `A`–`Z` |
| `D` | `0`–`9` |
| `SIM` | `+ - * / ( ) { } ; , = < > ! . & \| "` |
| `BL` | espacio, tabulador, salto de línea |
| `OTRO` | cualquier otro carácter → error léxico |

---

## 3. Palabras reservadas

`real` · `entero` · `si` · `sino` · `para` · `imprimir` · `retornar` · `inicio`

Se reconocen como identificadores y se resuelven por búsqueda en tabla, no con
estados propios del autómata.

---

## 4. Tabla de tokens

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

---

## 5. Estructura del programa

Un programa del lenguaje comienza con el nombre reservado `inicio`, sin
parámetros y sin valor de retorno.
- Pueden existir otras funciones.

---

## 6. Gramática
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

<salida>              ::= IMPRIMIR '(' CADENA ')' ';'
                        | IMPRIMIR '(' <expresion> ')' ';'
                        | IMPRIMIR '(' <invocacion> ')' ';'

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

---

## 7. Semántica

| Regla | Definición |
|---|---|
| R1 | Inicialización por defecto: variables enteras se inicializan en `0` y reales en `0.0` |
| R2 | En operaciones aritméticas entre `entero` y `real`, el entero se promueve a real y el resultado de la operación es real |
| R3 | En asignación de `real` a variable de tipo `entero`, se realiza truncamiento descartando la parte decimal |
| R4 | En asignación de `entero` a variable de tipo `real`, se realiza promoción exacta a decimal |
| R5 | En comparaciones entre `entero` y `real`, el entero se promueve a real |

---

## 8. Responsabilidad de cada error

| Código | Descripción | Fase que lo detecta |
|---|---|---|
| E1 | Carácter no perteneciente al alfabeto | Léxico |
| E2 | Constante numérica mal formada (ej. `12.3.4`, `10abc`) | Léxico |
| E3 | Comentario de bloque sin cerrar al fin de archivo (`/* ...`) | Léxico |
| E4 | Identificador excede longitud máxima de 20 caracteres (advertencia y truncamiento) | Léxico |
| E5 | Sentencia o estructura mal formada | Sintáctico |
| E6 | Variable no declarada al finalizar el análisis | Semántico |
| E7 | Variable redeclarada en el mismo alcance | Semántico |
| E8 | Contador de bucle `para` real | Semántico |
| E9 | Función con más de 2 parámetros | Sintáctico / Semántico |
| E10 | Función sin parametros | Sintáctico / Semántico |
| E11 | Llamada recursiva | Semántico |
| E12 | Constante numérica fuera de rango | Léxico / Semántico |
| E13 | Cantidad o tipo incompatible de argumentos en invocación de función | Semántico |
| E14 | Variable negativa en el campo de actualizacion del bucle `para` | Semántico |
| E15 | Variable `real` en el campo de actualizacion del bucle `para` | Semántico |
| E16 | Constante `real` como variable de control en el bucle `para` | Semántico |
| E17 | División por cero en tiempo de ejecución | Ejecución |

---

## 9. Programa de ejemplo

```
real calcular(real a, entero b) {
    real var;

    si (a >= 0) {
      var = a * b;
    } sino {
      var = a / b;
    }

    retornar var;
}

inicio {
    entero i, acum;
    real res;

    res = calcular(10.5, 2);
    acum = 0;

    para (i = 3; i > 0; 1) {
        si ((i >= 2) && (res > 10.0)) {
            acum = acum + i;
        } sino {
            acum = acum - 1;
        }
    }

    imprimir(acum);
}
```
---

```
real calcularSuma(entero a, real b){
  retornar (a+b);
}

inicio {
  entero x = 10;
  real y = 2.5;
  entero i, resultado;

  resultado = calcularSuma(x,y);
  imprimir ("Resultado truncado:");
  imprimir (resultado);

  para (i=6 ; i>=0 && x>5 ; 2){
    si(i == 0){
      imprimir("Llegamos a cero");
    }
    sino{
      imprimir("Valor actual de i:");
      imprimir(i);
    }
  }
}

```
---

## 10. Automata finito

![Automata](assets/images/automata.svg)

---

## 11. Matriz de nuevos estados

| Descripción                  | ESTADOS | letra | digito | \=  | !     | <     | \>    | &     | \|    | +     | \-    | \*    | /     | (     | )     | {     | }     | ;     | ,     | .     | "     | otro  |
| ---------------------------- | ------- | ----- | ------ | --- | ----- | ----- | ----- | ----- | ----- | ----- | ----- | ----- | ----- | ----- | ----- | ----- | ----- | ----- | ----- | ----- | ----- | ----- |
| inicio                       | q0      | q1    | q2     | q4  | q6    | q8    | q10   | q12   | q13   | q14   | q15   | q16   | q17   | q20   | q21   | q22   | q23   | q24   | q25   | error | q26   | error |
| id o palabra reservada       | q1      | q1    | q1     | f   | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     |
| constante entera o real      | q2      | f     | q2     | f   | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | q3    | f     | f     |
| constante real               | q3      | f     | q3     | f   | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     |
| posible asignacion           | q4      | f     | f      | q5  | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     |
| igual                        | q5      | f     | f      | f   | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     |
| posible distinto             | q6      | error | error  | q7  | error | error | error | error | error | error | error | error | error | error | error | error | error | error | error | error | error | error |
| distinto confirmado          | q7      | f     | f      | f   | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     |
| menor o menor igual          | q8      | f     | f      | q9  | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     |
| menor igual confirmado       | q9      | f     | f      | f   | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     |
| mayor o mayor igual          | q10     | f     | f      | q11 | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     |
| mayor igual confirmado       | q11     | f     | f      | f   | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     |
| and                          | q12     | f     | f      | f   | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     |
| or                           | q13     | f     | f      | f   | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     |
| suma                         | q14     | f     | f      | f   | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     |
| resta                        | q15     | f     | f      | f   | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     |
| producto                     | q16     | f     | f      | f   | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     |
| division o inicio comentario | q17     | f     | f      | f   | f     | f     | f     | f     | f     | f     | f     | q18   | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     |
| inicio comentario            | q18     | q18   | q18    | q18 | q18   | q18   | q18   | q18   | q18   | q18   | q18   | q19   | q18   | q18   | q18   | q18   | q18   | q18   | q18   | q18   | q18   | q18   |
| posible fin comentario       | q19     | q18   | q18    | q18 | q18   | q18   | q18   | q18   | q18   | q18   | q18   | q19   | q0    | q18   | q18   | q18   | q18   | q18   | q18   | q18   | q18   | q18   |
| parentesis abre              | q20     | f     | f      | f   | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     |
| parentesis cierra            | q21     | f     | f      | f   | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     |
| llave abre                   | q22     | f     | f      | f   | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     |
| llave cierra                 | q23     | f     | f      | f   | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     |
| fin de linea                 | q24     | f     | f      | f   | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     |
| coma                         | q25     | f     | f      | f   | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     |
| inicio cadena                | q26     | q26   | q26    | q26 | q26   | q26   | q26   | q26   | q26   | q26   | q26   | q26   | q26   | q26   | q26   | q26   | q26   | q26   | q26   | q26   | f     | q26   |
| fin cadena                   | q27     | f     | f      | f   | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     | f     |

---

## 12. Matriz de tokens

| Descripción                  | Estado (q) | letra | digito | \=  | !   | <   | \>  | &   | \|  | +   | \-  | \*  | /   | (   | )   | {   | }   | ;   | ,   | .   | "   | otro |
| ---------------------------- | ---------- | ----- | ------ | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | ---- |
| inicio                       | q0         | q1    | q2     | q4  | q6  | q8  | q10 | q12 | q13 | q14 | q15 | q16 | q17 | q20 | q21 | q22 | q23 | q24 | q25 | q27 | q26 | \-2  |
| id o palabra reservada       | q1         | q1    | q1     | \-1 | \-1 | \-1 | \-1 | \-1 | \-1 | \-1 | \-1 | \-1 | \-1 | \-1 | \-1 | \-1 | \-1 | \-1 | \-1 | \-1 | \-1 | \-1  |
| constante entera o real      | q2         | 101   | q2     | 101 | 101 | 101 | 101 | 101 | 101 | 101 | 101 | 101 | 101 | 101 | 101 | 101 | 101 | 101 | 101 | q3  | 101 | 101  |
| constante real               | q3         | 102   | q3     | 102 | 102 | 102 | 102 | 102 | 102 | 102 | 102 | 102 | 102 | 102 | 102 | 102 | 102 | 102 | 102 | 102 | 102 | 102  |
| posible asignacion           | q4         | 111   | 111    | q5  | 111 | 111 | 111 | 111 | 111 | 111 | 111 | 111 | 111 | 111 | 111 | 111 | 111 | 111 | 111 | 111 | 111 | 111  |
| igual                        | q5         | 112   | 112    | 112 | 112 | 112 | 112 | 112 | 112 | 112 | 112 | 112 | 112 | 112 | 112 | 112 | 112 | 112 | 112 | 112 | 112 | 112  |
| posible distinto             | q6         | \-2   | \-2    | q7  | \-2 | \-2 | \-2 | \-2 | \-2 | \-2 | \-2 | \-2 | \-2 | \-2 | \-2 | \-2 | \-2 | \-2 | \-2 | \-2 | \-2 | \-2  |
| distinto confirmado          | q7         | 113   | 113    | 113 | 113 | 113 | 113 | 113 | 113 | 113 | 113 | 113 | 113 | 113 | 113 | 113 | 113 | 113 | 113 | 113 | 113 | 113  |
| menor o menor igual          | q8         | 114   | 114    | q9  | 114 | 114 | 114 | 114 | 114 | 114 | 114 | 114 | 114 | 114 | 114 | 114 | 114 | 114 | 114 | 114 | 114 | 114  |
| menor igual confirmado       | q9         | 116   | 116    | 116 | 116 | 116 | 116 | 116 | 116 | 116 | 116 | 116 | 116 | 116 | 116 | 116 | 116 | 116 | 116 | 116 | 116 | 116  |
| mayor o mayor igual          | q10        | 115   | 115    | q11 | 115 | 115 | 115 | 115 | 115 | 115 | 115 | 115 | 115 | 115 | 115 | 115 | 115 | 115 | 115 | 115 | 115 | 115  |
| mayor igual confirmado       | q11        | 117   | 117    | 117 | 117 | 117 | 117 | 117 | 117 | 117 | 117 | 117 | 117 | 117 | 117 | 117 | 117 | 117 | 117 | 117 | 117 | 117  |
| and                          | q12        | 118   | 118    | 118 | 118 | 118 | 118 | 118 | 118 | 118 | 118 | 118 | 118 | 118 | 118 | 118 | 118 | 118 | 118 | 118 | 118 | 118  |
| or                           | q13        | 119   | 119    | 119 | 119 | 119 | 119 | 119 | 119 | 119 | 119 | 119 | 119 | 119 | 119 | 119 | 119 | 119 | 119 | 119 | 119 | 119  |
| suma                         | q14        | 120   | 120    | 120 | 120 | 120 | 120 | 120 | 120 | 120 | 120 | 120 | 120 | 120 | 120 | 120 | 120 | 120 | 120 | 120 | 120 | 120  |
| resta                        | q15        | 121   | 121    | 121 | 121 | 121 | 121 | 121 | 121 | 121 | 121 | 121 | 121 | 121 | 121 | 121 | 121 | 121 | 121 | 121 | 121 | 121  |
| producto                     | q16        | 122   | 122    | 122 | 122 | 122 | 122 | 122 | 122 | 122 | 122 | 122 | 122 | 122 | 122 | 122 | 122 | 122 | 122 | 122 | 122 | 122  |
| division o inicio comentario | q17        | 123   | 123    | 123 | 123 | 123 | 123 | 123 | 123 | 123 | 123 | q18 | 123 | 123 | 123 | 123 | 123 | 123 | 123 | 123 | 123 | 123  |
| inicio comentario            | q18        | q18   | q18    | q18 | q18 | q18 | q18 | q18 | q18 | q18 | q18 | q19 | q18 | q18 | q18 | q18 | q18 | q18 | q18 | q18 | q18 | q18  |
| posible fin comentario       | q19        | q18   | q18    | q18 | q18 | q18 | q18 | q18 | q18 | q18 | q18 | q19 | q0  | q18 | q18 | q18 | q18 | q18 | q18 | q18 | q18 | q18  |
| parentesis abre              | q20        | 124   | 124    | 124 | 124 | 124 | 124 | 124 | 124 | 124 | 124 | 124 | 124 | 124 | 124 | 124 | 124 | 124 | 124 | 124 | 124 | 124  |
| parentesis cierra            | q21        | 125   | 125    | 125 | 125 | 125 | 125 | 125 | 125 | 125 | 125 | 125 | 125 | 125 | 125 | 125 | 125 | 125 | 125 | 125 | 125 | 125  |
| llave abre                   | q22        | 126   | 126    | 126 | 126 | 126 | 126 | 126 | 126 | 126 | 126 | 126 | 126 | 126 | 126 | 126 | 126 | 126 | 126 | 126 | 126 | 126  |
| llave cierra                 | q23        | 127   | 127    | 127 | 127 | 127 | 127 | 127 | 127 | 127 | 127 | 127 | 127 | 127 | 127 | 127 | 127 | 127 | 127 | 127 | 127 | 127  |
| fin de linea                 | q24        | 128   | 128    | 128 | 128 | 128 | 128 | 128 | 128 | 128 | 128 | 128 | 128 | 128 | 128 | 128 | 128 | 128 | 128 | 128 | 128 | 128  |
| coma                         | q25        | 129   | 129    | 129 | 129 | 129 | 129 | 129 | 129 | 129 | 129 | 129 | 129 | 129 | 129 | 129 | 129 | 129 | 129 | 129 | 129 | 129  |
| inicio cadena                | q26        | q26   | q26    | q26 | q26 | q26 | q26 | q26 | q26 | q26 | q26 | q26 | q26 | q26 | q26 | q26 | q26 | q26 | q26 | q26 | q27 | q26  |
| fin cadena                   | q27        | 132   | 132    | 132 | 132 | 132 | 132 | 132 | 132 | 132 | 132 | 132 | 132 | 132 | 132 | 132 | 132 | 132 | 132 | 132 | 132 | 132  |

---

## 13. Matriz de funciones

| Descripción                  | ESTADOS | letra   | digito  | \=      | !       | <       | \>      | &       | \|      | +       | \-      | \*      | /       | (       | )       | {       | }       | ;       | ,       | .       | "       | ESP-TAB | otro    |
| ---------------------------- | ------- | ------- | ------- | ------- | ------- | ------- | ------- | ------- | ------- | ------- | ------- | ------- | ------- | ------- | ------- | ------- | ------- | ------- | ------- | ------- | ------- | ------- | ------- |
| inicio                       | q0      | q1-f1   | q2-f2   | q4-fn   | q6-fn   | q8-fn   | q10-fn  | q12-fn  | q13-fn  | q14-fn  | q15-fn  | q16-fn  | q17-fn  | q20-fn  | q21-fn  | q22-fn  | q23-fn  | q24-fn  | q25-fn  | f-error | q26-f10 | fn      | f-error |
| id o palabra reservada       | q1      | q1-f3   | q1-f3   | f-f4    | f-f4    | f-f4    | f-f4    | f-f4    | f-f4    | f-f4    | f-f4    | f-f4    | f-f4    | f-f4    | f-f4    | f-f4    | f-f4    | f-f4    | f-f4    | f-f4    | f-f4    | f-f4    | f-f4    |
| constante entera o real      | q2      | f-f6    | q2-f5   | f-f6    | f-f6    | f-f6    | f-f6    | f-f6    | f-f6    | f-f6    | f-f6    | f-f6    | f-f6    | f-f6    | f-f6    | f-f6    | f-f6    | f-f6    | f-f6    | q3-f7   | f-f6    | f-f6    | f-f6    |
| constante real               | q3      | f-f9    | q3-f8   | f-f9    | f-f9    | f-f9    | f-f9    | f-f9    | f-f9    | f-f9    | f-f9    | f-f9    | f-f9    | f-f9    | f-f9    | f-f9    | f-f9    | f-f9    | f-f9    | f-f9    | f-f9    | f-f9    | f-f9    |
| posible asignacion           | q4      | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    |
| igual                        | q5      | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    |
| posible distinto             | q6      | f-error | f-error | q7-fn   | f-error | f-error | f-error | f-error | f-error | f-error | f-error | f-error | f-error | f-error | f-error | f-error | f-error | f-error | f-error | f-error | f-error | f-error | f-error |
| distinto confirmado          | q7      | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    |
| menor o menor igual          | q8      | f-fn    | f-fn    | q9-fn   | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    |
| menor igual confirmado       | q9      | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    |
| mayor o mayor igual          | q10     | f-fn    | f-fn    | q11-fn  | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    |
| mayor igual confirmado       | q11     | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    |
| and                          | q12     | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    |
| or                           | q13     | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    |
| suma                         | q14     | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    |
| resta                        | q15     | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    |
| producto                     | q16     | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    |
| division o inicio comentario | q17     | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | q18-f13 | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    |
| inicio comentario            | q18     | q18-f13 | q18-f13 | q18-f13 | q18-f13 | q18-f13 | q18-f13 | q18-f13 | q18-f13 | q18-f13 | q18-f13 | q19-f13 | q18-f13 | q18-f13 | q18-f13 | q18-f13 | q18-f13 | q18-f13 | q18-f13 | q18-f13 | q18-f13 | q18-f13 | q18-f13 |
| posible fin comentario       | q19     | q18     | q18     | q18-f13 | q18-f13 | q18-f13 | q18-f13 | q18-f13 | q18-f13 | q18-f13 | q18-f13 | q19-f13 | q0-f13  | q18-f13 | q18-f13 | q18-f13 | q18-f13 | q18-f13 | q18-f13 | q18-f13 | q18-f13 | q18-f13 | q18-f13 |
| parentesis abre              | q20     | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    |
| parentesis cierra            | q21     | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    |
| llave abre                   | q22     | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    |
| llave cierra                 | q23     | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    |
| fin de linea                 | q24     | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    |
| coma                         | q25     | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    |
| inicio cadena                | q26     | q26-f11 | q26-f11 | q26-f11 | q26-f11 | q26-f11 | q26-f11 | q26-f11 | q26-f11 | q26-f11 | q26-f11 | q26-f11 | q26-f11 | q26-f11 | q26-f11 | q26-f11 | q26-f11 | q26-f11 | q26-f11 | q26-f11 | q27-f12 | q26-f11 | q26-f11 |
| fin cadena                   | q27     | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    | f-fn    |

---

| FUNCIONES                        | DESCRIPCION                                                                                                          |
| -------------------------------- | -------------------------------------------------------------------------------------------------------------------- |
| f1 (iniciar bufer id)            | : limpia el bufe, asigna el tipo "identificador" y almacena el caracter                                              |
| f2 (iniciar bufer de numero)     | : limpia el bufer de texto, asigna el tipo "constante numerica" y guarda el primer digito                            |
| f3 (acumular y validar id)       | : agrega la letra o digito al bufer, controla la restricción de 20 caracteres, pasado los 20 no los agrega al bufer  |
| f4 (reconocer id/pal reservada)  | : finaliza la recolección, busca en la tabla de palabras reservadas, si no existe asigna la categorira de token a id |
| f5 (acumular entero)             | : añade el dígito al bufer                                                                                           |
| f6 (reconocer y validar entero)  | : finaliza la constante entera, convierte el texto acumulado a entero y comprueba el rango, si exede notifica        |
| f7 (acumular real/ transición)   | : agrega el punto al bufer de enteros para transformarlo a real, inicializa el contador de mantiza                   |
| f8 (acumular real)               | : agrega el digito decimal al bufer de reales                                                                        |
| f9 (reconocer y validar real)    | : finaliza la constante real, comprueba el rango, si exede notifica                                                  |
| f10 (iniciar cadena)             | : limpia el bufer y almacena el string sin comillas                                                                  |
| f11 (acumular cadena)            | : agrega cualquier caracter dentro del cuerpo de la cadena                                                           |
| f12 (reconocer cadena)           | : cierra el almacenamiento de la constante                                                                           |
| f13 (ignorar caracter)           | : utilizado en el cuerpo y cierre de comentarios, descarta lo leido sin guardarlo                                    |
| fn (función nula)                | : no realiza ninguna modificación sobre el bufer                                                                     |
| f-error (manejo de error lexico) | : reporta carácter no valido o secuencia erronea                                                                     |

---

## 14 Matriz de unreads

| Descripción                  | ESTADOS | letra | digito | \= | ! | < | \> | & | \| | + | \- | \* | / | ( | ) | { | } | ; | , | . | " | ESP-TAB | otro |
| ---------------------------- | ------- | ----- | ------ | -- | - | - | -- | - | -- | - | -- | -- | - | - | - | - | - | - | - | - | - | ------- | ---- |
| inicio                       | q0      | 0     | 0      | 0  | 0 | 0 | 0  | 0 | 0  | 0 | 0  | 0  | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0       | 0    |
| id o palabra reservada       | q1      | 0     | 0      | 1  | 1 | 1 | 1  | 1 | 1  | 1 | 1  | 1  | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 0       | 1    |
| constante entera o real      | q2      | 1     | 0      | 1  | 1 | 1 | 1  | 1 | 1  | 1 | 1  | 1  | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 0 | 1 | 0       | 1    |
| constante real               | q3      | 1     | 0      | 1  | 1 | 1 | 1  | 1 | 1  | 1 | 1  | 1  | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 0       | 1    |
| posible asignacion           | q4      | 1     | 1      | 0  | 1 | 1 | 1  | 1 | 1  | 1 | 1  | 1  | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 0       | 1    |
| igual                        | q5      | 1     | 1      | 1  | 1 | 1 | 1  | 1 | 1  | 1 | 1  | 1  | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 0       | 1    |
| posible distinto             | q6      | 1     | 1      | 0  | 1 | 1 | 1  | 1 | 1  | 1 | 1  | 1  | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 0       | 1    |
| distinto confirmado          | q7      | 1     | 1      | 1  | 1 | 1 | 1  | 1 | 1  | 1 | 1  | 1  | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 0       | 1    |
| menor o menor igual          | q8      | 1     | 1      | 0  | 1 | 1 | 1  | 1 | 1  | 1 | 1  | 1  | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 0       | 1    |
| menor igual confirmado       | q9      | 1     | 1      | 1  | 1 | 1 | 1  | 1 | 1  | 1 | 1  | 1  | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 0       | 1    |
| mayor o mayor igual          | q10     | 1     | 1      | 0  | 1 | 1 | 1  | 1 | 1  | 1 | 1  | 1  | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 0       | 1    |
| mayor igual confirmado       | q11     | 1     | 1      | 1  | 1 | 1 | 1  | 1 | 1  | 1 | 1  | 1  | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 0       | 1    |
| and                          | q12     | 1     | 1      | 1  | 1 | 1 | 1  | 1 | 1  | 1 | 1  | 1  | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 0       | 1    |
| or                           | q13     | 1     | 1      | 1  | 1 | 1 | 1  | 1 | 1  | 1 | 1  | 1  | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 0       | 1    |
| suma                         | q14     | 1     | 1      | 1  | 1 | 1 | 1  | 1 | 1  | 1 | 1  | 1  | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 0       | 1    |
| resta                        | q15     | 1     | 1      | 1  | 1 | 1 | 1  | 1 | 1  | 1 | 1  | 1  | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 0       | 1    |
| producto                     | q16     | 1     | 1      | 1  | 1 | 1 | 1  | 1 | 1  | 1 | 1  | 1  | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 0       | 1    |
| division o inicio comentario | q17     | 1     | 1      | 1  | 1 | 1 | 1  | 1 | 1  | 1 | 1  | 0  | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 0       | 1    |
| inicio comentario            | q18     | 0     | 0      | 0  | 0 | 0 | 0  | 0 | 0  | 0 | 0  | 0  | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0       | 0    |
| posible fin comentario       | q19     | 0     | 0      | 0  | 0 | 0 | 0  | 0 | 0  | 0 | 0  | 0  | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0       | 0    |
| parentesis abre              | q20     | 1     | 1      | 1  | 1 | 1 | 1  | 1 | 1  | 1 | 1  | 1  | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 0       | 1    |
| parentesis cierra            | q21     | 1     | 1      | 1  | 1 | 1 | 1  | 1 | 1  | 1 | 1  | 1  | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 0       | 1    |
| llave abre                   | q22     | 1     | 1      | 1  | 1 | 1 | 1  | 1 | 1  | 1 | 1  | 1  | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 0       | 1    |
| llave cierra                 | q23     | 1     | 1      | 1  | 1 | 1 | 1  | 1 | 1  | 1 | 1  | 1  | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 0       | 1    |
| fin de linea                 | q24     | 1     | 1      | 1  | 1 | 1 | 1  | 1 | 1  | 1 | 1  | 1  | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 0       | 1    |
| coma                         | q25     | 1     | 1      | 1  | 1 | 1 | 1  | 1 | 1  | 1 | 1  | 1  | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 0       | 1    |
| inicio cadena                | q26     | 0     | 0      | 0  | 0 | 0 | 0  | 0 | 0  | 0 | 0  | 0  | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0       | 0    |
| fin cadena                   | q27     | 1     | 1      | 1  | 1 | 1 | 1  | 1 | 1  | 1 | 1  | 1  | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 0       | 1    |

---

## 10. Fuera de alcance

Se deja constancia de lo que el lenguaje **no** incluye:

- Tipos de datos adicionales (caracter, cadena de caracteres, arreglos, booleanos)
- Recursión
- Estructuras de control iterativas distintas a `para` (no hay `while` o `do while`)
- Operador lógico de negación para expresiones, condiciones o IDs.
- Combinar cadenas y expresiones en una misma función `IMPRIMIR`.