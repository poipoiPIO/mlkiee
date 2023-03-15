#include "Ast.hh"
#include "Typechecker.hh"
#include "Evaluator.hh"
#include "iostream"

extern int yyparse();
extern AstE* result;

int main() {
  yyparse();
  if(!result) {
    std::cout << "Please provide okay-ish input!" << std::endl;
    exit(1);
  }

  Typechecker typeChecker = Typechecker();

  Block* base = dynamic_cast<Block*>(result);
  SymTab symtab = SymTab();
  Evaluator eval = Evaluator();

  for(auto& e : base->exprs) {
    typeChecker.assignTNames(e, symtab);
    typeChecker.genEquations(e);
    typeChecker.getType(e, typeChecker.unifyAll())->print(std::cerr);
    std::cerr << std::endl;
    eval.eval(e, eval.globalLookupTab)
      ->print(std::cerr);
    std::cerr << std::endl;
  }
  return 0;
}
