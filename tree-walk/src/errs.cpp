#include <errs.h>

namespace plox {
namespace treewalk {

std::ostream &operator<<(std::ostream &os, const ParseError &err) {
  os << "Message: " << err.d_msg;
  return os;
}

std::ostream &operator<<(std::ostream &os, const SyntaxError &err) {
  os << "Line: " << err.d_line << ". Message: " << err.d_msg;
  return os;
}

std::ostream &operator<<(std::ostream &os, const InterpretError &err) {
  os << "Message: " << err.d_msg;
  return os;
}

} // namespace treewalk
} // namespace plox
