#ifndef PLOX_INTERPRETER
#define PLOX_INTERPRETER

#include <environment.h>
#include <errs.h>
#include <stmt.h>

#include <vector>

namespace plox {
namespace treewalk {

void interpret(const std::vector<stmt::Stmt> &stmts,
               std::shared_ptr<Environment> &env,
               std::vector<InterpretException> &errs);

} // namespace treewalk
} // namespace plox

#endif