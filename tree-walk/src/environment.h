#ifndef PLOX_ENVIRONMENT
#define PLOX_ENVIRONMENT

#include <value.h>

#include <map>
#include <memory>
#include <string>
#include <variant>

namespace plox {
namespace treewalk {

/*
 Environment is a class to store variables within the program.

 Environments can represent a whole scope, or part of a scope. An example where
 representing just part of a scope can be useful is for a function declaration,
 where the function should only be only aware of scope variables defined before
 the function is declared.

 Environments are chained as Directed Acyclic Graphs.
*/

class Environment {
public:
  // Factories
  static std::shared_ptr<Environment>
  create(std::shared_ptr<Environment> parent = nullptr);
  static std::shared_ptr<Environment>
  extend(std::shared_ptr<Environment> scope);

  // Operations
  void assign(const std::string &name, const Value &v);
  void define(const std::string &name, const Value &v = {});
  Value get(const std::string &name) const;

private:
  Environment(std::shared_ptr<Environment> parent);

  bool isVarInScope(const std::string &name) const;

  std::map<std::string, Value> d_map;
  std::shared_ptr<Environment> d_parent;
  bool d_isScopeStart;
  bool d_isScopeEnd;
};

namespace environmentutils {
// An RAII class to swap the environments in 2 shared pointers when the
// constructor or destructor is called
class ScopedSwap {
public:
  ScopedSwap(std::shared_ptr<Environment> &a, std::shared_ptr<Environment> &b);
  ~ScopedSwap();

private:
  std::shared_ptr<Environment> &d_a;
  std::shared_ptr<Environment> &d_b;
};
} // namespace environmentutils

} // namespace treewalk
} // namespace plox

#endif