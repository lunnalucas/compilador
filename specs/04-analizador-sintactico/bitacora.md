# Bitácora — 04-analizador-sintactico

| Fecha | Quién | Qué se hizo / decidió | Notas |
|-------|-------|-----------------------|-------|
|       |       |                       |       |
### Iteración 1 — Generación del Analizador Sintáctico a partir de las Especificaciones
**Fecha:** 08/10/2026 · **Modelo:** Gemini

* **Qué pedí:** Generar el archivo `src/parser.y` en Bison teniendo en cuenta todo el panorama general del proyecto: la especificación general del lenguaje (`specs/01-lenguaje/spec.md`), la especificación del analizador sintáctico (`specs/04-analizador-sintactico/spec.md`) y la estructura de la tabla de símbolos, para garantizar que la gramática, tokens, precedencias y reglas se mantengan alineadas a lo definido sin desviaciones.
* **Qué se hizo:**
  * Se creó la estructura base de `src/parser.y` incluyendo las declaraciones de tokens (asociados a los códigos numéricos de `lexer.h`), las reglas gramaticales y la función `main.c` para evaluar el resultado (`resultado == 0` para éxito o error sintáctico).
  * Punto de entrada del sintáctico (`yyparse`): Se configuró la llamada a `yyparse()` en `src/main.c` como el motor principal del analizador sintáctico, coordinando el pedido de tokens al analizador léxico (`yylex()`).
  * Construcción del AST:Se integró la creación dinámica de nodos en la memoria mediante `crear_nodo` y `envolver_nodo` dentro de las acciones semánticas de las reglas en `parser.y`, logrando generar el Árbol Sintáctico Abstracto completo durante el análisis.
* **Decisiones tomadas:** Se probó el parser con un programa `.beta` sintácticamente correcto. La compilación y ejecución fueron exitosas devolviendo el mensaje `Compilacion Sintactica Exitosa`.
* **Impacto:** Se validó la integración inicial de la gramática en un escenario sin errores entre Flex, Bison y C, respetando los lineamientos globales del lenguaje.
* **Estado al cierre de la iteración:** El parser funciona correctamente para código válido, pero cuando probamos con uno con error solo dice `Compilacion con error Sintactico`y no dice cual es el error.
* Queda pendiente verificar el armado del arbol

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

### Iteración 3 — Corrección de Tipos de Datos y Estabilización de `yyerror()`
**Fecha:** 08/10/2026 · **Modelo:** Gemini

* **Qué pedí:** Identificar la causa raíz del `Segmentation fault` y solucionar la falla de memoria para que el reporte de errores sintácticos sea preciso y estable.
* **Qué se hizo:**
  * Se revisó la definición en `lexer.h` y se confirmó que `yy_lexema` está definido como un buffer estático de caracteres (`char yy_lexema[8192]`) y no como un puntero simple (`char *`).
  * En `src/parser.y`, se corrigió la declaración externa a `extern char yy_lexema[8192];`.
  * En `yyerror()`, se reemplazó la condición `yy_lexema != NULL` (que siempre era cierta al ser un arreglo estático) por la verificación del primer carácter `yy_lexema[0] != '\0'` para evitar lecturas de memoria inválida. Se agregó `fflush(stdout);`.
* **Decisiones tomadas:** Se consolidó el acceso seguro a las variables de ubicación en `parser.y`.
* **Impacto:** Se estabilizó la fase de análisis sintáctico. El compilador reporta correctamente los errores sin interrumpir la ejecución del programa.
* **Estado al cierre de la iteración:** 
  * Al probar `tests/prueba.beta` con la falta de `;` en la línea 12, el compilador emitió limpiamente:
    `[ERROR SINTÁCTICO] Línea 14, Columna 3: syntax error, unexpected PARA, expecting P_COMA (cerca de 'para')`
    seguido de `Error Sintactico en la compilacion` en la salida principal, finalizando exitosamente el ciclo de pruebas de la fase sintáctica.
  - **Pendiente de verificación:** Queda pendiente para la próxima iteración implementar un mecanismo de visualización o recorrido (impresión del AST) para verificar en consola que la estructura en memoria se construya correctamente antes de avanzar a las etapas de análisis semántico y código intermedio.
 
### Iteración 4 — Requerimiento de Detección Multierror y Lectura Completa hasta EOF
**Fecha:** 14/10/2026 · **Modelo:** Copilot

- **Qué pedí:** 
  @workspace Implementa un sistema completo de recolección de múltiples errores (léxicos y sintácticos) que procese el archivo `.beta` completo hasta el final (`EOF`) sin abortar en la primera falla. Pedí modificar el lexer para que no corte al encontrar un error léxico, definir la resincronización en la regla de sentencias en el parser usando una regla de `error` para reanudar la lectura, y ajustar `yyerror()` para registrar la línea exacta del fallo sin interrumpir la ejecución.

- **Qué se hizo:**
  - **Analizador Léxico (`src/lexer.l`):** Se ajustó el manejo de errores léxicos para registrar la falla y continuar entregando tokens a la gramática hasta llegar a `EOF`.
  - **Analizador Sintáctico (`src/parser.y`):** Se agregó la regla de recuperación por error a nivel de `sentencia` para que, tras detectar una falla, el parser continúe procesando las siguientes instrucciones.
  - **Manejador de Errores (`src/main.c` / `yyerror`):** Se adaptó la función `yyerror()` para almacenar múltiples errores en una lista global en lugar de detener el programa en la primera llamada.

- **Decisiones tomadas y fallas detectadas:**
  - **Avance alcanzado:** Al probar `tests/prueba.beta`, se confirmó que el lexer y el parser lograron recorrer el archivo de principio a fin, imprimiendo la tabla de tokens completa hasta la línea final (línea 23).
  - **Problema detectado:** El primer error sintáctico fue detectado con precisión (falta de `;` antes de la palabra `para`), pero la resincronización descartó el token `para` con el que se dio cuenta de la falla. Al eliminar ese token válido, el encabezado del bucle quedó incompleto, provocando que la mayor parte del código posterior se interpretara como erróneo y generando una ráfaga de falsos positivos en esa misma estructura.

- **Impacto:** Se logró el objetivo principal de procesar el archivo completo hasta `EOF`, pero la estrategia de recuperación destruye el token con el que detecta la falla, rompiendo las estructuras válidas que le siguen.

- **Estado al cierre de la iteración:**
  - El compilador no aborta y analiza todo el archivo, pero la resincronización actual corrompe la lectura inmediatamente posterior al primer error.
  - **Queda pendiente:** Ajustar la regla de recuperación para que conserve el token donde se detectó el error y permita reanudar el análisis desde esa misma palabra reservada o instrucción.
