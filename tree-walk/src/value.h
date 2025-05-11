#ifndef PLOX_VALUE
#define PLOX_VALUE

#include <map>
#include <sstream>
#include <string>
#include <variant>

namespace plox {
namespace treewalk {

using Value = std::variant<std::monostate, std::string, bool, double>;

struct ValuePrinter {
  std::string operator()(std::monostate);
  std::string operator()(auto &&simpleType) {
    std::ostringstream ss;
    ss << simpleType;
    return ss.str();
  }
};

} // namespace treewalk
} // namespace plox

#endif