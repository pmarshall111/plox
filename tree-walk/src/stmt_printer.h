#ifndef PLOX_STMT_PRINTER
#define PLOX_STMT_PRINTER

#include <stmt.h>

#include <string>

namespace plox {
namespace treewalk {
namespace stmt {

struct PrinterVisitor {
  std::string operator()(const Print &print);
  std::string operator()(const Expression &expr);
};

} // namespace stmt
} // namespace treewalk
} // namespace plox

#endif