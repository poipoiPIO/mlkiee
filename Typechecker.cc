#include "Typechecker.hh"
#include <exception>
#include <iostream>
#include <algorithm>
#include <ostream>
#include <string>
#include <array>

T::~T() {};

std::ostream& operator<<(std::ostream& os, const T& t) {
  return os;
}

template <typename V, typename = typename std::enable_if<std::is_base_of<V, T>::value>::type>
std::ostream& operator<<(std::ostream& os, const V& t) {
  t.print(os); return os;
}

void TBool::print(std::ostream& os) const {
  os << "BOOLEAN";
}

void TInt::print(std::ostream& os) const {
  os << "INTEGER";
}

TVar::TVar(std::string t) : type(t) {}

void TVar::print(std::ostream& os) const {
  os << type;
}

TFoo::TFoo(T* arg, T* ret) : argType(arg), retType(ret) {}

void TFoo::print(std::ostream& os) const {
  os << "Function: "; argType->print(os); os << " -> "; argType->print(os);
}

TEquation::TEquation(std::string left, T* right, AstE* node)
  : left(left), right(right), node(node) {}

void TEquation::print(std::ostream& os) const {
  os << "Equation: " << left << " | ";
  right->print(os);
  os << " |\n";
  node->print(os);
  os << "\n";
}

std::ostream& operator<<(std::ostream& os, const TEquation& t) {
  t.print(os); return os;
}

Typechecker::Typechecker() {
  typecounter = 0;
  tEquations = std::list<TEquation>();
}

std::string Typechecker::getTName() {
  return std::string("type").append(std::to_string(typecounter++));
}

bool isNum(const std::string& val) {
  try {std::stoi(val); } catch (...) {
    return false;
  } return true; 
}

template<typename V> 
void Typechecker::assignNamesHelper(V node, SymTab& symtab) {
  node->type = getTName();
  node->traverseChild([&symtab, this](AstE* child) {
      assignTNames(child, symtab);
  });
}

void Typechecker::assignTNames(AstE* node, SymTab& symtab) {
  AstVal* valCheck = dynamic_cast<AstVal*>(node); 

  if(valCheck) {
    if(valCheck->value == "TRUE" || valCheck->value == "FALSE") { // Bool
      valCheck->type = "BOOL"; return;
    } else if (isNum(valCheck->value)) { // Num
      valCheck->type = "NUM"; return;
    } else { // ID
      auto search = symtab.find(valCheck->value);

      if ( search != symtab.end()) {
        valCheck->type = search->second.type; return;
      } else {
        std::cerr << "Unbound value: " <<  valCheck->value << "!" << std::endl;
        throw std::exception();
      }
    }
  }

  AstUnary* UnaryCheck = dynamic_cast<AstUnary*>(node);

  if(UnaryCheck) {
    UnaryCheck->type = getTName();
    assignTNames(UnaryCheck->value, symtab);
    return;
  }

  AstFCall* FCallCheck = dynamic_cast<AstFCall*>(node);

  if(FCallCheck) {
    node->type = getTName();
    assignTNames(FCallCheck->value, symtab);
    assignTNames(FCallCheck->name, symtab);
    return;
  }

  AstAssign* AssignCheck = dynamic_cast<AstAssign*>(node);

  if(AssignCheck) {
    node->type = getTName();
    symtab.insert({AssignCheck->name, TVar(node->type)});
    assignTNames(AssignCheck->value, symtab);
    return;
  }

  AstIf* IfCheck = dynamic_cast<AstIf*>(node);
  if(IfCheck) { assignNamesHelper(IfCheck, symtab); return; }

  AstBinary* BinaryCheck = dynamic_cast<AstBinary*>(node);
  if(BinaryCheck) { assignNamesHelper(BinaryCheck, symtab); return; }
  
  AstFunction* FunctionCheck = dynamic_cast<AstFunction*>(node);

  if(FunctionCheck) {
    FunctionCheck->type = getTName();

    SymTab local_symtab = SymTab(symtab);

    std::string varName = (dynamic_cast<AstVal*>(FunctionCheck->arg))->value;
    local_symtab.insert({varName, TVar(getTName())});
    assignTNames(FunctionCheck->value, local_symtab);
    assignTNames(FunctionCheck->arg, local_symtab);
  }
}

T* Typechecker::stringToType(std::string type) {
  auto castMap = std::map<std::string, std::function<T*()>>();

  castMap.insert({"NUM", [](){ return new TInt(); }});
  castMap.insert({"BOOL", [](){ return new TBool(); }});

  if (type.rfind("type", 0) == 0) {
    return new TVar(type);
  }

  auto pos = castMap.find(type);

  if (pos == castMap.end()) {
    std::cerr << "WrongType: " <<  type << "!" << std::endl;
    throw std::exception();
  } else {
      return pos->second();
  }
}

void Typechecker::genUnaryEEquations(AstUnary* node) {
    genEquations(node->value);

    if (node->op ==  "NOT") {
      tEquations.push_back(TEquation(node->value->type, new TBool(), node));
      tEquations.push_back(TEquation(node->type, new TBool(), node));
      return;
    } 

    tEquations.push_back(TEquation(node->value->type, new TInt(), node));
    tEquations.push_back(TEquation(node->type, new TInt(), node));
}

void Typechecker::genBinaryEEquations(AstBinary* BinaryCheck) {
    BinaryCheck->traverseChild([this](AstE* c){
      genEquations(c);
    });

    auto arithm_binop = std::array<std::string, 4> {"+", "-", "*", "/"};

    if (std::find(std::begin(arithm_binop),
          std::end(arithm_binop), BinaryCheck->op) != std::end(arithm_binop)) {

      tEquations.push_back(TEquation(BinaryCheck->left->type, new TInt(), BinaryCheck));
      tEquations.push_back(TEquation(BinaryCheck->right->type, new TInt(), BinaryCheck));

      tEquations.push_back(TEquation(BinaryCheck->type, new TInt(), BinaryCheck));
      return;
    } 

    tEquations.push_back(TEquation(BinaryCheck->left->type, new TBool(), BinaryCheck));
    tEquations.push_back(TEquation(BinaryCheck->right->type, new TBool(), BinaryCheck));

    tEquations.push_back(TEquation(BinaryCheck->type, new TBool(), BinaryCheck));
}

void Typechecker::genFCallEEquations(AstFCall* FCallCheck) {
  genEquations(FCallCheck->name);
  genEquations(FCallCheck->value);

  tEquations.push_back(TEquation(
        FCallCheck->type, 
        new TFoo(stringToType(FCallCheck->name->type),
          stringToType(FCallCheck->value->type)),
        FCallCheck));
}

void Typechecker::genIfEEquations(AstIf* IfCheck) {
  IfCheck->traverseChild([this](AstE* node){
    genEquations(node);
  });

  tEquations.push_back(TEquation(IfCheck->cond->type, new TBool(), IfCheck));

  tEquations.push_back(TEquation( IfCheck->type,
        stringToType(IfCheck->true_brunch->type), IfCheck));

  tEquations.push_back(TEquation( IfCheck->type,
        stringToType(IfCheck->true_brunch->type), IfCheck));
}

void Typechecker::genFunEEquations(AstFunction* FunctionCheck) {
    genEquations(FunctionCheck->arg);
    genEquations(FunctionCheck->value);

    tEquations.push_back(
        TEquation(FunctionCheck->type,
          new TFoo(
            stringToType(FunctionCheck->arg->type),
            stringToType(FunctionCheck->value->type)),
          FunctionCheck));
}

void Typechecker::genEquations(AstE* node) {
  AstVal* valCheck = dynamic_cast<AstVal*>(node); 

  if(valCheck) {
    if(valCheck->type == "BOOL") {
      tEquations.push_back(TEquation(valCheck->type, new TBool(), valCheck));
      return;
    } else if (valCheck->type == "NUM") {
      tEquations.push_back(TEquation(valCheck->type, new TInt(), valCheck));
      return;
    } else { return; }
  }

  AstUnary* UnaryCheck = dynamic_cast<AstUnary*>(node);
  if(UnaryCheck) { genUnaryEEquations(UnaryCheck); return; }

  AstFCall* FCallCheck = dynamic_cast<AstFCall*>(node);
  if(FCallCheck) { genFCallEEquations(FCallCheck); return; }

  AstAssign* AssignCheck = dynamic_cast<AstAssign*>(node);
  if(AssignCheck) {
    genEquations(AssignCheck->value);

    tEquations.push_back(
        TEquation(AssignCheck->type,
        stringToType(AssignCheck->value->type), AssignCheck));
    return;
  }

  AstIf* IfCheck = dynamic_cast<AstIf*>(node);
  if(IfCheck) { genIfEEquations(IfCheck); return; }

  AstBinary* BinaryCheck = dynamic_cast<AstBinary*>(node);
  if(BinaryCheck) { genBinaryEEquations(BinaryCheck); return; }
  
  AstFunction* FunctionCheck = dynamic_cast<AstFunction*>(node);
  if(FunctionCheck) { genFunEEquations(FunctionCheck); return; }
}

// TODO:: make separated virtual methods
bool TEquals(const T* x, const T* y) {
  bool boolP = dynamic_cast<const TBool*>(x) && dynamic_cast<const TBool*>(y),
       intP  = dynamic_cast<const TInt*>(x) && dynamic_cast<const TInt*>(y),
       fooP = false, varP = false;

  const TVar* varX = dynamic_cast<const TVar*>(x),
        *varY = dynamic_cast<const TVar*>(y);

  if(varX && varY) {
    varP = (varX->type) == (varY->type);
  }

  const TFoo* fooX = dynamic_cast<const TFoo*>(x),
        *fooY =  dynamic_cast<const TFoo*>(y);

  if(fooX && fooY) {
    fooP = TEquals(fooX->argType, fooY->argType) && TEquals(fooX->retType, fooY->retType); 
  }

  return boolP || intP || fooP || varP;
}

bool Typechecker::checkOccurance(T* v, T* t, Substitution& subst) {
  if(TEquals(v, t)) { 
    return true;
  }

  if(TVar* varT = dynamic_cast<TVar*>(t)) {
    auto lookupT = subst.find(varT->type);

    if (lookupT != subst.end()) {
      return checkOccurance(v, lookupT->second, subst);
    } 
  } 

  if(TFoo* fooT = dynamic_cast<TFoo*>(t)) {
    return checkOccurance(v, fooT->retType, subst)
      || checkOccurance(v, fooT->argType, subst);
  }
  return false;
}

Substitution& Typechecker::unifyVar(T* v, T* typeY, Substitution& subst) {
  TVar* varX = dynamic_cast<TVar*>(v), *varY = dynamic_cast<TVar*>(typeY);
  
  if (!varX) {
    std::cerr << "Something weird happend :: excepted type Var, but: ";
    v->print(std::cerr);
    std::cerr << " was occured!!" << std::endl;
    throw std::exception();
  }

  auto lookupX = subst.find(varX->type);
  if (lookupX != subst.end()) {
    return unify(lookupX->second, typeY, subst);
  } 

  if(varY) {
    auto lookupY = subst.find(varY->type);
    if (lookupY != subst.end()) {
      return unify(v, lookupY->second, subst);
    }
  } 

  if (checkOccurance(v, typeY, subst)) {
    throw TypeError();
  }

  subst.insert({varX->type, typeY});
  return subst;
}

Substitution& Typechecker::unify(T* typeX, T* typeY, Substitution& subst) {
  if(TEquals(typeX, typeY)) { return subst; } 

  if (dynamic_cast<TVar*>(typeX)) {
    return unifyVar(typeX, typeY, subst);
  } 

  if (dynamic_cast<TVar*>(typeY)) {
    return unifyVar(typeY, typeX, subst);
  }

  TFoo* fooX = dynamic_cast<TFoo*>(typeX),
       *fooY = dynamic_cast<TFoo*>(typeY);

  if (fooX && fooY) {
    subst = unify(fooX->retType, fooY->retType, subst);
    subst = unify(fooX->retType, fooY->retType, subst);
    return subst;
  }

  std::cerr << "WrongType: cannot provide substitution for equations!" << std::endl;
  throw std::exception();
}

Substitution& Typechecker::unifyAll() {
  tSubst = Substitution();

  for (auto& e : tEquations) {
    std::cerr << "Trying to unify: ";
    e.print(std::cerr);
    std::cerr << std::endl;

    try {
      std::cerr << "Left:: " << e.left << std::endl << "right:: ";
      e.right->print(std::cerr);
      std::cerr << std::endl;

      tSubst = unify(stringToType(e.left), e.right, tSubst);
    } catch (TypeError t) {
      std::cerr << "TypeError :: cannot unify expr: ";
      e.print(std::cerr);
      std::cerr << std::endl;
      throw TypeError();
    }

    auto lookup = tSubst.find(e.left);
    if(lookup != tSubst.end()) {
      std::cerr << "type unified to: ";
      lookup->second->print(std::cerr);
      std::cerr << std::endl;
    }
    
  }

  return tSubst;
}

T* Typechecker::doUnify(T* t, Substitution& subst) {
  if (subst.empty() || dynamic_cast<TBool*>(t) || dynamic_cast<TInt*>(t)) {
    return t;
  } if (auto varT = dynamic_cast<TVar*>(t)) {
    auto lookup = subst.find(varT->type);
    if (lookup != subst.end()) {
      return doUnify(lookup->second, subst);
    } else {
      return t;
    }
  } if (auto fooT = dynamic_cast<TFoo*>(t)) {
    return new TFoo(doUnify(fooT->argType, subst),
        doUnify(fooT->retType, subst));
  } 

  std::cerr << "WrongType : cannot provide substitution for expression of type!";
  t->print(std::cerr);
  std::cerr << std::endl;
  throw std::exception();
}

void reassignNames(T* t, int& counter, SymTab& local_sym) {
    if(auto varT = dynamic_cast<TVar*>(t)) {
      SymTab::iterator lookup = local_sym.find(varT->type);

      if (lookup != local_sym.end()) {
        varT->type = lookup->second.type;
      } else {
        std::string name = std::string("t").append(std::to_string(counter++));

        local_sym.insert({varT->type, TVar(name)});
        local_sym.insert({name, TVar(name)});
        varT->type = local_sym.find(name)->second.type;
      }
    } if(auto fooT = dynamic_cast<TFoo*>(t)) {
      reassignNames(fooT->argType, counter, local_sym);      
      reassignNames(fooT->retType, counter, local_sym);      
    }
}

T* Typechecker::getType(AstE* exp, Substitution& subst) {
  int counter = 0;
  SymTab local_sym = SymTab();
  T* type = doUnify(stringToType(exp->type), subst); 
  reassignNames(type, counter, local_sym);

  return type;
}

