#ifndef TREEWALK_CLASS_H
#define TREEWALK_CLASS_H

#include <environment.h>

#include <map>
#include <string_view>

namespace plox {
namespace treewalk {

class ClassFactory {
public:
  ClassFactory(std::string_view name, std::shared_ptr<Environment> closure);

  std::string_view getName() const;
  std::shared_ptr<Environment> &getClosure();

private:
  std::string_view d_name;
  std::shared_ptr<Environment> d_closure;
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

std::ostream &operator<<(std::ostream &os, const ClassFactory &cls);
std::ostream &operator<<(std::ostream &os, const ClassInstance &cls);

} // namespace treewalk
} // namespace plox

#endif