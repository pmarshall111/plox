#include <interpreter.h>

#include <ast_printer.h>

#include <charconv>
#include <iostream>

namespace plox {
namespace treewalk {

using namespace ast;
using namespace stmt;

namespace {
// Visitors are defined for each interpret operation.

struct InterpreterVisitor {
  InterpreterVisitor(Environment &env) : d_env(env){};

  // Statements do not need to return anything
  void operator()(const Expression &expr);
  void operator()(const Print &print);
  void operator()(const VarDecl &varDecl);
  // Other operations called by statements return Values
  Value operator()(const Assign &assign);
  Value operator()(const Binary &bin);
  Value operator()(const Grouping &grp);
  Value operator()(const Literal &ltrl);
  Value operator()(const Unary &unary);
  Value operator()(const Variable &var);

private:
  Environment &d_env;
};

struct AdditionVisitor {
  double operator()(double l, double r);
  std::string operator()(const std::string &l, const std::string &r);
  double operator()(auto &&l, auto &&r);
} g_adder;

struct SubtractionVisitor {
  double operator()(double l, double r);
  double operator()(auto &&l, auto &&r);
} g_subtractor;

struct MultiplyVisitor {
  double operator()(double l, double r);
  double operator()(auto &&l, auto &&r);
} g_multiplier;

struct DivideVisitor {
  double operator()(double l, double r);
  double operator()(auto &&l, auto &&r);
} g_divider;

struct TruthyVisitor {
  bool operator()(std::monostate);
  bool operator()(bool b);
  bool operator()(auto &&);
} g_truther;

double getNum(const Literal &ltrl) {
  double val;
  auto start = ltrl.value.data();
  auto end = ltrl.value.data() + ltrl.value.size();
  auto [parseEnd, ec] = std::from_chars(start, end, val);
  if (ec == std::errc::result_out_of_range) {
    throw InterpretException({"Number too large: " + std::string(ltrl.value)});
  }
  if (parseEnd != end) {
    throw InterpretException{
        {"Unable to read number: " + std::string(ltrl.value)}};
  }
  return val;
}

void InterpreterVisitor::operator()(const Expression &expr) {
  std::visit(*this, *expr.expr);
}

void InterpreterVisitor::operator()(const Print &print) {
  // Calculate expression
  Value v = std::visit(*this, *print.expr);
  // Print
  static ValuePrinter s_valuePrinter;
  std::cout << std::visit(s_valuePrinter, v) << std::endl;
}

void InterpreterVisitor::operator()(const VarDecl &varDecl) {
  auto name = std::string(varDecl.name);
  Value val = {};
  if (varDecl.expr) {
    val = std::visit(*this, *varDecl.expr);
  }
  d_env.define(name, val);
}

Value InterpreterVisitor::operator()(const Assign &assign) {
  auto name = std::string(assign.name);
  Value val = std::visit(*this, *assign.value);
  d_env.assign(name, val);
  return val;
}

Value InterpreterVisitor::operator()(const Binary &bnry) {
  Value lhs = std::visit(*this, *bnry.left);
  Value rhs = std::visit(*this, *bnry.right);

  switch (bnry.op.type) {
  case TokenType::PLUS:
    return std::visit(g_adder, lhs, rhs);
  case TokenType::MINUS:
    return std::visit(g_subtractor, lhs, rhs);
  case TokenType::STAR:
    return std::visit(g_multiplier, lhs, rhs);
  case TokenType::SLASH:
    return std::visit(g_divider, lhs, rhs);
  case TokenType::EQUAL_EQUAL:
    return lhs == rhs;
  case TokenType::BANG_EQUAL:
    return lhs != rhs;
  case TokenType::GREATER:
    return lhs > rhs;
  case TokenType::GREATER_EQUAL:
    return lhs >= rhs;
  case TokenType::LESS:
    return lhs < rhs;
  case TokenType::LESS_EQUAL:
    return lhs <= rhs;
  default:
    throw InterpretException{{"Unable to interpret binary op: " +
                              tokenutils::tokenTypeToStr(bnry.op.type)}};
  }
}

Value InterpreterVisitor::operator()(const Grouping &grp) {
  return std::visit(*this, *grp.expr);
}

Value InterpreterVisitor::operator()(const Literal &ltrl) {
  switch (ltrl.type) {
  case TokenType::STRING:
    return std::string(ltrl.value);
  case TokenType::NUMBER: {
    return getNum(ltrl);
  }
  case TokenType::TRUE:
    return true;
  case TokenType::FALSE:
    return false;
  case TokenType::NUL:
    return {};
  default:
    throw InterpretException{
        {"Unable to interpret type: " + tokenutils::tokenTypeToStr(ltrl.type)}};
  }
}

Value InterpreterVisitor::operator()(const Unary &unry) {
  Value right = std::visit(*this, *unry.right);
  switch (unry.op.type) {
  case TokenType::MINUS: {
    Value zero = 0.0;
    return std::visit(g_subtractor, zero, right);
  }
  case TokenType::BANG:
    return !std::visit(g_truther, right);
  default:
    throw InterpretException{{"Unable to interpret unary op: " +
                              tokenutils::tokenTypeToStr(unry.op.type)}};
  }
}

Value InterpreterVisitor::operator()(const Variable &var) {
  return d_env.get(std::string(var.name));
}

double AdditionVisitor::operator()(double l, double r) { return l + r; }

std::string AdditionVisitor::operator()(const std::string &l,
                                        const std::string &r) {
  return l + r;
}

double AdditionVisitor::operator()(auto &&l, auto &&r) {
  throw InterpretException{
      {"Unable to add types: " + std::string(typeid(l).name()) + " and " +
       std::string(typeid(r).name())}};
}

double SubtractionVisitor::operator()(double l, double r) { return l - r; }

double SubtractionVisitor::operator()(auto &&l, auto &&r) {
  throw InterpretException{
      {"Unable to subtract types: " + std::string(typeid(l).name()) + " and " +
       std::string(typeid(r).name())}};
}

double MultiplyVisitor::operator()(double l, double r) { return l * r; }

double MultiplyVisitor::operator()(auto &&l, auto &&r) {
  throw InterpretException{
      {"Unable to multiply types: " + std::string(typeid(l).name()) + " and " +
       std::string(typeid(r).name())}};
}

double DivideVisitor::operator()(double l, double r) { return l / r; }

double DivideVisitor::operator()(auto &&l, auto &&r) {
  throw InterpretException{
      {"Unable to divide types: " + std::string(typeid(l).name()) + " and " +
       std::string(typeid(r).name())}};
};

bool TruthyVisitor::operator()(std::monostate) { return false; }

bool TruthyVisitor::operator()(bool b) { return b; }

bool TruthyVisitor::operator()(auto &&) { return true; }

} // namespace

void interpret(const std::vector<stmt::Stmt> &stmts, Environment &env,
               std::vector<InterpretException> &errs) {
  try {
    InterpreterVisitor v{env};
    for (const auto &s : stmts) {
      std::visit(v, s);
    }
  } catch (const InterpretException &e) {
    errs.push_back(e);
  }
}

} // namespace treewalk
} // namespace plox