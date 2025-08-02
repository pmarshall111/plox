#ifndef PLOX_STMT_PRINTER
#define PLOX_STMT_PRINTER

#include <stmt.h>

#include <string>

namespace plox {
namespace treewalk {
namespace stmt {

struct PrinterVisitor {
  std::string operator()(const Block &blk);
  std::string operator()(const For &forStmt);
  std::string operator()(const Fun &funStmt);
  std::string operator()(const If &ifStmt);
  std::string operator()(const Expression &expr);
  std::string operator()(const Print &print);
  std::string operator()(const Return &ret);
  std::string operator()(const VarDecl &varDecl);
  std::string operator()(const While &whileStmt);
};

} // namespace stmt
} // namespace treewalk
} // namespace plox

#endif