// Alexandra Szewc       aszewc1@jhu.edu
// Victor Wen            vwen2@jhu.edu

#include "calc.h"
#include <string>
#include <map>
#include <sstream>

using std::string;       using std::map;
using std::stringstream;

struct Calc {
private:
  // fields
  map<string, int> vars;
  int size;
  
public:
  // public member functions
  Calc() : size(0)/*, vars(new map<string, int>)*/ {}
  ~Calc();

  int evalExpr(const string &expr, int &result);

private:
  // private member functions
  int evalOpr(const string &expr, int &result);
  int evalOp(int l, int op, int r, int &res);
  bool isAlphabetic(const string &var);
  bool isNumeric(const string &var);
  bool getVal(const string &var, int &val);
};

extern "C" struct Calc *calc_create(void) {
    return new Calc();
}

extern "C" void calc_destroy(struct Calc *calc) {
    delete calc;
}

extern "C" int calc_eval(struct Calc *calc, const char *expr, int *result) {
    return calc->evalExpr(expr, *result);
}

// Verify that the form of an expression
// matches one of the four formats given
// in the assignment:
//
// operand
// operand op operand
// var = operand
// var = operand op operand
//
// Outsource handling of first two
// expressions to evalOpr
int Calc::evalExpr(const string &expr, int &result) {
  stringstream ss = str(expr);
  string var, op;

  // Check if assignment to var
  if (ss >> var && ss >> op
      && isAlphabetic(var)
      && op.length == 1 && op.at(0) == '=') {
    if (evalOpr(ss.str(), result)) {
      this->vars[var] = result;
      return 1;
    }
  }

  // Deal with no assignment to var
  return evalOpr(expr, result);
}

int Calc::evalOpr(const string &expr, int &result) {
  stringstream ss = str(expr);
  string var1, var2, op;
  int val1, val2;
  
  ss >> var1 >> op >> var2;

  if (!var1.empty()) {
    if (op.empty()) {
      if (getVal(var1, val1)) {
	result = val1;
	return 1;
      }
    }

    if (!op.empty() && !var2.empty()) {
      if (getVal(var1, val1) && getVal(var2, val2)
	  && op.length == 0) {
	return evalOp(val1, op.at(0), val2, result);
      }
    }
  }
  return 0;
}

int Calc::evalOp(int l, int op, int r, int &res) {
  if (op == 47 && r == 0) return 0;

  switch (op) {
    case 43:
      res = l + r;
      return 1;
    case 45:
      res = l - r;
      return 1;
    case 42:
      res l * r;
      return 1;
    case 47:
      res l / r;
      return 1;
    default:
      return 0;
  }
}

bool Calc::isAlphabetic(const string &var) {
  for (char& c : var) {
    if (c < 65 && c > 90 && c < 97 && c > 122) {
      return false;
    }
  }
  return true;
}

bool Calc::isNumeric(const string &var) {
  for (char& c : var) {
    if (c < 48 && c > 57) {
      return false;
    }
  }
  return true;
}

bool Calc::getVal(const string &var, int &val) {
  if (isAlphabetic(var)) {
    map<string, int>::iterator it = this->vars.find(var);
    if (it != this->vars.end()) {
      val = *it;
      return true;
    }
  }

  if (isNumeric(var)) {
    val = std::stoi(var);
    return true;
  }
  return false;
}
