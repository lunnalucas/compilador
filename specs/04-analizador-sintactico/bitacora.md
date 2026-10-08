# Bitácora — 04-analizador-sintactico

| Fecha | Quién | Qué se hizo / decidió | Notas |
|-------|-------|-----------------------|-------|
|       |       |                       |       |
### Iteración 1 — Generación del Analizador Sintáctico a partir de las Especificaciones
**Fecha:** 08/10/2026 · **Modelo:** Gemini

* **Qué pedí:** Generar el archivo `src/parser.y` en Bison teniendo en cuenta todo el panorama general del proyecto: la especificación general del lenguaje (`specs/01-lenguaje/spec.md`), la especificación del analizador sintáctico (`specs/04-analizador-sintactico/spec.md`) y la estructura de la tabla de símbolos, para garantizar que la gramática, tokens, precedencias y reglas se mantengan alineadas a lo definido sin desviaciones.
* **Qué se hizo:**
  * Se creó la estructura base de `src/parser.y` incluyendo las declaraciones de tokens (asociados a los códigos numéricos de `lexer.h`), las reglas gramaticales y la función `main.c` para evaluar el resultado (`resultado == 0` para éxito o error sintáctico).
  * Se vinculó el parser generado (`parser.tab.c`) con el lexer (`lexer.c`) y la función principal.
* **Decisiones tomadas:** Se probó el parser con un programa `.beta` sintácticamente correcto. La compilación y ejecución fueron exitosas devolviendo el mensaje `Compilacion Sintactica Exitosa`.
* **Impacto:** Se validó la integración inicial de la gramática en un escenario sin errores entre Flex, Bison y C, respetando los lineamientos globales del lenguaje.
* **Estado al cierre de la iteración:** El parser funciona correctamente para código válido, pero cuando probamos con uno con error solo dice `Compilacion con error Sintactico`y no dice cual es el error.

### Iteración 2 — Requerimiento de Detalle de Errores y Aparición de Segmentation Fault
**Fecha:** 08/10/2026 · **Modelo:** Gemini

* **Qué pedí:** Modificar la función `yyerror()` para que al detectar un fallo sintáctico en un archivo `.beta` con errores (por ejemplo, falta de `;`), muestre por pantalla el mensaje detallado con la línea, columna y el lexema específico donde falló, en lugar de mostrar solo un mensaje genérico.
* **Qué se hizo:**
  * Se modificó `yyerror()` en `src/parser.y` para incluir las variables globales de ubicación (`yy_linea`, `yy_col_ini`, `yy_lexema`) mediante declaraciones `extern`.
  * Se probó el ejecutable con `tests/prueba.beta` con un error sintáctico inducido (falta de `;` en la línea 12).
* **Decisiones tomadas y fallas detectadas:** 
  * Al ejecutar el test con error, el programa no llegó a imprimir la ubicación ni el mensaje final del `main.c`, colapsando inmediatamente con un **`Segmentation fault`**.
* **Impacto:** Se evidenció una incompatibilidad en la gestión de memoria entre los tipos de datos declarados en la interfaz de `parser.y` y las variables globales del analizador léxico.
* **Estado al cierre de la iteración:** El reporte detallado falló debido a una violación de acceso a memoria cuando Bison intentaba invocar `yyerror()`.
