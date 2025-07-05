#ifndef PLOX_VALUE
#define PLOX_VALUE

#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <variant>

namespace plox {
namespace treewalk {

struct Function;
using FuncShrdPtr = std::shared_ptr<Function>;
using Value =
    std::variant<std::monostate, std::string, bool, double, FuncShrdPtr>;

struct ValuePrinter {
  std::string operator()(std::monostate);

  std::string operator()(auto &&streamableType) {
    std::ostringstream ss;
    ss << streamableType;
    return ss.str();
  }

  std::string operator()(const std::shared_ptr<auto> &streamableTypePtr) {

    if (streamableTypePtr) {
      return operator()(*streamableTypePtr);
    }
    return "nullptr";
  }
};

} // namespace treewalk
} // namespace plox

#endif