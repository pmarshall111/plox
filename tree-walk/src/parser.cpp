#include <parser.h>

// program        → statement* EOF ;
//
// statement      → exprStmt
//                | printStmt
//                | varStmt ;
//
// varStmt        → "var" IDENTIFIER ( "=" expression )? ";" ;
// exprStmt       → expression ";" ;
// printStmt      → "print" expression ";" ;

// expression     → equality ;
// equality       → comparison ( ( "!=" | "==" ) comparison )* ;
// comparison     → term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
// term           → factor ( ( "-" | "+" ) factor )* ;
// factor         → unary ( ( "/" | "*" ) unary )* ;
// unary          → ( "!" | "-" ) unary
//                | primary ;
// primary        → NUMBER | STRING | "true" | "false" | "nil"
//                | "(" expression ")" | IDENTIFIER;

// Recursive descent parsing starts at the rule of lowest precedence. TODO: why

namespace plox {
namespace treewalk {

namespace {

// A wrapper around ParseError. Using exceptions helps to stop parsing a bad
// statement.
// TODO: Replace Errors with just these exception classes?
class ParseException : public std::exception {
public:
  explicit ParseException(const ParseError &err) : d_err(err) {}

  const char *what() const noexcept override {
    static std::string str;
    std::ostringstream ss;
    ss << d_err;
    str = ss.str();
    return str.c_str();
  }

  const ParseError &getErr() const noexcept { return d_err; }

private:
  ParseError d_err;
};

std::unique_ptr<ast::Expr> expression(const std::vector<Token> &tokens,
                                      int &pos);

std::unique_ptr<ast::Expr> primary(const std::vector<Token> &tokens, int &pos) {
  switch (tokens[pos].type) {
  case TokenType::NUMBER:
  case TokenType::STRING:
  case TokenType::TRUE:
  case TokenType::FALSE:
  case TokenType::NUL: {
    TokenType type = tokens[pos].type;
    return std::make_unique<ast::Expr>(ast::Literal{tokens[pos++].value, type});
  }
  case TokenType::IDENTIFIER: {
    TokenType type = tokens[pos].type;
    return std::make_unique<ast::Expr>(ast::Variable{tokens[pos++].value});
  }
  case TokenType::LEFT_PAREN: {
    auto grp =
        std::make_unique<ast::Expr>(ast::Grouping{expression(tokens, ++pos)});
    if (tokens[pos].type == TokenType::RIGHT_PAREN) {
      pos++;
      return grp;
    }
    throw ParseException({"No closing paren found!"});
  }
  default:
    throw ParseException({"Unknown token!"});
  }
}

std::unique_ptr<ast::Expr> unary(const std::vector<Token> &tokens, int &pos) {
  switch (tokens[pos].type) {
  case TokenType::BANG:
  case TokenType::MINUS: {
    const Token &tok = tokens[pos];
    return std::make_unique<ast::Expr>(ast::Unary{tok, unary(tokens, ++pos)});
  }
  default:
    return primary(tokens, pos);
  }
}

std::unique_ptr<ast::Expr> factor(const std::vector<Token> &tokens, int &pos) {
  std::unique_ptr<ast::Expr> leftOp = unary(tokens, pos);
  switch (tokens[pos].type) {
  case TokenType::STAR:
  case TokenType::SLASH: {
    const Token &tok = tokens[pos];
    return std::make_unique<ast::Expr>(
        ast::Binary{std::move(leftOp), tok, factor(tokens, ++pos)});
  }
  default:
    return leftOp;
  }
}

std::unique_ptr<ast::Expr> term(const std::vector<Token> &tokens, int &pos) {
  std::unique_ptr<ast::Expr> leftOp = factor(tokens, pos);
  switch (tokens[pos].type) {
  case TokenType::PLUS:
  case TokenType::MINUS: {
    const Token &tok = tokens[pos];
    return std::make_unique<ast::Expr>(
        ast::Binary{std::move(leftOp), tok, term(tokens, ++pos)});
  }
  default:
    return leftOp;
  }
}

std::unique_ptr<ast::Expr> comparison(const std::vector<Token> &tokens,
                                      int &pos) {
  std::unique_ptr<ast::Expr> leftOp = term(tokens, pos);
  switch (tokens[pos].type) {
  case TokenType::LESS:
  case TokenType::LESS_EQUAL:
  case TokenType::GREATER:
  case TokenType::GREATER_EQUAL: {
    const Token &tok = tokens[pos];
    return std::make_unique<ast::Expr>(
        ast::Binary{std::move(leftOp), tok, comparison(tokens, ++pos)});
  }
  default:
    return leftOp;
  }
}

std::unique_ptr<ast::Expr> equality(const std::vector<Token> &tokens,
                                    int &pos) {
  std::unique_ptr<ast::Expr> leftOp = comparison(tokens, pos);
  switch (tokens[pos].type) {
  case TokenType::BANG_EQUAL:
  case TokenType::EQUAL_EQUAL: {
    const Token &tok = tokens[pos];
    return std::make_unique<ast::Expr>(
        ast::Binary{std::move(leftOp), tok, equality(tokens, ++pos)});
  }
  default:
    return leftOp;
  }
}

std::unique_ptr<ast::Expr> expression(const std::vector<Token> &tokens,
                                      int &pos) {
  return equality(tokens, pos);
}

std::unique_ptr<stmt::Stmt> printStatement(const std::vector<Token> &tokens,
                                           int &pos) {
  std::unique_ptr<ast::Expr> expr = expression(tokens, pos);
  switch (tokens[pos].type) {
  case TokenType::SEMICOLON: {
    pos++;
    return std::make_unique<stmt::Stmt>(stmt::Print{std::move(expr)});
  }
  default:
    // TODO: make better error message - line? surrounding toks?
    throw ParseException({"No ending semi colon found!"});
  }
}

std::unique_ptr<stmt::Stmt> exprStatement(const std::vector<Token> &tokens,
                                          int &pos) {
  std::unique_ptr<ast::Expr> expr = expression(tokens, pos);
  switch (tokens[pos].type) {
  case TokenType::SEMICOLON: {
    pos++;
    return std::make_unique<stmt::Stmt>(stmt::Expression{std::move(expr)});
  }
  default:
    // TODO: make better error message - line? surrounding toks?
    throw ParseException({"No ending semi colon found!"});
  }
}

std::unique_ptr<stmt::Stmt> varStatement(const std::vector<Token> &tokens,
                                         int &pos) {
  const Token &varName = tokens[pos++];
  if (varName.type != TokenType::IDENTIFIER) {
    throw ParseException({"Variable declaration not followed by identifier!"});
  }

  switch (tokens[pos].type) {
  case TokenType::SEMICOLON: {
    pos++;
    return std::make_unique<stmt::Stmt>(stmt::VarDecl{varName, {}});
  }
  case TokenType::EQUAL: {
    auto expr = expression(tokens, ++pos);
    if (tokens[pos].type == TokenType::SEMICOLON) {
      pos++;
      return std::make_unique<stmt::Stmt>(
          stmt::VarDecl{varName, std::move(expr)});
    }
  }
  default:
    throw ParseException({"Invalid token following var decl!"});
  }
}

std::unique_ptr<stmt::Stmt> statement(const std::vector<Token> &tokens,
                                      int &pos) {
  switch (tokens[pos].type) {
  case TokenType::PRINT:
    return printStatement(
        tokens,
        ++pos); // TODO: Need checks for pos being within bounds of tokens vec
  case TokenType::VAR:
    return varStatement(tokens, ++pos);
  default:
    return exprStatement(tokens, pos);
  }
}

void skipUntilSemiColon(const std::vector<Token> &tokens, int &pos) {
  while (tokens[pos].type != TokenType::SEMICOLON && pos < tokens.size()) {
    pos++;
  }
  if (pos < tokens.size()) {
    // Found a semi-colon. Continue to next char
    pos++;
  }
}

} // namespace

std::vector<stmt::Stmt> parse(const std::vector<Token> &tokens,
                              std::vector<ParseError> &errs) {
  std::vector<stmt::Stmt> statements;
  int pos = 0;
  while (pos != tokens.size()) {
    try {
      auto s = statement(tokens, pos);
      // TODO: Check statement is not null
      statements.push_back(std::move(*s));
    } catch (const ParseException &e) {
      // Error while parsing this statement. Continue parsing the next statement
      // so the user knows all errors in their code.
      skipUntilSemiColon(tokens, pos);
      errs.push_back(e.getErr());
    }
  }
  return statements;
}

} // namespace treewalk
} // namespace plox