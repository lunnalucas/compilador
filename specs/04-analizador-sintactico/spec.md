# Spec — 04-analizador-sintactico

## 1- Objetivo
Diseñar e implementar el Analizador Sintáctico (Parser) para el lenguaje BETA utilizando **Yacc / Bison** e integrado con el Analizador Léxico previo (`yylex()`). 
Su función es verificar que la secuencia de tokens cumpla con las reglas gramaticales del lenguaje BETA y construir la representación intermedia basada en un **Árbol Sintáctico Abstracto (AST)** con nodos anotados de conversión de tipos (Entero/Real).

## 2- Alcance
* **Estructura Gramatical:** Reconocimiento de todas las reglas BNF del lenguaje BETA:
  * Declaraciones de variables (`entero`, `real`).
  * Estructuras de control (`si`, `sino`, `para`).
  * Asignaciones y expresiones aritmético-lógicas.
  * Definición e invocación de funciones (máximo 2 parámetros, sin llamadas recursivas).
  * Salida por pantalla (`imprimir`).
* **Representación Intermedia (AST):** Construcción dinámica de nodos del Árbol Sintáctico Abstracto en C durante el análisis.
* **Manejo de Conversiones:** Inserción de nodos de conversión implícita/explícita en el AST (`ENTERO_A_REAL` y `REAL_A_ENTERO`) según las reglas de compatibilidad de tipos (R2–R5).
* **Integración:** Interfaz directa entre `yyparse()` y el consumidor del lexer (`yylex()`).
* **Fuera del alcance:** La generación final de código Assembler y la ejecución en tiempo de ejecución (etapas posteriores).
  
## 3- Entradas / Salidas

## 4- Reglas y decisiones de diseño

## 5- Criterios de aceptación
