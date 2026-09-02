# Spec — Diseño del lenguaje: ...

**Grupo:** B · **Lenguaje de implementación:** C  
**Estado:** En desarrollo.

---

## 1. Decisiones globales

| #  | Decisión | Valor |
|----|---|---|
| D1 | Tipo de datos | Dos tipos: Entero con signo y Real con signo |
| D2 | Rango | Entero: 32 bits y Real: 64 bits  |
| D3 | Declaración | Obligatoria con tipo; no se exige orden entre declaración y primer uso. |
| D4 | Alcance y Gestión de memoria | Memoria estática: variables globales, locales y parámetros residen en posiciones fijas asignadas en tiempo de compilación. No existe stack dinámico de ejecución |
| D5 | Sensibilidad a mayúsculas | Sí. `Total` y `total` son variables distintas |
| D6 | Longitud máxima de identificador | 20 caracteres; más largo se trunca con advertencia |
| D7 | Comentarios | De bloque, delimitados por `/*` y `*/` |
| D8 | Compatibilidad de tipos | Total entre enteros y reales, con conversiones implícitas automáticas visibles en tercetos |
| D9 | División por cero | Error en ejecución, con mensaje y cancelación |
| D10 | Plataforma destino | Por definir |

---

## 2. Alfabeto

| Clase | Caracteres |
|---|---|
| `L` | `a`–`z`, `A`–`Z` |
| `D` | `0`–`9` |
| `SIM` | `+ - * / ( ) { } ; , = < > !` |
| `BL` | espacio, tabulador, salto de línea |
| `OTRO` | cualquier otro carácter → error léxico |

---

## 3. Palabras reservadas

`real` · `entero` · `si` · `sino` · `mientras` · `para` · `imprimir` · `retornar`

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
| 118 | `AND` | `&&` |
| 119 | `OR` | `\|\|` |
| 120 | `INCREMENTO` | `++` |
| 121 | `ASIG_SUMA` | `+=` |
| 122 | `DECREMENTO` | `--` |
| 123 | `ASIG_RESTA` | `-=` |
| 124 | `IGUAL` | `==` |
| 125 | `ASIG_DIVISION` | `/=` |
| 126 | `ASIG_MULT` | `*=` |
| — | Literales | `+ - * / ( ) { } ; , !` se devuelven como su propio valor |

---

## 5. Estructura del programa

Un programa del lenguaje comienza con el nombre reservado `inicio`, sin
parámetros y sin valor de retorno.
<br> Pueden existir otras funciones.

---

## 6. Gramática
```
<programa>          ::= <unidades>

<unidades>          ::= <unidades> <unidad>
                      | <unidad>

<unidad>            ::= <dec_funcion>
                      | <principal>

<principal>         ::= INICIO '(' ')' <bloque>

<dec_funcion>       ::= <tipo> ID '(' <parametros> ')' <cuerpo_funcion>

<cuerpo_funcion>    ::= '{' <sentencias> <retorno> '}'

<parametros>        ::= <lista_parametros>
                      | lambda

<lista_parametros>  ::= <tipo> ID ',' <tipo> ID
                      | <tipo> ID

<tipo>              ::= ENTERO
                      | REAL

<bloque>            ::= '{' <sentencias> '}'

<sentencias>        ::= <sentencias> <sentencia>
                      | <sentencia>

<sentencia>         ::= <declaracion>
                      | <asignacion>
                      | <seleccion>
                      | <iteracion>
                      | <salida>
                      | <retorno>
                      | <invocacion> ';'

<declaracion>       ::= <tipo> <lista_ids> ';'

<lista_ids>         ::= <lista_ids> ',' ID
                      | ID

<asignacion>        ::= ID <op_asig> <expresion> ';'

<op_asig>           ::= ASIG
                      | ASIG_SUMA
                      | ASIG_RESTA
                      | ASIG_MULT
                      | ASIG_DIVISION

<seleccion>         ::= SI '(' <condicional> ')' <bloque>
                      | SI '(' <condicional> ')' <bloque> SINO <bloque>

<iteracion>         ::= PARA '(' <asignacion_para> ';' <condicion> ';' ID DECREMENTO ')' <bloque>

<asignacion_para>   ::= ID ASIG CTE_E
                      | ID

<salida>            ::= IMPRIMIR '(' <expresion> ')' ';'

<retorno>           ::= RETORNAR <expresion> ';'
                      | RETORNAR ';'

<condicional>       ::= <condicional> OR <termino_logico>
                      | <termino_logico>

<termino_logico>    ::= <termino_logico> AND <factor_logico>
                      | <factor_logico>

<factor_logico>     ::= <condicion>
                      | '(' <condicional> ')'
                      | '!' <factor_logico>

<condicion>         ::= <expresion> <comparador> <expresion>

<comparador>        ::= IGUAL 
                      | DISTINTO 
                      | MENOR 
                      | MAYOR 
                      | MENOR_IGUAL 
                      | MAYOR_IGUAL

<expresion>         ::= <expresion> '+' <termino>
                      | <expresion> '-' <termino>
                      | <termino>

<termino>           ::= <termino> '*' <factor>
                      | <termino> '/' <factor>
                      | <factor>

<factor>            ::= ID
                      | CTE_E
                      | CTE_R
                      | <invocacion>
                      | '(' <expresion> ')'
                      | '-' <factor>

<invocacion>        ::= ID '(' <argumentos> ')'

<argumentos>        ::= <expresion> ',' <expresion>
                      | <expresion>
                      | lambda


```

---

## 7. Semántica

| Regla | Definición |
|---|---|
| R1 | Toda variable debe ser declarada explícitamente con su tipo (`entero` o `real`). Puede usarse antes o después de su declaración, pero si al finalizar el análisis no fue declarada, es error semántico |
| R2 | Redeclarar un identificador dentro del mismo alcance es error semántico |
| R3 | Inicialización por defecto: variables enteras se inicializan en `0` y reales en `0.0` |
| R4 | En operaciones aritméticas entre `entero` y `real`, el entero se promueve implícitamente a real y el resultado de la operación es real |
| R5 | En asignación de `real` a variable de tipo `entero`, se realiza truncamiento implícito descartando la parte fraccionaria |
| R6 | En asignación de `entero` a variable de tipo `real`, se realiza promoción implícita exacta a punto flotante |
| R7 | En comparaciones entre `entero` y `real`, el operando entero se promueve implícitamente a real |
| R8 | El bucle `para` opera de forma decreciente. Su contador debe ser obligatoriamente de tipo `entero`, el uso de un `real` como contador es error semántico |
| R9 | Las funciones admiten hasta 2 parámetros pasados por valor. Los parámetros y variables locales se alojan en direcciones estáticas fijas |
| R10 | La recursión directa está prohibida y debe ser reportada como error semántico |
| R11 | Las constantes numéricas fuera de los rangos soportados generan error semántico |
| R12 | El programa principal es una función con nombre reservado `inicio` que indica el punto de inicio de la ejecución |

---

## 8. Responsabilidad de cada error

| Código | Descripción | Fase que lo detecta |
|---|---|---|
| E1 | Carácter no perteneciente al alfabeto | Léxico |
| E2 | Constante numérica mal formada (ej. `12.3.4`, `10abc`) | Léxico |
| E3 | Comentario de bloque sin cerrar al fin de archivo (`/* ...`) | Léxico |
| E4 | Identificador excede longitud máxima de 20 caracteres (advertencia y truncamiento) | Léxico |
| E5 | Error de sintaxis / Sentencia o estructura mal formada | Sintáctico |
| E6 | Variable no declarada al finalizar el análisis (R1) | Semántico (sobre tabla de símbolos) |
| E7 | Variable redeclarada en el mismo alcance (R2) | Semántico (sobre tabla de símbolos) |
| E8 | Contador de bucle `para` no entero / uso de variable real en el control (R8) | Semántico |
| E9 | Función con más de 2 parámetros declarados (R9) | Sintáctico / Semántico |
| E10 | Llamada recursiva directa detectada (R10) | Semántico |
| E11 | Constante numérica fuera de rango (R11) | Léxico / Semántico |
| E12 | Cantidad o tipo incompatible de argumentos en invocación de función | Semántico |
| E13 | División por cero en tiempo de ejecución (D11) | Ejecución (código emitido por GCA) |

Ninguno de E1 a E12 aborta la compilación: se registran y se sigue leyendo.

---

## 9. Programa de ejemplo

```
real calcular(real base, entero factor) {
    retornar base * factor;
}

inicio() {
    res = calcular(10.5, 2);
    acum = 0;

    entero i, acum;
    real res;

    para (i = 3; i > 0; i--) {
        si ((i >= 2) && (res > 10.0)) {
            acum += i;
        } sino {
            acum -= 1;
        }
    }

    acum = res; 

    imprimir(acum);
}
```

Salida esperada: `21`

---

## 10. Fuera de alcance

Se deja constancia de lo que el lenguaje **no** incluye:

- Tipos de datos adicionales (caracteres `char`, cadenas `string` mutables, arreglos, estructuras o tipos booleanos nativos como variables)
- Modos de pasaje de parámetros por referencia o más de 2 parámetros por función
- Recursión
- Estructuras en memoria dinámica (pila dinámica de ejecución, heap, punteros o asignación dinámica `malloc`/`free`)
- Estructuras de control iterativas distintas a `para` (no hay `while` o `do while`)