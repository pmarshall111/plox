#ifndef PLOX_VALUEPRINTER_H
#define PLOX_VALUEPRINTER_H

#include <value.h>

#include <class.h>
#include <func.h>

#include <memory>
#include <sstream>
#include <string>
#include <variant>

namespace plox {
namespace treewalk {

using ClsInstShrdPtr = std::shared_ptr<ClassInstance>;
using ClsDefShrdPtr = std::shared_ptr<ClassDefinition>;
using FnDescShrdPtr = std::shared_ptr<FunctionDescription>;

using Value = std::variant<std::monostate, std::string, bool, double,
                           FnDescShrdPtr, ClsDefShrdPtr, ClsInstShrdPtr>;

// Concepts to control which template method should be chosen
template <typename T>
concept SharedPtr =
    std::same_as<std::decay_t<T>,
                 std::shared_ptr<typename std::decay_t<T>::element_type>>;
template <typename T>
concept NotSharedPtr = !SharedPtr<T>;

struct ValuePrinter {
  std::string operator()(std::monostate);

  template <NotSharedPtr T> std::string operator()(T &&streamableType) {
    std::ostringstream ss;
    ss << streamableType;
    return ss.str();
  }

  template <SharedPtr T> std::string operator()(T &&streamableTypePtr) {
    if (streamableTypePtr) {
      return operator()(*streamableTypePtr);
    }
    return "nullptr";
  }
};

} // namespace treewalk
} // namespace plox

#endif