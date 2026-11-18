# Spec — 06-codigo-intermedio

**Grupo:** B · **Lenguaje de implementación:** C
**Depende de:**
[`specs/01-diseno/spec.md`](../01-diseno/spec.md),
[`specs/02-tabla-simbolos/spec.md`](../02-tabla-simbolos/spec.md),
[`specs/03-analizador-lexico/spec.md`](../03-analizador-lexico/spec.md) y
[`specs/04-analizador-sintactico/spec.md`](../04-analizador-sintactico/spec.md)
**Consume:** programa válido.
**Produce:** codigo intermedio en notacion polaca inversa.
**Estado:** A IMPLEMENTAR.

---

## 1. Objetivo

Se debe generar el codigo intermedio mediante la polaca inversa para hacer
el pasaje a codigo Assembler.

Esta etapa completa las acciones semanticas de [`/src/parser.y`](/src/parser.y) 
que construyen el código intermedio.

En esta especificacion decimos como generar el codigo intermedio, no se debe
hacer algo que no este escrito aca.

---

## 2. Alcance

### 2.1. Lo que incluye

1. Generación de la polaca inversa
2. Resultado de la polaca del programa en `out/polaca.txt`.

### 2.2. Lo que no incluye

- Detección o reporte de errores.
- Generación de assembler.
- Tercetos, AST o cualquier representación distinta de la polaca.

---

## 3. Entradas y Salidas

| Entrada/Salida | Descripcion |
|---|---|
| `Entrada` | Se recibe un progama beta sin errores lexicos. |
| `Salida` | Con el analisis sintactico sin errores se genera el archivo `polaca.txt` con la notacion polaca inversa completa en orden de todo el programa. |

## 4. Reglas para generar la Polaca

### Reglas Generales de Construcción
* **Estructura de la Polaca:** Se almacena en una lista o vector secuencial de pasos indexados por posición (`nro_paso`, iniciando en `0`). Cada celda de la polaca guarda tanto el elemento (operando, operador o instrucción) como su **tipo de dato asociado** (`entero`, `real` o vacío/sin tipo para saltos y etiquetas).
* **Pila de Saltos:** Se utiliza una pila auxiliar de direcciones para almacenar las posiciones pendientes de salto (`BF` y `BI`) y rellenarlas posteriormente.
* **Orden de Escritura:** Todo operando se escribe primero de forma secuencial y los operadores o instrucciones se escriben después.
* **Regla de Paso:** Toda producción de la gramática que no figure en esta lista (como `<expresion> ::= <termino>`, `<termino> ::= <factor>`, `<f_condicional> ::= <condicion>`, `<bloque>`, etc.) no escribe nada en la polaca y simplemente hereda sin cambios lo generado y el tipo de su componente hijo.

### Programa y Funciones
* **`<principal> ::= INICIO <bloque_i>`**
  * **Acción:** Escribe en la polaca `INICIO_PROG` (sin tipo), genera la polaca de `<bloque_i>`, y escribe en la polaca `FIN_PROG` (sin tipo).
* **`<unidad> ::= <tipo> ID '(' <parametros> ')' <bloque>`**
  * **Acción:** Escribe en la polaca `FUNC` junto con el `ID` de la función (con su tipo de retorno), procesa `<parametros>`, genera la polaca de `<bloque>`, y escribe en la polaca `RET` (sin tipo).
* **`<parametros> ::= <tipo> ID`**
  * **Acción:** Escribe en la polaca el `ID` (con su `<tipo>`) y luego escribe en la polaca `PARAM` (con su `<tipo>`).
* **`<parametros> ::= <tipo> ID ',' <tipo> ID`**
  * **Acción:** Escribe en la polaca el primer `ID` (con su `<tipo>`) seguido de `PARAM` (con su `<tipo>`), y luego el segundo `ID` (con su `<tipo>`) seguido de `PARAM` (con su `<tipo>`).

### Declaraciones y Asignaciones
* **`<declaracion> ::= <tipo> ID '=' <expresion> ';'`**
  * **Acción:** Escribe en la polaca el `ID` (con su `<tipo>`), genera la polaca de `<expresion>`, aplica conversión de tipo si difieren (truncamiento descartando la parte decimal si el `ID` es `entero` y la expresión es `real`; promoción exacta a decimal si el `ID` es `real` y la expresión es `entero`), y escribe en la polaca `=` (con el `<tipo>` del `ID`).
* **`<declaracion> ::= <tipo> ID ';'`**
  * **Acción:** Escribe en la polaca el `ID` (con su `<tipo>`), escribe en la polaca su valor por defecto (`0` con tipo `entero` o `0.0` con tipo `real`), y escribe en la polaca `=` (con el `<tipo>` del `ID`).
* **`<declaracion> ::= <tipo> <ids> ';'`**
  * **Acción:** Por cada `ID` acumulado en `<ids>`, escribe en la polaca el `ID` (con su `<tipo>`), escribe en la polaca su valor por defecto (`0` con tipo `entero` o `0.0` con tipo `real`), y escribe en la polaca `=` (con el `<tipo>` del `ID`).
* **`<asignacion> ::= ID '=' <expresion> ';'`**
  * **Acción:** Escribe en la polaca el `ID` de destino (con su tipo), genera la polaca de `<expresion>`, aplica conversión de tipo si difieren (truncamiento si el destino es `entero` y la expresión `real`; promoción exacta a decimal si el destino es `real` y la expresión `entero`), y escribe en la polaca `=` (con el tipo del `ID`).
* **`<asignacion> ::= ID '=' <invocacion> ';'`**
  * **Acción:** Escribe en la polaca el `ID` de destino (con su tipo), genera la polaca de `<invocacion>`, aplica conversión de tipo si el tipo de retorno difiere del tipo del `ID`, y escribe en la polaca `=` (con el tipo del `ID`).

### Invocación de Funciones
* **`<invocacion> ::= ID '(' <argumentos> ')'`**
  * **Acción:** Genera la polaca de `<argumentos>` (de izquierda a derecha), escribe en la polaca el `ID` de la función (con su tipo de retorno), y escribe en la polaca `CALL` indicando la cantidad de argumentos (1 o 2) y el tipo de retorno de la función.

### Expresiones Aritméticas y Operandos
*(Regla de tipos: En cualquier operación aritmética binaria entre un `entero` y un `real`, el operando `entero` se promueve a `real` antes de operar y el resultado de la operación es de tipo `real`).*
* **`<expresion> ::= <expresion> '+' <termino>`**
  * **Acción:** Polaca de `<expresion>`, polaca de `<termino>`, y escribe en la polaca `+` acompañado del tipo resultante (`entero` o `real`).
* **`<expresion> ::= <expresion> '-' <termino>`**
  * **Acción:** Polaca de `<expresion>`, polaca de `<termino>`, y escribe en la polaca `-` acompañado del tipo resultante (`entero` o `real`).
* **`<termino> ::= <termino> '*' <factor>`**
  * **Acción:** Polaca de `<termino>`, polaca de `<factor>`, y escribe en la polaca `*` acompañado del tipo resultante (`entero` o `real`).
* **`<termino> ::= <termino> '/' <factor>`**
  * **Acción:** Polaca de `<termino>`, polaca de `<factor>`, y escribe en la polaca `/` acompañado del tipo resultante (`entero` o `real`).
* **`<factor> ::= ID | CTE_E | CTE_R`**
  * **Acción:** Escribe directamente en la polaca el lexema o valor del `ID` (con su tipo registrado), `CTE_E` (con tipo `entero`) o `CTE_R` (con tipo `real`).

### Condicionales, Comparaciones y Lógica
* **`<condicion> ::= <expresion> <comparador> <expresion>`**
  * **Acción:** Polaca de la primera `<expresion>`, polaca de la segunda `<expresion>` (si un operando es `entero` y el otro es `real`, el `entero` se promueve a `real`), y escribe en la polaca el operador relacional de `<comparador>` (`==`, `!=`, `<`, `>`, `<=`, `>=`) sin tipo.
* **`<t_condicional> ::= <t_condicional> AND <f_condicional>`**
  * **Acción:** Polaca de `<t_condicional>`, polaca de `<f_condicional>`, y escribe en la polaca `&` (sin tipo).
* **`<condicional> ::= <condicional> OR <t_condicional>`**
  * **Acción:** Polaca de `<condicional>`, polaca de `<t_condicional>`, y escribe en la polaca `|` (sin tipo).

### Estructura de Selección (`SI` / `SINO`)
* **`<seleccion> ::= SI '(' <condicional> ')' <bloque>`**
  * **Modificacion de la gramatica:** Se debe modificar la gramatica para detectar el fin de la
  condicion para agregar un `BF` por si es falsa. La regla de `seleccion` quedaria de la 
  siguiente manera:

  `<seleccion> ::= <fin_condicion> <bloque>`

  `<fin_condicion> ::= SI '(' <condicional> ')'`

  * **Reduccion regla fin_condicion:** Se evalua `<condicional>` generando su polaca correspondiente y
  se emite en la polaca la instruccion `BF` en el numero de paso + 1, dejando pendiente el numero de paso
  actual por si la condicion es falsa. Luego apilamos la posicion de este `BF` para poder completarlo mas adelante.
  * **Reduccion regla seleccion:** Generamos la polaca completa y al finalizar desapilamos la direccion
  del `BF` pendiente y se completa con el numero de paso actual que marca el fin del bloque `SI`.

* **`<seleccion> ::= SI '(' <condicional> ')' <bloque> SINO <bloque>`**
  * **Modificacion de la gramatica:** Se debe modificar la gramatica para detectar fin del bloque verdadero
  de `SI`, fin de la condicion e inicio de `SINO`. La regla de `seleccion` quedaria de la siguiente
  manera:

  `<seleccion> ::= <fin_SI> <inicio_SINO> <bloque>`

  `<fin_SI> ::= SI '(' <fin_condicional> ')' <bloque>`

  `<inicio_SINO> ::= SINO`

  `<fin_condicional> ::= <condicional>`

  * **Fin del bloque SI dentro de SI-SINO:** Se modifica la regla para detectar el fin del bloque `SI`
  dentro de un `SI-SINO` para evitar que se ejecute `SINO` si condicion verdadera. Es la regla `<fin_SI>`.
  * **Fin condicional:** Se modifica la gramatica y se agrega una nueva regla para detectar el fin de la
  condicion para poder saltar al `SINO` si la condicion es falsa. Agregamos esta regla `<fin_condicional>`.
  * **Inicio SINO:** Se modifica la gramatica y se agrega una nueva regla para detectar el inicio del
  bloque `SINO` para llenar con este numero de paso la instruccion `BF` que ira luego de la condicion
  del `SI`.

  * **Reduccion regla fin_condicional:** Al terminar de generar la polaca de la condicion emitimos la 
  instruccion `BF` en el numero de paso + 1, dejando pendiente el numero de paso actual para poder
  completarlo mas adelante con el numero de paso del inicio del `SINO` por si la condicion es falsa.
  * **Reduccion regla fin_SI:** Al terminar el bloque verdadero del `SI` se debe emitir una instruccion
  `BI` en el numero de paso + 1, dejando pendiente el numero de paso actual para poder completarlo mas
  adelante con el numero de paso del fin del bloque `SI-SINO` para evitar que se ejecute `SINO` cuando
  la condicion fue verdadera. Apilamos el numero de paso actual.
  * **Reduccion regla inicio_SINO:** Desapilamos la direccion del `BF` pendiente y se completa con el numero de
  paso actual que indica el inicio del bloque `SINO`.
  * **Reduccion regla seleccion:** Se genera la polaca de seleccion y luego se desapila la posicion del `BI` pendiente y se completa con el numero de paso actual que indica el fin del bloque `SI-SINO`.

### Estructura de Bucle (`PARA`)
* **`<bucle> ::= PARA '(' <control> ';' <condicional> ';' CTE_E ')' <bloque>`**
  * **Modificacion de la gramatica:** Se debe modificar la gramatica para detectar el inicio de la
  condicion y almacenar temporalmente la variable de control, el fin de la condicion, almacenar
  temporalmente el entero de decremento para usarlos al final del bloque y por ultimo el fin del 
  bloque `PARA`. La regla de `bucle` quedaria de la siguiente manera:

  `<bucle>              ::= <bucle_cabecera> <bloque>`

  `<bucle_cabecera>     ::= <bucle_condicion> ';' CTE_E ')'`

  `<bucle_condicion>    ::= <bucle_inicio> <condicional>`

  `<bucle_inicio>       ::= PARA '(' <control> ';'`

  * **Inicio de la condicion:** En la regla `bucle_inicio` se registra el numero de paso actual que
  corresponde al destino del salto incondicional de cada iteracion. Generamos la polaca de la variable
  de control.

  * **Fin de la condicion:** En la regla `bucle_condicion` generamos la polaca de la condicion del bucle
  y apilamos el numero de paso actual que luego completaremos cuando sepamos donde es el fin del bucle, 
  luego agregamos la instruccion `BF` en el numero de paso + 1.

  * **Decremento:** En la regla `bucle_cabecera` almacenamos temporalmente el valor de `CTE_E` y el
  identificador de la variable de control (extraido previamente de `<control>`) para usarlos al final
  del bloque.

  * **Fin PARA:** En la regla `bucle` generamos la polaca de `bloque`, decrementamos la variable de
  control con respecto a la variable de decremento, agregamos la instruccion `BI` en el numero de paso
  + 1 y en el numero de paso actual agregamos el numero de paso que registramos en `bucle_inicio` para
  indicar el numero de paso al que hay que saltar para comenzar el bucle nuevamente. Desapilamos la
  direccion `BF` pendiente e indicamos el salto si la condicion es falsa, en este caso seria el numero
  de paso actual + 2.

### Salida e Impresión (`IMPRIMIR`) y Retorno (`RETORNAR`)

* **`<salida> ::= IMPRIMIR '(' <elementos_salida> ')' ';'`**
  * **Acción:** Genera la polaca de cada `<elemento_salida>`. Cada elemento debe quedar
    seguido por una instrucción de impresión que consuma únicamente el valor
    producido por ese elemento.

* **`<elemento_salida> ::= CADENA`**
  * **Acción:** Escribe en la polaca el literal de cadena y luego
    `IMP_CADENA`, sin tipo numérico. El literal debe conservar su contenido
    sin las comillas delimitadoras.

* **`<elemento_salida> ::= <expresion>`**
  * **Acción:** Genera primero la polaca de `<expresion>` y luego escribe una
    instrucción de impresión según el tipo resultante: `IMP_ENTERO` si es
    `entero` o `IMP_REAL` si es `real`. La instrucción de impresión
    conserva el tipo del valor que consume.

* **`<elemento_salida> ::= <invocacion>`**
  * **Acción:** Genera la polaca de `<invocacion>` y luego escribe
    `IMP_ENTERO` o `IMP_REAL` según el tipo de retorno de la función.

* **`<retorno> ::= RETORNAR <expresion> ';'`**
  * **Acción:** Genera la polaca de `<expresion>`, aplica la conversión de tipo
    correspondiente al tipo de retorno declarado por la función (truncamiento
    de `real` a `entero` o promoción de `entero` a `real`), y escribe `RET`
    con el tipo de retorno de la función.

## 5. Criterios de aceptacion

Para cualquier programa:

1. Cada `BI` y `BF` tienen sus saltos al numero de paso correspondiente.
2. La polaca contiene `INICIO_PROG` antes de las sentencias de `inicio` y
   `FIN_PROG` después de ellas.
3. Cada operando, operador e instrucción conserva el tipo de dato definido en
   las reglas de esta especificación.
4. Las expresiones respetan la precedencia y asociatividad de la gramática.
