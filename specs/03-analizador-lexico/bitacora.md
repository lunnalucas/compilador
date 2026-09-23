# Bitácora — Analizador léxico de beta

**Grupo:** B · **Lenguaje de implementación:** C
**Spec de referencia:** `specs/03-analizador-lexico/spec.md`
**Modelos usados:** GPT-5.6

---

## Iteración 1 — Incorporación de las tablas del autómata
**Fecha:** 22/09/2026 · GPT-5.6

**Qué pedí:** Pasar a código C las matrices y tablas definidas en la
especificación del analizador léxico para implementar `yylex()` posteriormente.

**Qué se hizo:** Se creó `src/lexico/` con los siguientes archivos:

- `lexer_tables.h`: estados, eventos, acciones, códigos de tokens,
  declaraciones de las matrices y tabla de palabras reservadas.
- `lexer_tables.c`: matrices `nuevo_estado`, `proceso` y `token`, tabla de
  palabras reservadas y función `get_evento()`.
- `README.md`: descripción breve de la organización y del uso previsto.

Las tablas se dejaron como arreglos para que el futuro lexer pueda
recorrer el autómata mediante `proceso[estado][evento]`,
`nuevo_estado[estado][evento]` y `token[estado][evento]`.

**Decisiones tomadas:**

- Las palabras reservadas (`inicio`, `entero`, `real`, `si`, `sino`, `para`,
  `imprimir` y `retornar`) se mantienen en una tabla separada. Se reconocen
  inicialmente como identificadores y se resuelven al cerrar el lexema, sin
  agregar estados específicos para cada palabra.
- Los estados `-1` y `-2` representan finalización y error,
  respectivamente.
- `get_evento()` centraliza la clasificación de caracteres en las columnas del
  autómata.
- La tabla `proceso` conserva la columna de espacios, mientras que las otras
  matrices mantienen una columna alineada para que el acceso por índice sea
  directo y sencillo.


**Impacto en la spec:** No se modificaron las reglas del lenguaje. Se agregó
la implementación inicial de las tablas en `src/lexico/`.

---

## Estado al cierre de la iteración

Las tablas del autómata ya están disponibles como código C. El siguiente paso es implementar `yylex()` usando estas
estructuras.

---

## Iteración 2 — Implementación de `yylex()`
**Fecha:** 22/09/2026 · GPT-5.6

**Qué pedí:** Implementar el lexer sobre las tablas y estructuras
creadas en la iteración anterior.

**Qué se hizo:** Se agregaron `src/lexico/lexer.h` y
`src/lexico/lexer.c`. La interfaz expone:

- `lexer_init(FILE *)` para asociar el archivo fuente.
- `yylex()` para devolver un token por llamada.
- `yytext`, `yyleng` y `yylineno` para consultar el lexema actual, su longitud
  y la línea de origen.

El lexer implementa:

- identificadores y palabras reservadas;
- constantes enteras y reales;
- operadores simples y dobles (`=`, `==`, `!=`, `<=` y `>=`);
- operadores aritméticos, lógicos y delimitadores;
- cadenas delimitadas por comillas;
- comentarios de bloque `/* ... */`;
- espacios, tabuladores y saltos de línea;
- errores por caracteres inválidos, secuencias incompletas, números mal
  formados, identificadores truncados y comentarios o cadenas sin cerrar.

**Decisiones tomadas:**

- Se usa un carácter pendiente (`unread`) para no perder el primer carácter que
  ya fue leído pero pertenece al token siguiente.
- Las palabras reservadas se buscan en `reserved_words`, sin crear estados
  adicionales para cada palabra.
- Los identificadores se almacenan con un máximo de 20 caracteres.
- Los errores se informan por `stderr` y el análisis continúa cuando es seguro
  hacerlo.


**Impacto en la spec:** Se implementó la interfaz `yylex()` prevista en la
sección 1.

---

## Estado al cierre de la iteración

El analizador léxico ya puede leer un `FILE *` y producir tokens de `beta` uno
por uno.
