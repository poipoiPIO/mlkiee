
.PHONY: parser

all: parser

parser.tab.cc parser.tab.hh:	parser.yy
	bison -d parser.yy

lex.yy.c: lexer.ll parser.tab.hh
	lex lexer.ll

lex.yy.o: lex.yy.c parser.tab.hh
	clang -c lex.yy.c -O2

Ast.o: Ast.cc Ast.hh
	clang++ -c -ggdb Ast.cc -O2

Evaluator.o: Evaluator.cc Evaluator.hh
	clang++ -c -ggdb Evaluator.cc -O2

main.o: main.cc
	clang++ -c -ggdb main.cc -O2

Typechecker.o: Typechecker.hh Utils.o Typechecker.cc
	clang++ -c -ggdb Typechecker.cc -O2

parser: lex.yy.o parser.tab.cc Evaluator.o parser.tab.hh Ast.hh Typechecker.o Ast.o main.o
	clang++ -o parser main.o Ast.o Evaluator.o Typechecker.o parser.tab.cc lex.yy.o -O2 -ggdb 

clean:
	rm lex.yy.c *.tab.* parser *.o
