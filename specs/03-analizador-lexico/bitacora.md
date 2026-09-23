# Bitácora — Analizador léxico de beta

**Grupo:** B · **Lenguaje de implementación:** C
**Spec de referencia:** `specs/03-analizador-lexico/spec.md`
**Modelos usados:** GPT-5.6

---

## Iteración 1 — Incorporación de las tablas del autómata
**Fecha:** 22/09/2026 · GPT-5.6

**Qué pedí:** Pasar a código C las matrices y tablas definidas en la
especificación del analizador léxico, dejando una estructura simple y fácil de
navegar para implementar `yylex()` posteriormente.

**Qué se hizo:** Se creó `src/lexico/` con los siguientes archivos:

- `lexer_tables.h`: estados, eventos, acciones, códigos de tokens,
  declaraciones de las matrices y tabla de palabras reservadas.
- `lexer_tables.c`: matrices `nuevo_estado`, `proceso` y `token`, tabla de
  palabras reservadas y función `get_event()`.
- `README.md`: descripción breve de la organización y del uso previsto.

Las tablas se dejaron como arreglos explícitos para que el futuro lexer pueda
recorrer el autómata mediante `proceso[estado][evento]`,
`nuevo_estado[estado][evento]` y `token[estado][evento]`, en lugar de reemplazar
las reglas por una cadena de condicionales.

**Decisiones tomadas:**

- Las palabras reservadas (`inicio`, `entero`, `real`, `si`, `sino`, `para`,
  `imprimir` y `retornar`) se mantienen en una tabla separada. Se reconocen
  inicialmente como identificadores y se resuelven al cerrar el lexema, sin
  agregar estados específicos para cada palabra.
- Los estados `-1` y `-2` representan finalización y error,
  respectivamente.
- `get_event()` centraliza la clasificación de caracteres en las columnas del
  autómata.
- La tabla `proceso` conserva la columna de espacios, mientras que las otras
  matrices mantienen una columna alineada para que el acceso por índice sea
  directo y sencillo.

**Qué se revisó:** La especificación contiene algunas diferencias entre las
matrices y otras secciones, especialmente en la representación de ciertas
columnas y del punto (`.`). No se modificaron las reglas del lenguaje ni se
inventaron transiciones nuevas: se conservaron los valores documentados y se
dejó la resolución de esas diferencias para la implementación completa de
`yylex()`.

**Impacto en la spec:** No se modificaron las reglas del lenguaje. Se agregó
la implementación inicial de las tablas en `src/lexico/`; `yylex()` y la
integración con la tabla de símbolos quedan para una iteración posterior.

---

## Estado al cierre de la iteración

Las tablas del autómata ya están disponibles como código C reutilizable y
documentado. El siguiente paso es implementar `yylex()` usando estas
estructuras, incluyendo el manejo del buffer de lexemas, `unread`, errores
léxicos y actualización de la tabla de símbolos.
