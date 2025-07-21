#ifndef PLOX_INTERPRETER_VISITOR
#define PLOX_INTERPRETER_VISITOR

#include <environment.h>
#include <stmt.h>

namespace plox {
namespace treewalk {

// Visitors are defined for each interpret operation.
struct InterpreterVisitor {
  InterpreterVisitor(std::shared_ptr<Environment> &env);

  // Statements do not need to return anything
  void operator()(const stmt::Block &blk);
  void operator()(const stmt::Expression &expr);
  void operator()(const stmt::For &forStmt);
  void operator()(stmt::Fun &funStmt);
  void operator()(const stmt::If &ifStmt);
  void operator()(const stmt::Print &print);
  void operator()(const stmt::Return &ret);
  void operator()(const stmt::VarDecl &varDecl);
  void operator()(const stmt::While &whileStmt);
  // Other operations called by statements return Values
  Value operator()(const ast::Assign &assign);
  Value operator()(const ast::Binary &bin);
  Value operator()(const ast::Call &call);
  Value operator()(const ast::Grouping &grp);
  Value operator()(const ast::Literal &ltrl);
  Value operator()(const ast::Unary &unary);
  Value operator()(const ast::Variable &var);

private:
  std::shared_ptr<Environment> d_env;
};

} // namespace treewalk
} // namespace plox

#endif