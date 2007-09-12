OBJECTS = sina_symbols.o phelper.o simple_allocator.o lex.yy.o pprinter.o sina_parser.tab.o main.o

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
	cat test.sina | ./sina test.sina

clean:
	rm *.o *.tab.c lex.yy.c sina