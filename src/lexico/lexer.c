#include "lexer.h"

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

FILE *yyin;
char yytext[LEXER_BUFFER_SIZE];
int yyleng;
int yylineno;

static int pushed_character = EOF;

static int read_character(void)
{
    int character;

    if (pushed_character != EOF) {
        character = pushed_character;
        pushed_character = EOF;
        return character;
    }

    return fgetc(yyin);
}

static void unread_character(int character)
{
    if (character != EOF)
        pushed_character = character;
}

static void clear_lexeme(void)
{
    yyleng = 0;
    yytext[0] = '\0';
}

static void add_character(int character)
{
    if (yyleng < LEXER_BUFFER_SIZE - 1)
        yytext[yyleng++] = (char) character;
    yytext[yyleng] = '\0';
}

static void report_error(const char *message, int character)
{
    if (character == EOF)
        fprintf(stderr, "Error lexico en linea %d: %s\n", yylineno, message);
    else
        fprintf(stderr, "Error lexico en linea %d: %s ('%c')\n",
                yylineno, message,
                isprint((unsigned char) character) ? character : '?');
}

static int reserved_token(void)
{
    size_t index;

    for (index = 0; index < reserved_words_count; ++index) {
        if (strcmp(yytext, reserved_words[index].lexeme) == 0)
            return reserved_words[index].token;
    }

    return TOKEN_ID;
}

static int integer_token(void)
{
    char *end;
    long value;

    errno = 0;
    value = strtol(yytext, &end, 10);
    if (errno == ERANGE || end == yytext || value > INT_MAX) {
        report_error("constante entera fuera de rango", EOF);
        return TOKEN_INTEGER;
    }

    return TOKEN_INTEGER;
}

static int read_identifier(void)
{
    int character;
    int truncated = 0;

    for (;;) {
        character = read_character();
        if (character == EOF || !(isalpha((unsigned char) character) ||
                                  isdigit((unsigned char) character))) {
            unread_character(character);
            break;
        }

        if (yyleng < LEXER_LEXEME_MAX) {
            add_character(character);
        } else if (!truncated) {
            report_error("identificador truncado a 20 caracteres", character);
            truncated = 1;
        }
    }

    return reserved_token();
}

static int read_number(void)
{
    int character;
    int has_dot = 0;

    for (;;) {
        character = read_character();
        if (isdigit((unsigned char) character)) {
            add_character(character);
            continue;
        }

        if (character == '.' && !has_dot) {
            has_dot = 1;
            add_character(character);
            continue;
        }

        if (isalpha((unsigned char) character) || character == '.') {
            report_error("constante numerica mal formada", character);
            while (isalnum((unsigned char) character) || character == '.') {
                character = read_character();
            }
            unread_character(character);
            clear_lexeme();
            return yylex();
        }

        unread_character(character);
        break;
    }

    if (has_dot)
        return TOKEN_REAL;
    return integer_token();
}

static int read_string(void)
{
    int character;

    for (;;) {
        character = read_character();
        if (character == EOF) {
            report_error("cadena sin cerrar", EOF);
            return 0;
        }
        if (character == '"') {
            return TOKEN_STRING;
        }
        if (character == '\n')
            ++yylineno;
        add_character(character);
    }
}

static void skip_comment(void)
{
    int character;
    int previous = 0;

    for (;;) {
        character = read_character();
        if (character == EOF) {
            report_error("comentario de bloque sin cerrar", EOF);
            return;
        }
        if (character == '\n')
            ++yylineno;
        if (previous == '*' && character == '/')
            return;
        previous = character;
    }
}

void lexer_init(FILE *input)
{
    yyin = input;
    yylineno = 1;
    pushed_character = EOF;
    clear_lexeme();
}

int yylex(void)
{
    int character;
    int next;

    if (yyin == NULL)
        return 0;

    clear_lexeme();

    for (;;) {
        character = read_character();

        if (character == EOF)
            return 0;
        if (character == ' ' || character == '\t' || character == '\r')
            continue;
        if (character == '\n') {
            ++yylineno;
            continue;
        }

        if (isalpha((unsigned char) character)) {
            add_character(character);
            return read_identifier();
        }

        if (isdigit((unsigned char) character)) {
            add_character(character);
            return read_number();
        }

        if (character == '"')
            return read_string();

        if (character == '/') {
            next = read_character();
            if (next == '*') {
                skip_comment();
                clear_lexeme();
                continue;
            }
            unread_character(next);
            add_character(character);
            return TOKEN_DIVIDE;
        }

        switch (character) {
        case '=':
            next = read_character();
            if (next == '=') {
                strcpy(yytext, "==");
                yyleng = 2;
                return TOKEN_EQUAL;
            }
            unread_character(next);
            add_character(character);
            return TOKEN_ASSIGN;
        case '!':
            next = read_character();
            if (next == '=') {
                strcpy(yytext, "!=");
                yyleng = 2;
                return TOKEN_NOT_EQUAL;
            }
            unread_character(next);
            report_error("se esperaba '=' despues de '!'", character);
            continue;
        case '<':
            next = read_character();
            if (next == '=') {
                strcpy(yytext, "<=");
                yyleng = 2;
                return TOKEN_LESS_EQUAL;
            }
            unread_character(next);
            add_character(character);
            return TOKEN_LESS;
        case '>':
            next = read_character();
            if (next == '=') {
                strcpy(yytext, ">=");
                yyleng = 2;
                return TOKEN_GREATER_EQUAL;
            }
            unread_character(next);
            add_character(character);
            return TOKEN_GREATER;
        case '&': add_character(character); return TOKEN_AND;
        case '|': add_character(character); return TOKEN_OR;
        case '+': add_character(character); return TOKEN_PLUS;
        case '-': add_character(character); return TOKEN_MINUS;
        case '*': add_character(character); return TOKEN_MULTIPLY;
        case '(': add_character(character); return TOKEN_OPEN_PAREN;
        case ')': add_character(character); return TOKEN_CLOSE_PAREN;
        case '{': add_character(character); return TOKEN_OPEN_BRACE;
        case '}': add_character(character); return TOKEN_CLOSE_BRACE;
        case ';': add_character(character); return TOKEN_SEMICOLON;
        case ',': add_character(character); return TOKEN_COMMA;
        default:
            report_error("caracter no valido", character);
            break;
        }
    }
}
