# Spec — Diseño del lenguaje: ...

**Grupo:** B · **Lenguaje de implementación:** C  
**Estado:** En desarrollo.

---

## 1. Decisiones globales

| #  | Decisión | Valor |
|----|---|---|
| D1 | Tipo de datos | Dos tipos:<br> Entero con signo (32 bits)<br> Real con signo (64 bits).|
| D2 | Rango | Entero: −2.147.483.648 a 2.147.483.647<br>Real: 2.225 × 10⁻³⁰⁸ a 1.797 × 10³⁰⁸  |
| D3 | Declaración | Obligatoria, debe preceder al primer uso |
| D4 | Alcance | Único y global: no hay variables locales |
| D5 | Sensibilidad a mayúsculas | Sí. `Total` y `total` son variables distintas |
| D6 | Longitud máxima de identificador | 20 caracteres; más largo se trunca con advertencia |
| D7 | Comentarios | De línea, `//` hasta fin de línea |
| D8 | Resultado de una comparación | No es un valor. Solo aparece en condiciones |
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
| 100 | `ID` | id |
| 101 | `CTE_E` | constante entera |
| 102 | `CTE_R` | constante real |
| 103 | `CADENA` | cadena de caracteres |
| 104 | `INICIO` | `inicio` |
| 105 | `SI` | `si` |
| 106 | `SINO` | `sino` |
| 107 | `ENTERO` | `entero` |
| 108 | `REAL` | `real` |
| 109 | `INICIO_LLAVE` | `{` |
| 110 | `FIN_LLAVE` | `}` |
| 111 | `INICIO_PARENTESIS` | `(` |
| 112 | `FIN_PARENTESIS` | `)` |
| 113 | `MAYOR` | `>` |
| 114 | `MAYOR_IGUAL` | `>=` |
| 115 | `MENOR` | `<` |
| 116 | `DISTINTO` | `!=` |
| 117 | `MENOR_IGUAL` | `<=` |
| 118 | `AND` | `&&` |
| 119 | `OR` | `\|\|` |
| 120 | `SUMA` | `+` |
| 121 | `INCREMENTO` | `++` |
| 122 | `ASIG_SUMA` | `+=` |
| 123 | `RESTA` | `-` |
| 124 | `DECREMENTO` | `--` |
| 125 | `ASIG_RESTA` | `-=` |
| 126 | `ASIGNACION` | `=` |
| 127 | `IGUAL` | `==` |
| 128 | `FIN_LINEA` | `;` |
| 129 | `COMA` | `,` |
| 130 | `DIVISION` | `/` |
| 131 | `ASIG_DIVISION` | `/=` |
| 132 | `COMENTARIO` | comentario |
| 133 | `MULTIPLICACION` | `*` |
| 134 | `ASIG_MULT` | `*=` |
| 135 | `NEGACION` | `!` |

---

## 5. Estructura del programa

Un programa \*nombre del lenguaje\* es **una sola función**, de nombre reservado `inicio`, sin
parámetros y sin valor de retorno. No hay otras funciones.

---

## 6. Gramática

`` POR DEFINIR ``



## 7. Semántica

| Regla | Definición |
|----|---|
| R1 | Usar un `ID` no declarado es error semántico |
| R2 | Declarar dos veces el mismo `ID` es error semántico |
| R3 | Toda variable se inicializa en 0 antes de la primera sentencia |
| R4 | Las constantes se registran en la tabla de símbolos con nombre `_valor` |
| R5 | Una constante fuera del rango D2 es error semántico, con línea |
| R6 | `imprimir` imprime el valor seguido de un salto de línea |

---

## 8. Responsabilidad de cada error

| Código | Descripción | Fase que lo detecta |
|---|---|---|
| E1 | Carácter fuera del alfabeto | Léxico |
| E2 | Constante mal formada (`12x`) | Léxico |
| E3 | Constante fuera de rango (R5) | Léxico |
| E4 | Sentencia mal formada | Sintáctico |
| E5 | Variable no declarada (R1) | Sintáctico (sobre tabla de símbolos) |
| E6 | Variable redeclarada (R2) | Sintáctico (sobre tabla de símbolos) |
| E7 | División por cero (D9) | Ejecución (código emitido por GCA) |

Ninguno de E1 a E6 aborta la compilación: se registran y se sigue leyendo.

---

## 9. Programa de ejemplo

``POR DEFINIR``

---

## 10. Fuera de alcance

Se deja constancia de lo que el lenguaje **no** tiene, para que ninguna fase lo asuma:

`` POR DEFINIR ``