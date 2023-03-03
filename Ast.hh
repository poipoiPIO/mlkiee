#pragma once

#include <string>
#include <list>
#include <functional>

struct AstE {
  std::string type;

  AstE(const std::string& type);
  AstE();
  virtual ~AstE();

  virtual void print(std::ostream& os) const = 0;

  friend std::ostream& operator<<(std::ostream& os, const AstE& dt);
};

struct AstVal : AstE {
  std::string value;
  
  AstVal(const std::string& val);
  AstVal(const std::string& val, const std::string& t);
  
  friend std::ostream& operator<<(std::ostream& os, const AstVal& dt);

  virtual void print(std::ostream& os) const override;
};

struct AstUnary : AstE {
  AstE* value;
  std::string op;

  AstUnary(const std::string& op, AstE* value);

  friend std::ostream& operator<<(std::ostream& os, const AstUnary& dt);

  void print(std::ostream& os) const override;
};

struct AstBinary : AstE {
  std::string op;
  AstE* right;
  AstE* left;

  AstBinary(const std::string& op, AstE* left, AstE* right);
  friend std::ostream& operator<<(std::ostream& os, const AstBinary& dt);

  void print(std::ostream& os) const override;
  void traverseChild(std::function<void(AstE*)>);
};

struct AstAssign : AstE {
  AstE* value;
  std::string name;

  AstAssign(const std::string& name, AstE* value);
  friend std::ostream& operator<<(std::ostream& os, const AstAssign& dt);

  void print(std::ostream& os) const override;
};

struct AstFunction : AstE {
  AstE* value;
  AstE* arg;

  AstFunction(AstE* arg, AstE* value);
  friend std::ostream& operator<<(std::ostream& os, const AstFunction& dt);

  void print(std::ostream& os) const override;
};

struct AstFCall : AstE {
  AstE* value;
  AstE* name;

  AstFCall(AstE* name, AstE* value);
  friend std::ostream& operator<<(std::ostream& os, const AstFCall& dt);

  void print(std::ostream& os) const override;
};

struct AstIf : AstE {
  AstE* cond;
  AstE* true_brunch;
  AstE* false_brunch;

  AstIf(AstE* cond, AstE* if_true, AstE* if_false);
  friend std::ostream& operator<<(std::ostream& os, const AstIf& dt);
  void print(std::ostream& os) const override;
  void traverseChild(std::function<void(AstE*)>);
};

struct Block : AstE {
  std::list<AstE*> exprs;

  void append(AstE* op);

  Block(AstE* op);
  Block(AstE* op, AstE* op2);

  virtual ~Block();
  friend std::ostream& operator<<(std::ostream& os, const Block& dt);
  void print(std::ostream& os) const override;
};
