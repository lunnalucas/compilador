#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include "tokens.h"
#include "lexer.h"
#include "lexer_tables.h"

/* ============ estado persistente ============ */
char yy_lexema[8192];
int yy_linea = 1, yy_col_ini = 1, yy_col_fin = 1;
long yy_ival = 0;
double yy_fval = 0.0;
int lex_error_code = 0;
char lex_error_msg[256] = {0};
int lex_warning_code = 0;
int lex_warning_line = 0, lex_warning_col = 0;
char lex_warning_msg[256] = {0};
char lex_warning_lexema[21] = {0};

static FILE *lex_fp = NULL;
static int cur_line = 1, cur_col = 1;     /* posición del próximo char */
static int last_line = 1, last_col = 1;   /* posición del último char leído */
static int pb_valid = 0, pb_c = 0;

static char lex_buf[8192];
static int lex_len = 0;
static int id_excess = 0;
static int num_overflow = 0;
static int real_has_frac = 0;
static int tok_line_ini = 1, tok_col_ini = 1;
static int op_start_line = 1, op_start_col = 1;
static int tok_pending = -1;
static int skip_auto_unread = 0;
static int cur_estado_global = 0;

/* TS mínima: identificadores únicos (truncados) vistos */
#define TS_MAX 2048
static char ts_ids[TS_MAX][21];
static int ts_n = 0;

typedef struct {
    int codigo;
    int linea;
    int columna;
    int es_advertencia;
    char mensaje[256];
    char lexema[8192];
} DiagnosticoLexico;

static DiagnosticoLexico *diagnosticos = NULL;
static size_t cantidad_diagnosticos = 0;
static size_t capacidad_diagnosticos = 0;
static int error_memoria_diagnosticos = 0;

/* Conserva una copia del diagnóstico para informarlo al final en orden. */
static void registrar_diagnostico(int codigo, int linea, int columna,
                                  int es_advertencia, const char *mensaje,
                                  const char *lexema) {
    if (error_memoria_diagnosticos) return;

    if (cantidad_diagnosticos == capacidad_diagnosticos) {
        size_t nueva_capacidad = capacidad_diagnosticos == 0
                                     ? 8
                                     : capacidad_diagnosticos * 2;
        if (nueva_capacidad < capacidad_diagnosticos ||
            nueva_capacidad > (size_t)-1 / sizeof(*diagnosticos)) {
            error_memoria_diagnosticos = 1;
            return;
        }

        DiagnosticoLexico *nuevo =
            realloc(diagnosticos, nueva_capacidad * sizeof(*diagnosticos));
        if (nuevo == NULL) {
            error_memoria_diagnosticos = 1;
            return;
        }
        diagnosticos = nuevo;
        capacidad_diagnosticos = nueva_capacidad;
    }

    DiagnosticoLexico *diagnostico = &diagnosticos[cantidad_diagnosticos++];
    diagnostico->codigo = codigo;
    diagnostico->linea = linea;
    diagnostico->columna = columna;
    diagnostico->es_advertencia = es_advertencia;
    snprintf(diagnostico->mensaje, sizeof(diagnostico->mensaje), "%s", mensaje);
    snprintf(diagnostico->lexema, sizeof(diagnostico->lexema), "%s", lexema);
}

/* Añade un carácter si hay espacio y mantiene el buffer terminado en NUL. */
static void anexar_fragmento(char *fragmento, size_t capacidad, size_t *longitud,
                             char c) {
    if (*longitud + 1 < capacidad) {
        fragmento[(*longitud)++] = c;
        fragmento[*longitud] = '\0';
    }
}

/* Registra el error actual exactamente una vez antes de devolverlo a main. */
static int devolver_error_lexico(void) {
    registrar_diagnostico(lex_error_code, yy_linea, yy_col_ini, 0,
                          lex_error_msg, yy_lexema);
    return ERROR_LEXICO;
}

/* ============ utilidades de lectura ============ */
/* Lee un carácter y actualiza la posición de línea y columna. */
static int leer_caracter(void) {
    int c;
    if (pb_valid) { c = pb_c; pb_valid = 0; }
    else c = lex_fp ? fgetc(lex_fp) : EOF;
    if (c == EOF) return EOF;
    last_line = cur_line; last_col = cur_col;
    if (c == '\n') { cur_line++; cur_col = 1; }
    else if (c == '\r') { cur_line++; cur_col = 1; } /* \r solo o parte de \r\n en modo texto */
    else cur_col++;
    return c;
}

/* Devuelve un carácter al flujo y revierte su avance de posición. */
static void unread_char(int c) {
    if (c == EOF) return;
    pb_c = c; pb_valid = 1;
    cur_line = last_line; cur_col = last_col; /* sin duplicar avance */
}

/* Inicializa el flujo y limpia los diagnósticos de la siguiente pasada. */
void lexer_init(FILE *f) {
    free(diagnosticos);
    diagnosticos = NULL;
    cantidad_diagnosticos = 0;
    capacidad_diagnosticos = 0;
    error_memoria_diagnosticos = 0;
    lex_fp = f;
    cur_line = 1; cur_col = 1; last_line = 1; last_col = 1;
    pb_valid = 0; lex_len = 0; tok_pending = -1;
    lex_error_code = 0; lex_error_msg[0] = '\0';
    lex_warning_code = 0; lex_warning_msg[0] = '\0';
    lex_warning_line = 0; lex_warning_col = 0;
    lex_warning_lexema[0] = '\0';
}

/* Devuelve el nombre legible asociado al código público de un token. */
const char *token_nombre(int tok) {
    switch (tok) {
        case 100: return "ID";
        case 101: return "CTE_E";
        case 102: return "CTE_R";
        case 103: return "INICIO";
        case 104: return "ENTERO";
        case 105: return "REAL";
        case 106: return "SI";
        case 107: return "SINO";
        case 108: return "PARA";
        case 109: return "IMPRIMIR";
        case 110: return "RETORNAR";
        case 111: return "ASIG";
        case 112: return "IGUAL";
        case 113: return "DISTINTO";
        case 114: return "MENOR";
        case 115: return "MAYOR";
        case 116: return "MENOR_IGUAL";
        case 117: return "MAYOR_IGUAL";
        case 118: return "AND";
        case 119: return "OR";
        case 120: return "SUMA";
        case 121: return "RESTA";
        case 122: return "PRODUCTO";
        case 123: return "DIVISION";
        case 124: return "PAR_ABRE";
        case 125: return "PAR_CIERRA";
        case 126: return "LLAVE_ABRE";
        case 127: return "LLAVE_CIERRA";
        case 128: return "P_COMA";
        case 129: return "COMA";
        case 130: return "PUNTO";
        case 131: return "COMILLA";
        case 132: return "CADENA";
        default: return "DESCONOCIDO";
    }
}

/* Obtiene la representación textual de operadores y delimitadores. */
static const char *lexema_fijo(int tok) {
    switch (tok) {
        case 111: return "=";
        case 112: return "==";
        case 113: return "!=";
        case 114: return "<";
        case 115: return ">";
        case 116: return "<=";
        case 117: return ">=";
        case 118: return "&";
        case 119: return "|";
        case 120: return "+";
        case 121: return "-";
        case 122: return "*";
        case 123: return "/";
        case 124: return "(";
        case 125: return ")";
        case 126: return "{";
        case 127: return "}";
        case 128: return ";";
        case 129: return ",";
        case 130: return ".";
        case 131: return "\"";
        default: return NULL;
    }
}

/* ============ acciones semánticas (void) ============ */
/* Acciones semánticas sin efecto para transiciones estructurales. */
void hacer_nada(int c) { (void)c; }

/* Descarta el contenido de un comentario; la lectura ya mantiene ubicación. */
void ignorar_comentario(int c) { (void)c; }

/* Comienza a recolectar un identificador y registra su posición inicial. */
void iniciar_id(int c) {
    lex_len = 0; lex_buf[lex_len++] = (char)c;
    tok_line_ini = last_line; tok_col_ini = last_col;
    id_excess = 0;
}
/* Conserva hasta 20 caracteres e indica si el nombre excede el límite. */
void agregar_id(int c) {
    if (lex_len < 20) lex_buf[lex_len++] = (char)c;
    else id_excess = 1;
}
/* Agrega una sola vez cada identificador truncado a la tabla mínima de fase. */
static void ts_agregar(const char *s) {
    for (int i = 0; i < ts_n; i++) if (strcmp(ts_ids[i], s) == 0) return;
    if (ts_n < TS_MAX) { strncpy(ts_ids[ts_n], s, 20); ts_ids[ts_n][20] = '\0'; ts_n++; }
}
/* Finaliza el identificador, resuelve palabras reservadas y registra E4. */
void fin_id(int c) {
    (void)c;
    lex_buf[lex_len] = '\0';
    if (strcmp(lex_buf, "inicio") == 0) tok_pending = INICIO;
    else if (strcmp(lex_buf, "entero") == 0) tok_pending = ENTERO;
    else if (strcmp(lex_buf, "real") == 0) tok_pending = REAL;
    else if (strcmp(lex_buf, "si") == 0) tok_pending = SI;
    else if (strcmp(lex_buf, "sino") == 0) tok_pending = SINO;
    else if (strcmp(lex_buf, "para") == 0) tok_pending = PARA;
    else if (strcmp(lex_buf, "imprimir") == 0) tok_pending = IMPRIMIR;
    else if (strcmp(lex_buf, "retornar") == 0) tok_pending = RETORNAR;
    else { tok_pending = ID; ts_agregar(lex_buf); }
    if (id_excess) {
        lex_warning_code = 4;
        lex_warning_line = tok_line_ini;
        lex_warning_col = tok_col_ini;
        snprintf(lex_warning_msg, sizeof(lex_warning_msg),
                 "Identificador truncado a 20 caracteres");
        memcpy(lex_warning_lexema, lex_buf, sizeof(lex_warning_lexema));
        lex_warning_lexema[sizeof(lex_warning_lexema) - 1] = '\0';
    }
}

/* Inicia una constante entera y registra dónde comienza. */
void iniciar_entero(int c) {
    lex_len = 0; lex_buf[lex_len++] = (char)c;
    tok_line_ini = last_line; tok_col_ini = last_col;
    num_overflow = 0;
}
/* Añade un dígito a la forma textual de la constante entera. */
void agregar_entero(int c) {
    if (lex_len < (int)sizeof(lex_buf) - 1) lex_buf[lex_len++] = (char)c;
    else num_overflow = 1;
}
/* Añade el punto decimal o un dígito de la parte fraccionaria. */
void agregar_real(int c) {
    if (c == '.') {
        if (lex_len < (int)sizeof(lex_buf) - 1) lex_buf[lex_len++] = '.';
        else num_overflow = 1;
        real_has_frac = 0;
    } else {
        if (lex_len < (int)sizeof(lex_buf) - 1) lex_buf[lex_len++] = (char)c;
        else num_overflow = 1;
        real_has_frac = 1;
    }
}
/* Valida la forma y el rango de una constante entera al cerrar el lexema. */
void fin_entero(int c) {
    /* E2: dígitos seguidos de letra sin separador, ej. 10abc */
    if (c != EOF && isalpha((unsigned char)c)) {
        size_t n;
        lex_buf[lex_len] = '\0';
        n = strlen(lex_buf);
        memcpy(yy_lexema, lex_buf, n + 1);
        anexar_fragmento(yy_lexema, sizeof(yy_lexema), &n, (char)c);
        int nc;
        while ((nc = leer_caracter()) != EOF && isalnum((unsigned char)nc)) {
            anexar_fragmento(yy_lexema, sizeof(yy_lexema), &n, (char)nc);
        }
        if (nc != EOF) unread_char(nc);
        lex_error_code = 2;
        snprintf(lex_error_msg, sizeof(lex_error_msg), "Constante numerica mal formada");
        yy_linea = tok_line_ini; yy_col_ini = tok_col_ini;
        yy_col_fin = tok_col_ini + (int)n - 1;
        skip_auto_unread = 1;
        return;
    }
    lex_buf[lex_len] = '\0';
    if (num_overflow) {
        lex_error_code = 12;
        snprintf(lex_error_msg, sizeof(lex_error_msg), "Constante numerica fuera de rango");
        strncpy(yy_lexema, lex_buf, sizeof(yy_lexema) - 1);
        yy_linea = tok_line_ini; yy_col_ini = tok_col_ini;
        yy_col_fin = tok_col_ini + (int)strlen(lex_buf) - 1;
        return;
    }
    errno = 0;
    char *end = NULL;
    long v = strtol(lex_buf, &end, 10);
    if (errno == ERANGE || v > INT_MAX || v < INT_MIN || (end && *end != '\0')) {
        lex_error_code = 12;
        snprintf(lex_error_msg, sizeof(lex_error_msg), "Constante numerica fuera de rango");
        strncpy(yy_lexema, lex_buf, sizeof(yy_lexema) - 1);
        yy_linea = tok_line_ini; yy_col_ini = tok_col_ini;
        yy_col_fin = tok_col_ini + (int)strlen(lex_buf) - 1;
        return;
    }
    yy_ival = v;
    tok_pending = CTE_E;
}
/* Valida la forma y el rango de una constante real al cerrar el lexema. */
void fin_real(int c) {
    /* E2: 12.3.4 -> segundo punto seguido de dígito */
    if (c == '.') {
        int nc = leer_caracter();
        if (nc != EOF && isdigit((unsigned char)nc)) {
            char err[8192];
            size_t n;
            lex_buf[lex_len] = '\0';
            n = strlen(lex_buf);
            memcpy(err, lex_buf, n + 1);
            anexar_fragmento(err, sizeof(err), &n, '.');
            anexar_fragmento(err, sizeof(err), &n, (char)nc);
            int mc;
            while ((mc = leer_caracter()) != EOF &&
                   (isdigit((unsigned char)mc) || mc == '.' || isalpha((unsigned char)mc))) {
                /* si aparece letra también es E2; cortar en separador real */
                if (mc == '.' || isalpha((unsigned char)mc)) {
                    /* consumir resto alfanumérico/puntos para un solo diagnóstico */
                    anexar_fragmento(err, sizeof(err), &n, (char)mc);
                    int qc;
                    while ((qc = leer_caracter()) != EOF && (isalnum((unsigned char)qc) || qc == '.')) {
                        anexar_fragmento(err, sizeof(err), &n, (char)qc);
                    }
                    if (qc != EOF) unread_char(qc);
                    break;
                }
                anexar_fragmento(err, sizeof(err), &n, (char)mc);
            }
            if (mc != EOF && !(mc == '.' || isalpha((unsigned char)mc))) unread_char(mc);
            lex_error_code = 2;
            snprintf(lex_error_msg, sizeof(lex_error_msg), "Constante numerica mal formada");
            strncpy(yy_lexema, err, sizeof(yy_lexema) - 1);
            yy_lexema[sizeof(yy_lexema) - 1] = '\0';
            yy_linea = tok_line_ini; yy_col_ini = tok_col_ini;
            yy_col_fin = tok_col_ini + (int)n - 1;
            skip_auto_unread = 1;
            return;
        }
        if (nc != EOF) unread_char(nc);
        /* si no hay dígito tras el segundo punto, cae a fin_real normal+E20/E2 según forma */
    }
    lex_buf[lex_len] = '\0';
    if (!real_has_frac) { /* ej. 12. seguido de ; -> sin decimales */
        lex_error_code = 20;
        snprintf(lex_error_msg, sizeof(lex_error_msg), "Constante real mal formada");
        strncpy(yy_lexema, lex_buf, sizeof(yy_lexema) - 1);
        yy_linea = tok_line_ini; yy_col_ini = tok_col_ini;
        yy_col_fin = tok_col_ini + (int)strlen(lex_buf) - 1;
        return;
    }
    if (num_overflow) {
        lex_error_code = 12;
        snprintf(lex_error_msg, sizeof(lex_error_msg), "Constante numerica fuera de rango");
        strncpy(yy_lexema, lex_buf, sizeof(yy_lexema) - 1);
        yy_linea = tok_line_ini; yy_col_ini = tok_col_ini;
        yy_col_fin = tok_col_ini + (int)strlen(lex_buf) - 1;
        return;
    }
    errno = 0;
    char *end = NULL;
    double v = strtod(lex_buf, &end);
    if (errno == ERANGE || (end && *end != '\0')) {
        lex_error_code = 12;
        snprintf(lex_error_msg, sizeof(lex_error_msg), "Constante numerica fuera de rango");
        strncpy(yy_lexema, lex_buf, sizeof(yy_lexema) - 1);
        yy_linea = tok_line_ini; yy_col_ini = tok_col_ini;
        yy_col_fin = tok_col_ini + (int)strlen(lex_buf) - 1;
        return;
    }
    yy_fval = v;
    tok_pending = CTE_R;
}

/* Prepara el buffer para recolectar el contenido entre comillas. */
void iniciar_salida(int c) {
    (void)c;
    lex_len = 0;
    tok_line_ini = last_line; tok_col_ini = last_col;
    num_overflow = 0;
}
/* Agrega el contenido de la cadena sin interpretar secuencias de escape. */
void agregar_salida(int c) {
    if (c == EOF) return;
    if (c == '\r') return; /* normalizar */
    if (lex_len < (int)sizeof(lex_buf) - 1) lex_buf[lex_len++] = (char)c;
    else num_overflow = 1;
}
/* Termina la cadena y asegura que su contenido sea una cadena C válida. */
void fin_salida(int c) { (void)c; lex_buf[lex_len] = '\0'; }

/* Construye el diagnóstico correspondiente a la transición de error. */
void ERROR(int c) {
    char frag[64] = {0};
    int error_line = last_line;
    int error_col = last_col;
    if (c == EOF) snprintf(frag, sizeof(frag), "EOF");
    else if (c == '\n') snprintf(frag, sizeof(frag), "\\n");
    else if ((unsigned char)c < 0x20 || (unsigned char)c >= 0x7f)
    {
        unsigned char bytes[4];
        int cantidad = 1;
        int continuaciones = 0;
        bytes[0] = (unsigned char)c;

        /* Agrupa los bytes UTF-8 de un carácter ajeno al alfabeto en un E1. */
        if (bytes[0] >= 0xC2 && bytes[0] <= 0xDF) continuaciones = 1;
        else if (bytes[0] >= 0xE0 && bytes[0] <= 0xEF) continuaciones = 2;
        else if (bytes[0] >= 0xF0 && bytes[0] <= 0xF4) continuaciones = 3;

        while (cantidad <= continuaciones) {
            int siguiente = leer_caracter();
            if (siguiente == EOF) break;
            if (((unsigned char)siguiente & 0xC0) != 0x80) {
                unread_char(siguiente);
                break;
            }
            bytes[cantidad++] = (unsigned char)siguiente;
        }

        /* Los bytes agrupados representan una sola columna del archivo fuente. */
        if (cantidad > 1) {
            cur_col -= cantidad - 1;
            last_line = error_line;
            last_col = error_col;
        }

        /* Conserva los bytes UTF-8 para mostrar el carácter original (p. ej. ¿). */
        memcpy(frag, bytes, (size_t)cantidad);
        frag[cantidad] = '\0';
    }
    else snprintf(frag, sizeof(frag), "%c", (char)c);
    yy_linea = error_line; yy_col_ini = error_col; yy_col_fin = error_col;
    strncpy(yy_lexema, frag, sizeof(yy_lexema) - 1);
    if (cur_estado_global == 0 && c == '.') {
        lex_error_code = 20;
        snprintf(lex_error_msg, sizeof(lex_error_msg), "Constante real mal formada");
    } else if (cur_estado_global == 6) {
        lex_error_code = 19;
        snprintf(lex_error_msg, sizeof(lex_error_msg), "Operador ! aislado no valido");
        strncpy(yy_lexema, "!", sizeof(yy_lexema) - 1);
        yy_linea = tok_line_ini; yy_col_ini = tok_col_ini; yy_col_fin = tok_col_ini;
        /* tok start del !: viene de op_start */
        yy_linea = op_start_line; yy_col_ini = op_start_col; yy_col_fin = op_start_col;
    } else if ((cur_estado_global == 18 || cur_estado_global == 19) && c == EOF) {
        lex_error_code = 3;
        snprintf(lex_error_msg, sizeof(lex_error_msg), "Comentario de bloque sin cerrar");
    } else if (cur_estado_global == 26 && c == EOF) {
        lex_error_code = 18;
        snprintf(lex_error_msg, sizeof(lex_error_msg), "Cadena sin cerrar");
    } else {
        lex_error_code = 1;
        snprintf(lex_error_msg, sizeof(lex_error_msg), "Caracter no perteneciente al alfabeto lexico");
    }
}



/* Recorre las matrices hasta formar un token, un diagnóstico o llegar a EOF. */
int yylex(void) {
    if (!lex_fp) return 0;
    tok_pending = -1;
    lex_error_code = 0; lex_error_msg[0] = '\0';
    lex_warning_code = 0; lex_warning_msg[0] = '\0';
    lex_warning_lexema[0] = '\0';
    skip_auto_unread = 0;

    int estado = 0;
    int c = 0, col = 0;
    int prev_estado = 0, prev_col = 0;
    int tuvo_prev = 0;

    while (estado != -1 && estado != -2) {
        c = leer_caracter();
        col = get_evento(c);
        if (col == -1) { /* OTRO */
            if (estado == 26) { agregar_salida(c); continue; }
            cur_estado_global = estado;
            ERROR(c);
            estado = -2;
            tuvo_prev = 0;
            break;
        }
        if (estado == 0 && col >= 0 && col <= 19) {
            op_start_line = last_line; op_start_col = last_col;
            tok_line_ini = last_line; tok_col_ini = last_col;
        }
        cur_estado_global = estado;
        proceso[estado][col](c);
        if (lex_error_code != 0) { /* fin_entero/fin_real detectó E2/E12/E20 */
            prev_estado = estado; prev_col = col; tuvo_prev = 1;
            estado = -2;
            break;
        }
        prev_estado = estado; prev_col = col; tuvo_prev = 1;
        estado = nuevo_estado[estado][col];
    }

    if (estado == -2) return devolver_error_lexico();

    /* estado == -1: token armado */
    if (!tuvo_prev) return 0;
    if (!skip_auto_unread && unread_mat[prev_estado][prev_col] == 1) unread_char(c);

    if (lex_error_code != 0) return devolver_error_lexico();

    int tok = (tok_pending != -1) ? tok_pending : token_matriz[prev_estado][prev_col];
    if (tok == -1) return 0; /* EOF normal desde estado 0 */
    if (tok == -2) {
        lex_error_code = 1;
        snprintf(lex_error_msg, sizeof(lex_error_msg), "Caracter no perteneciente al alfabeto lexico");
        return devolver_error_lexico();
    }

    /* publicar yy_lexema y ubicación */
    if (tok == ID || tok == INICIO || tok == ENTERO || tok == REAL || tok == SI ||
        tok == SINO || tok == PARA || tok == IMPRIMIR || tok == RETORNAR) {
        lex_buf[lex_len] = '\0';
        strncpy(yy_lexema, lex_buf, sizeof(yy_lexema) - 1);
        yy_linea = tok_line_ini; yy_col_ini = tok_col_ini;
        yy_col_fin = tok_col_ini + (int)strlen(yy_lexema) - 1;
    } else if (tok == CTE_E || tok == CTE_R) {
        lex_buf[lex_len] = '\0';
        strncpy(yy_lexema, lex_buf, sizeof(yy_lexema) - 1);
        yy_linea = tok_line_ini; yy_col_ini = tok_col_ini;
        yy_col_fin = tok_col_ini + (int)strlen(yy_lexema) - 1;
    } else if (tok == CADENA) {
        lex_buf[lex_len] = '\0';
        strncpy(yy_lexema, lex_buf, sizeof(yy_lexema) - 1);
        yy_linea = tok_line_ini; yy_col_ini = tok_col_ini;
        yy_col_fin = tok_col_ini + (int)strlen(yy_lexema) + 1; /* incluye comillas */
    } else {
        const char *f = lexema_fijo(tok);
        strncpy(yy_lexema, f ? f : "", sizeof(yy_lexema) - 1);
        yy_linea = op_start_line; yy_col_ini = op_start_col;
        yy_col_fin = op_start_col + (int)strlen(yy_lexema) - 1;
    }
    if (lex_warning_code != 0) {
        registrar_diagnostico(lex_warning_code, lex_warning_line,
                              lex_warning_col, 1, lex_warning_msg,
                              lex_warning_lexema);
    }
    return tok;
}

/* Informa en orden los errores y advertencias registrados durante el análisis. */
int informar_errores(void) {
    int hubo_error_memoria = error_memoria_diagnosticos;

    if (cantidad_diagnosticos > 0) {
        fprintf(stderr, "Diagnosticos lexicos:\n");
        for (size_t i = 0; i < cantidad_diagnosticos; i++) {
            const DiagnosticoLexico *diagnostico = &diagnosticos[i];
            fprintf(stderr, "%s E%d en %d:%d: %s (lexema: %s)\n",
                    diagnostico->es_advertencia ? "Advertencia" : "Error",
                    diagnostico->codigo, diagnostico->linea,
                    diagnostico->columna, diagnostico->mensaje,
                    diagnostico->lexema[0] == '\0'
                        ? "<vacio>"
                        : diagnostico->lexema);
        }
    }

    free(diagnosticos);
    diagnosticos = NULL;
    cantidad_diagnosticos = 0;
    capacidad_diagnosticos = 0;
    error_memoria_diagnosticos = 0;

    if (hubo_error_memoria) {
        fprintf(stderr, "No se pudo reservar memoria para guardar diagnosticos lexicos\n");
        return 1;
    }
    return 0;
}

/* Muestra los identificadores únicos recolectados durante el análisis. */
void mostrarTS(void) {
    printf("---- Tabla de simbolos (fase lexica: identificadores) ----\n");
    if (ts_n == 0) { printf("(vacia)\n"); return; }
    for (int i = 0; i < ts_n; i++)
        printf("%d: nombre=%s longitud=%d\n", i + 1, ts_ids[i], (int)strlen(ts_ids[i]));
}
