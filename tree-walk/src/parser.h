#ifndef TREEWALK_PARSER_H
#define TREEWALK_PARSER_H

#include <scanner.h>
#include <stmt.h>

#include <memory>

namespace plox {
namespace treewalk {

std::vector<stmt::Stmt> parse(const std::vector<Token> &tokens,
                              std::vector<ParseError> &errs);

} // namespace treewalk
} // namespace plox

#endif