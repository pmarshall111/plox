#ifndef PLOX_ENVIRONMENT
#define PLOX_ENVIRONMENT

#include <value.h>

#include <map>
#include <memory>
#include <string>
#include <variant>

namespace plox {
namespace treewalk {

class Environment {
public:
  Environment() = default;
  Environment(std::shared_ptr<Environment> &parent);

  void assign(const std::string &name, const Value &v);
  void define(const std::string &name, const Value &v = {});
  Value get(const std::string &name) const;
  std::shared_ptr<Environment> getParentScope() const;

private:
  std::map<std::string, Value> d_map;
  std::shared_ptr<Environment> d_parent;
};

} // namespace treewalk
} // namespace plox

#endif