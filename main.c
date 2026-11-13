#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "lexer_api.h"

#define ANSI_RESET  "\x1b[0m"
#define ANSI_BLUE   "\x1b[34m"
#define ANSI_CYAN   "\x1b[36m"
#define ANSI_GREEN  "\x1b[32m"
#define ANSI_RED    "\x1b[31m"
#define ANSI_YELLOW "\x1b[33m"
#define ANSI_WHITE  "\x1b[97m"
#define ANSI_BOLD   "\x1b[1m"

typedef struct yy_buffer_state *YY_BUFFER_STATE;

extern int yyparse(void);
extern int yylineno;
extern int lexicalError;

extern Node *root;
extern ParseNode *parseRoot;

extern YY_BUFFER_STATE yy_scan_string(const char *text);
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);

static void printBanner(void)
{
    printf("\n");

    printf(ANSI_CYAN ANSI_BOLD
           "============================================================\n"
           "              ARITHMETIC EXPRESSION PARSER\n"
           "============================================================\n"
           ANSI_RESET);

    printf("Compiler Design Mini Project\n");
    printf("Enter an expression or type "
           ANSI_WHITE "exit" ANSI_RESET
           " to close.\n");
}

static int parseExpression(const char *expression)
{
    YY_BUFFER_STATE buffer;
    int result;

    root = NULL;
    parseRoot = NULL;
    lexicalError = 0;
    yylineno = 1;

    buffer = yy_scan_string(expression);
    result = yyparse();
    yy_delete_buffer(buffer);

    return result == 0 &&
           root != NULL &&
           lexicalError == 0;
}

static void printMenu(void)
{
    printf("\n" ANSI_BLUE
           "-------------------- ANALYSIS MENU ---------------------\n"
           ANSI_RESET);

    printf("1. Display tokens\n");
    printf("2. Display postfix expression\n");
    printf("3. Display grammar parse tree\n");
    printf("4. Display abstract syntax tree\n");
    printf("5. Evaluate expression\n");
    printf("6. Display expression statistics\n");
    printf("7. Enter another expression\n");
    printf("8. Exit\n");

    printf(ANSI_BLUE
           "----------------------------------------------------------\n"
           ANSI_RESET);
}

static int analysisMenu(const char *expression)
{
    char choice[32];

    while (1)
    {
        printMenu();

        printf(ANSI_CYAN "Select an option: " ANSI_RESET);

        if (fgets(choice, sizeof(choice), stdin) == NULL)
            return 1;

        switch (choice[0])
        {
            case '1':
                printf(ANSI_GREEN ANSI_BOLD
                       "\nLEXICAL ANALYSIS\n"
                       ANSI_RESET);

                printf("Input: %s\n", expression);
                displayTokens(expression);
                break;

            case '2':
                printf(ANSI_GREEN ANSI_BOLD
                       "\nPOSTFIX EXPRESSION\n"
                       ANSI_RESET);

                printf("Infix:   %s\n", expression);
                printf("Postfix: ");

                printPostfix(root);

                printf("\n");
                break;

            case '3':
                printf(ANSI_GREEN ANSI_BOLD
                       "\nGRAMMAR PARSE TREE\n"
                       ANSI_RESET);

                printParseTree(parseRoot, "", 1, 1);
                break;

            case '4':
                printf(ANSI_GREEN ANSI_BOLD
                       "\nABSTRACT SYNTAX TREE\n"
                       ANSI_RESET);

                printTree(root, 0);
                break;

            case '5':
            {
                double result;

                printf(ANSI_GREEN ANSI_BOLD
                       "\nEXPRESSION EVALUATION\n"
                       ANSI_RESET);

                if (evaluateTree(root, &result))
                {
                    printf("Result: %.6g\n", result);
                }
                else
                {
                    printf(ANSI_YELLOW
                           "Evaluation failed. The expression may contain "
                           "variables or an invalid operation.\n"
                           ANSI_RESET);
                }

                break;
            }

            case '6':
                printf(ANSI_GREEN ANSI_BOLD
                       "\nEXPRESSION STATISTICS\n"
                       ANSI_RESET);

                printf("Operands       : %d\n", countOperands(root));
                printf("Operators      : %d\n", countOperators(root));
                printf("Total AST nodes: %d\n", countNodes(root));
                printf("AST height     : %d\n", treeHeight(root));
                break;

            case '7':
                return 0;

            case '8':
                return 1;

            default:
                printf(ANSI_RED
                       "Invalid option. Choose 1 to 8.\n"
                       ANSI_RESET);
        }
    }
}

int main(int argc, char *argv[])
{
    FILE *input = stdin;
    int interactive = 1;
    int shouldExit = 0;
    char expression[1024];

    if (argc > 2)
    {
        fprintf(stderr,
                "Usage: %s [expression-file]\n",
                argv[0]);

        return EXIT_FAILURE;
    }

    if (argc == 2)
    {
        input = fopen(argv[1], "r");
        interactive = 0;

        if (input == NULL)
        {
            perror(argv[1]);
            return EXIT_FAILURE;
        }
    }

    printBanner();

    while (!shouldExit)
    {
        size_t length;

        if (interactive)
            printf(ANSI_CYAN "\n>> " ANSI_RESET);

        if (fgets(expression, sizeof(expression), input) == NULL)
            break;

        length = strcspn(expression, "\r\n");
        expression[length] = '\0';

        if (interactive &&
            (strcmp(expression, "exit") == 0 ||
             strcmp(expression, "quit") == 0))
        {
            break;
        }

        if (expression[0] == '\0')
        {
            printf(ANSI_RED
                   "Please enter an expression.\n"
                   ANSI_RESET);

            continue;
        }

        if (!parseExpression(expression))
        {
            printf(ANSI_RED
                   "The expression is invalid. Please try again.\n"
                   ANSI_RESET);

            freeTree(root);
            freeParseTree(parseRoot);

            root = NULL;
            parseRoot = NULL;

            continue;
        }

        printf(ANSI_GREEN ANSI_BOLD
               "\nExpression parsed successfully.\n"
               ANSI_RESET);

        if (analysisMenu(expression))
            shouldExit = 1;

        freeTree(root);
        freeParseTree(parseRoot);

        root = NULL;
        parseRoot = NULL;
    }

    if (!interactive)
        fclose(input);

    printf("\n" ANSI_CYAN ANSI_BOLD
           "Thank you for using the Arithmetic Expression Parser.\n"
           ANSI_RESET);

    return EXIT_SUCCESS;
}