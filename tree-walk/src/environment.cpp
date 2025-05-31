#include <environment.h>

#include <errs.h>

namespace plox {
namespace treewalk {

void Environment::assign(const std::string &name, Value v) {
  if (!d_map.contains(name)) {
    throw InterpretException("Cannot assign unknown variable: " + name);
  }

  d_map[name] = v;
}

void Environment::define(const std::string &name, Value v) {
  if (d_map.contains(name)) {
    throw InterpretException("Cannot redefine variable: " + name);
  }

  d_map[name] = v;
}

Value Environment::get(const std::string &name) const {
  if (!d_map.contains(name)) {
    throw InterpretException("Unknown variable: " + name);
  }

  return d_map.at(name);
}

} // namespace treewalk
} // namespace plox
