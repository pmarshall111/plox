#include <interpreter.h>

#include <charconv>

namespace plox {
namespace treewalk {

using namespace ast;

namespace {
// Visitors are defined for each interpret operation.

struct InterpreterVisitor {
  Value operator()(const Binary &bin);
  Value operator()(const Grouping &grp);
  Value operator()(const Literal &ltrl);
  Value operator()(const Unary &unary);
} g_interpreter;

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

// A wrapper around InterpretError. This is a solution to not being able to
// call std::visit() with args that are not std::variant's.
// Using exceptions also helps to stop executing a bad statement.
class InterpretException : public std::exception {
public:
  explicit InterpretException(const InterpretError &err) : d_err(err) {}

  const char *what() const noexcept override {
    static std::ostringstream ss;
    ss.clear();
    ss << d_err;
    return ss.str().c_str();
  }

  const InterpretError &getErr() const noexcept { return d_err; }

private:
  InterpretError d_err;
};

Value interpretOrThrow(const std::unique_ptr<ast::Expr> &expr) {
  // Helper function since `interpret()` expects an errs vector
  std::vector<InterpretError> errs;
  Value val = interpret(expr, errs);
  if (errs.size()) {
    throw InterpretException{errs[0]};
  }
  return val;
}

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

Value InterpreterVisitor::operator()(const Binary &bnry) {
  Value lhs = std::visit(g_interpreter, *bnry.left);
  Value rhs = std::visit(g_interpreter, *bnry.right);

  switch (bnry.op.getType()) {
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
                              tokenutils::tokenTypeToStr(bnry.op.getType())}};
  }
}

Value InterpreterVisitor::operator()(const Unary &unry) {
  Value right = interpretOrThrow(unry.right);
  switch (unry.op.getType()) {
  case TokenType::MINUS: {
    Value zero = 0.0;
    return std::visit(g_subtractor, zero, right);
  }
  case TokenType::BANG:
    return !std::visit(g_truther, right);
  default:
    throw InterpretException{{"Unable to interpret unary op: " +
                              tokenutils::tokenTypeToStr(unry.op.getType())}};
  }
}

Value InterpreterVisitor::operator()(const Grouping &grp) {
  return interpretOrThrow(grp.expr);
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

Value interpret(const std::unique_ptr<ast::Expr> &expr,
                std::vector<InterpretError> &errs) {
  try {
    return std::visit(g_interpreter, *expr);
  } catch (const InterpretException &e) {
    errs.push_back({e.what()});
    return {};
  }
}

} // namespace treewalk
} // namespace plox