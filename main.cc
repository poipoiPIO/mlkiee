#include "Ast.hh"
#include "Typechecker.hh"
#include "iostream"

extern int yyparse();
extern AstE* result;

int main() {

  yyparse();

  Typechecker typeChecker = Typechecker();

  Block* base = dynamic_cast<Block*>(result);
  SymTab symtab = SymTab();

  typeChecker.assignTNames(base->exprs.front(), symtab);

  std::cout << "______Assigned_Types______" << std::endl;
  base->exprs.front()->print(std::cerr);

  std::cout << "\n_____Equations_Assign_____" << std::endl;
  typeChecker.genEquations(base->exprs.front());

  for(TEquation& e : typeChecker.tEquations) 
    e.print(std::cerr);

  std::cout << "\n_____Infered Type_____" << std::endl;
  typeChecker.getType(base->exprs.front(), typeChecker.unifyAll())->print(std::cerr);
  std::cout << std::endl;

  return 0;
}
