#ifndef PLOX_INTERPRETER
#define PLOX_INTERPRETER

#include <err.h>
#include <stmt.h>

#include <memory>
#include <string>
#include <variant>

namespace plox {
namespace treewalk {

using Value = std::variant<std::monostate, std::string, bool, double>;

Value interpret(const std::vector<stmt::Stmt> &stmts,
                std::vector<InterpretError> &errs);

} // namespace treewalk
} // namespace plox

#endif