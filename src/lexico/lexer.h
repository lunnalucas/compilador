#ifndef BETA_LEXER_H
#define BETA_LEXER_H

#include <stdio.h>

#include "lexer_tables.h"

#define LEXER_LEXEME_MAX 20
#define LEXER_BUFFER_SIZE 256

extern FILE *yyin;
extern char yytext[LEXER_BUFFER_SIZE];
extern int yyleng;
extern int yylineno;

void lexer_init(FILE *input);
int yylex(void);

#endif
