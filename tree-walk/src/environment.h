#ifndef PLOX_ENVIRONMENT
#define PLOX_ENVIRONMENT

#include <value.h>

#include <map>
#include <string>
#include <variant>

namespace plox {
namespace treewalk {

class Environment {
public:
  Environment() = default;

  void set(const std::string &name, Value v = {});
  Value get(const std::string &name) const;

private:
  std::map<std::string, Value> d_map;
};

} // namespace treewalk
} // namespace plox

#endif