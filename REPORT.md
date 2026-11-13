# Arithmetic Expression Parser — Project Report

## 1. Objective

To construct a parser that accepts arithmetic expressions, converts them into
postfix notation, and generates the corresponding parse tree.

## 2. Tools used

- C
- Flex for lexical analysis
- Bison for syntax analysis
- GCC for compilation
- Linked tree structures for parse-tree representation

## 3. Lexical analysis

The lexer recognizes numbers, identifiers, operators, and parentheses.

Example:

```text
3 + 4 * 12
```

Tokens:

```text
3       NUMBER
+       PLUS
4       NUMBER
*       MULTIPLY
12      NUMBER
```

## 4. Grammar

```text
Expression → Expression + Term
           | Expression - Term
           | Term

Term       → Term * Factor
           | Term / Factor
           | Term % Factor
           | Factor

Factor     → NUMBER
           | IDENTIFIER
           | ( Expression )
           | - Factor
           | + Factor
```

The grammar gives multiplication, division, and modulus higher precedence than
addition and subtraction. Operators at the same level are left associative.

## 5. Parse tree and AST

The project displays both structures.

The grammar parse tree contains non-terminals such as `Expression`, `Term`,
and `Factor`. The abstract syntax tree contains only the meaningful operators
and operands.

For:

```text
3 + 4 * 12
```

the AST is:

```text
        +
       / \
      3   *
         / \
        4   12
```

## 6. Postfix conversion

The postfix expression is produced using postorder traversal of the AST.

```text
Infix:   3 + 4 * 12
Postfix: 3 4 12 * +
```

## 7. Evaluation

Numeric expressions are evaluated recursively from the AST.

```text
3 + 4 * 12 = 51
```

Expressions containing identifiers are parsed successfully but are not
evaluated because no symbol-value table is included.

## 8. Error handling

The parser reports invalid expressions such as:

```text
3 + * 5
(3 + 4
3 / 0
```

The evaluator also detects division by zero and modulus by zero.

## 9. Complexity

For an expression containing `n` tree nodes:

- Lexical analysis: `O(n)`
- Parsing: `O(n)`
- Postfix traversal: `O(n)`
- Evaluation: `O(n)`
- Tree memory: `O(n)`

## 10. Future enhancements

- Variables with a symbol table
- Functions such as `sin`, `cos`, and `sqrt`
- Exponentiation
- Multiple expressions in an input file
- Three-address-code generation
