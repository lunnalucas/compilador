#include <stdio.h>
#include <stdlib.h>

#include "../src/lexico/lexer.h"

static const char *token_name(int token)
{
    switch (token) {
    case TOKEN_ID: return "ID";
    case TOKEN_INTEGER: return "INTEGER";
    case TOKEN_REAL: return "REAL";
    case TOKEN_START: return "START";
    case TOKEN_INTEGER_TYPE: return "INTEGER_TYPE";
    case TOKEN_REAL_TYPE: return "REAL_TYPE";
    case TOKEN_IF: return "IF";
    case TOKEN_ELSE: return "ELSE";
    case TOKEN_FOR: return "FOR";
    case TOKEN_PRINT: return "PRINT";
    case TOKEN_RETURN: return "RETURN";
    case TOKEN_ASSIGN: return "ASSIGN";
    case TOKEN_EQUAL: return "EQUAL";
    case TOKEN_NOT_EQUAL: return "NOT_EQUAL";
    case TOKEN_LESS: return "LESS";
    case TOKEN_GREATER: return "GREATER";
    case TOKEN_LESS_EQUAL: return "LESS_EQUAL";
    case TOKEN_GREATER_EQUAL: return "GREATER_EQUAL";
    case TOKEN_AND: return "AND";
    case TOKEN_OR: return "OR";
    case TOKEN_PLUS: return "PLUS";
    case TOKEN_MINUS: return "MINUS";
    case TOKEN_MULTIPLY: return "MULTIPLY";
    case TOKEN_DIVIDE: return "DIVIDE";
    case TOKEN_OPEN_PAREN: return "OPEN_PAREN";
    case TOKEN_CLOSE_PAREN: return "CLOSE_PAREN";
    case TOKEN_OPEN_BRACE: return "OPEN_BRACE";
    case TOKEN_CLOSE_BRACE: return "CLOSE_BRACE";
    case TOKEN_SEMICOLON: return "SEMICOLON";
    case TOKEN_COMMA: return "COMMA";
    case TOKEN_DOT: return "DOT";
    case TOKEN_QUOTE: return "QUOTE";
    case TOKEN_STRING: return "STRING";
    default: return "UNKNOWN";
    }
}

int main(int argc, char **argv)
{
    FILE *input;
    int token;

    if (argc != 2) {
        fprintf(stderr, "Uso: %s archivo.beta\n", argv[0]);
        return EXIT_FAILURE;
    }

    input = fopen(argv[1], "r");
    if (input == NULL) {
        fprintf(stderr, "Error: no se pudo abrir '%s'.\n", argv[1]);
        return EXIT_FAILURE;
    }

    lexer_init(input);

    printf("=== ANALISIS LEXICO: %s ===\n", argv[1]);
    printf("%-8s %-16s %-8s %s\n", "LINEA", "TOKEN", "CODIGO", "LEXEMA");
    printf("--------------------------------------------------\n");

    while ((token = yylex()) != 0) {
        printf("%-8d %-16s %-8d %s\n",
               yylineno, token_name(token), token, yytext);
    }

    printf("--------------------------------------------------\n");
    printf("=== FIN DEL ANALISIS ===\n");

    fclose(input);
    return EXIT_SUCCESS;
}
