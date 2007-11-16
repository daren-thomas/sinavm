OBJECTS = sina_symbols.o sinavm.o lex.yy.o pprinter.o \
          sina_parser.tab.o main.o sina_interpreter.o sina_error.o \
          sina_builtins.o sina_types.o sina_allocator.o

HEAP_SIZE = 1024

default: concurrent

simple: ALLOCATOR = simple_allocator
simple: build

twospace: ALLOCATOR = twospace_allocator
twospace: build

concurrent: ALLOCATOR = concurrent_allocator
concurrent: build

parser: lexer sina_parser.y
	bison -d sina_parser.y
	cc -c lex.yy.c sina_parser.tab.c

sina_parser.tab.o: sina_parser.tab.c

lexer: sina_lexer.l
	flex sina_lexer.l

build: parser allocator main $(OBJECTS) 
	cc $(ALLOCATOR).o $(OBJECTS) -o sina -lpthread

main: main.c
	cc -D HEAP_SIZE=$(HEAP_SIZE) -c -o main.o main.c

allocator:
	cc -c -o $(ALLOCATOR).o $(ALLOCATOR).c

test:
	cat abcdefg.txt |  ./sina cesar_shift.sina
	#./sina symbols.sina

clean:
	rm -f *.o *.tab.c *.tab.h lex.yy.c *.gch
	rm -f sina sina.exe
