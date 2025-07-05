#ifndef TREEWALK_FUNC_H
#define TREEWALK_FUNC_H

#include <environment.h>
#include <stmt.h>
#include <value.h>

#include <string_view>
#include <vector>

namespace plox {
namespace treewalk {

class Function {
public:
  Function(std::string_view name, std::vector<std::string_view> &&argNames,
           std::shared_ptr<Environment> closure,
           std::vector<stmt::Stmt> &&body);

  std::string_view getName() const;
  int getArity() const;
  const std::vector<std::string_view> &getArgNames() const;
  std::shared_ptr<Environment> &
  getClosure(); // TODO: Not best practice to expose?
  const std::vector<stmt::Stmt> &getBody()
      const; // TODO: Do I need the type in the vector here to also be const?

private:
  std::string_view d_name;
  std::vector<std::string_view> d_argNames;
  std::shared_ptr<Environment> d_closure;
  std::vector<stmt::Stmt> d_body;
};

std::ostream &operator<<(std::ostream &os, const Function &fn);

} // namespace treewalk
} // namespace plox

#endif