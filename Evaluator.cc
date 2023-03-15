#include "Evaluator.hh"
#include <fstream>
#include <iostream>
#include "Utils.hh"

Evaluator::Evaluator() {}
Evaluator::Evaluator(LookupTab lt) : globalLookupTab(lt) {}

AstE* Evaluator::evalVar(AstVal* e, LookupTab& lookupTab) {
  auto lookup = lookupTab.find(e->value); 

  if(lookup != lookupTab.end()) {
    return lookup->second;
  }

  std::cout << "Undefined variable: " << e->value << "!" << std::endl;
  throw std::exception();
}

AstE* Evaluator::eval(AstVal* e, LookupTab& lookupTab) {
  if(e->type.rfind("type", 0) == 0) {
    return evalVar(e, lookupTab);
  }

  return e;
}

AstE* Evaluator::eval(AstUnary* e, LookupTab& lookupTab) {
  AstVal* value = dynamic_cast<AstVal*>(eval(e->value, lookupTab));

  if(!value) {
    std::cout << "Strange value: ";
      e->print(std::cout);
    std::cout  << ", in Unary expression!" << std::endl;

    throw std::exception();
  }

  if(e->op == "NOT") {

    if(value->value == "TRUE") {
      value->value = "FALSE";
    } else if (value->value == "FALSE") {
      value->value = "TRUE";
    } else {
      std::cout << "NON-BOOL value: " << value->value
        << ", in expression of type BOOL!" << std::endl;

      throw std::exception();
    }

    return value; 
  }

  if(isNum(value->value)) {
    if(e->op == "+") {
      return value;
    } if(e->op == "-") {
      value->value = value->value.insert(0, "-");
      return value;
    }
  }

  std::cout << "NON-NUMERIC value: " << value->value
    << ", in expression of type NUM!" << std::endl;

  throw std::exception();
}

AstE* Evaluator::eval(AstBinary* e, LookupTab& lookupTab) {
  AstVal* right = dynamic_cast<AstVal*>(eval(e->right, lookupTab)),
        * left = dynamic_cast<AstVal*>(eval(e->left, lookupTab));
  
  if(isNum(right->value) && isNum(left->value)) {
      double left_val = std::stoi(left->value),
             right_val = std::stoi(right->value);
    if(e->op == "+") {
      return new AstVal(
        std::to_string(left_val + right_val)
      ); 
    }

    if(e->op == "-") {
      return new AstVal(
        std::to_string(left_val - right_val)
      );
    }

    if(e->op == "*") {
      return new AstVal(
        std::to_string(left_val * right_val)
      );
    }

    if(e->op == "/") {
      return new AstVal(
        std::to_string(left_val / right_val)
      );
    }
  }

  bool left_bool = left->value == "TRUE" ? true : false,
       right_bool = right->value == "TRUE" ? true : false;

  if(e->op == "AND") {
    return new AstVal(
      left_bool && right_bool ? "TRUE" : "FALSE"
    ); 
  }

  if(e->op == "OR") {
    return new AstVal(
      left_bool || right_bool ? "TRUE" : "FALSE"
    ); 
  }

  // TODO
  std::cout << "OP: " << e->op
    << ", not implemented yet!!" << std::endl;

  throw std::exception();
}

AstE* Evaluator::eval(AstAssign* e, LookupTab& lookupTab) {
  std::pair<std::string, AstE*> newVar = {e->name, e->value};
  lookupTab.insert(newVar);
  return e->value;
}

AstE* Evaluator::eval(AstFCall* e, LookupTab& lookupTab) {
  LookupTab localTab = LookupTab(lookupTab);
  AstFunction* callee = dynamic_cast<AstFunction*>(eval(e->name, lookupTab));

  if(!callee) {
    std::cout << "Non-function call! :: excepted function, but found: ";
    e->print(std::cout);
    std::cout << std::endl;

    throw std::exception();
  }

  AstVal* argname = dynamic_cast<AstVal*>(callee->arg);
  std::pair<std::string, AstE*> newVar = {argname->value, e->value};
  localTab.insert(newVar);

  return eval(callee->value, localTab);
}

AstE* Evaluator::eval(AstFunction* e, LookupTab& lookupTab) {
  return e;
}

AstE* Evaluator::eval(AstIf* e, LookupTab& lookupTab) {
  AstVal* cond = dynamic_cast<AstVal*>(eval(e->cond, lookupTab));

  return cond->value == "TRUE" ?
    eval(e->true_brunch, lookupTab)
    : eval(e->false_brunch, lookupTab);
}

AstE* Evaluator::eval(AstE* e, LookupTab& lookupTab) {
  if(AstVal* astVal = dynamic_cast<AstVal*>(e)) {
    return eval(astVal, lookupTab); 
  } if(AstUnary* astUnary = dynamic_cast<AstUnary*>(e)) {
    return eval(astUnary, lookupTab); 
  } if(AstBinary* astBinary = dynamic_cast<AstBinary*>(e)) {
    return eval(astBinary, lookupTab); 
  } if(AstAssign* astAssign = dynamic_cast<AstAssign*>(e)) {
    return eval(astAssign, lookupTab); 
  } if(AstFCall* astFCall = dynamic_cast<AstFCall*>(e)) {
    return eval(astFCall, lookupTab); 
  } if(AstFunction* astFoo = dynamic_cast<AstFunction*>(e)) {
    return eval(astFoo, lookupTab); 
  } if(AstIf* astIf = dynamic_cast<AstIf*>(e)) {
    return eval(astIf, lookupTab); 
  }

  std::cout << "Fatal error something strange happened :: "
    << "weird node: ";
  e->print(std::cout);
  std::cout << "Passed to the eval function" << std::endl;

  throw std::exception();
}
