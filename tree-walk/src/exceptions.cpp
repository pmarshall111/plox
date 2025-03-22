#include <exceptions.h>

namespace plox {
namespace treewalk {

SyntaxException::SyntaxException(const std::string msg, int line)
    : d_msg(msg), d_line(line){};

const char *SyntaxException::what() const noexcept { return d_msg.c_str(); }

} // namespace treewalk
} // namespace plox