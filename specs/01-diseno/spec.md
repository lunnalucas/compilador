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
| D12 | Imprimir | cada linea imprime una cadena de caracteres, una expresión o una invocación |
| D13 | División por cero | Error en ejecución |
| D14 | Plataforma destino | Por definir |

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
                        | lambda

<bloque>              ::= '{' <sentencias> '}'

<bloque_i>            ::= '{' <sentencias_i> '}'

<sentencias_i>        ::= <sentencias_i> <sentencia_i>
                        | <sentencia_i>

<sentencia_i>         ::= <declaracion>
                        | <asignacion>
                        | <seleccion>
                        | <bucle>
                        | <salida>

<sentencias>          ::= <sentencias> <sentencia>
                        | <sentencia>

<sentencia>           ::= <declaracion>
                        | <asignacion>
                        | <seleccion>
                        | <bucle>
                        | <salida>
                        | <retorno>

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

<comparador>          ::= IGUAL
                        | DISTINTO
                        | MENOR
                        | MAYOR
                        | MENOR_IGUAL
                        | MAYOR_IGUAL

<bucle>               ::= PARA '(' <control> ';' <condicional> ';' CTE_E ')' <bloque>

<control>             ::= ENTERO ID '=' CTE_E
                        | ID '=' CTE_E

<salida>              ::= IMPRIMIR '(' CADENA ')' ';'
                        | IMPRIMIR '(' <expresion> ')' ';'
                        | IMPRIMIR '(' <invocacion> ')' ';'

<retorno>             ::= RETORNAR <expresion> ';'

<argumentos>          ::= <expresion> ',' <expresion>
                        | <expresion>
                        | lambda

<expresion>           ::= <expresion> '+' <termino>
                        | <expresion> '-' <termino>
                        | <termino>

<termino>             ::= <termino> '*' <factor>
                        | <termino> '/' <factor>
                        | <factor>

<factor>              ::= ID
                        | CTE_E
                        | CTE_R
                        | '(' <expresion> ')'

<tipo>                ::= ENTERO
                        | REAL
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
| E10 | Llamada recursiva | Semántico |
| E11 | Constante numérica fuera de rango | Léxico / Semántico |
| E12 | Cantidad o tipo incompatible de argumentos en invocación de función | Semántico |
| E13 | Variable negativa en el campo de actualizacion del bucle `para` | Semántico |
| E14 | Variable `real` en el campo de actualizacion del bucle `para` | Semántico |
| E15 | Constante `real` como variable de control en el bucle `para` | Semántico |
| E16 | División por cero en tiempo de ejecución | Ejecución |

---

## 9. Programa de ejemplo

```
real calcular(real base, entero factor) {
    retornar base * factor;
}

inicio() {
    entero i, acum;
    real res;

    res = calcular(10.5, 2);
    acum = 0;

    para (i = 3; i > 0; i--) {
        si ((i >= 2) && (res > 10.0)) {
            acum = acum + i;
        } sino {
            acum = acum - 1;
        }
    }

    acum = res; 

    imprimir(acum);
}
```
---

```
real calcularSuma(entero a, real b){
  retornar (a+b);
}

inicio(){
  entero x = 10;
  real y = 2.5;
  entero i, resultado;

  resultado = calcularSuma(x,y);
  imprimir ("Resultado con Redondeo: ");
  imprimir (resultado);

  para (i=6 ; i>=0 && x>5 ; i--){
    si(i == 0){
      imprimir("Llegamos a cero");
    }
    sino{
      imprimir("Valor actual de i: ");
      imprimir(i);
    }
  }
}

```
---

## 10. Fuera de alcance

Se deja constancia de lo que el lenguaje **no** incluye:

- Tipos de datos adicionales (caracter, cadena de caracteres, arreglos, booleanos)
- Recursión
- Estructuras de control iterativas distintas a `para` (no hay `while` o `do while`)