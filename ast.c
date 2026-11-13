#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"

#define ANSI_RESET  "\x1b[0m"
#define ANSI_GREEN  "\x1b[32m"
#define ANSI_YELLOW "\x1b[33m"

static int isOperator(const char *value)
{
    return strcmp(value, "+") == 0 ||
           strcmp(value, "-") == 0 ||
           strcmp(value, "*") == 0 ||
           strcmp(value, "/") == 0 ||
           strcmp(value, "%") == 0 ||
           strcmp(value, "^") == 0 ||
           strcmp(value, "neg") == 0;
}

static void printValue(const char *value)
{
    if (isOperator(value))
        printf(ANSI_YELLOW "%s" ANSI_RESET, value);
    else
        printf(ANSI_GREEN "%s" ANSI_RESET, value);
}

Node *createNode(const char *value, Node *left, Node *right)
{
    Node *node;

    node = (Node *)malloc(sizeof(Node));

    if (node == NULL)
    {
        fprintf(stderr, "Error: memory allocation failed.\n");
        return NULL;
    }

    strncpy(node->value, value, AST_VALUE_SIZE - 1);
    node->value[AST_VALUE_SIZE - 1] = '\0';

    node->left = left;
    node->right = right;

    return node;
}

void printPostfix(const Node *root)
{
    if (root == NULL)
        return;

    printPostfix(root->left);
    printPostfix(root->right);

    printValue(root->value);
    printf(" ");
}

int treeHeight(const Node *root)
{
    int leftHeight;
    int rightHeight;

    if (root == NULL)
        return 0;

    leftHeight = treeHeight(root->left);
    rightHeight = treeHeight(root->right);

    return 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);
}

static void drawTree(const Node *node,
                     int level,
                     int position,
                     int spacing,
                     char *canvas,
                     int width,
                     int rows)
{
    int row;
    int start;
    int i;
    int branchPosition;

    if (node == NULL)
        return;

    row = level * 2;

    if (row >= rows)
        return;

    start = position - (int)strlen(node->value) / 2;

    for (i = 0; node->value[i] != '\0'; i++)
    {
        if (start + i >= 0 && start + i < width)
            canvas[row * width + start + i] = node->value[i];
    }

    if (node->left != NULL)
    {
        branchPosition = position - spacing / 2;

        if (level * 2 + 1 < rows &&
            branchPosition >= 0 &&
            branchPosition < width)
        {
            canvas[(level * 2 + 1) * width + branchPosition] = '/';
        }

        drawTree(node->left,
                 level + 1,
                 position - spacing,
                 spacing / 2,
                 canvas,
                 width,
                 rows);
    }

    if (node->right != NULL)
    {
        branchPosition = position + spacing / 2;

        if (level * 2 + 1 < rows &&
            branchPosition >= 0 &&
            branchPosition < width)
        {
            canvas[(level * 2 + 1) * width + branchPosition] = '\\';
        }

        drawTree(node->right,
                 level + 1,
                 position + spacing,
                 spacing / 2,
                 canvas,
                 width,
                 rows);
    }
}

void printTree(const Node *root, int depth)
{
    int height;
    int width = 1;
    int rows;
    int row;
    int lastCharacter;
    char *canvas;

    (void)depth;

    if (root == NULL)
        return;

    height = treeHeight(root);

    for (row = 0; row < height + 2; row++)
        width *= 2;

    rows = height * 2 + 1;

    canvas = (char *)malloc((size_t)width * rows);

    if (canvas == NULL)
    {
        fprintf(stderr, "Error: unable to display tree.\n");
        return;
    }

    for (row = 0; row < width * rows; row++)
        canvas[row] = ' ';

    drawTree(root,
             0,
             width / 2,
             width / 4,
             canvas,
             width,
             rows);

    for (row = 0; row < rows; row++)
    {
        lastCharacter = width - 1;

        while (lastCharacter >= 0 &&
               canvas[row * width + lastCharacter] == ' ')
        {
            lastCharacter--;
        }

        if (lastCharacter >= 0)
        {
            printf("%.*s\n",
                   lastCharacter + 1,
                   &canvas[row * width]);
        }
    }

    free(canvas);
}

ParseNode *createParseNode(const char *label, int childCount, ...)
{
    ParseNode *node;
    va_list arguments;
    int i;

    node = (ParseNode *)malloc(sizeof(ParseNode));

    if (node == NULL)
        return NULL;

    strncpy(node->label, label, AST_VALUE_SIZE - 1);
    node->label[AST_VALUE_SIZE - 1] = '\0';

    node->childCount = childCount;
    node->children = NULL;

    if (childCount > 0)
    {
        node->children = (ParseNode **)malloc(
            sizeof(ParseNode *) * (size_t)childCount);

        if (node->children == NULL)
        {
            free(node);
            return NULL;
        }

        va_start(arguments, childCount);

        for (i = 0; i < childCount; i++)
            node->children[i] = va_arg(arguments, ParseNode *);

        va_end(arguments);
    }

    return node;
}

void printParseTree(const ParseNode *root,
                    const char *prefix,
                    int isLast,
                    int isRoot)
{
    char childPrefix[1024];
    int i;

    if (root == NULL)
        return;

    if (isRoot)
    {
        printf("%s\n", root->label);
        childPrefix[0] = '\0';
    }
    else
    {
        printf("%s%s %s\n",
               prefix,
               isLast ? "`-->" : "|-->",
               root->label);

        snprintf(childPrefix,
                 sizeof(childPrefix),
                 "%s%s",
                 prefix,
                 isLast ? "    " : "|   ");
    }

    for (i = 0; i < root->childCount; i++)
    {
        printParseTree(root->children[i],
                       childPrefix,
                       i == root->childCount - 1,
                       0);
    }
}

void freeParseTree(ParseNode *root)
{
    int i;

    if (root == NULL)
        return;

    for (i = 0; i < root->childCount; i++)
        freeParseTree(root->children[i]);

    free(root->children);
    free(root);
}

int countNodes(const Node *root)
{
    if (root == NULL)
        return 0;

    return 1 +
           countNodes(root->left) +
           countNodes(root->right);
}

int countOperators(const Node *root)
{
    if (root == NULL)
        return 0;

    return (isOperator(root->value) ? 1 : 0) +
           countOperators(root->left) +
           countOperators(root->right);
}

int countOperands(const Node *root)
{
    if (root == NULL)
        return 0;

    return (!isOperator(root->value) ? 1 : 0) +
           countOperands(root->left) +
           countOperands(root->right);
}

int evaluateTree(const Node *root, double *result)
{
    char *end;
    double left;
    double right;

    if (root == NULL || result == NULL)
        return 0;

    if (!isOperator(root->value))
    {
        *result = strtod(root->value, &end);

        if (*end != '\0')
            return 0;

        return 1;
    }

    if (strcmp(root->value, "neg") == 0)
    {
        if (!evaluateTree(root->right, &right))
            return 0;

        *result = -right;
        return 1;
    }

    if (!evaluateTree(root->left, &left) ||
        !evaluateTree(root->right, &right))
    {
        return 0;
    }

    if (strcmp(root->value, "+") == 0)
    {
        *result = left + right;
    }
    else if (strcmp(root->value, "-") == 0)
    {
        *result = left - right;
    }
    else if (strcmp(root->value, "*") == 0)
    {
        *result = left * right;
    }
    else if (strcmp(root->value, "/") == 0)
    {
        if (right == 0.0)
        {
            fprintf(stderr,
                    "\x1b[31mEvaluation error: division by zero.\x1b[0m\n");
            return 0;
        }

        *result = left / right;
    }
    else if (strcmp(root->value, "%") == 0)
    {
        if (right == 0.0)
        {
            fprintf(stderr,
                    "\x1b[31mEvaluation error: modulus by zero.\x1b[0m\n");
            return 0;
        }

        *result = fmod(left, right);
    }
    else if (strcmp(root->value, "^") == 0)
    {
        *result = pow(left, right);
    }
    else
    {
        return 0;
    }

    return 1;
}

void freeTree(Node *root)
{
    if (root == NULL)
        return;

    freeTree(root->left);
    freeTree(root->right);

    free(root);
}