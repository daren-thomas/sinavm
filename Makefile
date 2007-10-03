OBJECTS = sina_symbols.o sinavm.o lex.yy.o pprinter.o \
          sina_parser.tab.o main.o sina_interpreter.o sina_error.o \
          sina_builtins.o sina_types.o

default: ALLOCATOR = twospace_allocator
default: test

simple: ALLOCATOR = simple_allocator
simple: test

parser: lexer sina_parser.y
	bison -d sina_parser.y
	cc -c lex.yy.c sina_parser.tab.c

sina_parser.tab.o: sina_parser.tab.c

lexer: sina_lexer.l
	flex sina_lexer.l

build: parser allocator $(OBJECTS) 
	cc $(ALLOCATOR).o $(OBJECTS) -o sina

allocator:
	cc -c -o $(ALLOCATOR).o $(ALLOCATOR).c

test: build
	#echo "abcdef" |  ./sina cesar_shift.sina
	./sina test.sina

clean:
	rm -f *.o *.tab.c *.tab.h lex.yy.c *.gch
	rm -f sina sina.exe
