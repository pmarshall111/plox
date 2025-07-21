#include <interpreter_visitor.h>

#include <ast_printer.h>
#include <func.h>

#include <charconv>
#include <iostream>

namespace plox {
namespace treewalk {

using namespace ast;
using namespace stmt;

namespace {
static ValuePrinter s_valuePrinter;

struct AdditionVisitor {
  Value operator()(double l, double r);
  Value operator()(std::string &l, std::string &r);
  Value operator()(auto &&l, auto &&r);
} s_adder;

struct SubtractionVisitor {
  double operator()(double l, double r);
  double operator()(auto &&l, auto &&r);
} s_subtractor;

struct MultiplyVisitor {
  double operator()(double l, double r);
  double operator()(auto &&l, auto &&r);
} s_multiplier;

struct DivideVisitor {
  double operator()(double l, double r);
  double operator()(auto &&l, auto &&r);
} s_divider;

struct TruthyVisitor {
  bool operator()(std::monostate);
  bool operator()(bool b);
  bool operator()(double b);
  bool operator()(auto &&);
} s_truther;

struct ReturnEx : public std::runtime_error {
  ReturnEx(Value v) : d_val(v), std::runtime_error("return"){};
  Value d_val;
};
} // namespace

InterpreterVisitor::InterpreterVisitor(std::shared_ptr<Environment> &env)
    : d_env(env) {}

void InterpreterVisitor::operator()(const Block &blk) {
  // Create new scope and restore it after this func
  auto newEnv = std::make_shared<Environment>(d_env);
  environmentutils::ScopedSwap swapGuard(d_env, newEnv);

  // Run statements within block now new env is installed
  for (auto &stmt : blk.stmts) {
    std::visit(*this, *stmt);
  }
}

void InterpreterVisitor::operator()(const For &forStmt) {
  if (forStmt.initialiser) {
    std::visit(*this, *forStmt.initialiser);
  }

  auto condition = [&]() {
    if (forStmt.condition) {
      return std::visit(s_truther, std::visit(*this, *forStmt.condition));
    }
    // It's possible to have no condition - in that case the loop should run
    // forever
    return true;
  };

  while (condition()) {
    std::visit(*this, *forStmt.body);
    if (forStmt.incrementer) {
      std::visit(*this, *forStmt.incrementer);
    }
  }
}

void InterpreterVisitor::operator()(Fun &funStmt) {
  // Create a function object and store it in the current env
  auto f = std::make_shared<Function>(funStmt.name, std::move(funStmt.params),
                                      d_env, std::move(funStmt.stmts));
  d_env->define(std::string(funStmt.name), f);
}

void InterpreterVisitor::operator()(const Expression &expr) {
  std::visit(*this, *expr.expr);
}

void InterpreterVisitor::operator()(const If &ifStmt) {
  Value evaluatedCondition = std::visit(*this, *ifStmt.condition);
  bool isTruthy = std::visit(s_truther, evaluatedCondition);
  if (isTruthy) {
    std::visit(*this, *ifStmt.ifBranch);
  } else if (ifStmt.elseBranch) {
    std::visit(*this, *ifStmt.elseBranch);
  }
}

void InterpreterVisitor::operator()(const Print &print) {
  // Calculate expression
  Value v = std::visit(*this, *print.expr);
  // Print
  std::cout << std::visit(s_valuePrinter, v) << std::endl;
}

void InterpreterVisitor::operator()(const Return &ret) {
  Value v = {};
  if (ret.expr) {
    v = std::visit(*this, *ret.expr);
  }
  throw ReturnEx{v};
}

void InterpreterVisitor::operator()(const VarDecl &varDecl) {
  auto name = std::string(varDecl.name);
  Value val = {};
  if (varDecl.expr) {
    val = std::visit(*this, *varDecl.expr);
  }
  d_env->define(name, val);
}

void InterpreterVisitor::operator()(const While &whileStmt) {
  while (std::visit(s_truther, std::visit(*this, *whileStmt.condition))) {
    std::visit(*this, *whileStmt.body);
  }
}

Value InterpreterVisitor::operator()(const Assign &assign) {
  auto name = std::string(assign.name);
  Value val = std::visit(*this, *assign.value);
  d_env->assign(name, val);
  return val;
}

Value InterpreterVisitor::operator()(const Binary &bnry) {
  Value lhs = std::visit(*this, *bnry.left);
  Value rhs = std::visit(*this, *bnry.right);

  switch (bnry.op.type) {
  case TokenType::PLUS:
    return std::visit(s_adder, lhs, rhs);
  case TokenType::MINUS:
    return std::visit(s_subtractor, lhs, rhs);
  case TokenType::STAR:
    return std::visit(s_multiplier, lhs, rhs);
  case TokenType::SLASH:
    return std::visit(s_divider, lhs, rhs);
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
    throw InterpretException("Unable to interpret binary op: " +
                             tokenutils::tokenTypeToStr(bnry.op.type));
  }
}

Value InterpreterVisitor::operator()(const Call &call) {
  // Evaluate the callee. Normally this would just be a function name, but
  // in chains we may need to evaluate a preceeding function i.e. fn(1)(2);
  Value callee = std::visit(*this, *call.callee);
  if (!std::holds_alternative<FuncShrdPtr>(callee)) {
    throw InterpretException("Tried to call non callable object " +
                             std::visit(s_valuePrinter, callee));
  }
  auto fShrdPtr = std::get<FuncShrdPtr>(callee);
  if (!fShrdPtr) {
    throw InterpretException("Internal error! Function pointer is null!");
  }

  // Check if the number of args matches the callee args
  if (call.args.size() != fShrdPtr->getArity()) {
    std::ostringstream ss;
    ss << "Tried to call " << fShrdPtr->getName() << " with "
       << call.args.size() << " args when function accepts "
       << fShrdPtr->getArity() << " args.";
    throw InterpretException(ss.str());
  }

  // Update environment to be the environment of the function, and swap back on
  // destruction
  std::shared_ptr<Environment> fEnv(fShrdPtr->getClosure());
  environmentutils::ScopedSwap swapGuard(d_env, fEnv);

  // Set args in new environment
  const std::vector<std::string_view> &fArgNames = fShrdPtr->getArgNames();
  for (int i = 0; i < call.args.size(); i++) {
    Value v = std::visit(*this, *call.args[i]);
    d_env->define(std::string(fArgNames[i]), v);
  }

  try {
    // Pass execution to function. If the user has written a return statement it
    // will throw and be caught below
    return fShrdPtr->execute(d_env, *this);
  } catch (ReturnEx ex) {
    return ex.d_val;
  }
}

Value InterpreterVisitor::operator()(const Grouping &grp) {
  return std::visit(*this, *grp.expr);
}

namespace {

double getNum(const Literal &ltrl) {
  double val;
  auto start = ltrl.value.data();
  auto end = ltrl.value.data() + ltrl.value.size();
  auto [parseEnd, ec] = std::from_chars(start, end, val);
  if (ec == std::errc::result_out_of_range) {
    throw InterpretException("Number too large: " + std::string(ltrl.value));
  }
  if (parseEnd != end) {
    throw InterpretException("Unable to read number: " +
                             std::string(ltrl.value));
  }
  return val;
}
} // namespace

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
    throw InterpretException("Unable to interpret type: " +
                             tokenutils::tokenTypeToStr(ltrl.type));
  }
}

Value InterpreterVisitor::operator()(const Unary &unry) {
  Value right = std::visit(*this, *unry.right);
  switch (unry.op.type) {
  case TokenType::MINUS: {
    Value zero = 0.0;
    return std::visit(s_subtractor, zero, right);
  }
  case TokenType::BANG:
    return !std::visit(s_truther, right);
  default:
    throw InterpretException("Unable to interpret unary op: " +
                             tokenutils::tokenTypeToStr(unry.op.type));
  }
}

Value InterpreterVisitor::operator()(const Variable &var) {
  return d_env->get(std::string(var.name));
}

Value AdditionVisitor::operator()(double l, double r) { return l + r; }

Value AdditionVisitor::operator()(std::string &l, std::string &r) {
  return l + r;
}

Value AdditionVisitor::operator()(auto &&l, auto &&r) {
  throw InterpretException(
      "Unable to add types: " + std::string(typeid(l).name()) + " and " +
      std::string(typeid(r).name()));
}

double SubtractionVisitor::operator()(double l, double r) { return l - r; }

double SubtractionVisitor::operator()(auto &&l, auto &&r) {
  throw InterpretException(
      "Unable to subtract types: " + std::string(typeid(l).name()) + " and " +
      std::string(typeid(r).name()));
}

double MultiplyVisitor::operator()(double l, double r) { return l * r; }

double MultiplyVisitor::operator()(auto &&l, auto &&r) {
  throw InterpretException(
      "Unable to multiply types: " + std::string(typeid(l).name()) + " and " +
      std::string(typeid(r).name()));
}

double DivideVisitor::operator()(double l, double r) { return l / r; }

double DivideVisitor::operator()(auto &&l, auto &&r) {
  throw InterpretException(
      "Unable to divide types: " + std::string(typeid(l).name()) + " and " +
      std::string(typeid(r).name()));
};

bool TruthyVisitor::operator()(std::monostate) { return false; }

bool TruthyVisitor::operator()(bool b) { return b; }

bool TruthyVisitor::operator()(double d) { return static_cast<bool>(d); }

bool TruthyVisitor::operator()(auto &&) { return true; }

} // namespace treewalk
} // namespace plox