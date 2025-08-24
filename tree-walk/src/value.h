#ifndef PLOX_VALUE_H
#define PLOX_VALUE_H

#include <memory>
#include <string>
#include <variant>

namespace plox {
namespace treewalk {

// Forward declarations for ptrs to prevent circular deps
struct ClassFactory;
using ClsFactShrdPtr = std::shared_ptr<ClassFactory>;
struct Function;
using FuncShrdPtr = std::shared_ptr<Function>;

using Value = std::variant<std::monostate, std::string, bool, double,
                           FuncShrdPtr, ClsFactShrdPtr>;

} // namespace treewalk
} // namespace plox

#endif