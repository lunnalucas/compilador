#include "assembler.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { TIPO_SIN_TIPO, TIPO_ENTERO, TIPO_REAL };

typedef struct {
    size_t paso;
    char elemento[8192];
    int tipo;
    int cadena;
} Celda;

typedef struct {
    char nombre[64];
    int tipo;
    int ambito;
} Variable;

typedef struct {
    char nombre[64];
    int tipo;
    int parametros[2];
    size_t cantidad_parametros;
} Funcion;

typedef struct {
    char nombre[8192];
} Cadena;

typedef struct {
    char lexema[8192];
} ConstanteReal;

typedef struct {
    int tipo;
    int variable;
    int cadena;
} ValorPila;

static Celda *celdas;
static size_t cantidad_celdas;
static size_t capacidad_celdas;
static Variable *variables;
static size_t cantidad_variables;
static size_t capacidad_variables;
static Funcion *funciones;
static size_t cantidad_funciones;
static size_t capacidad_funciones;
static Cadena *cadenas;
static size_t cantidad_cadenas;
static size_t capacidad_cadenas;
static ConstanteReal *constantes_reales;
static size_t cantidad_constantes_reales;
static size_t capacidad_constantes_reales;
static ValorPila *pila;
static size_t cantidad_pila;
static size_t capacidad_pila;
static int error_memoria;

static int reservar(void **datos, size_t *capacidad, size_t requerido,
                    size_t tamano_elemento) {
    if (*capacidad >= requerido) return 1;
    size_t nueva = *capacidad == 0 ? 32 : *capacidad;
    while (nueva < requerido) {
        if (nueva > (size_t)-1 / 2) return 0;
        nueva *= 2;
    }
    if (nueva > (size_t)-1 / tamano_elemento) return 0;
    void *nuevo = realloc(*datos, nueva * tamano_elemento);
    if (nuevo == NULL) return 0;
    *datos = nuevo;
    *capacidad = nueva;
    return 1;
}

static int tipo_desde_texto(const char *tipo) {
    if (strcmp(tipo, "entero") == 0) return TIPO_ENTERO;
    if (strcmp(tipo, "real") == 0) return TIPO_REAL;
    return TIPO_SIN_TIPO;
}

static int es_numero(const char *texto, int *es_real) {
    char *fin;
    errno = 0;
    (void)strtod(texto, &fin);
    if (fin == texto || *fin != '\0' || errno == ERANGE) return 0;
    *es_real = strchr(texto, '.') != NULL;
    return 1;
}

static int es_operador_o_instruccion(const char *texto) {
    static const char *const palabras[] = {
        "PARAM", "RET", "INICIO_PROG", "FIN_PROG", "=", "+", "-", "*",
        "/", "==", "!=", "<", ">", "<=", ">=", "&", "|", "IMP_ENTERO",
        "IMP_REAL", "IMP_CADENA"
    };
    if (strcmp(texto, "BF") == 0 || strcmp(texto, "BI") == 0) return 1;
    for (size_t i = 0; i < sizeof(palabras) / sizeof(palabras[0]); i++)
        if (strcmp(texto, palabras[i]) == 0) return 1;
    return strncmp(texto, "BF ", 3) == 0 ||
           strncmp(texto, "BI ", 3) == 0 ||
           strncmp(texto, "CALL ", 5) == 0 ||
           strncmp(texto, "FUNC ", 5) == 0;
}

static int agregar_celda(size_t paso, const char *elemento, const char *tipo) {
    if (!reservar((void **)&celdas, &capacidad_celdas, cantidad_celdas + 1,
                  sizeof(*celdas))) return 0;
    Celda *celda = &celdas[cantidad_celdas++];
    celda->paso = paso;
    snprintf(celda->elemento, sizeof(celda->elemento), "%s", elemento);
    celda->tipo = tipo_desde_texto(tipo);
    celda->cadena = -1;
    return 1;
}

static int leer_polaca(const char *nombre) {
    FILE *archivo = fopen(nombre, "r");
    if (archivo == NULL) {
        fprintf(stderr, "No se pudo abrir la polaca: %s\n", nombre);
        return 0;
    }
    char linea[16384];
    size_t esperado = 0;
    while (fgets(linea, sizeof(linea), archivo) != NULL) {
        size_t longitud = strlen(linea);
        if (longitud > 0 && linea[longitud - 1] != '\n' && !feof(archivo)) {
            fprintf(stderr, "Linea demasiado larga en %s\n", nombre);
            fclose(archivo);
            return 0;
        }
        while (longitud > 0 &&
               (linea[longitud - 1] == '\n' || linea[longitud - 1] == '\r'))
            linea[--longitud] = '\0';
        char *separador_paso = strchr(linea, '|');
        char *separador_tipo = strrchr(linea, '|');
        if (separador_paso == NULL || separador_tipo == separador_paso) {
            fprintf(stderr, "Formato invalido en polaca, linea %zu\n", esperado + 1);
            fclose(archivo);
            return 0;
        }
        *separador_paso = '\0';
        *separador_tipo = '\0';
        char *fin;
        errno = 0;
        unsigned long paso = strtoul(linea, &fin, 10);
        if (errno != 0 || fin == linea || *fin != '\0' ||
            paso != esperado || !agregar_celda((size_t)paso,
                                               separador_paso + 1,
                                               separador_tipo + 1)) {
            fprintf(stderr, "Paso invalido en polaca, linea %zu\n", esperado + 1);
            fclose(archivo);
            return 0;
        }
        esperado++;
    }
    if (ferror(archivo)) {
        fprintf(stderr, "Error al leer la polaca: %s\n", nombre);
        fclose(archivo);
        return 0;
    }
    fclose(archivo);
    if (cantidad_celdas == 0) {
        fprintf(stderr, "La polaca esta vacia\n");
        return 0;
    }
    return 1;
}

static int buscar_funcion(const char *nombre) {
    for (size_t i = 0; i < cantidad_funciones; i++)
        if (strcmp(funciones[i].nombre, nombre) == 0) return (int)i;
    return -1;
}

static int agregar_funcion(const char *nombre, int tipo) {
    if (!reservar((void **)&funciones, &capacidad_funciones,
                  cantidad_funciones + 1, sizeof(*funciones))) return -1;
    Funcion *funcion = &funciones[cantidad_funciones];
    memset(funcion, 0, sizeof(*funcion));
    snprintf(funcion->nombre, sizeof(funcion->nombre), "%s", nombre);
    funcion->tipo = tipo;
    return (int)cantidad_funciones++;
}

static int buscar_variable(int ambito, const char *nombre) {
    for (size_t i = 0; i < cantidad_variables; i++)
        if (variables[i].ambito == ambito &&
            strcmp(variables[i].nombre, nombre) == 0) return (int)i;
    return -1;
}

static int agregar_variable(int ambito, const char *nombre, int tipo) {
    int existente = buscar_variable(ambito, nombre);
    if (existente >= 0) {
        if (tipo != TIPO_SIN_TIPO) variables[existente].tipo = tipo;
        return existente;
    }
    if (!reservar((void **)&variables, &capacidad_variables,
                  cantidad_variables + 1, sizeof(*variables))) return -1;
    Variable *variable = &variables[cantidad_variables];
    memset(variable, 0, sizeof(*variable));
    snprintf(variable->nombre, sizeof(variable->nombre), "%s", nombre);
    variable->tipo = tipo;
    variable->ambito = ambito;
    return (int)cantidad_variables++;
}

static int agregar_cadena(const char *texto) {
    if (!reservar((void **)&cadenas, &capacidad_cadenas,
                  cantidad_cadenas + 1, sizeof(*cadenas))) return -1;
    snprintf(cadenas[cantidad_cadenas].nombre,
             sizeof(cadenas[cantidad_cadenas].nombre), "%s", texto);
    return (int)cantidad_cadenas++;
}

static int agregar_real(const char *texto) {
    for (size_t i = 0; i < cantidad_constantes_reales; i++)
        if (strcmp(constantes_reales[i].lexema, texto) == 0)
            return (int)i;
    if (!reservar((void **)&constantes_reales, &capacidad_constantes_reales,
                  cantidad_constantes_reales + 1,
                  sizeof(*constantes_reales))) return -1;
    ConstanteReal *constante = &constantes_reales[cantidad_constantes_reales];
    snprintf(constante->lexema, sizeof(constante->lexema), "%s", texto);
    return (int)cantidad_constantes_reales++;
}

static int preprocesar(void) {
    int ambito = -1;
    for (size_t i = 0; i < cantidad_celdas; i++) {
        Celda *celda = &celdas[i];
        if (strncmp(celda->elemento, "FUNC ", 5) == 0) {
            ambito = agregar_funcion(celda->elemento + 5, celda->tipo);
            if (ambito < 0) return 0;
            continue;
        }
        if (strcmp(celda->elemento, "INICIO_PROG") == 0) {
            ambito = -1;
            continue;
        }
        int real = 0;
        if (es_numero(celda->elemento, &real)) {
            if (real && agregar_real(celda->elemento) < 0) return 0;
            continue;
        }
        if (strcmp(celda->elemento, "PARAM") == 0 ||
            es_operador_o_instruccion(celda->elemento)) continue;
        if (i + 1 < cantidad_celdas &&
            strncmp(celdas[i + 1].elemento, "CALL ", 5) == 0) continue;
        if (i + 1 < cantidad_celdas &&
            strcmp(celdas[i + 1].elemento, "IMP_CADENA") == 0) {
            celda->cadena = agregar_cadena(celda->elemento);
            if (celda->cadena < 0) return 0;
            continue;
        }
        int variable = agregar_variable(ambito, celda->elemento, celda->tipo);
        if (variable < 0) return 0;
        if (i + 1 < cantidad_celdas &&
            strcmp(celdas[i + 1].elemento, "PARAM") == 0) {
            if (ambito < 0 || cantidad_funciones <= (size_t)ambito ||
                funciones[ambito].cantidad_parametros >= 2) {
                fprintf(stderr, "Definicion de parametros invalida en polaca\n");
                return 0;
            }
            funciones[ambito].parametros[funciones[ambito].cantidad_parametros++] =
                variable;
        }
    }
    return 1;
}

static const char *etiqueta_variable(int indice) {
    static char etiquetas[8][128];
    static unsigned siguiente;
    char *etiqueta = etiquetas[siguiente++ % 8];
    if (indice < 0 || (size_t)indice >= cantidad_variables) return "_v_invalida";
    const Variable *variable = &variables[indice];
    if (variable->ambito < 0)
        snprintf(etiqueta, 128, "_v_main_%.*s", 20, variable->nombre);
    else
        snprintf(etiqueta, 128, "_v_%.*s_%.*s",
                 20, funciones[variable->ambito].nombre,
                 20, variable->nombre);
    return etiqueta;
}

static const char *etiqueta_funcion(int indice) {
    static char etiqueta[128];
    if (indice < 0 || (size_t)indice >= cantidad_funciones) return "_fn_invalida";
    snprintf(etiqueta, sizeof(etiqueta), "_fn_%s", funciones[indice].nombre);
    return etiqueta;
}

static int apilar(int tipo, int variable, int cadena) {
    if (!reservar((void **)&pila, &capacidad_pila, cantidad_pila + 1,
                  sizeof(*pila))) {
        error_memoria = 1;
        return 0;
    }
    pila[cantidad_pila].tipo = tipo;
    pila[cantidad_pila].variable = variable;
    pila[cantidad_pila].cadena = cadena;
    cantidad_pila++;
    return 1;
}

static ValorPila desapilar(void) {
    ValorPila vacio = { TIPO_SIN_TIPO, -1, -1 };
    if (cantidad_pila == 0) return vacio;
    return pila[--cantidad_pila];
}

static void volcar_instruccion(FILE *asm_file, FILE *traza, size_t paso,
                               const char *elemento, const char *texto) {
    if (texto != NULL && texto[0] != '\0') fprintf(asm_file, "%s\n", texto);
    fprintf(traza, "%zu | %-28s | %s\n", paso, elemento,
            texto != NULL ? texto : "sin instruccion");
}

static void emitir_push_entero(FILE *asm_file, const char *origen) {
    fprintf(asm_file, "    push dword %s\n", origen);
}

static void emitir_sacar_real(FILE *asm_file, int tipo, const char *destino) {
    if (tipo == TIPO_REAL) {
        fprintf(asm_file, "    fld qword [esp]\n"
                          "    add esp, 8\n"
                          "    fstp qword [%s]\n", destino);
    } else {
        fprintf(asm_file, "    pop dword [_tmp_entero]\n"
                          "    fild dword [_tmp_entero]\n"
                          "    fstp qword [%s]\n", destino);
    }
}

static void emitir_sacar_entero(FILE *asm_file, int tipo, const char *destino) {
    if (tipo == TIPO_REAL) {
        fprintf(asm_file, "    fld qword [esp]\n"
                          "    add esp, 8\n"
                          "    fldcw [_cw_truncar]\n"
                          "    fistp dword [%s]\n"
                          "    fldcw [_cw_normal]\n", destino);
    } else {
        fprintf(asm_file, "    pop dword [%s]\n", destino);
    }
}

static void emitir_cargar_variable(FILE *asm_file, const Celda *celda,
                                   int ambito, FILE *traza) {
    int variable = buscar_variable(ambito, celda->elemento);
    char instruccion[256];
    if (variable < 0) {
        fprintf(stderr, "Variable '%s' no registrada en polaca\n",
                celda->elemento);
        instruccion[0] = '\0';
    } else if (celda->tipo == TIPO_REAL) {
        snprintf(instruccion, sizeof(instruccion), "    sub esp, 8\n"
                 "    fld qword [%s]\n    fstp qword [esp]",
                 etiqueta_variable(variable));
        fprintf(asm_file, "%s\n", instruccion);
        fprintf(traza, "%zu | %-28s | PUSH real %s\n", celda->paso,
                celda->elemento, etiqueta_variable(variable));
        apilar(TIPO_REAL, variable, -1);
        return;
    } else {
        snprintf(instruccion, sizeof(instruccion), "    push dword [%s]",
                 etiqueta_variable(variable));
    }
    if (instruccion[0] != '\0') {
        fprintf(asm_file, "%s\n", instruccion);
        fprintf(traza, "%zu | %-28s | PUSH %s\n", celda->paso,
                celda->elemento, instruccion);
        apilar(celda->tipo == TIPO_SIN_TIPO ? TIPO_ENTERO : celda->tipo,
               variable, -1);
    }
}

static int emitir_llamada(FILE *asm_file, FILE *traza, Celda *celda) {
    char *fin;
    long cantidad = strtol(celda->elemento + 5, &fin, 10);
    if (fin == celda->elemento + 5 || *fin != '\0' ||
        cantidad < 1 || cantidad > 2 || cantidad_pila < (size_t)cantidad) {
        fprintf(stderr, "Llamada invalida en polaca, paso %zu\n", celda->paso);
        return 0;
    }
    if (celda->paso == 0 ||
        cantidad_celdas <= celda->paso - 1) return 0;
    Celda *celda_funcion = &celdas[celda->paso - 1];
    int funcion_indice = buscar_funcion(celda_funcion->elemento);
    if (funcion_indice < 0 ||
        funciones[funcion_indice].cantidad_parametros != (size_t)cantidad) {
        fprintf(stderr, "Funcion '%s' no definida o con parametros incompatibles\n",
                celda_funcion->elemento);
        return 0;
    }
    Funcion *funcion = &funciones[funcion_indice];
    ValorPila argumentos[2];
    for (int i = (int)cantidad - 1; i >= 0; i--) {
        argumentos[i] = desapilar();
        char temporal[32];
        snprintf(temporal, sizeof(temporal), "_tmp_arg%d", i);
        int tipo_destino = variables[funcion->parametros[i]].tipo;
        if (tipo_destino == TIPO_REAL)
            emitir_sacar_real(asm_file, argumentos[i].tipo, temporal);
        else
            emitir_sacar_entero(asm_file, argumentos[i].tipo, temporal);
    }
    for (int i = (int)cantidad - 1; i >= 0; i--) {
        char temporal[32];
        snprintf(temporal, sizeof(temporal), "_tmp_arg%d", i);
        if (variables[funcion->parametros[i]].tipo == TIPO_REAL) {
            fprintf(asm_file, "    push dword [%s + 4]\n"
                              "    push dword [%s]\n", temporal, temporal);
        } else {
            fprintf(asm_file, "    push dword [%s]\n", temporal);
        }
    }
    fprintf(asm_file, "    call %s\n", etiqueta_funcion(funcion_indice));
    size_t argumentos_bytes = 0;
    for (int i = 0; i < cantidad; i++)
        argumentos_bytes += variables[funcion->parametros[i]].tipo ==
                                    TIPO_REAL ? 8u : 4u;
    fprintf(asm_file, "    add esp, %zu\n", argumentos_bytes);
    if (funcion->tipo == TIPO_REAL)
        fprintf(asm_file, "    push edx\n    push eax\n");
    else
        fprintf(asm_file, "    push eax\n");
    fprintf(traza, "%zu | %-28s | CALL %s\n", celda->paso,
            celda->elemento, etiqueta_funcion(funcion_indice));
    return apilar(funcion->tipo, -1, -1);
}

static void emitir_conversion_a_real(FILE *asm_file, ValorPila valor,
                                     const char *temporal) {
    emitir_sacar_real(asm_file, valor.tipo, temporal);
}

static int emitir_operador(FILE *asm_file, FILE *traza, Celda *celda) {
    if (cantidad_pila < 2) {
        fprintf(stderr, "Pila de expresion invalida en paso %zu\n", celda->paso);
        return 0;
    }
    ValorPila derecho = desapilar();
    ValorPila izquierdo = desapilar();
    const char *op = celda->elemento;
    int es_actualizacion_para =
        strcmp(op, "-") == 0 &&
        celda->paso + 1 < cantidad_celdas &&
        strcmp(celdas[celda->paso + 1].elemento, "=") == 0 &&
        cantidad_pila == 0 && izquierdo.variable >= 0;
    if (strcmp(op, "&") == 0 || strcmp(op, "|") == 0) {
        fprintf(asm_file, "    pop ebx\n    pop eax\n    %s eax, ebx\n"
                          "    test eax, eax\n    setne al\n    movzx eax, al\n"
                          "    push eax\n",
                strcmp(op, "&") == 0 ? "and" : "or");
        apilar(TIPO_ENTERO, -1, -1);
    } else if (strcmp(op, "==") == 0 || strcmp(op, "!=") == 0 ||
               strcmp(op, "<") == 0 || strcmp(op, ">") == 0 ||
               strcmp(op, "<=") == 0 || strcmp(op, ">=") == 0) {
        int comparacion_real =
            izquierdo.tipo == TIPO_REAL || derecho.tipo == TIPO_REAL;
        const char *cc;
        if (strcmp(op, "==") == 0) cc = "e";
        else if (strcmp(op, "!=") == 0) cc = "ne";
        else if (comparacion_real && strcmp(op, "<") == 0) cc = "b";
        else if (comparacion_real && strcmp(op, ">") == 0) cc = "a";
        else if (comparacion_real && strcmp(op, "<=") == 0) cc = "be";
        else if (comparacion_real && strcmp(op, ">=") == 0) cc = "ae";
        else if (strcmp(op, "<") == 0) cc = "l";
        else if (strcmp(op, ">") == 0) cc = "g";
        else if (strcmp(op, "<=") == 0) cc = "le";
        else cc = "ge";
        if (comparacion_real) {
            emitir_conversion_a_real(asm_file, derecho, "_tmp_real_derecho");
            emitir_conversion_a_real(asm_file, izquierdo, "_tmp_real_izquierdo");
            fprintf(asm_file, "    fld qword [_tmp_real_izquierdo]\n"
                              "    fcomp qword [_tmp_real_derecho]\n"
                              "    fnstsw ax\n    sahf\n    set%s al\n",
                    cc);
            if (strcmp(op, "!=") == 0)
                fprintf(asm_file, "    setp dl\n    or al, dl\n");
            else
                fprintf(asm_file, "    setnp dl\n    and al, dl\n");
            fprintf(asm_file, "    movzx eax, al\n    push eax\n");
        } else {
            fprintf(asm_file, "    pop ebx\n    pop eax\n"
                              "    cmp eax, ebx\n    set%s al\n"
                              "    movzx eax, al\n    push eax\n", cc);
        }
        apilar(TIPO_ENTERO, -1, -1);
    } else if (celda->tipo == TIPO_REAL) {
        emitir_conversion_a_real(asm_file, derecho, "_tmp_real_derecho");
        emitir_conversion_a_real(asm_file, izquierdo, "_tmp_real_izquierdo");
        fprintf(asm_file, "    fld qword [_tmp_real_izquierdo]\n");
        if (strcmp(op, "+") == 0)
            fprintf(asm_file, "    fadd qword [_tmp_real_derecho]\n");
        else if (strcmp(op, "-") == 0)
            fprintf(asm_file, "    fsub qword [_tmp_real_derecho]\n");
        else if (strcmp(op, "*") == 0)
            fprintf(asm_file, "    fmul qword [_tmp_real_derecho]\n");
        else if (strcmp(op, "/") == 0)
            fprintf(asm_file, "    fdiv qword [_tmp_real_derecho]\n");
        else return 0;
        fprintf(asm_file, "    sub esp, 8\n    fstp qword [esp]\n");
        apilar(TIPO_REAL, -1, -1);
    } else {
        fprintf(asm_file, "    pop ebx\n    pop eax\n");
        if (strcmp(op, "+") == 0) fprintf(asm_file, "    add eax, ebx\n");
        else if (strcmp(op, "-") == 0) fprintf(asm_file, "    sub eax, ebx\n");
        else if (strcmp(op, "*") == 0) fprintf(asm_file, "    imul eax, ebx\n");
        else if (strcmp(op, "/") == 0)
            fprintf(asm_file, "    cdq\n    idiv ebx\n");
        else return 0;
        if (es_actualizacion_para) {
            fprintf(asm_file, "    push dword 0\n    push eax\n");
            apilar(izquierdo.tipo, izquierdo.variable, -1);
            apilar(TIPO_ENTERO, -1, -1);
        } else {
            fprintf(asm_file, "    push eax\n");
            apilar(TIPO_ENTERO, -1, -1);
        }
    }
    fprintf(traza, "%zu | %-28s | operador %s\n",
            celda->paso, celda->elemento, celda->elemento);
    return 1;
}

static int emitir_asignacion(FILE *asm_file, FILE *traza, Celda *celda) {
    if (cantidad_pila < 2) {
        fprintf(stderr, "Asignacion invalida en paso %zu\n", celda->paso);
        return 0;
    }
    ValorPila expresion = desapilar();
    ValorPila destino = desapilar();
    if (destino.variable < 0) {
        fprintf(stderr, "Destino de asignacion invalido en paso %zu\n",
                celda->paso);
        return 0;
    }
    const char *etiqueta = etiqueta_variable(destino.variable);
    if (variables[destino.variable].tipo == TIPO_REAL)
        emitir_sacar_real(asm_file, expresion.tipo, etiqueta);
    else
        emitir_sacar_entero(asm_file, expresion.tipo, etiqueta);
    fprintf(asm_file, "    add esp, %d\n",
            variables[destino.variable].tipo == TIPO_REAL ? 8 : 4);
    fprintf(traza, "%zu | %-28s | asignar a %s\n",
            celda->paso, celda->elemento, etiqueta);
    return 1;
}

static int ejecutar_codigo(const char *archivo_assembler) {
    char archivo_objeto[1024];
    char archivo_ejecutable[1024];
    char comando[3072];
    char *extension;

    if (strlen(archivo_assembler) >= sizeof(archivo_objeto) ||
        strlen(archivo_assembler) >= sizeof(archivo_ejecutable))
        return 0;
    snprintf(archivo_objeto, sizeof(archivo_objeto), "%s", archivo_assembler);
    extension = strrchr(archivo_objeto, '.');
    if (extension == NULL) return 0;
    snprintf(extension, (size_t)(archivo_objeto + sizeof(archivo_objeto) -
                                 extension), ".obj");
    snprintf(archivo_ejecutable, sizeof(archivo_ejecutable), "%s",
             archivo_assembler);
    extension = strrchr(archivo_ejecutable, '.');
    if (extension == NULL) return 0;
    snprintf(extension, (size_t)(archivo_ejecutable + sizeof(archivo_ejecutable) -
                                 extension), ".exe");

#ifdef _WIN32
    snprintf(comando, sizeof(comando),
             "nasm -f win32 \"%s\" -o \"%s\" && "
             "gcc -m32 \"%s\" -o \"%s\" && \"%s\"",
             archivo_assembler, archivo_objeto, archivo_objeto,
             archivo_ejecutable, archivo_ejecutable);
#else
    snprintf(comando, sizeof(comando),
             "nasm -f elf32 \"%s\" -o \"%s\" && "
             "gcc -m32 \"%s\" -o \"%s\" && \"%s\"",
             archivo_assembler, archivo_objeto, archivo_objeto,
             archivo_ejecutable, archivo_ejecutable);
#endif
    if (system(comando) != 0) {
        fprintf(stderr, "No se pudo ensamblar, enlazar o ejecutar el programa generado\n");
        return 0;
    }
    return 1;
}

static int emitir_impresion(FILE *asm_file, FILE *traza, Celda *celda) {
    if (strcmp(celda->elemento, "IMP_CADENA") == 0) {
        if (celda->paso == 0 || celdas[celda->paso - 1].cadena < 0) {
            fprintf(stderr, "Literal de cadena invalido en paso %zu\n", celda->paso);
            return 0;
        }
        fprintf(asm_file, "    push dword _cadena_%d\n"
                          "    push dword _format_str\n"
                          "    call _printf\n    add esp, 8\n",
                celdas[celda->paso - 1].cadena);
    } else {
        if (cantidad_pila == 0) {
            fprintf(stderr, "Impresion sin operando en paso %zu\n", celda->paso);
            return 0;
        }
        ValorPila valor = desapilar();
        if (strcmp(celda->elemento, "IMP_REAL") == 0) {
            emitir_sacar_real(asm_file, valor.tipo, "_tmp_real_resultado");
            fprintf(asm_file, "    push dword [_tmp_real_resultado + 4]\n"
                              "    push dword [_tmp_real_resultado]\n"
                              "    push dword _format_real\n"
                              "    call _printf\n    add esp, 12\n");
        } else {
            emitir_sacar_entero(asm_file, valor.tipo, "_tmp_entero");
            fprintf(asm_file, "    push dword [_tmp_entero]\n"
                              "    push dword _format_int\n"
                              "    call _printf\n    add esp, 8\n");
        }
    }
    fprintf(traza, "%zu | %-28s | imprimir\n", celda->paso, celda->elemento);
    return 1;
}

static int emitir_retorno(FILE *asm_file, FILE *traza, Celda *celda,
                          int ambito_funcion) {
    if (celda->tipo == TIPO_SIN_TIPO) {
        fprintf(asm_file, "    xor eax, eax\n    mov ebx, [ebp - 4]\n"
                          "    leave\n    ret\n");
    } else {
        if (cantidad_pila == 0 || ambito_funcion < 0) {
            fprintf(stderr, "Retorno invalido en paso %zu\n", celda->paso);
            return 0;
        }
        ValorPila valor = desapilar();
        if (celda->tipo == TIPO_REAL) {
            emitir_sacar_real(asm_file, valor.tipo, "_tmp_real_resultado");
            fprintf(asm_file, "    mov eax, [_tmp_real_resultado]\n"
                              "    mov edx, [_tmp_real_resultado + 4]\n"
                              "    mov ebx, [ebp - 4]\n"
                              "    leave\n    ret\n");
        } else {
            emitir_sacar_entero(asm_file, valor.tipo, "_tmp_entero");
            fprintf(asm_file, "    mov eax, [_tmp_entero]\n"
                              "    mov ebx, [ebp - 4]\n"
                              "    leave\n    ret\n");
        }
    }
    fprintf(traza, "%zu | %-28s | retornar\n", celda->paso, celda->elemento);
    return 1;
}

static void escribir_datos(FILE *asm_file) {
    fprintf(asm_file,
            "bits 32\n"
            "extern _printf\n"
            "global _main\n\n"
            "section .data\n"
            "_format_int db 37, 100, 10, 0\n"
            "_format_real db 37, 102, 10, 0\n"
            "_format_str db 37, 115, 10, 0\n"
            "_cw_normal dw 0x037f\n"
            "_cw_truncar dw 0x0f7f\n"
            "_tmp_entero dd 0\n"
            "_tmp_real_izquierdo dq 0.0\n"
            "_tmp_real_derecho dq 0.0\n"
            "_tmp_real_resultado dq 0.0\n"
            "_tmp_arg0 dq 0.0\n"
            "_tmp_arg1 dq 0.0\n");
    for (size_t i = 0; i < cantidad_variables; i++) {
        fprintf(asm_file, "%s %s 0\n",
                etiqueta_variable((int)i),
                variables[i].tipo == TIPO_REAL ? "dq" : "dd");
    }
    for (size_t i = 0; i < cantidad_constantes_reales; i++)
        fprintf(asm_file, "_cte_real_%zu dq __float64__(%s)\n", i,
                constantes_reales[i].lexema);
    for (size_t i = 0; i < cantidad_cadenas; i++) {
        fprintf(asm_file, "_cadena_%zu db ", i);
        const unsigned char *p =
            (const unsigned char *)cadenas[i].nombre;
        if (*p == '\0') fprintf(asm_file, "0");
        for (size_t j = 0; p[j] != '\0'; j++)
            fprintf(asm_file, "%s%u", j == 0 ? "" : ", ",
                    (unsigned int)p[j]);
        fprintf(asm_file, ", 0\n");
    }
    fprintf(asm_file, "\nsection .text\n");
}

static int generar_codigo(FILE *asm_file, FILE *traza) {
    int ambito = -1;
    escribir_datos(asm_file);
    for (size_t i = 0; i < cantidad_celdas; i++) {
        Celda *celda = &celdas[i];
        fprintf(asm_file, "_paso_%zu:\n", celda->paso);
        if (strncmp(celda->elemento, "FUNC ", 5) == 0) {
            ambito = buscar_funcion(celda->elemento + 5);
            if (ambito < 0) return 0;
            fprintf(asm_file, "%s:\n    push ebp\n    mov ebp, esp\n"
                              "    push ebx\n",
                    etiqueta_funcion(ambito));
            int offset = 8;
            for (size_t p = 0; p < funciones[ambito].cantidad_parametros; p++) {
                int variable = funciones[ambito].parametros[p];
                const char *etiqueta = etiqueta_variable(variable);
                if (variables[variable].tipo == TIPO_REAL) {
                    fprintf(asm_file, "    mov eax, [ebp + %d]\n"
                                      "    mov [%s], eax\n"
                                      "    mov eax, [ebp + %d]\n"
                                      "    mov [%s + 4], eax\n",
                            offset, etiqueta, offset + 4, etiqueta);
                    offset += 8;
                } else {
                    fprintf(asm_file, "    mov eax, [ebp + %d]\n"
                                      "    mov [%s], eax\n", offset, etiqueta);
                    offset += 4;
                }
            }
            fprintf(traza, "%zu | %-28s | definir %s\n",
                    celda->paso, celda->elemento, etiqueta_funcion(ambito));
        } else if (strcmp(celda->elemento, "INICIO_PROG") == 0) {
            ambito = -1;
            fprintf(asm_file, "_main:\n    push ebp\n    mov ebp, esp\n"
                              "    push ebx\n");
            fprintf(traza, "%zu | %-28s | iniciar main\n",
                    celda->paso, celda->elemento);
        } else if (strcmp(celda->elemento, "FIN_PROG") == 0) {
            fprintf(asm_file, "    xor eax, eax\n    mov ebx, [ebp - 4]\n"
                              "    leave\n    ret\n");
            fprintf(traza, "%zu | %-28s | finalizar main\n",
                    celda->paso, celda->elemento);
        } else if (strncmp(celda->elemento, "BF ", 3) == 0 ||
                   strncmp(celda->elemento, "BI ", 3) == 0) {
            char *fin;
            long destino = strtol(celda->elemento + 3, &fin, 10);
            if (fin == celda->elemento + 3 || *fin != '\0' ||
                destino < 0 || (size_t)destino > cantidad_celdas) {
                fprintf(stderr, "Destino de salto invalido en paso %zu\n",
                        celda->paso);
                return 0;
            }
            if (celda->elemento[1] == 'F') {
                if (cantidad_pila == 0) {
                    fprintf(stderr, "Salto BF sin condicion en paso %zu\n",
                            celda->paso);
                    return 0;
                }
                (void)desapilar();
                fprintf(asm_file, "    pop eax\n    test eax, eax\n"
                                  "    jz _paso_%ld\n", destino);
            } else {
                fprintf(asm_file, "    jmp _paso_%ld\n", destino);
            }
            fprintf(traza, "%zu | %-28s | salto a _paso_%ld\n",
                    celda->paso, celda->elemento, destino);
        } else if (strncmp(celda->elemento, "CALL ", 5) == 0) {
            if (!emitir_llamada(asm_file, traza, celda)) return 0;
        } else if (strcmp(celda->elemento, "PARAM") == 0) {
            volcar_instruccion(asm_file, traza, celda->paso,
                               celda->elemento, NULL);
        } else if (strcmp(celda->elemento, "RET") == 0) {
            if (!emitir_retorno(asm_file, traza, celda, ambito)) return 0;
        } else if (strcmp(celda->elemento, "=") == 0) {
            if (!emitir_asignacion(asm_file, traza, celda)) return 0;
        } else if (strcmp(celda->elemento, "IMP_ENTERO") == 0 ||
                   strcmp(celda->elemento, "IMP_REAL") == 0 ||
                   strcmp(celda->elemento, "IMP_CADENA") == 0) {
            if (!emitir_impresion(asm_file, traza, celda)) return 0;
        } else if (strcmp(celda->elemento, "+") == 0 ||
                   strcmp(celda->elemento, "-") == 0 ||
                   strcmp(celda->elemento, "*") == 0 ||
                   strcmp(celda->elemento, "/") == 0 ||
                   strcmp(celda->elemento, "==") == 0 ||
                   strcmp(celda->elemento, "!=") == 0 ||
                   strcmp(celda->elemento, "<") == 0 ||
                   strcmp(celda->elemento, ">") == 0 ||
                   strcmp(celda->elemento, "<=") == 0 ||
                   strcmp(celda->elemento, ">=") == 0 ||
                   strcmp(celda->elemento, "&") == 0 ||
                   strcmp(celda->elemento, "|") == 0) {
            if (!emitir_operador(asm_file, traza, celda)) return 0;
        } else if (celda->cadena >= 0) {
            fprintf(traza, "%zu | %-28s | literal _cadena_%d\n",
                    celda->paso, celda->elemento, celda->cadena);
        } else {
            int real = 0;
            if (es_numero(celda->elemento, &real)) {
                if (real) {
                    int indice = agregar_real(celda->elemento);
                    if (indice < 0) return 0;
                    fprintf(asm_file, "    sub esp, 8\n"
                                      "    fld qword [_cte_real_%d]\n"
                                      "    fstp qword [esp]\n", indice);
                    apilar(TIPO_REAL, -1, -1);
                } else {
                    emitir_push_entero(asm_file, celda->elemento);
                    apilar(TIPO_ENTERO, -1, -1);
                }
                fprintf(traza, "%zu | %-28s | PUSH constante\n",
                        celda->paso, celda->elemento);
            } else if (i + 1 < cantidad_celdas &&
                       (strcmp(celdas[i + 1].elemento, "PARAM") == 0 ||
                        strncmp(celdas[i + 1].elemento, "CALL ", 5) == 0)) {
                volcar_instruccion(asm_file, traza, celda->paso,
                                   celda->elemento, NULL);
            } else {
                emitir_cargar_variable(asm_file, celda, ambito, traza);
                if (buscar_variable(ambito, celda->elemento) < 0) return 0;
            }
        }
        if (ferror(asm_file) || ferror(traza)) {
            fprintf(stderr, "Error al escribir la salida assembler\n");
            return 0;
        }
    }
    fprintf(asm_file, "_paso_%zu:\n", cantidad_celdas);
    if (error_memoria) {
        fprintf(stderr, "No se pudo reservar memoria para la pila de valores\n");
        return 0;
    }
    if (cantidad_pila != 0) {
        fprintf(stderr, "La polaca dejo valores pendientes en la pila\n");
        return 0;
    }
    return 1;
}

static void liberar_estado(void) {
    free(celdas);
    free(variables);
    free(funciones);
    free(cadenas);
    free(constantes_reales);
    free(pila);
    celdas = NULL;
    variables = NULL;
    funciones = NULL;
    cadenas = NULL;
    constantes_reales = NULL;
    pila = NULL;
    cantidad_celdas = capacidad_celdas = 0;
    cantidad_variables = capacidad_variables = 0;
    cantidad_funciones = capacidad_funciones = 0;
    cantidad_cadenas = capacidad_cadenas = 0;
    cantidad_constantes_reales = capacidad_constantes_reales = 0;
    cantidad_pila = capacidad_pila = 0;
    error_memoria = 0;
}

int assembler_generar(const char *archivo_polaca,
                      const char *archivo_assembler,
                      const char *archivo_traza) {
    liberar_estado();
    if (!leer_polaca(archivo_polaca) || !preprocesar()) {
        fprintf(stderr, "No se pudo preparar la polaca para traducir\n");
        liberar_estado();
        return 1;
    }
    FILE *asm_file = fopen(archivo_assembler, "w");
    if (asm_file == NULL) {
        fprintf(stderr, "No se pudo crear el assembler: %s\n", archivo_assembler);
        liberar_estado();
        return 1;
    }
    FILE *traza = fopen(archivo_traza, "w");
    if (traza == NULL) {
        fprintf(stderr, "No se pudo crear la traza assembler: %s\n", archivo_traza);
        fclose(asm_file);
        liberar_estado();
        return 1;
    }
    fprintf(traza, "Traza de traduccion NASM x86 (32 bits)\n");
    int correcto = generar_codigo(asm_file, traza);
    int error_cierre_asm = fclose(asm_file) != 0;
    int error_cierre_traza = fclose(traza) != 0;
    if (error_cierre_asm || error_cierre_traza) {
        fprintf(stderr, "No se pudieron cerrar los archivos assembler/traza\n");
        correcto = 0;
    }
    if (correcto && !ejecutar_codigo(archivo_assembler))
        correcto = 0;
    liberar_estado();
    return correcto ? 0 : 1;
}
