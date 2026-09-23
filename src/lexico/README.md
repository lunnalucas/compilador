# Tablas del analizador léxico

`lexer_tables.h` publica las tablas del autómata finito definido en
`specs/03-analizador-lexico/spec.md`:

- `nuevo_estado[estado][evento]`: siguiente estado.
- `proceso[estado][evento]`: siguiente estado y acción semántica.
- `token[estado][evento]`: token resultante al cerrar el lexema.
- `reserved_words`: palabras reservadas, resueltas como identificadores.

Los estados `-1` y `-2` representan, respectivamente, finalización y error.
Las matrices tienen una columna adicional reservada para mantener el mismo
índice de evento que `proceso`; los espacios, tabuladores y saltos de línea se
descartan desde el autómata.

Este módulo solo contiene datos y la clasificación de caracteres; `yylex()`
se implementará sobre estas tablas en la siguiente etapa.
