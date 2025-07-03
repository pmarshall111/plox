#ifndef TREEWALK_EXCEPTIONS_H
#define TREEWALK_EXCEPTIONS_H

#include <sstream>
#include <string>

namespace plox {
namespace treewalk {

class InterpretException : public std::runtime_error {
public:
  explicit InterpretException(const std::string &msg)
      : std::runtime_error(msg){};
};

class ParseException : public std::runtime_error {
public:
  explicit ParseException(const std::string &msg, int line);
};

class SyntaxException : public std::runtime_error {
public:
  explicit SyntaxException(const std::string &msg, int line);
};

std::ostream &operator<<(std::ostream &os, const std::runtime_error &err);

} // namespace treewalk
} // namespace plox

#endif