#include <map>
#include <string>
#include "Ast.hh"

typedef std::map<std::string, AstE*> LookupTab;

struct Evaluator {
  LookupTab globalLookupTab;
  Evaluator();
  Evaluator(LookupTab);

  AstE* eval(AstE*, LookupTab&);

  private:
    AstE* evalVar(AstVal*, LookupTab&);

    AstE* eval(AstVal*, LookupTab&);
    AstE* eval(AstFCall*, LookupTab&);
    AstE* eval(AstUnary*, LookupTab&);
    AstE* eval(AstAssign*, LookupTab&);
    AstE* eval(AstBinary*, LookupTab&);
    AstE* eval(AstIf*, LookupTab&);
    AstE* eval(AstFunction*, LookupTab&);
};
