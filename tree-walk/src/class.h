#ifndef TREEWALK_CLASS_H
#define TREEWALK_CLASS_H

#include <environment.h>

#include <map>
#include <string_view>

namespace plox {
namespace treewalk {

class ClassDefinition {
public:
  ClassDefinition(std::string_view name, std::shared_ptr<Environment> closure,
                  std::shared_ptr<ClassDefinition> d_super);

  std::string_view getName() const;
  std::shared_ptr<Environment> &getClosure();
  std::shared_ptr<ClassDefinition> getSuper();

private:
  std::string_view d_name;
  std::shared_ptr<Environment> d_closure;
  std::shared_ptr<ClassDefinition> d_super;
};

class ClassInstance {
public:
  ClassInstance(std::string_view name, std::shared_ptr<Environment> closure);

  std::string_view getName() const;
  std::shared_ptr<Environment> &getClosure();

private:
  std::string_view d_name;
  std::shared_ptr<Environment> d_closure;
};

std::ostream &operator<<(std::ostream &os, const ClassDefinition &cls);
std::ostream &operator<<(std::ostream &os, const ClassInstance &cls);

} // namespace treewalk
} // namespace plox

#endif