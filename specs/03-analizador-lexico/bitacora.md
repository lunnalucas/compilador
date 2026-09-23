# Bitácora — Analizador léxico de beta

**Grupo:** B · **Lenguaje de implementación:** C
**Spec de referencia:** `specs/03-analizador-lexico/spec.md`
**Modelos usados:** GPT-5.6, 3.8 Flash

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

El analizador léxico ya puede leer 
un `FILE *` y producir tokens de `beta` uno
por uno.

---

## Iteración 3 — Creación de Suite de Prueba y verificación del lexer
**Fecha:** 23/09/2026 · 3.8 Flash

**Qué pedí:** Basándote en la especificación de specs/03-analizador-lexico/spec.md, genérame un archivo de prueba en C llamado tests/test_lexer.c que abra un archivo prueba.uno y ejecute la función yylex() hasta llegar al final, imprimiendo cada token detectado con su número y texto.
Qué se hizo:

**Qué se hizo:** 
- Se creó `tests/test_lexer.c` para inicializar el analizador léxico sobre `prueba.uno`, llamar a `yylex()` iterativamente e imprimir la línea, código de token y lexema en consola.
  
- Se creó el archivo de entrada `prueba.uno` para alimentar las pruebas del lexer.

-Se ejecutó la primera prueba con el siguiente código de control en `prueba.uno`:
  ```text
  int x = 10;
  if (x >= 5) {
      print "Hola";
  }
```

**Decisiones tomadas:**

- Se aisló la prueba en un ejecutable dedicado (test_lexer) para inspeccionar la salida en consola antes de proceder con la integración sintáctica.

  **Resultados y anomalías detectadas:**

- Duplicación de tokens: Al procesar print "Hola", la función yylex() devolvió la secuencia repetida (print, Hola, print, Hola).

- Incumplimiento de arquitectura: Se confirmó que lexer.c utiliza un bloque switch/case manual para reconocer los tokens en lugar de basarse en la matriz de transiciones de lexer_tables.c.

**Impacto en la spec:** Se evidenció la necesidad de refactorizar el motor léxico para alinearlo a la especificación del autómata basado en tablas y corregir la duplicación en cadenas.

---

## Estado al cierre de la iteración

La prueba anterior estaba operativa, pero usaba `prueba.uno` y palabras de
otro lenguaje. Se reemplazó por una prueba que recibe cualquier archivo por
la línea de comandos.

---

## Iteración 4 — Prueba de programas beta
**Fecha:** 23/09/2026 · GPT-5.6

**Qué pedí:** Se debe poder probar programas pertenecientes a mi lenguaje mediante archivos `.beta` al analizador léxico y ver todos los tokens que devuelve.

**Qué se hizo:**

- Se actualizó `tests/test_lexer.c` para recibir la ruta del archivo como
  argumento, en lugar de abrir siempre `prueba.uno`.
- Se agregaron los nombres de los tokens junto con su código, línea y lexema.
- Se agregó `tests/correctos/ejemplo.beta` con palabras y estructuras del
  lenguaje beta.
- Se creó `specs/03-analizador-lexico/README.md` con los comandos para
  compilar y ejecutar la prueba.
- Se eliminaron `prueba.uno` y el ejecutable viejo porque correspondían al
  ejemplo de otro lenguaje. El ejecutable se genera nuevamente al compilar.

**Decisiones tomadas:**

- La herramienta recibe un solo archivo por ejecución para poder probar
  distintos programas sin cambiar el código.
- Se mantuvo `yylex()` y la implementación del lexer sin cambios: esta tarea
  agrega una forma de observar sus resultados, no nuevas reglas del lenguaje.

**Verificación:**

Se compiló la herramienta con GCC usando `-Wall -Wextra -std=c11` y se ejecutó
sobre `tests/correctos/ejemplo.beta`. El resultado mostró los tokens de beta
sin duplicaciones.

**Impacto en la spec:** No se modificaron las reglas del analizador léxico.
Se agregó una herramienta de prueba y su documentación.

---

## Estado al cierre de la iteración

El analizador léxico se puede probar con cualquier programa beta desde la
consola, la salida identifica cada token de forma clara pero no utiliza los nombres de los tokens definidos en las especificaciones del lenguaje.
