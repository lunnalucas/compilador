# Spec — 06-codigo-intermedio

**Grupo:** B · **Lenguaje de implementación:** C
**Depende de:**
[`specs/01-diseno/spec.md`](../01-diseno/spec.md),
[`specs/02-tabla-simbolos/spec.md`](../02-tabla-simbolos/spec.md),
[`specs/03-analizador-lexico/spec.md`](../03-analizador-lexico/spec.md) y
[`specs/04-analizador-sintactico/spec.md`](../04-analizador-sintactico/spec.md)
**Consume:** programa válido.
**Produce:** lista de las celdas de la polaca inversa.
**Estado:** A IMPLEMENTAR.

---

## 1. Objetivo

Se debe generar el codigo intermedio mediante la polaca inversa para hacer
el pasaje a codigo Assembler.

Esta etapa construye el código intermedio que será consumido por una etapa posterior
y completa las acciones semanticas de [`/src/parser.y`](/src/parser.y).

En esta especificacion decimos como generar el codigo intermedio, no se debe
hacer algo que no este escrito aca.

La spec define la estructura de cada celda, los opcodes y los ejemplos de polaca esperada.

---

## 2. Alcance

### 2.1. Lo que incluye

1. Generación de la polaca inversa
2. Resultado de la polaca del programa en `out/polaca.txt`.

### 2.2. Lo que no incluye

- Análisis léxico, sintáctico o semántico.
- Detección o reporte de errores.
- Generación de assembler.
- Tercetos, AST o cualquier representación distinta de la polaca.

---

## 3. Salidas

### 3.1. Salida en memoria

Una única estructura `Polaca` con un arreglo dinámico de celdas:

```c
typedef struct {
    CeldaPolaca *celdas;
    int cantidad;
    int capacidad;
} Polaca;
```

La polaca contiene todas las funciones del programa y termina cuando termina la
última función.

### 3.2. Salida en archivo

Se genera `out/polaca.txt`, sobrescribiendo el archivo si ya existe.

Formato: una celda por línea, con columnas separadas por espacios. Los campos
vacíos se representan con `-`.

Ejemplo:

```text
#  CLASE        OP          REF        TIPO       LINEA
0  FUNCION_INICIO -         FUNC[2]    -          10
1  OPERANDO      -          TS[3]      ENTERO     11
2  OPERANDO      -          TS[5]      ENTERO     11
3  OPERADOR      SUMA       -          ENTERO     11
4  OPERADOR      ASIG       -          ENTERO     11
5  FUNCION_FIN   -          FUNC[2]    -          12
```
Aclaracion: FUNCION_INICIO significa el inicio de una funcion,
no necesariamente es la funcion inicio del lenguaje.

En la impresion del archivo de texto las referencias mostrarian en nombre
de la variable, no la referencia.

## 4. Representación de la polaca

### 4.1. La celda

Tendremos 9 clases de celdas:

- operando
- operador
- etiqueta
- salto
- llamada
- retorno
- funcion_inicio
- funcion_fin
- cadena

La celda tambien tendra el tipo de dato: entero, real, booleano o ninguno.

Finalmente cada celda de la polaca tendra lo siguiente:

- clase de la celda
- operador (opcode 0 si la clase no utiliza)
- referencia
- tipo de dato
- linea

### 4.2. Significado de `referencia` por clase

| Clase | `referencia` | `operador` | `tipo` |
|---|---|---|---|
| `operando` | índice TS de variable o constante | 0 | tipo del valor |
| `cadena` | índice en la tabla de cadenas | 0 | `TIPO_NINGUNO` |
| `operador` | 0 | opcode | según §5.3 |
| `etiqueta` | ID de etiqueta | 0 | `TIPO_NINGUNO` |
| `salto` | ID de etiqueta destino | `OP_JMP` o `OP_BF` | `TIPO_NINGUNO` |
| `llamada` | índice TS de la función | `OP_CALL` | tipo de retorno |
| `retorno` | 0 | `OP_RET` | tipo de retorno |
| `funcion_inicio` | índice TS de la función | 0 | `TIPO_NINGUNO` |
| `funcion_fin` | índice TS de la función | 0 | `TIPO_NINGUNO` |

### 4.3. Operadores

No se permiten operadores fuera de esta tabla.

```c
typedef enum {
    OP_SUMA,
    OP_RESTA,
    OP_PRODUCTO,
    OP_DIVISION,
    OP_ASIG,
    OP_IGUAL,
    OP_DISTINTO,
    OP_MENOR,
    OP_MAYOR,
    OP_MENOR_IGUAL,
    OP_MAYOR_IGUAL,
    OP_AND,
    OP_OR,
    OP_JMP,
    OP_BF,
    OP_CALL,
    OP_RET,
    OP_PRINT_STR,
    OP_PRINT_INT,
    OP_PRINT_REAL
} Opcode;
```

### 4.4. Organización de la polaca

Se usa **una sola polaca** para todo el programa:

```text
FUNCION_INICIO f1
  celdas de f1
FUNCION_FIN f1
FUNCION_INICIO f2
  celdas de f2
FUNCION_FIN f2
FUNCION_INICIO inicio
  celdas de inicio
FUNCION_FIN inicio
```

Reglas:

1. Las funciones se emiten en el orden en que aparecen en el fuente.
2. `inicio` es el punto de entrada.
3. Cada función tiene exactamente un `FUNCION_INICIO` y un `FUNCION_FIN`.
4. La ejecución comienza en el `FUNCION_INICIO` de `inicio`.

---

## 5. Convenciones de generación

### 5.1. Post-orden

Primero se emiten los operandos y después el operador, ejemplo:

```text
a + b        →  a b SUMA
a + b * c    →  a b c PRODUCTO SUMA
(a + b) * c  →  a b SUMA c PRODUCTO
```

La precedencia y la asociatividad ya las resuelve la gramática de [`specs/01-diseno/spec.md`](../01-diseno/spec.md).

### 5.2. Significado del campo `tipo`

| Celda | Significado de `tipo` |
|---|---|
| `CELDA_OPERANDO` | tipo del valor (variable o constante) |
| `SUMA`, `RESTA`, `PRODUCTO`, `DIVISION` | tipo del resultado después de la promoción |
| `IGUAL`, `DISTINTO`, `MENOR`, `MAYOR`, `MENOR_IGUAL`, `MAYOR_IGUAL` | tipo de los operandos después de la promoción; el resultado siempre es booleano |
| `AND`, `OR` | `TIPO_BOOLEANO` |
| `ASIGNACION` | tipo del destino |
| `CALL` | tipo de retorno de la función |
| `RETORNO` | tipo de retorno de la función actual |
| `PRINT_INT` | `TIPO_ENTERO` |
| `PRINT_REAL` | `TIPO_REAL` |
| `PRINT_STR` | `TIPO_NINGUNO` |

### 5.3. Etiquetas y saltos

1. Las etiquetas son enteros generados por un contador global que comienza en 1. 
Estos numeros corresponden al identificador de cada etiqueta.
2. Cada etiqueta se emite una sola vez con `CELDA_ETIQUETA`.
3. `JMP` salta siempre a la etiqueta indicada.
4. `BF` consume un booleano y salta si es falso (`0`).

## 6. Interfaz del generador de codigo intermedio

Se implementa en `src/codigo_intermedio.h` y `src/codigo_intermedio.c`.

### 6.1. Funciones de emisión

```c
void polaca_iniciar(void); // Inicia la polaca
void polaca_liberar(void); // Libera espacio por si ya habia una polaca creada anteriormente

int emitir_operando(int ref_ts, TipoDato tipo); //crea una celda operando en la polaca
int emitir_constante_entero(long valor); //crea una celda de la variable entera en la polaca
int emitir_constante_real(double valor); //crea una celda de la variable real en la polaca
int emitir_cadena(int ref_cadena); //crea una celda de la cadena en la polaca; es para la funcion imprimir
int emitir_operador(int op, TipoDato tipo); //crea una celda operador
int emitir_salto(int op, int etiqueta);//crea la celda de salto en la polaca
int emitir_etiqueta(int etiqueta);//crea la celda de etiqueta en la polaca
int emitir_llamada(int ref_funcion, TipoDato tipo_retorno);//crea la celda de llamada a funcion en la polaca
int emitir_retorno(TipoDato tipo_retorno);//crea la celda de retorno en la polaca
int emitir_funcion_inicio(int ref_funcion);//crea la celda de inicio de funcion en la polaca
int emitir_funcion_fin(int ref_funcion);//crea la celda de fun de funcion en la polaca

void polaca_guardar(const char *ruta);//guarda la polaca en un archivo de texto
```

Cada `emitir_*` agrega una celda al final y devuelve su índice.

`emitir_constante_entero` y `emitir_constante_real` insertan la constante en la
TS y devuelven su índice.

### 6.2. Etiquetas

```c
int nueva_etiqueta(void);
```

Devuelve un ID nuevo, empezando en 1. El contador se reinicia en
`polaca_iniciar`.


## 7. Ejemplo completo

Programa:

```beta
entero doble(entero x) {
    retornar x * 2;
}

inicio {
    entero a, b;
    a = 3;
    b = doble(a);
    si (b > 5) {
        imprimir("mayor");
    } sino {
        imprimir("menor");
    }
}
```

Polaca esperada:

```text
FUNCION_INICIO FUNC[doble]
  x 2 PRODUCTO RET
FUNCION_FIN FUNC[doble]

FUNCION_INICIO FUNC[inicio]
  3 a ASIG
  a CALL doble b ASIG
  b 5 MAYOR
  BF L1
  STR[0] PRINT_STR
  JMP L2
L1:
  STR[1] PRINT_STR
L2:
FUNCION_FIN FUNC[inicio]
```

Tabla de celdas:

| # | Clase | Op | Ref | Tipo |
|---|---|---|---|---|
| 0 | `FUNCION_INICIO` | - | `FUNC[doble]` | - |
| 1 | `OPERANDO` | - | `TS[x]` | `ENTERO` |
| 2 | `OPERANDO` | - | `TS[2]` | `ENTERO` |
| 3 | `OPERADOR` | `PRODUCTO` | - | `ENTERO` |
| 4 | `RETORNO` | `RET` | - | `ENTERO` |
| 5 | `FUNCION_FIN` | - | `FUNC[doble]` | - |
| 6 | `FUNCION_INICIO` | - | `FUNC[inicio]` | - |
| 7 | `OPERANDO` | - | `TS[3]` | `ENTERO` |
| 8 | `OPERANDO` | - | `TS[a]` | `ENTERO` |
| 9 | `OPERADOR` | `ASIG` | - | `ENTERO` |
| 10 | `OPERANDO` | - | `TS[a]` | `ENTERO` |
| 11 | `LLAMADA` | `CALL` | `FUNC[doble]` | `ENTERO` |
| 12 | `OPERANDO` | - | `TS[b]` | `ENTERO` |
| 13 | `OPERADOR` | `ASIG` | - | `ENTERO` |
| 14 | `OPERANDO` | - | `TS[b]` | `ENTERO` |
| 15 | `OPERANDO` | - | `TS[5]` | `ENTERO` |
| 16 | `OPERADOR` | `MAYOR` | - | `ENTERO` |
| 17 | `SALTO` | `BF` | `L[1]` | - |
| 18 | `CADENA` | - | `STR[0]` | - |
| 19 | `OPERADOR` | `PRINT_STR` | - | - |
| 20 | `SALTO` | `JMP` | `L[2]` | - |
| 21 | `ETIQUETA` | - | `L[1]` | - |
| 22 | `CADENA` | - | `STR[1]` | - |
| 23 | `OPERADOR` | `PRINT_STR` | - | - |
| 24 | `ETIQUETA` | - | `L[2]` | - |
| 25 | `FUNCION_FIN` | - | `FUNC[inicio]` | - |

Los nombres `x`, `a` y `b` son solo para legibilidad: en la implementación y en
`out/polaca.txt` se usan los índices reales de la TS.

---

### 8. Verificaciones

Para cualquier programa:

1. Cada `FUNCION_INICIO` tiene su `FUNCION_FIN`.
2. Cada `JMP` y `BF` referencia una etiqueta definida exactamente una vez.
3. Cada `REF` de tipo `TS[n]` existe.
4. Cada `REF` de tipo `STR[n]` existe.
5. Cada `CALL` referencia una función registrada.
6. Toda celda que produce valor tiene un `tipo` distinto de `TIPO_NINGUNO`.
7. Crear tests de programas beta para verificar su funcionamiento.