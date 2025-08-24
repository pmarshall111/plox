#ifndef PLOX_VALUE_H
#define PLOX_VALUE_H

#include <memory>
#include <string>
#include <variant>

namespace plox {
namespace treewalk {

// Forward declarations for ptrs to prevent circular deps
struct ClassInstance;
using ClsInstShrdPtr = std::shared_ptr<ClassInstance>;
struct ClassFactory;
using ClsFactShrdPtr = std::shared_ptr<ClassFactory>;
struct FunctionClosure;
using FnClosureShrdPtr = std::shared_ptr<FunctionClosure>;

using Value = std::variant<std::monostate, std::string, bool, double,
                           FnClosureShrdPtr, ClsFactShrdPtr, ClsInstShrdPtr>;

} // namespace treewalk
} // namespace plox

#endif