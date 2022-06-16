CC = gcc
CFLAGS = -std=c99 -g -Wall -Werror -Wextra -pedantic
BINARIES = test_lexer test_parser test_codegen

all: $(BINARIES)

# Only test_* executables can be stored into repo/tests subfolder
test_lexer: lexer_suite/test_lexer.o error.o lexer.o dynamicstring.o
	$(CC) $(CFLAGS) -o $@ $?

test_parser: parser_suite/test_parser.o error.o lexer.o dynamicstring.o symtable.o codegenerator.o
	$(CC) $(CFLAGS) -o $@ $?

test_codegen: codegen_suite/test_codegen.o error.o dynamicstring.o
	$(CC) $(CFLAGS) -o $@ $?

# Localize compiler binaries to repo/tests subfolder
error.o: ../error.c
	$(CC) $(CFLAGS) -c -o $@ $?

lexer.o: ../lexer.c
	$(CC) $(CFLAGS) -c -o $@ $?

dynamicstring.o: ../dynamicstring.c
	$(CC) $(CFLAGS) -c -o $@ $?

parser.o: ../parser.c
	$(CC) $(CFLAGS) -c -o $@ $?

symtable.o: ../symtable.c
	$(CC) $(CFLAGS) -c -o $@ $?

codegenerator.o: ../codegenerator.c
	$(CC) $(CFLAGS) -c -o $@ $?

clean:
	rm -f $(BINARIES)
	rm -f *.o *_suite/*.o