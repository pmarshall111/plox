#include <environment.h>

#include <errs.h>

namespace plox {
namespace treewalk {

Environment::Environment(std::shared_ptr<Environment> &parent)
    : d_parent(parent) {}

void Environment::assign(const std::string &name, const Value &v) {
  // Assignment dictates the var must already exist
  if (d_map.contains(name)) {
    d_map[name] = v;
  } else if (d_parent) {
    d_parent->assign(name, v);
  } else {
    throw InterpretException("Cannot assign unknown variable: " + name);
  }
}

void Environment::define(const std::string &name, const Value &v) {
  // For defining a variable Lox allows shadowing variables in higher scopes.
  // Therefore no need to check parent scopes.
  if (d_map.contains(name)) {
    throw InterpretException("Cannot redefine variable: " + name);
  }

  d_map[name] = v;
}

Value Environment::get(const std::string &name) const {
  if (d_map.contains(name)) {
    return d_map.at(name);
  } else if (d_parent) {
    return d_parent->get(name);
  } else {
    throw InterpretException("Unknown variable: " + name);
  }
}

std::shared_ptr<Environment> Environment::getParentScope() const {
  return d_parent;
}

} // namespace treewalk
} // namespace plox
