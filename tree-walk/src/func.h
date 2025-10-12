#ifndef TREEWALK_FUNC_H
#define TREEWALK_FUNC_H

#include <environment.h>
#include <func_native.h>
#include <interpreter.h>
#include <ownershiphelper.h>
#include <stmt.h>
#include <value.h>

#include <functional>
#include <string_view>
#include <vector>

namespace plox {
namespace treewalk {

class Function {
public:
  Function(std::vector<std::string_view> &&argNames,
           std::variant<std::vector<std::unique_ptr<stmt::Stmt>>,
                        nativefunc::Fn> &&body);

  int getArity() const;
  const std::vector<std::string_view> &getArgNames() const;
  Value execute(std::shared_ptr<Environment> env,
                InterpreterVisitor &interp) const;

private:
  std::vector<std::string_view> d_argNames;
  std::variant<std::vector<std::unique_ptr<stmt::Stmt>>, nativefunc::Fn> d_body;
};

class FunctionDescription {
public:
  FunctionDescription(std::string_view name, std::weak_ptr<Environment> closure,
                      std::shared_ptr<const Function> fn);

  std::string_view getName() const;
  void setName(std::string_view name);
  std::shared_ptr<Environment> getClosure();
  void setClosure(std::weak_ptr<Environment> closure);
  const std::shared_ptr<const Function> &getFunction() const;
  bool isInitialiser() const;
  void setIsInitialiser(bool b);
  void ownClosure();

private:
  std::string_view d_name;
  // d_closure will ordinarily be a weak_ptr to avoid memory leaks through
  // circular references. Note in the ordinary case the scope owns the function
  // through a strong ptr.
  //
  // parentEnv <-strong- fnEnv <-weak- Fn
  //     |                              ^
  //     |                              |
  //     +------------strong------------+
  //
  // When a function is returned from a scope, it must now own the environment
  // to keep it alive. In this case d_closure will be a shared_ptr.
  //
  // parentEnv <-strong- fnEnv <-strong- Fn
  //     |                                ^
  //     |                                |
  //     +------------weak----------------+
  //
  std::shared_ptr<OwnershipHelper<Environment>> d_closure;
  std::shared_ptr<const Function> d_fn;
  bool d_isInitialiser;
};

std::ostream &operator<<(std::ostream &os, const Function &fn);
std::ostream &operator<<(std::ostream &os, const FunctionDescription &fn);

} // namespace treewalk
} // namespace plox

#endif