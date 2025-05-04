#ifndef PLOX_INTERPRETER
#define PLOX_INTERPRETER

#include <ast.h>
#include <err.h>

#include <memory>
#include <string>
#include <variant>

namespace plox {
namespace treewalk {

using Value = std::variant<std::monostate, std::string, bool, double>;

Value interpret(const std::unique_ptr<ast::Expr> &expr,
                std::vector<InterpretError> &errs);

namespace interpretutils {
std::string valueToString(const Value &v);
}

} // namespace treewalk
} // namespace plox

#endif