#include "Ast.hh"
#include <iostream>
#include <type_traits>

AstE::~AstE() {};
AstE::AstE() {};
AstE::AstE(const std::string& t) : type(t) {};

std::ostream& operator<<(std::ostream& os, const AstE& t) { 
  return os;
}

AstVal::AstVal(const std::string& val) : value(val) {}
AstVal::AstVal(const std::string& val, const std::string& t) : value(val), AstE(t) {}

template <typename T, typename = typename std::enable_if<std::is_base_of<T, AstE>::value>::type>
std::ostream& operator<<(std::ostream& os, const T& t) {
  t.print(os); return os;
}

void AstVal::print(std::ostream& os) const {
  os << "(AstVal :type " << type << " val: " << value << ")";
}

AstUnary::AstUnary(const std::string& op, AstE* val) : op(op), value(val) {}

void AstUnary::print(std::ostream& os) const { 
  os << "(AstUnary type: " << type << " op: " << op << " value: ";
  value->print(os);
  os << ")"; 
}

AstBinary::AstBinary(const std::string& op, AstE* left, AstE* right) 
  : op(op), left(left), right(right) {}

void AstBinary::print(std::ostream& os) const {
  os << "(AstBinary type: " << type << " op: " << op << " left: ";
  left->print(os); 
  os << " right: ";
  right->print(os);
  os << ")";
}

void AstBinary::traverseChild(std::function<void(AstE*)> fun) { fun(left); fun(right); }

AstAssign::AstAssign(const std::string& name, AstE* val) : value(val), name(name) {}

void AstAssign::print(std::ostream& os) const {
  os << "(AstAssign type: " << type << " name: " << name << " value: ";
  value->print(os);
  os << ")";
}

AstFunction::AstFunction(AstE* arg, AstE* val) : value(val), arg(arg) {}

void AstFunction::print(std::ostream& os) const {
  os << "(AstFunction type: " << type << " arg: ";
  arg->print(os);
  os << " body: ";
  value->print(os);
  os << ")";
}

AstFCall::AstFCall(AstE* name, AstE* val) : value(val), name(name) {}

void AstFCall::print(std::ostream& os) const {
  os << "(AstFCall type: " << type << " name: ";
  name->print(os);
  os << " value: ";
  value->print(os);
  os << ")";
}

AstIf::AstIf(AstE* cond, AstE* t_branch, AstE* f_branch): 
  cond(cond), true_brunch(t_branch), false_brunch(f_branch) {}

void AstIf::print(std::ostream& os) const { 
  os << "(AstIf type: " << type << " cond: ";
  cond->print(os); 
  os << " true: ";
  true_brunch->print(os); 
  os << " false: ";
  false_brunch->print(os);
  os << ")";
}

void AstIf::traverseChild(std::function<void(AstE*)> fun) {
  fun(cond);
  fun(true_brunch);
  fun(false_brunch);
}


Block::Block(AstE* op) {
  exprs = std::list<AstE*>();
  append(op);
} 

Block::Block(AstE* op, AstE* op2) {
  exprs = std::list<AstE*>();
  append(op);
  append(op2);
} 

void Block::append(AstE* op) {
  Block* b = dynamic_cast<Block*>(op);

  if(b) {
      exprs.splice(exprs.end(), b->exprs, b->exprs.begin(), b->exprs.end());
      delete b;
  }

  else exprs.push_back(op);
}

void Block::print(std::ostream& os) const {
  os << "(Block: ";

  for (auto& i : exprs) {
    i->print(os); 
    os << " ";
  }

  os << ")";
}

Block::~Block() {
  for(auto& i : exprs)
    delete i;
}
