#include <class.h>

namespace plox {
namespace treewalk {

ClassDefinition::ClassDefinition(std::string_view name,
                                 std::shared_ptr<Environment> closure,
                                 std::shared_ptr<ClassDefinition> super)
    : d_name(name), d_closure(closure), d_super(super){};

std::string_view ClassDefinition::getName() const { return d_name; }

std::shared_ptr<Environment> &ClassDefinition::getClosure() {
  return d_closure;
};

std::shared_ptr<ClassDefinition> ClassDefinition::getSuper() {
  return d_super;
};

std::ostream &operator<<(std::ostream &os, const ClassDefinition &cls) {
  os << "class " << cls.getName();
  return os;
}

ClassInstance::ClassInstance(std::string_view name,
                             std::shared_ptr<Environment> closure)
    : d_name(name), d_closure(closure){};

std::string_view ClassInstance::getName() const { return d_name; };

std::shared_ptr<Environment> &ClassInstance::getClosure() { return d_closure; };

std::ostream &operator<<(std::ostream &os, const ClassInstance &cls) {
  os << "class instance " << cls.getName();
  return os;
}

} // namespace treewalk
} // namespace plox
