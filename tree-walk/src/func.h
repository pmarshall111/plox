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
  Function(std::string_view name, std::vector<std::string_view> &&argNames,
           std::shared_ptr<Environment> closure,
           std::variant<std::vector<std::unique_ptr<stmt::Stmt>>,
                        nativefunc::Fn> &&body);

  std::string_view getName() const;
  int getArity() const;
  const std::vector<std::string_view> &getArgNames() const;
  std::shared_ptr<Environment> &getClosure();
  Value execute(std::shared_ptr<Environment> env, InterpreterVisitor &interp);

private:
  std::string_view d_name;
  std::vector<std::string_view> d_argNames;
  std::shared_ptr<Environment> d_closure;
  std::variant<std::vector<std::unique_ptr<stmt::Stmt>>, nativefunc::Fn> d_body;
};

std::ostream &operator<<(std::ostream &os, const Function &fn);

} // namespace treewalk
} // namespace plox

#endif