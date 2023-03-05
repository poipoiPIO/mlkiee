#pragma once

#include <string>
#include <map>
#include "Ast.hh"

struct T {
  virtual ~T();
  virtual void print(std::ostream& os) const = 0;
  friend std::ostream& operator<<(std::ostream& os, const T& t);
};

struct TInt : public T {
  void print(std::ostream& os) const override;
  friend std::ostream& operator<<(std::ostream& os, const TInt& t);
};

struct TBool : public T {
  void print(std::ostream& os) const override;
  friend std::ostream& operator<<(std::ostream& os, const TBool& t);
};

struct TVar : public T {
  std::string type;

  TVar(std::string);
  void print(std::ostream& os) const override;
  friend std::ostream& operator<<(std::ostream& os, const TVar& t);
};

struct TFoo : public T {
  T* retType, *argType;

  TFoo(T* argType, T* retType);
  void print(std::ostream& os) const override;
  friend std::ostream& operator<<(std::ostream& os, const TFoo& t);
};

struct TEquation {
  std::string left;
  T* right;
  AstE* node;
  
  TEquation(std::string left, T* right, AstE* node);
  friend std::ostream& operator<<(std::ostream& os, const TEquation& t);
  void print(std::ostream& os) const;
};

typedef std::map<std::string, TVar> SymTab;
typedef std::map<std::string, T*> Substitution;

struct Typechecker {
  std::list<TEquation> tEquations;
  Substitution tSubst;

  std::string getTName();
  T* stringToType(std::string type);

  Typechecker();

  Substitution& unifyVar(T* typeX, T* typeY, Substitution& subst);
  Substitution& unify(T* typeX, T* typeY, Substitution& subst);
  Substitution& unifyAll();
  T* doUnify(T* t, Substitution& subst);
  T* getType(AstE* expr, Substitution& subst);

  void genEquations(AstE* node);
  void assignTNames(AstE*, SymTab&);

  bool checkOccurance(T*, T*, Substitution&);

  private:
    int typecounter;
    template<typename V> void assignNamesHelper(V, SymTab&);
    void genUnaryEEquations(AstUnary* node);
    void genBinaryEEquations(AstBinary* node);
    void genFCallEEquations(AstFCall* node);
    void genIfEEquations(AstIf* node);
    void genFunEEquations(AstFunction* node);
};

class TypeError : public std::exception { };
