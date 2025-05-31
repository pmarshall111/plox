#include <ast_printer.h>

namespace plox {
namespace treewalk {
namespace ast {
namespace {

std::string addParens(const std::string &str) { return "(" + str + ")"; }
} // namespace

std::string PrinterVisitor::operator()(const Assign &assign) {
  std::ostringstream ss;
  ss << assign.name << "=" << std::visit(*this, *assign.value);
  return addParens(ss.str());
}

std::string PrinterVisitor::operator()(const Binary &bin) {
  std::ostringstream ss;
  ss << std::visit(*this, *bin.left) << bin.op.value
     << std::visit(*this, *bin.right);
  return addParens(ss.str());
}

std::string PrinterVisitor::operator()(const Grouping &grp) {
  std::ostringstream ss;
  ss << "group " << std::visit(*this, *grp.expr);
  return addParens(ss.str());
}

std::string PrinterVisitor::operator()(const Literal &ltrl) {
  return std::string(ltrl.value);
}

std::string PrinterVisitor::operator()(const Unary &unary) {
  std::ostringstream ss;
  ss << unary.op.value << std::visit(*this, *unary.right);
  return addParens(ss.str());
}

std::string PrinterVisitor::operator()(const Variable &var) {
  std::ostringstream ss;
  ss << "var " << var.name;
  return addParens(ss.str());
}

} // namespace ast
} // namespace treewalk
} // namespace plox