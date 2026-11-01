# Spec — 04-analizador-sintactico

**Grupo:** B · **Lenguaje de implementación:** C
**Depende de:** [`specs/01-diseno/spec.md`](../01-diseno/spec.md),
[`specs/02-tabla-simbolos/spec.md`](../02-tabla-simbolos/spec.md),
[`specs/03-analizador-lexico/spec.md`](../03-analizador-lexico/spec.md)
**Consume:** tokens producidos por el analizador lexico - `yylex()`.
**Produce:** verificación gramatical.
**Estado:** EN CURSO.

---

## 1. Objetivo

Diseñar e implementar el Analizador Sintáctico para el lenguaje BETA
utilizando **Yacc / Bison** e integrado con el Analizador Léxico previo
(`yylex()`).
Su función es verificar que la secuencia de tokens cumpla con las reglas
gramaticales del lenguaje BETA.

## 2. Alcance
* **Estructura Gramatical:** Reconocimiento de todas las reglas BNF del lenguaje BETA:
  * Declaraciones de variables (`entero`, `real`).
  * Estructuras de control (`si`, `sino`, `para`).
  * Asignaciones y expresiones aritmético-lógicas.
  * Definición e invocación de funciones (máximo 2 parámetros, sin llamadas recursivas).
  * Salida por pantalla (`imprimir`).
* **Integración Léxico-Sintáctica:** Interfaz directa y explícita entre `yyparse()` (Bison) y el analizador léxico desarrollado previamente (`yylex()`), el cual le suministra de forma iterativa los códigos de tokens (100–132) y sus atributos mediante la variable global de comunicación.
* - **Manejo de Errores y Recuperación (Panic Mode):** El analizador sintáctico no debe abortar en el primer error. Ante un fallo, registrará el tipo de error específico, la línea exacta y aplicará recuperación por pánico (`Panic Mode`) sincronizando en delimitadores seguros (`;` o `}`) para continuar procesando todo el archivo.
- **Trazabilidad del Proceso de Parsing:** Visualización y registro detallado de las operaciones del autómata LR (*shift* y *reduce*) mediante la directiva de depuración de Bison, volcando la salida a consola y al archivo de log correspondiente.
* **Fuera del alcance:** Codigo intermedio - Código Assembler.

## 3. Entrada / Salida

| Elemento | Contrato |
|---|---|
| Entrada | Secuencia de `int` devueltos por `yylex()`: 100-132 según `01-diseno/spec.md §4`. |
| Salida principal | `0` si el programa es gramaticalmente válido, `!= 0` si hay error sintáctico. |
**Archivos de Salida / Reportes** | - `parser.tab.c` / `parser.tab.h`: Código fuente y cabeceras generadas por Bison.<br>- `parser_debug.log`: Archivo de registro con la traza completa de las acciones *shift/reduce* del parser.<br>- Reporte en consola: Listado detallado de errores encontrados con su línea y descripción, finalizando con el veredicto (`"compilación completa con errores"` o `"compilación completa sin errores"`).

Flujo del Main (Pseudocódigo de ejecución)
```c
int main(int argc, char *argv[]) {
    // Inicialización de archivos y depuración
    yydebug = 1; // Habilita traza shift/reduce
    FILE *archivo = abrir_archivo(argv[1]);
    
    // Invocación del parser
    int resultado = yyparse();
    
    // Verificación de acumulación de errores y reporte final
    if (tiene_errores_sintacticos()) {
        printf("compilacion completa con errores\n");
        imprimir_reporte_errores_acumulados();
    } else {
        printf("compilacion completa sin errores\n");
    }
    
    cerrar_archivo(archivo);
    return resultado;
}

```

Algunos resultados esperados:

1. El parser nunca pide un token que el lexer no define (100–132, EOF).
2. Blancos y comentarios nunca llegan al parser (los consume el lexer).

## 4. Reglas y decisiones de diseño

| # | Decisión | Valor / justificación |
|---|---|---|
| S1 | Gramática | Copia literal de `01-diseno/spec.md §6`, transcripta a sintaxis Bison (§6). Sin agregar ni quitar producciones. |
| S2 | Gramatica | Recursion y asociatividad a la izquierda |
| S3 | Funciones | 1 o 2 parametros. 0 parametros o 3+ parametros es E10/E9. `inicio` es `<principal> ::= INICIO bloque_i`, sin parametros ni retorno; solo puede aparecer una vez (§6.3). |
| S4 | Manejo de Errores Específico y Recuperación| Se implementarán reglas de recuperación explícitas basadas en puntos de sincronización (`;` y `}`). Ante una sentencia inválida, el parser invocará la rutina de error específica, reportando el fallo sin abortar, descartando tokens hasta encontrar el delimitador y reanudando el análisis. |
| S5 | Estructura de Acumulación de Errores| Se utilizará una estructura de datos modular (lista/tabla de registros) que almacene dinámicamente por cada incidencia: código de error, número de línea exacto y descripción asociada, permitiendo un volcado consolidado al finalizar el archivo. |
| S6 | Trazabilidad (Debug)| Se habilitará `%define parse.trace` en Bison y se configurará la variable global `yydebug = 1` para auditar las transiciones *shift/reduce* enviándolas a `parser_debug.log`. |
| S7 | Interfaz con el Analizador Léxico| El parser invocará de forma estándar la función `yylex()` provista por el módulo léxico previo (`lexico.c`). La comunicación de tokens se realizará mediante el valor de retorno de `yylex()` y el uso de variables globales de soporte (como `yylval`), garantizando que no se utilicen generadores por defecto sino la implementación propia de la parte lexica de nuestro trabajo|

## 5. Tokens
Tomaremos los tokens ya definidos de  `01-diseno/spec.md §6`

## 6. Gramática 
Tomaremos la gramatica de  `01-diseno/spec.md §6`

### 6.1 Restricciones sintacticas

1. `principal` (`inicio`) aparece exactamente una vez y al final del archivo.
2. No hay funciones anidadas: `unidad` solo aparece en `unidades` a nivel
   programa.
3. `retorno` solo aparece en `sentencia` (funciones), nunca en `sentencia_i`
   (`inicio` no retorna, no tiene tipo).
4. `inicio` no lleva parámetros ni tipo de retorno.

## 7. Errores de esta fase

Solo E5, E9, E10 en esta etapa. Los léxicos (E1–E4, E12, E18–E20) ya
fueron emitidos antes.

| Código | Condición sintáctica | Mensaje |
|---|---|---|
| E5 | cualquier secuencia que no reduce según §6. Ejemplo: `entero = 5;`, `si x {}`, `para (i=0;i<5;) {}`, `imprimir();`, `retornar;` | `Error E5 en ...` |
| E9 | definición o llamada con más de 2 parámetros/argumentos | `Error E9: la funcion 'f' supera el maximo de 2 parametros/argumentos` |
| E10 | definición de función sin parámetros (`f()`) | `Error E10: la funcion 'f' debe tener 1 o 2 parametros` |

### 7.1 Casos de Prueba Requeridos:
Estos programas de prueba deben ser creados teniendo en cuenta las especificaciones de nuestro lenguaje BETA 
provistos en `01-diseno/spec.md §6`
- **Programas Correctos:**
  1. `pruebaCorrecto01.beta`: Valida expresiones, asignaciones, estructuras `si/sino` y bucles `para`.
  2. `pruebaCorrecto02.beta`: Valida definición de funciones con 1 y 2 parámetros e invocaciones correctas.
  3. `pruebaCorrecto03.beta`: Programa integral combinando funciones, estructuras de control anidadas y bloque `inicio`.
- **Programas Incorrectos (Validación de Panic Mode y Acumulación):**
  1. `pruebaIncorrecto01.beta`: Contiene múltiples errores E5 (sentencias malformadas) dispersas para verificar la recuperación en `;` y `}` sin abortar.
  2. `pruebaIncorrecto02.beta`: Contiene errores de cantidad de parámetros en funciones (E9 y E10).
  3. `pruebaIncorrecto03.beta`: Mezcla de errores sintácticos variados (E5, E9, E10) para comprobar el reporte consolidado al final.

## 8. Criterios de Aceptación
El Analizador Sintáctico se considerará aceptado cuando:
1. El archivo `src/parser.y` compile limpiamente con Bison (`bison -d src/parser.y -o src/parser.tab.c`) generando las cabeceras requeridas sin conflictos.
2. Reconozca la totalidad de las reglas gramaticales BNF (§6) consumiendo iterativamente los tokens `100–132` desde `yylex()`.
3. Aplique con éxito el manejo de errores específico y la recuperación por pánico (`Panic Mode`), acumulando y listando los errores al finalizar.
4. Genere la traza del proceso de parsing (`shift/reduce`) en consola y en el archivo `parser_debug.log`.
