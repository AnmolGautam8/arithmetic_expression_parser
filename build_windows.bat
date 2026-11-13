@echo off
setlocal

echo Generating parser...
win_bison -d parser.y
if errorlevel 1 exit /b 1

echo Generating lexer...
win_flex lexer.l
if errorlevel 1 exit /b 1

echo Compiling project...
gcc -Wall -Wextra -std=c11 main.c ast.c parser.tab.c lex.yy.c -o arithmetic_parser.exe -lm
if errorlevel 1 exit /b 1

echo.
echo Build successful. Run arithmetic_parser.exe
endlocal
