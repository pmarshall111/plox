#include <environment.h>

#include <errs.h>

namespace plox {
namespace treewalk {

void Environment::set(const std::string &name, Value v) { d_map[name] = v; }

Value Environment::get(const std::string &name) const {
  if (!d_map.contains(name)) {
    throw InterpretException("Unknown variable: " + name);
  }

  return d_map.at(name);
}

} // namespace treewalk
} // namespace plox
