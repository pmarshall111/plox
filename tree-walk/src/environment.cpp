#include <environment.h>

#include <errs.h>

#include <assert.h>

namespace plox {
namespace treewalk {

std::shared_ptr<Environment>
Environment::create(std::shared_ptr<Environment> parent) {
  auto envPtr = std::shared_ptr<Environment>(new Environment(parent));
  return envPtr;
}

std::shared_ptr<Environment>
Environment::extend(std::shared_ptr<Environment> scope) {
  scope->d_isScopeEnd = false;

  auto envPtr = std::shared_ptr<Environment>(new Environment(scope));
  envPtr->d_isScopeStart = false;
  return envPtr;
}

Environment::Environment(std::shared_ptr<Environment> parent)
    : d_parent(parent), d_isScopeStart(true), d_isScopeEnd(true) {}

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
  if (!d_isScopeEnd) {
    throw InterpretException(
        "Internal Lox error: Tried to define a variable '" + name +
        "' in a non-tail Environment for the scope.");
  }

  // For defining a variable Lox allows shadowing variables in higher scopes.
  // We only need to check within this scope
  if (isVarInScope(name)) {
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

bool Environment::isVarInScope(const std::string &name) const {
  // Check if var is in current env
  if (d_map.contains(name)) {
    return true;
  }

  // Continue recursing until we hit the start of the scope
  if (!d_isScopeStart && d_parent) {
    return d_parent->isVarInScope(name);
  }

  // We're at the start of the scope and didn't find the var
  return false;
}

Environment::const_iterator::const_iterator(
    std::shared_ptr<const Environment> env, bool isEnd)
    : d_env(env) {
  assert(env != nullptr);

  if (isEnd) {
    while (d_env->d_parent) {
      d_env = d_env->d_parent;
    }
    d_current = d_env->d_map.cend();
  } else {
    while (d_env->d_map.size() == 0 && d_env->d_parent) {
      d_env = d_env->d_parent;
    }
    d_current = d_env->d_map.cbegin();
  }
}

const std::pair<const std::string, Value> &
Environment::const_iterator::operator*() const {
  return *d_current;
}

Environment::const_iterator &Environment::const_iterator::operator++() {
  if (++d_current != d_env->d_map.cend()) {
    return *this;
  }

  while (d_current == d_env->d_map.cend() && d_env->d_parent) {
    d_env = d_env->d_parent;
    d_current = d_env->d_map.cbegin();
  }
  return *this;
}

Environment::const_iterator Environment::const_iterator::operator++(int) {
  Environment::const_iterator cpy = *this;
  if (++d_current != d_env->d_map.cend()) {
    return cpy;
  }

  while (d_current == d_env->d_map.cend() && d_env->d_parent) {
    d_env = d_env->d_parent;
    d_current = d_env->d_map.cbegin();
  }
  return cpy;
}

Environment::const_iterator Environment::begin() const {
  return Environment::const_iterator(shared_from_this(), false);
}

Environment::const_iterator Environment::end() const {
  return Environment::const_iterator(shared_from_this(), true);
}

namespace environmentutils {
ScopedSwap::ScopedSwap(std::shared_ptr<Environment> &a,
                       std::shared_ptr<Environment> &b)
    : d_a(a), d_b(b) {
  std::swap(d_a, d_b);
}
ScopedSwap::~ScopedSwap() { std::swap(d_a, d_b); }
} // namespace environmentutils

} // namespace treewalk
} // namespace plox
