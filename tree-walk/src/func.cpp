#include <func.h>

#include <value.h>

#include <sstream>

namespace plox {
namespace treewalk {

Function::Function(std::vector<std::string_view> &&argNames,
                   std::variant<std::vector<std::unique_ptr<stmt::Stmt>>,
                                nativefunc::Fn> &&body)
    : d_argNames(std::move(argNames)), d_body(std::move(body)) {}

int Function::getArity() const { return d_argNames.size(); }

const std::vector<std::string_view> &Function::getArgNames() const {
  return d_argNames;
}

Value Function::execute(std::shared_ptr<Environment> env,
                        InterpreterVisitor &interp) {
  if (std::holds_alternative<nativefunc::Fn>(d_body)) {
    return std::get<nativefunc::Fn>(d_body)(env, interp);
  }

  auto &stmtVec = std::get<std::vector<std::unique_ptr<stmt::Stmt>>>(d_body);
  for (auto &s : stmtVec) {
    std::visit(interp, *s);
  }
  return {}; // return null if the user doesn't explicitly add a return stmt.
}

FunctionClosure::FunctionClosure(std::string_view name,
                                 std::shared_ptr<Environment> closure,
                                 std::shared_ptr<Function> fn)
    : d_name(name), d_closure(closure), d_fn(fn) {}

std::string_view FunctionClosure::getName() const { return d_name; }

void FunctionClosure::setName(std::string_view name) { d_name = name; }

std::shared_ptr<Environment> &FunctionClosure::getClosure() {
  return d_closure;
}

std::shared_ptr<Function> &FunctionClosure::getFunction() { return d_fn; }

bool FunctionClosure::isInitialiser() const { return d_isInitialiser; }

void FunctionClosure::setIsInitialiser(bool b) { d_isInitialiser = b; }

std::ostream &operator<<(std::ostream &os, const Function &fn) {
  os << "(";

  auto &args = fn.getArgNames();
  if (args.size()) {
    os << args[0];
  }
  for (int i = 1; i < args.size(); ++i) {
    os << ", " << args[i];
  }

  os << ")";
  return os;
}

std::ostream &operator<<(std::ostream &os, FunctionClosure &fnCl) {
  os << "fun " << fnCl.getName() << *fnCl.getFunction();
  return os;
}

} // namespace treewalk
} // namespace plox
