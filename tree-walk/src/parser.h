#ifndef TREEWALK_PARSER_H
#define TREEWALK_PARSER_H

#include <ast.h>

#include <memory>

namespace plox {
namespace treewalk {

struct ParseError {
  std::string d_message;
};

std::ostream &operator<<(std::ostream &os, const ParseError &err);

std::unique_ptr<ast::Expr> parse(const std::vector<Token> &tokens,
                                 std::vector<ParseError> &errors);

} // namespace treewalk
} // namespace plox

#endif