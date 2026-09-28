# Spec — Tabla de símbolos de beta

**Grupo:** B · **Lenguaje de implementación:** C
**Depende de:** [`specs/01-diseno/spec.md`](../01-diseno/spec.md) y
[`specs/03-analizador-lexico/spec.md`](../03-analizador-lexico/spec.md)
**Consume:** identificadores, constantes y tokens producidos por el analizador
léxico.
**Utilizan sus datos:** analizador sintáctico, análisis semántico y generador de
código.

---

## 1. Objetivo

La tabla de símbolos registra la información asociada a cada identificador
que el compilador necesita conservar durante las fases posteriores. Como mínimo,
cada entrada debe contener:

- `nombre`
- `categoria`
- `tipo`
- `valor`
- `longitud`
- `alcance`
- `ubicacion`


Además, la tabla debe permitir resolver un nombre según su alcance, detectar
declaraciones duplicadas y conservar los metadatos necesarios de variables,
parámetros y funciones.

La tabla de símbolos no reemplaza al analizador sintáctico ni al análisis
semántico. No decide si una expresión es válida, si los tipos son compatibles o
si una función se invoca correctamente; proporciona la información para que
esas fases puedan hacerlo.

## 2. Alcance

Esta especificación incluye:

1. la representación de variables, parámetros, funciones y constantes;
2. la tabla de palabras reservadas;
3. la creación, búsqueda y salida de scopes;
4. la normalización de identificadores de acuerdo con el límite del lenguaje;
5. la información de tipos, valores, longitudes y almacenamiento;
6. los errores y advertencias propios de la tabla de símbolos;
7. la interfaz mínima que utilizarán las demás fases.

Quedan fuera de esta etapa:

- el reconocimiento de lexemas, que corresponde al analizador léxico
- el análisis de la gramática, que corresponde al analizador sintáctico
- la comprobación completa de tipos y expresiones
- la generación de offsets o instrucciones finales, salvo que se reserve el
  espacio necesario para agregarlos posteriormente.

## 3. Reglas del lenguaje que condicionan la tabla

| Regla | Consecuencia |
|---|---|
| D1 | Sólo existen los tipos de datos `entero` y `real`. |
| D2 | `entero` ocupa 32 bits y `real` ocupa 64 bits. |
| D3 | Toda variable debe declararse con un tipo. |
| D4 | Las variables son locales al ámbito en que se declaran y tienen almacenamiento estático. |
| D5 | Los nombres distinguen mayúsculas y minúsculas: `Total` y `total` son entradas diferentes. |
| D6 | El nombre efectivo tiene como máximo 20 caracteres; el lexer trunca los nombres más largos y emite E4. |
| D10/D13 | Una función puede tener como máximo dos parámetros y, según la gramática vigente, debe tener uno o dos. |
| D11 | Los parámetros se pasan por copia de valor. |
| R1 | Una variable `entero` se inicializa en `0` y una `real` en `0.0`. |
| R2–R5 | La información de tipo se consulta para promociones, truncamientos y comparaciones. |

El programa tiene una función principal `inicio`, sin parámetros ni valor de
retorno. Las otras funciones tienen un tipo de retorno (`entero` o `real`) y
entre uno y dos parámetros.

## 4. Modelo de scopes

### 4.1 Scopes requeridos

Se mantienen los siguientes scopes:

| Ámbito | Contenido |
|---|---|
| Global | Funciones y nombres reservados del compilador. |
| Función | Parámetros y variables pertenecientes a una función. |
| Principal | Variables declaradas dentro de `inicio`. | 
| Bloque anidado | Declaraciones de un bloque que admita declaraciones según la gramática. | 

Los scopes se organizan en una pila. La búsqueda comienza en el scope actual y
continúa hacia sus padres hasta llegar al global. Una declaración sólo se
compara con las entradas del scope actual.

Las funciones se registran en el ámbito global. `inicio` se registra como
función principal especial y no puede declararse otra función con ese nombre.
No se permiten funciones anidadas.

### 4.2 Resolución de nombres

La búsqueda debe distinguir tres resultados:

1. encontrado en el ámbito actual
2. encontrado en un ámbito padre
3. no encontrado

Una referencia a una variable o función usa la entrada visible más cercana. Una
palabra reservada no se trata como identificador de usuario aunque su lexema
aparezca en el flujo de tokens.

## 5. Categorías de entradas

Cada entrada tiene una categoría explícita:

| Categoría | Uso | Tiene valor almacenado |
|---|---|---|
| `PARAMETRO` | Parámetro formal de una función. | No requiere valor constante, conserva su tipo y paso por copia. |
| `FUNCION` | Función declarada por `<tipo> ID (...)`. | No, conserva retorno y parámetros. |
| `CONSTANTE_ENTERA` | Literal reconocido como `CTE_E`. | Sí, valor entero. |
| `CONSTANTE_REAL` | Literal reconocido como `CTE_R`. | Sí, valor real. |
| `RESERVADA` | Palabra reservada del lenguaje. | No, conserva token asociado. |


## 6. Registro de una entrada

La implementación mínima que debe utilizar este esquema:

| Campo  | Significado |
|---|---|
| `nombre` | nombre efectivo almacenado. Tiene como máximo 20 caracteres. |
| `categoria` | se indica si el simbolo es una funcion o una variable. |
| `tipo` | tipo de dato del símbolo o tipo de retorno de una función. |
| `valor` | valor de una constante o literal; ausente para parámetros y funciones. |
| `longitud` | cantidad de caracteres del `nombre` efectivo. |
| `alcance` | alcance donde se declaró o almacenó la entrada. |
| `ubicacion` | posición de la declaración o del literal que originó la entrada. |

Para entradas de categoría `FUNCION` se agregan:

| Campo | Significado |
|---|---|
| `cantidad_parametros` | uno o dos parametros segùn el diseño |
| `parametros` | Lista ordenada de referencias a entradas `PARAMETRO`. |


### 6.1 Valores y longitudes

- `nombre` no incluye comillas ni delimitadores.
- La longitud de un identificador es la longitud del nombre efectivo después del
  truncamiento definido por D6.
- `CTE_E` se almacena como entero de 32 bits.
- `CTE_R` se almacena como real de 64 bits.
- Una variable sin inicialización explícita recibe `0` o `0.0`, según su tipo,
  de acuerdo con R1.
- Una función no tiene `valor`, su resultado se obtiene durante la ejecución.

## 7. Tabla de palabras reservadas

Las palabras reservadas se mantienen en una tabla fija, separada de los símbolos
de usuario:

| Lexema | Token |
|---|---|
| `inicio` | `INICIO` (103) |
| `entero` | `ENTERO` (104) |
| `real` | `REAL` (105) |
| `si` | `SI` (106) |
| `sino` | `SINO` (107) |
| `para` | `PARA` (108) |
| `imprimir` | `IMPRIMIR` (109) |
| `retornar` | `RETORNAR` (110) |

La consulta debe ser sensible a mayúsculas. Por ejemplo, `Real` es un `ID`,
mientras que `real` produce `REAL`. El analizador léxico reconoce primero el
identificador y luego realiza esta consulta.


## 8. Criterios de aceptación

La tabla de símbolos se considera aceptada cuando:

1. toda entrada contiene como mínimo `nombre`, `categoria`, `tipo`, `valor`, `longitud`, `alcance` y `ubicacion`.
2. el valor está presente para constantes y literales, y es explícitamente
   ausente para funciones y parámetros
3. distingue variables, parámetros, funciones, constantes, cadenas y reservadas
4. resuelve nombres desde el ámbito más interno hacia el global
5. detecta redeclaraciones sólo dentro del ámbito donde ocurren
6. conserva el tipo `ENTERO` o `REAL` y sus rangos de 32 y 64 bits
7. mantiene nombres sensibles a mayúsculas y con el truncamiento ya realizado
   por el lexer
8. registra las funciones y sus parámetros en orden
9. aplica inicialización por defecto a las variables
10. permite consultar literales y conservar sus valores
11. no confunde “nombre inexistente” con una entrada válida
12. libera todos los ámbitos y entradas al finalizar la compilación
13. no modifica las reglas léxicas ni agrega tokens no definidos en el diseño
