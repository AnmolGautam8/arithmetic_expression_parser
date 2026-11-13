# Arithmetic Expression Parser

Compiler Design mini project using Flex and Bison.

The program accepts an arithmetic expression, performs lexical analysis and
syntax analysis, builds a grammar parse tree and an abstract syntax tree, and
generates postfix notation.

## Features

- Token display
- Arithmetic operators: `+`, `-`, `*`, `/`, `%`
- Parentheses
- Unary plus and unary minus
- Operator precedence and left associativity
- Grammar-based parse tree
- Abstract syntax tree
- Postfix conversion
- Numeric expression evaluation
- Operand/operator statistics
- Syntax and lexical error reporting
- Interactive menu
- Optional expression-file input

## Build on Windows

Run `build_windows.bat` from the project folder, or execute these commands:

```powershell
win_bison -d parser.y
win_flex lexer.l
gcc -Wall -Wextra -std=c11 main.c ast.c parser.tab.c lex.yy.c -o arithmetic_parser.exe -lm
```

## Build on Linux or macOS

```bash
bison -d parser.y
flex lexer.l
gcc -Wall -Wextra -std=c11 main.c ast.c parser.tab.c lex.yy.c -o arithmetic_parser -lm
```

## Run

Interactive mode:

```text
arithmetic_parser.exe
```

Enter an expression:

```text
3 + 4 * (2 - 1)
```

After parsing, choose an analysis option:

```text
1. Display tokens
2. Display postfix expression
3. Display grammar parse tree
4. Display abstract syntax tree
5. Evaluate expression
6. Display expression statistics
7. Enter another expression
8. Exit
```

Type `exit` or `quit` at the expression prompt to close the program.

File mode:

```text
arithmetic_parser.exe sample.expr
```

Expressions containing identifiers can be parsed, but they cannot be
evaluated because no values have been assigned to those identifiers.
