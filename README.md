# Compilador 2026

## Integrantes

* Lucas Luna
* Rosa Ortiz

## TP asignado

_Convivencia de enteros y reales con compatibilidad total y memoria estática_

## Lenguaje

- **Lenguaje fuente:** beta
- **Lenguaje de implementación:** _C_
- **Arquitectura destino:** NASM x86 de 32 bits
- **Entrada de los programas:** Archivos con extensión `.beta`

## Descripción

Compilador para el lenguaje Beta.

El proceso implementado comprende:

1. Análisis léxico.
2. Análisis sintáctico y semántico.
3. Generación de código intermedio en Polaca Inversa.
4. Traducción de la Polaca Inversa a NASM x86 de 32 bits.
5. Generación de trazas y archivos de salida.

## Características del lenguaje

Los detalles formales de nombres reservados, gramática, tipos y restricciones
se encuentran en [specs/](/specs/).

## Ejecución rápida

Abrir PowerShell en la raíz del repositorio

Compilar el compilador:

```powershell
gcc -std=c99 -Wall -Wextra -pedantic `
  -o out\betac.exe `
  src\parser.tab.c src\lexer.c src\lexer_tables.c src\assembler.c
```

Ejecutá un programa Beta:

```powershell
.\out\betac.exe tests\correctos\01_expresiones_aritmeticas.beta
```

Para probar los demás casos correctos:

```powershell
.\out\betac.exe tests\correctos\02_si_dentro_de_para.beta
.\out\betac.exe tests\correctos\03_para.beta
```

Los casos de `tests\incorrectos\` deben terminar con error:

```powershell
.\out\betac.exe tests\incorrectos\01_caracter_invalido.beta
.\out\betac.exe tests\incorrectos\02_parametros_funcion.beta
.\out\betac.exe tests\incorrectos\03_sintactico.beta
```

## Dependencias

Para probar el compilador completo en Windows se necesitan las siguientes
herramientas disponibles desde PowerShell/CMD:

### Bison

Genera el archivo `src/parser.tab.c` a partir de la gramática
`src/parser.y`.

### GCC con soporte para ejecutables de 32 bits

Se utiliza para:

1. Compilar el compilador en C.
2. Linkear el código ensamblador generado.

Se recomienda utilizar **MinGW-w64 i686** o cualquier toolchain que incluya
`gcc -m32`. Una instalación exclusiva de GCC x86-64 puede no poder enlazar
objetos de 32 bits.

### NASM

Ensamblador de sintaxis NASM. El formato generado por el proyecto es:

```powershell
nasm -f win32
```

## Estructura del repositorio

- `assets/` — imágenes y otros recursos estáticos.
- `src/` — implementación del compilador.
  - `lexer.c` y `lexer_tables.c`: análisis léxico.
  - `parser.y`: gramática y acciones semánticas.
  - `parser.tab.c` y `parser.tab.h`: archivos generados por Bison.
  - `intermedio_types.h`: tipos utilizados por el código intermedio.
  - `assembler.c` y `assembler.h`: generación de código NASM.
- `tests/` — archivos fuente `.beta` utilizados para verificar el compilador.
  - `correctos/` — programas que deben compilar sin errores
  - `incorrectos/` — pruebas de errores léxicos o sintácticos.
- `out/` — polaca, código NASM, trazas y ejecutables generados.
- `specs/` — especificaciones y bitácoras por etapa.
