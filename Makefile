
.PHONY: parser

all: parser

parser.tab.cc parser.tab.hh:	parser.yy
	bison -d parser.yy

lex.yy.c: lexer.ll parser.tab.hh
	lex lexer.ll

lex.yy.o: lex.yy.c parser.tab.hh
	clang -c lex.yy.c -Os

parser: lex.yy.o parser.tab.cc parser.tab.hh Ast.hh
	clang++ -o parser Ast.cc parser.tab.cc lex.yy.o -Os 

clean:
	rm lex.yy.c *.tab.* parser *.o
