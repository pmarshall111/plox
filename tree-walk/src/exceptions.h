#ifndef TREEWALK_EXCEPTIONS_H
#define TREEWALK_EXCEPTIONS_H

#include <exception>
#include <string>

namespace plox {
namespace treewalk {

class SyntaxException : public std::exception {
public:
  SyntaxException(std::string msg, int line);
  const char *what() const noexcept override;

private:
  std::string d_msg;
  int d_line;
};

} // namespace treewalk
} // namespace plox

#endif