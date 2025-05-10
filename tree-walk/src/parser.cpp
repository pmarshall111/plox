#include <parser.h>

// expression     → equality ;
// equality       → comparison ( ( "!=" | "==" ) comparison )* ;
// comparison     → term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
// term           → factor ( ( "-" | "+" ) factor )* ;
// factor         → unary ( ( "/" | "*" ) unary )* ;
// unary          → ( "!" | "-" ) unary
//                | primary ;
// primary        → NUMBER | STRING | "true" | "false" | "nil"
//                | "(" expression ")" ;

// Recursive descent parsing starts at the rule of lowest precedence. TODO: why

namespace plox {
namespace treewalk {

namespace {

std::unique_ptr<ast::Expr> expression(const std::vector<Token> &tokens,
                                      int &pos, std::vector<ParseError> &errs);

std::unique_ptr<ast::Expr> primary(const std::vector<Token> &tokens, int &pos,
                                   std::vector<ParseError> &errs) {
  switch (tokens[pos].type) {
  case TokenType::NUMBER:
  case TokenType::STRING:
  case TokenType::TRUE:
  case TokenType::FALSE:
  case TokenType::NUL: {
    TokenType type = tokens[pos].type;
    return std::make_unique<ast::Expr>(ast::Literal{tokens[pos++].value, type});
  }
  case TokenType::LEFT_PAREN: {
    auto grp =
        std::make_unique<ast::Expr>(ast::Grouping{expression(tokens, ++pos)});
    if (tokens[pos].type == TokenType::RIGHT_PAREN) {
      pos++;
      return grp;
    }
    errs.push_back({"No closing paren found!"});
    return std::unique_ptr<ast::Expr>();
  }
  default:
    errs.push_back({"Unknown token!"});
    return std::unique_ptr<ast::Expr>();
  }
}

std::unique_ptr<ast::Expr> unary(const std::vector<Token> &tokens, int &pos,
                                 std::vector<ParseError> &errs) {
  switch (tokens[pos].type) {
  case TokenType::BANG:
  case TokenType::MINUS: {
    const Token &tok = tokens[pos];
    return std::make_unique<ast::Expr>(
        ast::Unary{tok, unary(tokens, ++pos, errs)});
  }
  default:
    return primary(tokens, pos, errs);
  }
}

std::unique_ptr<ast::Expr> factor(const std::vector<Token> &tokens, int &pos,
                                  std::vector<ParseError> &errs) {
  std::unique_ptr<ast::Expr> leftOp = unary(tokens, pos, errs);
  switch (tokens[pos].type) {
  case TokenType::STAR:
  case TokenType::SLASH: {
    const Token &tok = tokens[pos];
    return std::make_unique<ast::Expr>(
        ast::Binary{std::move(leftOp), tok, factor(tokens, ++pos, errs)});
  }
  default:
    return leftOp;
  }
}

std::unique_ptr<ast::Expr> term(const std::vector<Token> &tokens, int &pos,
                                std::vector<ParseError> &errs) {
  std::unique_ptr<ast::Expr> leftOp = factor(tokens, pos, errs);
  switch (tokens[pos].type) {
  case TokenType::PLUS:
  case TokenType::MINUS: {
    const Token &tok = tokens[pos];
    return std::make_unique<ast::Expr>(
        ast::Binary{std::move(leftOp), tok, term(tokens, ++pos, errs)});
  }
  default:
    return leftOp;
  }
}

std::unique_ptr<ast::Expr> comparison(const std::vector<Token> &tokens,
                                      int &pos, std::vector<ParseError> &errs) {
  std::unique_ptr<ast::Expr> leftOp = term(tokens, pos, errs);
  switch (tokens[pos].type) {
  case TokenType::LESS:
  case TokenType::LESS_EQUAL:
  case TokenType::GREATER:
  case TokenType::GREATER_EQUAL: {
    const Token &tok = tokens[pos];
    return std::make_unique<ast::Expr>(
        ast::Binary{std::move(leftOp), tok, comparison(tokens, ++pos, errs)});
  }
  default:
    return leftOp;
  }
}

std::unique_ptr<ast::Expr> equality(const std::vector<Token> &tokens, int &pos,
                                    std::vector<ParseError> &errs) {
  std::unique_ptr<ast::Expr> leftOp = comparison(tokens, pos, errs);
  switch (tokens[pos].type) {
  case TokenType::BANG_EQUAL:
  case TokenType::EQUAL_EQUAL: {
    const Token &tok = tokens[pos];
    return std::make_unique<ast::Expr>(
        ast::Binary{std::move(leftOp), tok, equality(tokens, ++pos, errs)});
  }
  default:
    return leftOp;
  }
}

std::unique_ptr<ast::Expr> expression(const std::vector<Token> &tokens,
                                      int &pos, std::vector<ParseError> &errs) {
  return equality(tokens, pos, errs);
}

} // namespace

std::unique_ptr<ast::Expr> parse(const std::vector<Token> &tokens,
                                 std::vector<ParseError> &errs) {
  int pos = 0;
  return expression(tokens, pos, errs);
}

} // namespace treewalk
} // namespace plox