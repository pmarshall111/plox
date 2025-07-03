#include <errs.h>

namespace plox {
namespace treewalk {

namespace {
std::string formatMsg(const std::string &msg, int line) {
  std::ostringstream ss;
  ss << msg << " on line " << line;
  return ss.str();
}
} // namespace

ParseException::ParseException(const std::string &msg, int line)
    : std::runtime_error(formatMsg(msg, line)){};

SyntaxException::SyntaxException(const std::string &msg, int line)
    : std::runtime_error(formatMsg(msg, line)){};

std::ostream &operator<<(std::ostream &os, const std::runtime_error &ex) {
  os << "Message: " << ex.what();
  return os;
}

} // namespace treewalk
} // namespace plox
