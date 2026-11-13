CC = gcc
CFLAGS = -Wall -Wextra -std=c11
LIBS = -lm

TARGET = arithmetic_parser

all: $(TARGET)

parser.tab.c parser.tab.h: parser.y ast.h
	bison -d parser.y

lex.yy.c: lexer.l parser.tab.h
	flex lexer.l

$(TARGET): main.c ast.c parser.tab.c lex.yy.c ast.h lexer_api.h
	$(CC) $(CFLAGS) main.c ast.c parser.tab.c lex.yy.c -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET) parser.tab.c parser.tab.h lex.yy.c

.PHONY: all clean
