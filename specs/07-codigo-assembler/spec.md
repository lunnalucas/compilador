# Spec — 07-codigo-assembler

**Grupo:** B  
**Lenguaje de implementación:** C  
**Plataforma destino:** x86 (32 bits), NASM assembler, linkeado mediante GCC (MinGW) en Windows.  
**Dependencias:** 
* `specs/01-diseno/spec.md`
* `specs/02-tabla-simbolos/spec.md`
* `specs/03-analizador-lexico/spec.md`
* `specs/04-analizador-sintactico/spec.md`

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
  *   **Estructura de Polaca Inversa:** Arreglo lineal de celdas en memoria provisto por el módulo anterior. Cada celda contiene:
    * `tipo_de_celda`: Naturaleza del elemento (operador, operando, salto, etc.).
    * `opcode`: Operación a ejecutar (suma, asignación, `if`, etc.).
    * `referencia_ts`: Puntero o índice a la Tabla de Símbolos (si corresponde).
    * `tipo_de_dato`: Tipo de dato asociado al elemento.
    * `linea`: Número de línea en el código fuente original.

  *   **Tabla de Símbolos:** Estructura con las variables, constantes y cadenas literales definidas en el código fuente.

*   **Salidas:**
    *   **Archivo de Código Assembler (`.asm`):** Archivo de texto fuente con las instrucciones en lenguaje ensamblador (x86, 32 bits, formato NASM) listo para ser compilado y ejecutado.
    *   **Archivo de Trazabilidad / Log de Traducción:** Archivo generado durante la ejecución que registra el paso a paso del consumo de la polaca y su conversión a Assembler, el cual es volcado en pantalla al finalizar el proceso.

## 4. Reglas y decisiones de diseño

### 4.1. Plataforma Y Entorno

El código Assembler generado está diseñado para ejecutarse bajo los siguientes parámetros:
  * **Arquitectura:** x86 (32 bits).
  * **Sintaxis:** NASM.
  * **Entorno:** Windows, ensamblado y linkeado mediante GCC (MinGW).

### 4.2. Modelo de memoria estática y variables

El compilador de nuestro lenguaje Beta usara memoria estática, lo que implica que no se utiliza memoria dinámica ni marcos de pila (*stack frames*) dinámicos para la gestión de las variables. 

* **Asignación en la Sección de Datos (`.data`):** Todas las variables y constantes definidas en el código fuente (y registradas en la Tabla de Símbolos) se mapean a posiciones de memoria fijas y preasignadas en la sección de datos del archivo Assembler.
* **Manejo de Tipos (32 bits):** Dado que la arquitectura es de 32 bits, los espacios de almacenamiento para las variables numéricas se definen utilizando directivas de doble palabra (por ejemplo, `dd` en NASM), asegurando el alineamiento y tamaño de almacenamiento adecuado.
* **Constantes y Cadenas Literales:** Los literales de texto utilizados en operaciones como `imprimir` se almacenan estáticamente como cadenas terminadas en byte nulo (`0`), permitiendo su correcta compatibilidad y paso de argumentos hacia la función externa `printf` de C a través de MinGW.

### 4.3. Traducción de operadores y asignaciones

El proceso de traducción del código intermedio (Polaca Inversa) a Assembler lo realizaremos mediante un recorrido secuencial del arreglo de celdas de izquierda a derecha, apoyándonos en la pila del sistema y en los registros de la CPU para evaluar las expresiones:

* **Evaluación mediante Pila y Registros:** El algoritmo procesa cada celda según su tipo:
  * **Operandos (Variables o Constantes):** Al encontrarse con un operando, su valor o dirección se carga y se apila (`PUSH`) utilizando los registros del procesador (por ejemplo, `EAX`), emulando la lógica de evaluación de la Polaca.
  * **Operadores Aritméticos (`+`, `-`, `*`, `/`):** Al detectar un operador, se desapilan (`POP`) los operandos correspondientes de la pila hacia los registros de trabajo de la CPU (como `EAX` y `EBX`), se ejecuta la instrucción de Assembler respectiva (`ADD`, `SUB`, `IMUL`, etc.), y el resultado obtenido se vuelve a apilar (`PUSH eax`) para formar parte de expresiones más complejas si fuera necesario.
* **Resolución de Asignaciones (`:=`):** Cuando la celda indica una asignación, el valor resultante de la expresión (que se encuentra en la cima de la pila o en el registro acumulador) se extrae y se almacena directamente en la posición de memoria estática asignada a la variable de destino (`MOV [variable], eax`), completando la instrucción.

* **Ejemplo práctico de recorrido de la Polaca (para la expresión `a = b + 5`):**
  - **Vector de celdas recibido (Polaca Inversa):** 
    `[ b ] | [ 5 ] | [ + ] | [ a ] | [ := ]`

    1. Operando: `b` (Carga la variable desde la TS)
    2. Operando: `5` (Carga la constante desde la TS)
    3. Operador: `+` (Suma los dos valores de la pila)
    4. Operando: `a` (Variable de destino)
    5. Operador: `:=` (Asignación)

  - **Traducción generada en Assembler:**
    ```nasm
    MOV eax, [b]        ; Carga b en eax
    PUSH eax            ; Apila b
    MOV eax, 5          ; Carga la constante 5
    PUSH eax            ; Apila 5
    POP ebx             ; Saca el segundo operando (5) a ebx
    POP eax             ; Saca el primer operando (b) a eax
    ADD eax, ebx        ; Suma: eax = b + 5
    PUSH eax            ; Apila el resultado temporal
    POP eax             ; Recupera el resultado final
    MOV [_a], eax       ; Guarda el resultado en la variable estática a
    ```
    
### 4.4. Manejo de saltos y estructuras de control (`si` y `para`)

El lenguaje ensamblador x86 se ejecuta de manera secuencial, asi que el compilador traduce las estructuras de control (`si` y `para`) utilizando comparaciones (`CMP`) y saltos condicionales o incondicionales (`JMP`, `JZ`, `JNZ`, `JL`, `JG`, etc.) apoyándose en etiquetas virtuales únicas generadas durante la síntesis.

* **Estructura condicional (`si` / `sino`):**
  * Se evalúa la condición lógica generando una comparación en la CPU.
  * Si el resultado de la condición es falso, un operador de salto por falso (`BF`) desvía el flujo de ejecución hacia el bloque del `sino` (o directamente al final de la estructura si no existe el bloque alternativo).
  * Al finalizar el bloque verdadero, un salto incondicional (`JMP`) evita que se ejecute el código del `sino`.

* **Ejemplo práctico (`si (a > b) entonces a = 1`):**
    - **Vector de celdas recibido (Polaca Inversa):** 
      `[ a ] | [ b ] | [ > ] | [ BF, .etq_fin ] | [ a ] | [ 1 ] | [ := ] | [ ETIQUETA, .etq_fin ]`
    - **Traducción generada en Assembler:**
      ```nasm
      MOV eax, [a]        ; Carga a
      MOV ebx, [b]        ; Carga b
      CMP eax, ebx        ; Compara a con b
      JLE .etq_fin        ; Si a NO es mayor que b, salta al final (BF)
      
      ; --- Cuerpo del si ---
      MOV eax, 1
      MOV [_a], eax       ; a = 1
      
      .etq_fin:           ; Etiqueta de salida
      ```

* **Estructura iterativa decremental (`para`):**
  * **Lógica específica del lenguaje Beta:** El bucle `para` es estrictamente **decremental**. Comienza en un valor superior y decrementa en cada ciclo hasta alcanzar o superar el límite inferior establecido.
  * **Traducción:** 
    1. Se inicializa la variable de control.
    2. Se establece una etiqueta al inicio de la evaluación del ciclo.
    3. Se evalúa la condición de corte (verificando si el contador descendió por debajo del límite). Si la condición deja de cumplirse, un salto por falso (`BF`) finaliza el bucle.
    4. Se ejecutan las instrucciones contenidas en el cuerpo del bucle.
    5. Se aplica la operación de decremento sobre la variable de control.
    6. Un salto incondicional (`JMP`) retorna el flujo a la etiqueta de inicio para la siguiente iteración.

* **Ejemplo práctico (Bucle decremental `para i = 10 hasta 1 hacer ...`):**
    - **Vector de celdas recibido (Polaca Inversa):**
[ i ] [ 10 ] [ := ] [ ETIQ_INI ] [ i ] [ 1 ] [ >= ] [ BF, ETIQ_FIN ] [ CÓDIGO_DEL_CUERPO ] [ i ] [ i ] [ 1 ] [ - ] [ := ] [ BI, ETIQ_INI ] [ ETIQ_FIN ]
   
    - **Traducción generada en Assembler:**
      ```nasm
      MOV [_i], 10        ; Inicialización de la variable de control
      
      .inicio_para:       ; Etiqueta de inicio del loop
      MOV eax, [_i]
      CMP eax, 1          ; Compara i con el límite inferior (1)
      JL .fin_para        ; Si i < 1, sale del bucle (BF)
      
      ; --- Cuerpo del bucle ---
      
      ; --- Decremento ---
      DEC dword [_i]      ; Resta 1 a i
      
      JMP .inicio_para    ; Salta al inicio para reevaluar
      
      .fin_para:          ; Fin del bucle
      ```

## 5. Plantilla base del código Assembler generado

## 6. Criterios de aceptación
Para considerar que el módulo de Generación de Código Assembler es correcto y funcional, se deben cumplir los siguientes criterios de validación:

*   **1. Consumo íntegro de la Polaca Inversa:** El módulo debe procesar la totalidad de los elementos provistos en el arreglo en memoria de la polaca, sin omitir instrucciones ni desfasar la pila o los registros.

*   **2. Generación sintáctica correcta del `.asm`:** El archivo de salida generado debe cumplir estrictamente con la sintaxis de NASM para x86 (32 bits), permitiendo su ensamblaje sin errores ni advertencias fatales mediante herramientas estándar (NASM/GCC en MinGW).
  
*   **3. Registro de traza (Log):** El módulo debe generar correctamente el archivo de traza que documenta el paso a paso de la traducción y mostrarlo de forma legible en la consola al finalizar la ejecución exitosa del proceso.
