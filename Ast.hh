#include <string>
#include <list>

struct AstE {
  AstE();
  virtual ~AstE();
  virtual void print(std::ostream& os) const = 0;
  friend std::ostream& operator<<(std::ostream& os, const AstE& dt);
};

struct AstVal : AstE {
  std::string value;
  
  AstVal(const std::string& val);
  
  friend std::ostream& operator<<(std::ostream& os, const AstVal& dt);

  virtual void print(std::ostream& os) const override;
};

struct AstBool: AstVal {
  AstBool(const std::string& val);

  friend std::ostream& operator<<(std::ostream& os, const AstBool& dt);
  void print(std::ostream& os) const override;
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
  std::string arg;

  AstFunction(const std::string& arg, AstE* value);
  friend std::ostream& operator<<(std::ostream& os, const AstFunction& dt);

  void print(std::ostream& os) const override;
};

struct AstFCall : AstE {
  AstE* value;
  std::string name;

  AstFCall(const std::string& name, AstE* value);
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
