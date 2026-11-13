%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"

extern int yylex(void);
extern int yylineno;
extern FILE *yyin;

void yyerror(const char *message);

typedef struct SemanticValue {
    Node *ast;
    ParseNode *parse;
} SemanticValue;

Node *root = NULL;
ParseNode *parseRoot = NULL;

static SemanticValue *makeSemanticValue(Node *ast, ParseNode *parse)
{
    SemanticValue *value;

    value = (SemanticValue *)malloc(sizeof(SemanticValue));

    if (value == NULL)
    {
        fprintf(stderr,
                "\x1b[31mError: semantic-value allocation failed.\x1b[0m\n");
        return NULL;
    }

    value->ast = ast;
    value->parse = parse;

    return value;
}

static ParseNode *makeParseLeaf(const char *label)
{
    return createParseNode(label, 0);
}
%}

%union {
    char *str;
    void *value;
}

%token <str> NUMBER
%token <str> IDENTIFIER

%type <value> input expression term power factor

%start input

%%

input
    : expression
      {
          SemanticValue *value = (SemanticValue *)$1;

          root = value->ast;
          parseRoot = value->parse;

          free(value);
      }
    ;

expression
    : expression '+' term
      {
          SemanticValue *left = (SemanticValue *)$1;
          SemanticValue *right = (SemanticValue *)$3;

          $$ = makeSemanticValue(
              createNode("+", left->ast, right->ast),
              createParseNode("Expression", 3,
                              left->parse,
                              makeParseLeaf("+"),
                              right->parse));

          free(left);
          free(right);
      }
    | expression '-' term
      {
          SemanticValue *left = (SemanticValue *)$1;
          SemanticValue *right = (SemanticValue *)$3;

          $$ = makeSemanticValue(
              createNode("-", left->ast, right->ast),
              createParseNode("Expression", 3,
                              left->parse,
                              makeParseLeaf("-"),
                              right->parse));

          free(left);
          free(right);
      }
    | term
      {
          SemanticValue *value = (SemanticValue *)$1;

          $$ = makeSemanticValue(
              value->ast,
              createParseNode("Expression", 1, value->parse));

          free(value);
      }
    ;

term
    : term '*' power
      {
          SemanticValue *left = (SemanticValue *)$1;
          SemanticValue *right = (SemanticValue *)$3;

          $$ = makeSemanticValue(
              createNode("*", left->ast, right->ast),
              createParseNode("Term", 3,
                              left->parse,
                              makeParseLeaf("*"),
                              right->parse));

          free(left);
          free(right);
      }
    | term '/' power
      {
          SemanticValue *left = (SemanticValue *)$1;
          SemanticValue *right = (SemanticValue *)$3;

          $$ = makeSemanticValue(
              createNode("/", left->ast, right->ast),
              createParseNode("Term", 3,
                              left->parse,
                              makeParseLeaf("/"),
                              right->parse));

          free(left);
          free(right);
      }
    | term '%' power
      {
          SemanticValue *left = (SemanticValue *)$1;
          SemanticValue *right = (SemanticValue *)$3;

          $$ = makeSemanticValue(
              createNode("%", left->ast, right->ast),
              createParseNode("Term", 3,
                              left->parse,
                              makeParseLeaf("%"),
                              right->parse));

          free(left);
          free(right);
      }
    | power
      {
          SemanticValue *value = (SemanticValue *)$1;

          $$ = makeSemanticValue(
              value->ast,
              createParseNode("Term", 1, value->parse));

          free(value);
      }
    ;

power
    : factor '^' power
      {
          SemanticValue *left = (SemanticValue *)$1;
          SemanticValue *right = (SemanticValue *)$3;

          $$ = makeSemanticValue(
              createNode("^", left->ast, right->ast),
              createParseNode("Power", 3,
                              left->parse,
                              makeParseLeaf("^"),
                              right->parse));

          free(left);
          free(right);
      }
    | factor
      {
          SemanticValue *value = (SemanticValue *)$1;

          $$ = makeSemanticValue(
              value->ast,
              createParseNode("Power", 1, value->parse));

          free(value);
      }
    ;

factor
    : NUMBER
      {
          Node *ast;
          ParseNode *parse;

          ast = createNode($1, NULL, NULL);
          parse = createParseNode("Factor", 1,
                                  makeParseLeaf($1));

          $$ = makeSemanticValue(ast, parse);

          free($1);
      }
    | IDENTIFIER
      {
          Node *ast;
          ParseNode *parse;

          ast = createNode($1, NULL, NULL);
          parse = createParseNode("Factor", 1,
                                  makeParseLeaf($1));

          $$ = makeSemanticValue(ast, parse);

          free($1);
      }
    | '(' expression ')'
      {
          SemanticValue *value = (SemanticValue *)$2;

          $$ = makeSemanticValue(
              value->ast,
              createParseNode("Factor", 3,
                              makeParseLeaf("("),
                              value->parse,
                              makeParseLeaf(")")));

          free(value);
      }
    | '-' factor
      {
          SemanticValue *value = (SemanticValue *)$2;

          $$ = makeSemanticValue(
              createNode("neg", NULL, value->ast),
              createParseNode("Factor", 2,
                              makeParseLeaf("-"),
                              value->parse));

          free(value);
      }
    | '+' factor
      {
          SemanticValue *value = (SemanticValue *)$2;

          $$ = makeSemanticValue(
              value->ast,
              createParseNode("Factor", 2,
                              makeParseLeaf("+"),
                              value->parse));

          free(value);
      }
    ;

%%

void yyerror(const char *message)
{
    if (strstr(message, "end of file") != NULL)
    {
        fprintf(stderr,
                "\x1b[31mError at line %d: "
                "expression ended unexpectedly.\x1b[0m\n",
                yylineno);
    }
    else if (strstr(message, "')'") != NULL)
    {
        fprintf(stderr,
                "\x1b[31mError at line %d: "
                "unexpected closing parenthesis.\x1b[0m\n",
                yylineno);
    }
    else
    {
        fprintf(stderr,
                "\x1b[31mSyntax error at line %d: "
                "expected a number, identifier, or operator.\x1b[0m\n",
                yylineno);
    }
}