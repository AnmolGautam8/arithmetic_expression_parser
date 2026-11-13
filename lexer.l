%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.tab.h"

int lexicalError = 0;

static char *copyText(const char *text)
{
    size_t length = strlen(text) + 1;
    char *copy = (char *)malloc(length);

    if (copy != NULL)
        memcpy(copy, text, length);

    return copy;
}

static const char *tokenName(int token)
{
    switch (token)
    {
        case NUMBER:     return "NUMBER";
        case IDENTIFIER: return "IDENTIFIER";
        case '+':        return "PLUS";
        case '-':        return "MINUS";
        case '*':        return "MULTIPLY";
        case '/':        return "DIVIDE";
        case '%':        return "MODULO";
        case '^':        return "POWER";
        case '(':        return "LEFT_PAREN";
        case ')':        return "RIGHT_PAREN";
        default:         return "UNKNOWN";
    }
}
%}

%option noyywrap
%option yylineno

DIGIT       [0-9]
IDENTIFIER  [a-zA-Z_][a-zA-Z0-9_]*
NUMBER      ({DIGIT}+\.{DIGIT}*|\.{DIGIT}+|{DIGIT}+([eE][+-]?{DIGIT}+)?|{DIGIT}+\.{DIGIT}*([eE][+-]?{DIGIT}+)?)

%%

{NUMBER}        {
                    yylval.str = copyText(yytext);
                    return NUMBER;
                }

{IDENTIFIER}    {
                    yylval.str = copyText(yytext);
                    return IDENTIFIER;
                }

[ \t\r\n]+      {
                    /* Ignore whitespace. */
                }

"+"             { return '+'; }
"-"             { return '-'; }
"*"             { return '*'; }
"/"             { return '/'; }
"%"             { return '%'; }
"^"             { return '^'; }
"("             { return '('; }
")"             { return ')'; }

.               {
                    fprintf(stderr,
                            "\x1b[31mLexical error at line %d: "
                            "unexpected character '%s'\x1b[0m\n",
                            yylineno,
                            yytext);

                    lexicalError = 1;
                }

%%

void displayTokens(const char *text)
{
    YY_BUFFER_STATE buffer;
    int token;

    printf("\n%-20s %-20s\n", "LEXEME", "TOKEN");
    printf("----------------------------------------\n");

    buffer = yy_scan_string(text);

    while ((token = yylex()) != 0)
    {
        printf("%-20s %-20s\n", yytext, tokenName(token));

        if (token == NUMBER || token == IDENTIFIER)
            free(yylval.str);
    }

    yy_delete_buffer(buffer);
}