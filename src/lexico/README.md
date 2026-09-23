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

`lexer.c` implementa `yylex()` con un recorrido por los mismos estados y
eventos definidos en las tablas. La implementación mantiene las decisiones
operativas en funciones pequeñas (`read_identifier`, `read_number`,
`read_string` y `skip_comment`) para que resulte fácil de seguir.

## Uso básico

```c
FILE *source = fopen("programa.beta", "r");
lexer_init(source);

while ((token = yylex()) != 0) {
    /* procesar token y yytext */
}
```

`yytext`, `yyleng` y `yylineno` contienen el lexema actual, su longitud y la
línea donde fue leído. El lexer informa los errores por `stderr` y devuelve
`0` al llegar a EOF.
