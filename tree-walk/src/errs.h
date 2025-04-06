#ifndef TREEWALK_EXCEPTIONS_H
#define TREEWALK_EXCEPTIONS_H

#include <sstream>
#include <string>

namespace plox {
namespace treewalk {

struct ParseError {
  std::string d_msg;
};

struct SyntaxError {
  std::string d_msg;
  int d_line;
};

std::ostream &operator<<(std::ostream &os, const ParseError &err);
std::ostream &operator<<(std::ostream &os, const SyntaxError &err);

} // namespace treewalk
} // namespace plox

#endif