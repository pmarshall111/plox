#ifndef TREEWALK_FUNC_H
#define TREEWALK_FUNC_H

#include <environment.h>
#include <func_native.h>
#include <interpreter.h>
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
  Value execute(std::shared_ptr<Environment> env, InterpreterVisitor &interp);

private:
  std::vector<std::string_view> d_argNames;
  std::variant<std::vector<std::unique_ptr<stmt::Stmt>>, nativefunc::Fn> d_body;
};

class FunctionClosure {
public:
  FunctionClosure(std::string_view name, std::shared_ptr<Environment> closure,
                  std::shared_ptr<Function> fn);

  std::string_view getName() const;
  std::shared_ptr<Environment> &getClosure();
  std::shared_ptr<Function> &getFunction(); // TODO: make const

private:
  std::string_view d_name;
  std::shared_ptr<Environment> d_closure;
  std::shared_ptr<Function> d_fn;
};

std::ostream &operator<<(std::ostream &os, const Function &fn);
std::ostream &operator<<(std::ostream &os,
                         FunctionClosure &fn); // TODO: make const

} // namespace treewalk
} // namespace plox

#endif