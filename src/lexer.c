#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include "tokens.h"
#include "lexer.h"

/* ============ estado persistente ============ */
char yy_lexema[8192];
int yy_linea = 1, yy_col_ini = 1, yy_col_fin = 1;
long yy_ival = 0;
double yy_fval = 0.0;
int lex_error_code = 0;
char lex_error_msg[256] = {0};

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
static int op_c0 = 0, op_start_line = 1, op_start_col = 1;
static int tok_pending = -1;
static int skip_auto_unread = 0;
static int cur_estado_global = 0;

/* TS mínima: identificadores únicos (truncados) vistos */
#define TS_MAX 2048
static char ts_ids[TS_MAX][21];
static int ts_n = 0;

/* ============ utilidades de lectura ============ */
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

static void unread_char(int c) {
    if (c == EOF) return;
    pb_c = c; pb_valid = 1;
    cur_line = last_line; cur_col = last_col; /* sin duplicar avance */
}

void lexer_init(FILE *f) {
    lex_fp = f;
    cur_line = 1; cur_col = 1; last_line = 1; last_col = 1;
    pb_valid = 0; lex_len = 0; tok_pending = -1;
    lex_error_code = 0; lex_error_msg[0] = '\0';
}

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

/* ============ get_evento: 22 columnas ============ */
static int get_evento(int c) {
    if (c == EOF) return 21;
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) return 0;
    if (c >= '0' && c <= '9') return 1;
    switch (c) {
        case '=': return 2;
        case '!': return 3;
        case '<': return 4;
        case '>': return 5;
        case '&': return 6;
        case '|': return 7;
        case '+': return 8;
        case '-': return 9;
        case '*': return 10;
        case '/': return 11;
        case '(': return 12;
        case ')': return 13;
        case '{': return 14;
        case '}': return 15;
        case ';': return 16;
        case ',': return 17;
        case '.': return 18;
        case '"': return 19;
        case ' ':
        case '\t':
        case '\n':
        case '\r': return 20;
        default: return -1; /* OTRO */
    }
}

/* ============ acciones semánticas (void) ============ */
void hacer_nada(int c) { (void)c; }
void ignorar_comentario(int c) { (void)c; }

void iniciar_id(int c) {
    lex_len = 0; lex_buf[lex_len++] = (char)c;
    tok_line_ini = last_line; tok_col_ini = last_col;
    id_excess = 0;
}
void agregar_id(int c) {
    if (lex_len < 20) lex_buf[lex_len++] = (char)c;
    else id_excess = 1;
}
static void ts_agregar(const char *s) {
    for (int i = 0; i < ts_n; i++) if (strcmp(ts_ids[i], s) == 0) return;
    if (ts_n < TS_MAX) { strncpy(ts_ids[ts_n], s, 20); ts_ids[ts_n][20] = '\0'; ts_n++; }
}
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
        fprintf(stderr, "Advertencia E4 en %d:%d: identificador truncado a 20 caracteres (%s...)\n",
                tok_line_ini, tok_col_ini, lex_buf);
    }
}

void iniciar_entero(int c) {
    lex_len = 0; lex_buf[lex_len++] = (char)c;
    tok_line_ini = last_line; tok_col_ini = last_col;
    num_overflow = 0;
}
void agregar_entero(int c) {
    if (lex_len < (int)sizeof(lex_buf) - 1) lex_buf[lex_len++] = (char)c;
    else num_overflow = 1;
}
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
void fin_entero(int c) {
    /* E2: dígitos seguidos de letra sin separador, ej. 10abc */
    if (c != EOF && isalpha((unsigned char)c)) {
        char err[8192]; int n = 0;
        lex_buf[lex_len] = '\0';
        n += snprintf(err + n, sizeof(err) - n, "%s%c", lex_buf, (char)c);
        int nc;
        while ((nc = leer_caracter()) != EOF && isalnum((unsigned char)nc)) {
            if (n < (int)sizeof(err) - 1) err[n++] = (char)nc;
        }
        err[n] = '\0';
        if (nc != EOF) unread_char(nc);
        lex_error_code = 2;
        snprintf(lex_error_msg, sizeof(lex_error_msg), "Constante numerica mal formada");
        strncpy(yy_lexema, err, sizeof(yy_lexema) - 1);
        yy_linea = tok_line_ini; yy_col_ini = tok_col_ini; yy_col_fin = tok_col_ini + n - 1;
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
void fin_real(int c) {
    /* E2: 12.3.4 -> segundo punto seguido de dígito */
    if (c == '.') {
        int nc = leer_caracter();
        if (nc != EOF && isdigit((unsigned char)nc)) {
            char err[8192]; int n = 0;
            lex_buf[lex_len] = '\0';
            n += snprintf(err + n, sizeof(err) - n, "%s.%c", lex_buf, (char)nc);
            int mc;
            while ((mc = leer_caracter()) != EOF &&
                   (isdigit((unsigned char)mc) || mc == '.' || isalpha((unsigned char)mc))) {
                /* si aparece letra también es E2; cortar en separador real */
                if (mc == '.' || isalpha((unsigned char)mc)) {
                    /* consumir resto alfanumérico/puntos para un solo diagnóstico */
                    if (n < (int)sizeof(err) - 1) err[n++] = (char)mc;
                    int qc;
                    while ((qc = leer_caracter()) != EOF && (isalnum((unsigned char)qc) || qc == '.')) {
                        if (n < (int)sizeof(err) - 1) err[n++] = (char)qc;
                    }
                    if (qc != EOF) unread_char(qc);
                    break;
                }
                if (n < (int)sizeof(err) - 1) err[n++] = (char)mc;
            }
            if (mc != EOF && !(mc == '.' || isalpha((unsigned char)mc))) unread_char(mc);
            err[n] = '\0';
            lex_error_code = 2;
            snprintf(lex_error_msg, sizeof(lex_error_msg), "Constante numerica mal formada");
            strncpy(yy_lexema, err, sizeof(yy_lexema) - 1);
            yy_linea = tok_line_ini; yy_col_ini = tok_col_ini; yy_col_fin = tok_col_ini + n - 1;
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

void iniciar_salida(int c) {
    (void)c;
    lex_len = 0;
    tok_line_ini = last_line; tok_col_ini = last_col;
    num_overflow = 0;
}
void agregar_salida(int c) {
    if (c == EOF) return;
    if (c == '\r') return; /* normalizar */
    if (lex_len < (int)sizeof(lex_buf) - 1) lex_buf[lex_len++] = (char)c;
    else num_overflow = 1;
}
void fin_salida(int c) { (void)c; lex_buf[lex_len] = '\0'; }

/* ERROR de la matriz de procesos */
void ERROR(int c) {
    char frag[64] = {0};
    if (c == EOF) snprintf(frag, sizeof(frag), "EOF");
    else if (c == '\n') snprintf(frag, sizeof(frag), "\\n");
    else snprintf(frag, sizeof(frag), "%c", (char)c);
    yy_linea = last_line; yy_col_ini = last_col; yy_col_fin = last_col;
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

/* ============ matrices [28][22] ============ */
/* Columnas: 0 letra,1 dig,2 =,3 !,4 <,5 >,6 &,7 |,8 +,9 -,10 *,11 /,12 (,13 ),14 {,15 },16 ;,17 ",",18 .,19 "",20 ESP-TAB,21 EOF */
static const int nuevo_estado[28][22] = {
    {1,2,4,6,8,10,12,13,14,15,16,17,20,21,22,23,24,25,-2,26,0,-1},
    {1,1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    {-1,2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,3,-1,-1,-1},
    {-1,3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    {-1,-1,5,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    {-2,-2,7,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    {-1,-1,9,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    {-1,-1,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,18,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    {18,18,18,18,18,18,18,18,18,18,19,18,18,18,18,18,18,18,18,18,18,-2},
    {18,18,18,18,18,18,18,18,18,18,19,0,18,18,18,18,18,18,18,18,18,-2},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    {26,26,26,26,26,26,26,26,26,26,26,26,26,26,26,26,26,26,26,27,26,-2},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}
};

static void (*proceso[28][22])(int) = {
    {iniciar_id,iniciar_entero,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,ERROR,iniciar_salida,hacer_nada,hacer_nada},
    {agregar_id,agregar_id,fin_id,fin_id,fin_id,fin_id,fin_id,fin_id,fin_id,fin_id,fin_id,fin_id,fin_id,fin_id,fin_id,fin_id,fin_id,fin_id,fin_id,fin_id,fin_id,fin_id},
    {fin_entero,agregar_entero,fin_entero,fin_entero,fin_entero,fin_entero,fin_entero,fin_entero,fin_entero,fin_entero,fin_entero,fin_entero,fin_entero,fin_entero,fin_entero,fin_entero,fin_entero,fin_entero,agregar_real,fin_entero,fin_entero,fin_entero},
    {fin_real,agregar_real,fin_real,fin_real,fin_real,fin_real,fin_real,fin_real,fin_real,fin_real,fin_real,fin_real,fin_real,fin_real,fin_real,fin_real,fin_real,fin_real,fin_real,fin_real,fin_real,fin_real},
    {hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada},
    {hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada},
    {ERROR,ERROR,hacer_nada,ERROR,ERROR,ERROR,ERROR,ERROR,ERROR,ERROR,ERROR,ERROR,ERROR,ERROR,ERROR,ERROR,ERROR,ERROR,ERROR,ERROR,ERROR,ERROR},
    {hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada},
    {hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada},
    {hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada},
    {hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada},
    {hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada},
    {hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada},
    {hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada},
    {hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada},
    {hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada},
    {hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada},
    {hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,ignorar_comentario,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada},
    {ignorar_comentario,ignorar_comentario,ignorar_comentario,ignorar_comentario,ignorar_comentario,ignorar_comentario,ignorar_comentario,ignorar_comentario,ignorar_comentario,ignorar_comentario,ignorar_comentario,ignorar_comentario,ignorar_comentario,ignorar_comentario,ignorar_comentario,ignorar_comentario,ignorar_comentario,ignorar_comentario,ignorar_comentario,ignorar_comentario,ignorar_comentario,ERROR},
    {ignorar_comentario,ignorar_comentario,ignorar_comentario,ignorar_comentario,ignorar_comentario,ignorar_comentario,ignorar_comentario,ignorar_comentario,ignorar_comentario,ignorar_comentario,ignorar_comentario,ignorar_comentario,ignorar_comentario,ignorar_comentario,ignorar_comentario,ignorar_comentario,ignorar_comentario,ignorar_comentario,ignorar_comentario,ignorar_comentario,ignorar_comentario,ERROR},
    {hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada},
    {hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada},
    {hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada},
    {hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada},
    {hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada},
    {hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada},
    {agregar_salida,agregar_salida,agregar_salida,agregar_salida,agregar_salida,agregar_salida,agregar_salida,agregar_salida,agregar_salida,agregar_salida,agregar_salida,agregar_salida,agregar_salida,agregar_salida,agregar_salida,agregar_salida,agregar_salida,agregar_salida,agregar_salida,fin_salida,agregar_salida,ERROR},
    {hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada,hacer_nada}
};

static const int unread_mat[28][22] = {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1},
    {1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

static const int token_matriz[28][22] = {
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    {-1,-1,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100},
    {101,-1,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,-1,101,101,101},
    {102,-1,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102},
    {111,111,-1,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111},
    {112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112},
    {-2,-2,-1,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2},
    {113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113},
    {114,114,-1,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114},
    {116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116},
    {115,115,-1,115,115,115,115,115,115,115,115,115,115,115,115,115,115,115,115,115,115,115},
    {117,117,117,117,117,117,117,117,117,117,117,117,117,117,117,117,117,117,117,117,117,117},
    {118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118},
    {119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119},
    {120,120,120,120,120,120,120,120,120,120,120,120,120,120,120,120,120,120,120,120,120,120},
    {121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121},
    {122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122},
    {123,123,123,123,123,123,123,123,123,123,-1,123,123,123,123,123,123,123,123,123,123,123},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    {124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124},
    {125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125},
    {126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126},
    {127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127},
    {128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128},
    {129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,132,-1,-1},
    {132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132}
};

/* ============ yylex ============ */
int yylex(void) {
    if (!lex_fp) return 0;
    tok_pending = -1;
    lex_error_code = 0; lex_error_msg[0] = '\0';
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
            op_c0 = c; op_start_line = last_line; op_start_col = last_col;
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

    if (estado == -2) return ERROR_LEXICO;

    /* estado == -1: token armado */
    if (!tuvo_prev) return 0;
    if (!skip_auto_unread && unread_mat[prev_estado][prev_col] == 1) unread_char(c);

    if (lex_error_code != 0) return ERROR_LEXICO;

    int tok = (tok_pending != -1) ? tok_pending : token_matriz[prev_estado][prev_col];
    if (tok == -1) return 0; /* EOF normal desde estado 0 */
    if (tok == -2) {
        lex_error_code = 1;
        snprintf(lex_error_msg, sizeof(lex_error_msg), "Caracter no perteneciente al alfabeto lexico");
        return ERROR_LEXICO;
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
    return tok;
}

void mostrarTS(void) {
    printf("---- Tabla de simbolos (fase lexica: identificadores) ----\n");
    if (ts_n == 0) { printf("(vacia)\n"); return; }
    for (int i = 0; i < ts_n; i++)
        printf("%d: nombre=%s longitud=%d\n", i + 1, ts_ids[i], (int)strlen(ts_ids[i]));
}
