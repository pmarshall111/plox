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
struct ClassDefinition;
using ClsDefShrdPtr = std::shared_ptr<ClassDefinition>;
struct FunctionDescription;
template <typename T> struct OwnershipHelper;
using FnDescOwnrHlpr = std::shared_ptr<OwnershipHelper<FunctionDescription>>;

using Value = std::variant<std::monostate, std::string, bool, double,
                           FnDescOwnrHlpr, ClsDefShrdPtr, ClsInstShrdPtr>;

} // namespace treewalk
} // namespace plox

#endif