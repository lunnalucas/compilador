# Como probar el analizador lexico

La herramienta de prueba recibe un archivo de entrada y muestra cada token
que devuelve `yylex()`. El archivo puede tener cualquier nombre, pero se
recomienda usar la extension `.beta`.

## 1. Crear un programa beta

Se puede usar el ejemplo incluido en
`tests/correctos/ejemplo.beta` o crear otro archivo `.beta`.

## 2. Compilar la herramienta

Desde la carpeta raiz del repositorio, ejecutar:

```text
gcc -Wall -Wextra -std=c11 tests/test_lexer.c src/lexico/lexer.c src/lexico/lexer_tables.c -o tests/test_lexer.exe
```

## 3. Ejecutar el analisis

Indicar la ruta del programa beta como unico argumento:

```text
tests\test_lexer.exe tests\correctos\ejemplo.beta
```

En Windows tambien se puede ejecutar con:

```text
.\tests\test_lexer.exe .\tests\correctos\ejemplo.beta
```

La salida muestra la linea, el nombre del token, su codigo y el texto leido.
Los espacios, saltos de linea y comentarios no aparecen porque el lexer los
descarta. Los errores lexicos se informan por la salida de errores.

## 4. Probar otro archivo

Solo hay que cambiar la ruta del ultimo comando:

```text
.\tests\test_lexer.exe ruta\al\programa.beta
```

El analizador devuelve `0` al llegar al final del archivo.
