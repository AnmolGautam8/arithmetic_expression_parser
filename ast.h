#ifndef ARITHMETIC_AST_H
#define ARITHMETIC_AST_H

#define AST_VALUE_SIZE 32

typedef struct Node {
    char value[AST_VALUE_SIZE];
    struct Node *left;
    struct Node *right;
} Node;

typedef struct ParseNode {
    char label[AST_VALUE_SIZE];
    struct ParseNode **children;
    int childCount;
} ParseNode;

Node *createNode(const char *value, Node *left, Node *right);

void printPostfix(const Node *root);
void printTree(const Node *root, int depth);
void freeTree(Node *root);

ParseNode *createParseNode(const char *label, int childCount, ...);

void printParseTree(const ParseNode *root,
                    const char *prefix,
                    int isLast,
                    int isRoot);

void freeParseTree(ParseNode *root);

int evaluateTree(const Node *root, double *result);
int countNodes(const Node *root);
int countOperands(const Node *root);
int countOperators(const Node *root);
int treeHeight(const Node *root);

#endif