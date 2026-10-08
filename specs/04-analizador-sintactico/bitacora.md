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
