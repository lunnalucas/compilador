#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "lexer_tables.h"

// Retorna el nombre en texto asignado a cada número de token
const char* get_nombre_token(int token) {
    switch (token) {
        case TOKEN_ID:           return "ID";
        case TOKEN_CTE_E:        return "CTE_E";
        case TOKEN_CTE_R:        return "CTE_R";
        case TOKEN_INICIO:       return "INICIO";
        case TOKEN_ENTERO:       return "ENTERO";
        case TOKEN_REAL:         return "REAL";
        case TOKEN_SI:           return "SI";
        case TOKEN_SINO:         return "SINO";
        case TOKEN_PARA:         return "PARA";
        case TOKEN_IMPRIMIR:     return "IMPRIMIR";
        case TOKEN_RETORNAR:     return "RETORNAR";
        case TOKEN_ASIG:         return "ASIG";
        case TOKEN_IGUAL:        return "IGUAL";
        case TOKEN_DISTINTO:     return "DISTINTO";
        case TOKEN_MENOR:        return "MENOR";
        case TOKEN_MAYOR:        return "MAYOR";
        case TOKEN_MENOR_IGUAL:  return "MENOR_IGUAL";
        case TOKEN_MAYOR_IGUAL:  return "MAYOR_IGUAL";
        case TOKEN_AND:          return "AND";
        case TOKEN_OR:           return "OR";
        case TOKEN_SUMA:         return "SUMA";
        case TOKEN_RESTA:        return "RESTA";
        case TOKEN_PRODUCTO:     return "PRODUCTO";
        case TOKEN_DIVISION:     return "DIVISION";
        case TOKEN_PAR_ABRE:     return "PAR_ABRE";
        case TOKEN_PAR_CIERRA:   return "PAR_CIERRA";
        case TOKEN_LLAVE_ABRE:   return "LLAVE_ABRE";
        case TOKEN_LLAVE_CIERRA: return "LLAVE_CIERRA";
        case TOKEN_P_COMA:       return "P_COMA";
        case TOKEN_COMA:         return "COMA";
        case TOKEN_PUNTO:        return "PUNTO";
        case TOKEN_COMILLA:      return "COMILLA";
        case TOKEN_CADENA:       return "CADENA";
        default:                 return "DESCONOCIDO";
    }
}

// Muestra la representación de la Tabla de Símbolos al finalizar
void mostrarTS(void) {
    printf("\n--- TABLA DE SÍMBOLOS ---\n");
    printf("(Visualización de la Tabla de Símbolos)\n");
}

// Imprime en la consola cada token con el formato alineado correcto
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <archivo.beta>\n", argv[0]);
        return 1;
    }

    FILE *archivo = fopen(argv[1], "r");
    if (!archivo) {
        perror("Error al abrir el archivo");
        return 1;
    }

    // 1. Encabezado de la tabla alineado
    printf("\n%-7s | %-8s | %-15s | %s\n", "LÍNEA", "CÓDIGO", "TOKEN", "LEXEMA");
    printf("--------+----------+-----------------+-----------------\n");

    int token;
    int error_detectado = 0;

    while ((token = yylex(archivo)) != EOF) {
        if (token == ERROR_LEXICO) {
            printf("\n[ERROR LÉXICO] Carácter no reconocido en la línea %d\n", get_linea_actual());
            break;
        }

        // 2. Impresión de cada fila con anchos fijos
        printf("%-7d | %-8d | %-15s | '%s'\n",
               get_linea_actual(),
               token,
               get_nombre_token(token),
               get_lexema_actual());
    }

    fclose(archivo);
    printf("--------+----------+-----------------+-----------\n\n");
    
    // 4. Reporte final del estado del análisis
    if (error_detectado == 0) {
        printf("\nCompilacion exitosa\n");
        mostrarTS();
    } else {
        printf("\nAnalisis lexico completo con errores\n");
    }

    return 0;
}