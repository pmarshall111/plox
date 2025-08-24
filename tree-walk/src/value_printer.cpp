#include <value_printer.h>

#include <sstream>

namespace plox {
namespace treewalk {

std::string ValuePrinter::operator()(std::monostate) { return "NULL"; }

} // namespace treewalk
} // namespace plox
