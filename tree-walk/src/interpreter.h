#ifndef PLOX_INTERPRETER
#define PLOX_INTERPRETER

#include <environment.h>
#include <errs.h>
#include <stmt.h>

#include <vector>

namespace plox {
namespace treewalk {

// The entrypoint to Lox
void interpret(std::vector<stmt::Stmt> &stmts,
               std::shared_ptr<Environment> &env,
               std::vector<InterpretException> &errs);


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