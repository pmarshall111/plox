#ifndef PLOX_AST_PRINTER
#define PLOX_AST_PRINTER

#include <ast.h>

#include <memory>
#include <sstream>
#include <string>

namespace plox {
namespace treewalk {
namespace ast {

struct PrinterVisitor {
  std::string operator()(const Binary &bin);
  std::string operator()(const Grouping &grp);
  std::string operator()(const Literal &ltrl);
  std::string operator()(const Unary &unary);
};

} // namespace ast
} // namespace treewalk
} // namespace plox

#endif