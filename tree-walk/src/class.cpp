#include <class.h>

namespace plox {
namespace treewalk {

ClassFactory::ClassFactory(std::string_view name,
                           std::shared_ptr<Environment> closure)
    : d_name(name), d_closure(closure){};

std::string_view ClassFactory::getName() const { return d_name; }

std::ostream &operator<<(std::ostream &os, const ClassFactory &cls) {
  os << "class " << cls.getName();
  return os;
}

} // namespace treewalk
} // namespace plox
