#include <stmt_printer.h>

#include <ast_printer.h>

namespace plox {
namespace treewalk {
namespace stmt {

namespace {
ast::PrinterVisitor g_astPrinterVisitor;
}

std::string PrinterVisitor::operator()(const Block &blk) {
  std::ostringstream ss;
  ss << "{";
  for (const auto &stmt : blk.stmts) {
    ss << std::visit(*this, *stmt) << ";";
  }
  ss << "}";
  return ss.str();
}

std::string PrinterVisitor::operator()(const For &forStmt) {
  std::ostringstream ss;
  ss << "for" << "(";
  if (forStmt.initialiser) {
    ss << std::visit(*this, *forStmt.initialiser);
  }
  ss << ";";
  if (forStmt.condition) {
    ss << std::visit(g_astPrinterVisitor, *forStmt.condition);
  }
  ss << ";";
  if (forStmt.incrementer) {
    ss << std::visit(g_astPrinterVisitor, *forStmt.incrementer);
  }
  ss << ")" << std::visit(*this, *forStmt.body);
  return ss.str();
}

std::string PrinterVisitor::operator()(const If &ifStmt) {
  std::ostringstream ss;
  ss << "if" << "(" << std::visit(g_astPrinterVisitor, *ifStmt.condition) << ")"
     << "(" << std::visit(*this, *ifStmt.ifBranch) << ")";
  if (ifStmt.elseBranch) {
    ss << "else" << "(" << std::visit(*this, *ifStmt.elseBranch) << ")";
  }
  return ss.str();
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

std::string PrinterVisitor::operator()(const While &whileStmt) {
  std::ostringstream ss;
  ss << "while" << "(" << std::visit(g_astPrinterVisitor, *whileStmt.condition)
     << ")" << "(" << std::visit(*this, *whileStmt.body) << ")";
  return ss.str();
}

} // namespace stmt
} // namespace treewalk
} // namespace plox