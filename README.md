# Compilador 2026

## Integrantes

* Lucas Luna
* Rosa Ortiz

## TP asignado

_Convivencia de enteros y reales con compatibilidad total y memoria estática_

## Lenguaje

- **Lenguaje fuente:** beta
- **Lenguaje de implementación:** _C_
- **Arquitectura destino:** NASM x86

## Cómo compilar

En la carpeta raiz del repositorio se debe ejecutar:

```
bison -d src/parser.y -o src/parser.tab.c
gcc -std=c99 -Wall -Wextra -pedantic -o parser.exe src/parser.tab.c src/lexer.c src/lexer_tables.c
```

Si la instalación de Bison de GnuWin32 está dentro de `Program Files` y
falla al localizar `m4`, ejecutá esos comandos usando una ruta corta:

```
subst X: "C:\Program Files (x86)\GnuWin32"
$env:BISON_PKGDATADIR = "X:\share\bison"
$env:PATH = "X:\bin;$env:PATH"
bison -d src/parser.y -o src/parser.tab.c
subst X: /d
gcc -std=c99 -Wall -Wextra -pedantic -o parser.exe src/parser.tab.c src/lexer.c src/lexer_tables.c
```

## Cómo ejecutar

En la carpeta raiz del repositorio se debe ejecutar:

```
.\parser.exe tests\intermedio_prueba.beta
```

Con una compilación correcta se genera la polaca inversa en
`out\polaca.txt`. Cada línea tiene el formato `paso|elemento|tipo`, con pasos
indexados desde cero. Para probar otro programa, reemplazá la ruta por
cualquier archivo `.beta` válido.

## Estructura del repositorio

- `assets/` — recursos estáticos
- `docs/` — entregables formales de cada etapa
- `src/` — código del compilador
- `tests/` — casos de prueba
  - `correctos/` — programas que deben compilar sin errores
  - `errores-lexicos-sintacticos/`
  - `errores-semanticos/`
- `out/` — tercetos, tabla de símbolos exportada, `.asm` generado
- `specs/` — especificaciones y bitácoras por etapa
