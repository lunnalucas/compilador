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
El programa analiza el código fuente de principio a fin y produce un archivo
NASM listo para ensamblarse y enlazarse con GCC/MinGW.

El proceso implementado comprende:

1. Análisis léxico.
2. Análisis sintáctico y semántico.
3. Generación de código intermedio en Polaca Inversa.
4. Traducción de la Polaca Inversa a NASM x86 de 32 bits.
5. Generación de trazas y archivos de salida.

## Características del lenguaje

Los detalles formales de nombres reservados, gramática, tipos y restricciones
se encuentran en [specs/](/specs/).

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

Verificá las instalaciones con:

```powershell
bison --version
gcc --version
nasm -v
```

Los ejecutables `bison`, `gcc` y `nasm` deben encontrarse en el `PATH` o
deberán indicarse con su ruta completa al ejecutar los comandos.

## Compilar el compilador

Desde la raíz del repositorio:

```powershell
bison -d src/parser.y -o src/parser.tab.c
gcc -std=c99 -Wall -Wextra -pedantic -o out/compilador.exe src/parser.tab.c src/lexer.c src/lexer_tables.c src/assembler.c
```

Si la instalación de Bison de GnuWin32 está dentro de `Program Files` y
falla al localizar `m4`, ejecutá los comandos usando una unidad corta:

```powershell
subst X: "C:\Program Files (x86)\GnuWin32"
$env:BISON_PKGDATADIR = "X:\share\bison"
$env:PATH = "X:\bin;$env:PATH"
bison -d src/parser.y -o src/parser.tab.c
subst X: /d
gcc -std=c99 -Wall -Wextra -pedantic -o out/compilador.exe src/parser.tab.c src/lexer.c src/lexer_tables.c src/assembler.c
```

Al finalizar debe existir `compilador.exe` en `out/`.

## Prueba de punta a punta

El proceso completo consta de cuatro pasos:

1. Ejecutar el compilador sobre un archivo fuente `.beta`.
2. Verificar la generación de la polaca inversa.
3. Ensamblar y linkear el `.asm` generado.
4. Ejecutar el programa resultante.

### 1. Compilar un programa fuente

Ejemplo con el caso de expresiones aritméticas:

```powershell
out\compilador.exe tests\correctos\01_expresiones_aritmeticas.beta
```

Con una compilación correcta se crean:

- `out\polaca.txt`: polaca inversa final.
- `out\assembler.asm`: fuente NASM x86 de 32 bits.
- `out\traza_assembler.txt`: traza de traducción.

Cada línea de `out\polaca.txt` tiene el formato
`paso|elemento|tipo`, con pasos indexados desde cero.

La traza también se muestra en la consola. El mensaje final esperado para un
caso correcto es:

```text
Compilacion completa sin errores
Codigo NASM generado en out/assembler.asm
```

Para probar otro programa, reemplazá la ruta por otro archivo `.beta` válido:

```powershell
out\compilador.exe tests\correctos\02_si_dentro_de_para.beta
out\compilador.exe tests\correctos\03_para.beta
```

Cada ejecución sobrescribe los archivos de salida presentes en `out`.

### 2. Ensamblar el código generado

```powershell
nasm -f win32 out\assembler.asm -o out\assembler.obj
```

Si NASM no encuentra símbolos o informa errores de sintaxis, copiá el mensaje
completo junto con el archivo `out\assembler.asm`.

### 3. Linkear el objeto

```powershell
gcc -m32 out\assembler.obj -o out\assembler.exe
```

Esto genera `out\assembler.exe`, un ejecutable Win32 de 32 bits.

### 4. Ejecutar el programa generado

```powershell
.\out\assembler.exe
```

## Solución de problemas

- `bison: not found`: agregá el directorio `bin` de Bison al `PATH`.
- `nasm: not found`: agregá el directorio `bin` de NASM al `PATH`.
- `gcc: not found`: instalá MinGW-w64 o agregá su `bin` al `PATH`.
- `gcc -m32` no es reconocido o falla el linkeo: cambiá a un toolchain
  MinGW-w64 i686 con soporte para bibliotecas de 32 bits.

## Archivos generados

Durante las pruebas pueden generarse los siguientes archivos:

```text
out/
  polaca.txt
  assembler.asm
  traza_assembler.txt
  *.obj
  *.exe
```

## Estructura del repositorio

- `assets/` — imágenes y otros recursos estáticos.
- `docs/` — entregables formales del proyecto.
- `src/` — implementación del compilador.
  - `lexer.c` y `lexer_tables.c`: análisis léxico.
  - `parser.y`: gramática y acciones semánticas.
  - `parser.tab.c` y `parser.tab.h`: archivos generados por Bison.
  - `intermedio_types.h`: tipos utilizados por el código intermedio.
  - `assembler.c` y `assembler.h`: generación de código NASM.
- `tests/` — archivos fuente `.beta` utilizados para verificar el compilador.
  - `correctos/` — programas que deben compilar sin errores
  - `errores-lexicos-sintacticos/` — pruebas de errores léxicos o sintácticos.
  - `errores-semanticos/` — pruebas de errores semánticos.
- `out/` — polaca, código NASM, trazas y ejecutables generados.
- `specs/` — especificaciones y bitácoras por etapa.
