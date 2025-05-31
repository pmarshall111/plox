#include <stmt_printer.h>

#include <ast_printer.h>

namespace plox {
namespace treewalk {
namespace stmt {

namespace {
ast::PrinterVisitor g_astPrinterVisitor;
}

std::string PrinterVisitor::operator()(const Expression &expr) {
  return std::visit(g_astPrinterVisitor, *expr.expr);
}

std::string PrinterVisitor::operator()(const Print &print) {
  std::ostringstream ss;
  ss << "print " << std::visit(g_astPrinterVisitor, *print.expr);
  return ss.str();
}

std::string PrinterVisitor::operator()(const VarDecl &varDecl) {
  std::ostringstream ss;
  ss << "var " << varDecl.name;
  if (varDecl.expr) {
    ss << " = " << std::visit(g_astPrinterVisitor, *varDecl.expr);
  }
  return ss.str();
}

} // namespace stmt
} // namespace treewalk
} // namespace plox