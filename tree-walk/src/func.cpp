#include <func.h>

#include <value.h>

#include <sstream>

namespace plox {
namespace treewalk {

Function::Function(const std::string_view name,
                   std::vector<std::string_view> &&argNames,
                   std::shared_ptr<Environment> closure,
                   std::variant<std::vector<std::unique_ptr<stmt::Stmt>>,
                                nativefunc::Fn> &&body)
    : d_name(name), d_argNames(std::move(argNames)), d_closure(closure),
      d_body(std::move(body)) {}

std::string_view Function::getName() const { return d_name; }

int Function::getArity() const { return d_argNames.size(); }

const std::vector<std::string_view> &Function::getArgNames() const {
  return d_argNames;
}

std::shared_ptr<Environment> &Function::getClosure() { return d_closure; }

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

std::ostream &operator<<(std::ostream &os, const Function &fn) {
  // TODO: Add argument signature
  os << fn.getName() << "()";
  return os;
}

} // namespace treewalk
} // namespace plox
