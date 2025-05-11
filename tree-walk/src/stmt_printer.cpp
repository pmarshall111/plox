#include <stmt_printer.h>

#include <ast_printer.h>

namespace plox {
namespace treewalk {
namespace stmt {

namespace {
ast::PrinterVisitor g_astPrinterVisitor;
}

std::string PrinterVisitor::operator()(const Print &print) {
  return std::visit(g_astPrinterVisitor, *print.expr);
}

std::string PrinterVisitor::operator()(const Expression &expr) {
  return std::visit(g_astPrinterVisitor, *expr.expr);
}

} // namespace stmt
} // namespace treewalk
} // namespace plox