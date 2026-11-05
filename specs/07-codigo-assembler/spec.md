# Spec — 07-codigo-assembler

**Grupo:** B  
**Lenguaje de implementación:** C  
**Plataforma destino:** x86 (32 bits), NASM assembler, linkeado mediante GCC (MinGW) en Windows.  
**Dependencias:** 
* `specs/01-diseno/spec.md`
* `specs/02-tabla-simbolos/spec.md`
* `specs/03-analizador-lexico/spec.md`
* `specs/04-analizador-sintactico/spec.md`

---

## 1. Objetivo

El objetivo de este módulo es recibir el código intermedio (representado como un arreglo lineal de estructuras de Polaca Inversa) en conjunto con la Tabla de Símbolos, y traducirlo de manera estricta y sistemática a código fuente en lenguaje Assembler x86 (arquitectura de 32 bits, formato NASM). Este código generado debe ser completamente funcional, compatible con las restricciones estáticas del lenguaje Beta, y apto para ser ensamblado y linkeado mediante GCC en entornos Windows para su ejecución final.

## 2. Alcance

Este módulo abarca estrictamente la etapa final de generación de código del compilador. Sus límites operacionales son:

*   **Lo que SÍ incluye (In-scope):**
    *   La lectura y el procesamiento secuencial del código intermedio (Polaca Inversa) y la Tabla de Símbolos provistos por las fases anteriores.
    *   La generación de un archivo de código fuente en Assembler compatible con NASM (x86, arquitectura de 32 bits).
    *   La traducción de operaciones aritméticas, asignaciones, estructuras de control de flujo (si y para) y la sentencia de salida `imprimir`.
    *   El mapeo de variables y parámetros bajo el modelo de memoria estática.

*   **Lo que NO incluye (Out-of-scope):**
    *   Validaciones léxicas, sintácticas o semánticas (tareas exclusivas de los analizadores previos).
    *   Generación de código para arquitecturas de 64 bits u otros sistemas operativos que no sean Windows con GCC/MinGW.
    *   Soporte para memoria dinámica, asignación en *heap*, marcos de pila (*stack frames*) variables o funciones recursivas.
      
## 3. Entradas / Salidas

*   **Entradas:**
    *   **Estructura de Polaca Inversa:** Arreglo en memoria provisto por el módulo anterior que contiene los elementos a traducir.
    *   **Tabla de Símbolos:** Estructura con las variables, constantes y cadenas literales definidas en el código fuente.

*   **Salidas:**
    *   **Archivo de Código Assembler (`.asm`):** Archivo de texto fuente con las instrucciones en lenguaje ensamblador (x86, 32 bits, formato NASM) listo para ser compilado y ejecutado.
    *   **Archivo de Trazabilidad / Log de Traducción:** Archivo generado durante la ejecución que registra el paso a paso del consumo de la polaca y su conversión a Assembler, el cual es volcado en pantalla al finalizar el proceso.

## 4. Reglas y decisiones de diseño

* 4.1. Plataforma y Entorno
  
El código Assembler generado está diseñado para ejecutarse bajo los siguientes parámetros:
   *   **Arquitectura:** x86 (32 bits).
   *   **Sintaxis:** NASM.
   *   **Entorno:** Windows, ensamblado y linkeado mediante GCC (MinGW).

* 4.2. Modelo de memoria estática y variables



## 5. Plantilla base del código Assembler generado

## 6. Criterios de aceptación
Para considerar que el módulo de Generación de Código Assembler es correcto y funcional, se deben cumplir los siguientes criterios de validación:

*   **1. Consumo íntegro de la Polaca Inversa:** El módulo debe procesar la totalidad de los elementos provistos en el arreglo en memoria de la polaca, sin omitir instrucciones ni desfasar la pila o los registros.

*   **2. Generación sintáctica correcta del `.asm`:** El archivo de salida generado debe cumplir estrictamente con la sintaxis de NASM para x86 (32 bits), permitiendo su ensamblaje sin errores ni advertencias fatales mediante herramientas estándar (NASM/GCC en MinGW).
  
*   **3. Registro de traza (Log):** El módulo debe generar correctamente el archivo de traza que documenta el paso a paso de la traducción y mostrarlo de forma legible en la consola al finalizar la ejecución exitosa del proceso.
