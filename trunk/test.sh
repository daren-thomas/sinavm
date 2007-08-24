#!/usr/bin/env sh

flex sina_lexer.l
bison -d sina_parser.y
cc lex.yy.c sina_parser.tab.c
cat cs.sina | ./a.out
