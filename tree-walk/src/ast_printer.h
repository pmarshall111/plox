#ifndef PLOX_AST_PRINTER
#define PLOX_AST_PRINTER

#include <ast.h>

#include <string>

namespace plox {
namespace treewalk {
namespace ast {

struct PrinterVisitor {
  std::string operator()(const Assign &assign);
  std::string operator()(const Binary &bin);
  std::string operator()(const Call &call);
  std::string operator()(const Get &get);
  std::string operator()(const Grouping &grp);
  std::string operator()(const Literal &ltrl);
  std::string operator()(const Set &set);
  std::string operator()(const Unary &unary);
  std::string operator()(const Variable &var);
};

} // namespace ast
} // namespace treewalk
} // namespace plox

#endif