#include <interpreter.h>

#include <interpreter_visitor.h>

namespace plox {
namespace treewalk {

void interpret(std::vector<stmt::Stmt> &stmts,
               std::shared_ptr<Environment> &env,
               std::vector<InterpretException> &errs) {
  try {
    InterpreterVisitor v{env};
    for (auto &s : stmts) {
      std::visit(v, s);
    }
  } catch (const InterpretException &e) {
    errs.push_back(e);
  }
}

} // namespace treewalk
} // namespace plox