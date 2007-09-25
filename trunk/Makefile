OBJECTS = sina_symbols.o sinavm.o simple_allocator.o lex.yy.o pprinter.o \
          sina_parser.tab.o main.o sina_interpreter.o sina_error.o \
		  sina_builtins.o

default: test

parser: lexer sina_parser.y
	bison -d sina_parser.y
	cc -c lex.yy.c sina_parser.tab.c

sina_parser.tab.o: sina_parser.tab.c

lexer: sina_lexer.l
	flex sina_lexer.l

build: parser $(OBJECTS)
	cc $(OBJECTS) -o sina

test: build
	echo "abcdef" |  ./sina cesar_shift.sina

clean:
	rm *.o *.tab.c *.tab.h lex.yy.c *.gch
	rm sina
