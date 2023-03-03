#pragma once

#include <string>
#include <map>
#include "Ast.hh"

struct T {
  virtual ~T();
  virtual void print(std::ostream& os) const = 0;
};

struct TInt : public T {
  void print(std::ostream& os) const override;
};

struct TBool : public T {
  void print(std::ostream& os) const override;
};

struct TVar : public T {
  std::string type;

  TVar(std::string);
  void print(std::ostream& os) const override;
};

struct TFoo : public T {
  T* retType, *argType;

  TFoo(T* argType, T* retType);
  void print(std::ostream& os) const override;
};

struct TEquation {
  std::string left;
  T* right;
  AstE* node;
  
  TEquation(std::string left, T* right, AstE* node);
  void print(std::ostream& os) const;
};

typedef std::map<std::string, TVar> SymTab;

struct Typechecker {
  std::list<TEquation> tEquations;

  std::string getTName();
  T* stringToType(std::string type);

  Typechecker();

  private:
    int typecounter;
    template<typename V> void assignNamesHelper(V, SymTab&);
    void assignTNames(AstE*, SymTab&);
    void genEquations(AstE* node);
    void genUnaryEEquations(AstUnary* node);
    void genBinaryEEquations(AstBinary* node);
    void genFCallEEquations(AstFCall* node);
    void genIfEEquations(AstIf* node);
    void genFunEEquations(AstFunction* node);
};
