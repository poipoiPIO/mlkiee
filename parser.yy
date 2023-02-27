%{
    #include "Ast.hh"
    #include <iostream>

    extern int yyparse();
    extern "C" int yylex(void);
    extern "C" void yyerror(const char *s);

    
    extern "C" void yyerror(const char * str) {
        std::cerr << "Error parsing: " << str << std::endl;
    }
%}

%union {
  const char* str;
  AstE* expr;
}

%type<str> ATOM NUM 
%type<expr> Program Statement Expr ArithmExpr ArithmVal 
%type<expr> ArithmSubExpr BooleanExpr BooleanSubExpr Bool

%token TRUE FALSE ATOM NUM
%token NOT AND OR
%token ARROW IF ELSE LET

%%

Program: Statement { $1->print(std::cout); delete $1;};

Statement: Expr ';' 
  | Expr ';' Statement { $$ = new Block($1, $3); }
;

Expr: ArithmExpr
  | BooleanExpr
  | ATOM '.' Expr { $$ = new AstFCall($1, $3); }
  | '\\' ATOM ARROW Expr { $$ = new AstFunction($2, $4); }
  | LET ATOM ARROW Expr { $$ = new AstAssign($2, $4); }
  | IF Expr ARROW Expr ELSE Expr { $$ = new AstIf($2, $4, $6); }
;

ArithmExpr: ArithmSubExpr
  | ArithmExpr '+' ArithmSubExpr { $$ = new AstBinary("+", $1, $3); }
  | ArithmExpr '-' ArithmSubExpr { $$ = new AstBinary("+", $1, $3); }
;

ArithmSubExpr: ArithmVal
	| ArithmSubExpr '*' ArithmVal { $$ = new AstBinary("*", $1, $3); }
  | ArithmSubExpr '/' ArithmVal { $$ = new AstBinary("/", $1, $3); }
;

ArithmVal: NUM { $$ = new AstVal($1); }
  | '-' NUM { $$ = new AstUnary("-", new AstVal($2)); }
  | '+' NUM { $$ = new AstUnary("+", new AstVal($2)); }
  | ATOM { $$ = new AstVal($1); }
;
    
BooleanExpr: BooleanSubExpr
  | BooleanExpr AND BooleanSubExpr { $$ = new AstBinary("AND", $1, $3); }
  | BooleanExpr OR BooleanSubExpr { $$ = new AstBinary("OR", $1, $3); }
;

BooleanSubExpr: Bool
	| NOT Bool  { $$ = new AstUnary("NOT", $2); }
;

Bool: TRUE { $$ = new AstBool("TRUE"); }
  | FALSE  { $$ = new AstBool("FALSE"); }
;

%%

int main() {
  return yyparse();
}
