#include <errs.h>

namespace plox {
namespace treewalk {

std::string SyntaxException::formatMsg(const std::string &msg, int line) {
  std::ostringstream ss;
  ss << msg << " on line " << line;
  return ss.str();
}

std::ostream &operator<<(std::ostream &os, const std::runtime_error &ex) {
  os << "Message: " << ex.what();
  return os;
}

} // namespace treewalk
} // namespace plox
