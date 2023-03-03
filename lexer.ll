%{
  typedef void * AstE;
#include "parser.tab.hh"
#include <string.h>
%}

%option noyywrap

%%

[/][/].*\n      ; // comment

let             return LET;

if              return IF;
else            return ELSE;

true            return TRUE;
false           return FALSE;
not             return NOT;
and             return AND;
or              return OR;

[0-9]+          {
  yylval.str = strdup(yytext);
  return NUM;
}

->              return ARROW;
[a-zA-Z_][a-zA-Z0-9_]* { 
  yylval.str = strdup(yytext);
  return ATOM;
}
[ \t\r\n]       ; // whitespace
[\+|\-|\*|\\|;|\.] { return *yytext; }

. yyerror("Invalid character");

%%
