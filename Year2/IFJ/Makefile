# Makefile for compiler

CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -Werror -pedantic

FILES = codegenerator.c codegenerator.h dynamicstring.c dynamicstring.h error.c error.h
FILES += expressions.c expressions.h lexer.c lexer.h parser.c parser.h compiler.c
FILES += stack.c stack.h symtable.c symtable.h Makefile rozdeleni dokumentace.pdf

all: compiler

compiler : parser.o compiler.o error.o lexer.o dynamicstring.o symtable.o codegenerator.o stack.o expressions.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -rf *.o
	rm -rf lexer-test dynamicstring-test symtable-test parser-test codegenerator-test expressions-test

zip:
	rm -rf *.o
	rm -rf compiler
	zip xrucek00.zip $(FILES)
