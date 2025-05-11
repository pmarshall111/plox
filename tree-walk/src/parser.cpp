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

class TokenStream {
public:
  TokenStream(const std::vector<Token> &toks) : d_pos(0), d_toks(toks){};

  const Token &peek() const {
    if (d_pos >= d_toks.size()) {
      throw ParseException{{"Incomplete statement - expected more tokens!"}};
    }
    return d_toks[d_pos];
  };
  void next() { d_pos++; };
  bool hasNext() { return d_pos < d_toks.size() - 1; }
  void skipPastSemiColon() {
    while (hasNext() && d_toks[d_pos].type != TokenType::SEMICOLON) {
      // Keep on iterating until non semi-colon or end of arr
      next();
    }
    if (hasNext()) {
      // Current char is a semi-colon. Continue to next char
      next();
    }
  }

private:
  int d_pos;
  const std::vector<Token> &d_toks;
};

std::unique_ptr<ast::Expr> expression(TokenStream &tokStream);

std::unique_ptr<ast::Expr> primary(TokenStream &tokStream) {
  const Token &tok = tokStream.peek();
  switch (tok.type) {
  case TokenType::NUMBER:
  case TokenType::STRING:
  case TokenType::TRUE:
  case TokenType::FALSE:
  case TokenType::NUL: {
    tokStream.next();
    return std::make_unique<ast::Expr>(ast::Literal{tok.value, tok.type});
  }
  case TokenType::IDENTIFIER: {
    tokStream.next();
    return std::make_unique<ast::Expr>(ast::Variable{tok.value});
  }
  case TokenType::LEFT_PAREN: {
    tokStream.next();
    auto grp =
        std::make_unique<ast::Expr>(ast::Grouping{expression(tokStream)});
    if (tokStream.peek().type == TokenType::RIGHT_PAREN) {
      tokStream.next();
      return grp;
    }
    throw ParseException({"No closing paren found!"});
  }
  default:
    throw ParseException({"Unknown token!"});
  }
}

std::unique_ptr<ast::Expr> unary(TokenStream &tokStream) {
  switch (tokStream.peek().type) {
  case TokenType::BANG:
  case TokenType::MINUS: {
    const Token &op = tokStream.peek();
    tokStream.next();
    return std::make_unique<ast::Expr>(ast::Unary{op, unary(tokStream)});
  }
  default:
    return primary(tokStream);
  }
}

std::unique_ptr<ast::Expr> factor(TokenStream &tokStream) {
  std::unique_ptr<ast::Expr> leftOp = unary(tokStream);
  switch (tokStream.peek().type) {
  case TokenType::STAR:
  case TokenType::SLASH: {
    const Token &op = tokStream.peek();
    tokStream.next();
    return std::make_unique<ast::Expr>(
        ast::Binary{std::move(leftOp), op, factor(tokStream)});
  }
  default:
    return leftOp;
  }
}

std::unique_ptr<ast::Expr> term(TokenStream &tokStream) {
  std::unique_ptr<ast::Expr> leftOp = factor(tokStream);
  switch (tokStream.peek().type) {
  case TokenType::PLUS:
  case TokenType::MINUS: {
    const Token &op = tokStream.peek();
    tokStream.next();
    return std::make_unique<ast::Expr>(
        ast::Binary{std::move(leftOp), op, term(tokStream)});
  }
  default:
    return leftOp;
  }
}

std::unique_ptr<ast::Expr> comparison(TokenStream &tokStream) {
  std::unique_ptr<ast::Expr> leftOp = term(tokStream);
  switch (tokStream.peek().type) {
  case TokenType::LESS:
  case TokenType::LESS_EQUAL:
  case TokenType::GREATER:
  case TokenType::GREATER_EQUAL: {
    const Token &op = tokStream.peek();
    tokStream.next();
    return std::make_unique<ast::Expr>(
        ast::Binary{std::move(leftOp), op, comparison(tokStream)});
  }
  default:
    return leftOp;
  }
}

std::unique_ptr<ast::Expr> equality(TokenStream &tokStream) {
  std::unique_ptr<ast::Expr> leftOp = comparison(tokStream);
  switch (tokStream.peek().type) {
  case TokenType::BANG_EQUAL:
  case TokenType::EQUAL_EQUAL: {
    const Token &op = tokStream.peek();
    tokStream.next();
    return std::make_unique<ast::Expr>(
        ast::Binary{std::move(leftOp), op, equality(tokStream)});
  }
  default:
    return leftOp;
  }
}

std::unique_ptr<ast::Expr> expression(TokenStream &tokStream) {
  return equality(tokStream);
}

std::unique_ptr<stmt::Stmt> printStatement(TokenStream &tokStream) {
  std::unique_ptr<ast::Expr> expr = expression(tokStream);
  switch (tokStream.peek().type) {
  case TokenType::SEMICOLON: {
    tokStream.next();
    return std::make_unique<stmt::Stmt>(stmt::Print{std::move(expr)});
  }
  default:
    // TODO: make better error message - line? surrounding toks?
    throw ParseException({"No ending semi colon found!"});
  }
}

std::unique_ptr<stmt::Stmt> exprStatement(TokenStream &tokStream) {
  std::unique_ptr<ast::Expr> expr = expression(tokStream);
  switch (tokStream.peek().type) {
  case TokenType::SEMICOLON: {
    tokStream.next();
    return std::make_unique<stmt::Stmt>(stmt::Expression{std::move(expr)});
  }
  default:
    // TODO: make better error message - line? surrounding toks?
    throw ParseException({"No ending semi colon found!"});
  }
}

std::unique_ptr<stmt::Stmt> varStatement(TokenStream &tokStream) {
  const Token &varName = tokStream.peek();
  tokStream.next();
  if (varName.type != TokenType::IDENTIFIER) {
    throw ParseException({"Variable declaration not followed by identifier!"});
  }

  switch (tokStream.peek().type) {
  case TokenType::SEMICOLON: {
    tokStream.next();
    return std::make_unique<stmt::Stmt>(stmt::VarDecl{varName, {}});
  }
  case TokenType::EQUAL: {
    tokStream.next();
    auto expr = expression(tokStream);
    if (tokStream.peek().type == TokenType::SEMICOLON) {
      tokStream.next();
      return std::make_unique<stmt::Stmt>(
          stmt::VarDecl{varName, std::move(expr)});
    }
  }
  default:
    throw ParseException({"Invalid token following var decl!"});
  }
}

std::unique_ptr<stmt::Stmt> statement(TokenStream &tokStream) {
  switch (tokStream.peek().type) {
  case TokenType::PRINT: {
    tokStream.next();
    return printStatement(tokStream);
  }
  case TokenType::VAR: {
    tokStream.next();
    return varStatement(tokStream);
  }
  default:
    return exprStatement(tokStream);
  }
}

} // namespace

std::vector<stmt::Stmt> parse(const std::vector<Token> &tokens,
                              std::vector<ParseException> &errs) {
  std::vector<stmt::Stmt> statements;
  TokenStream tokStream{tokens};
  while (tokStream.hasNext()) {
    try {
      auto s = statement(tokStream);
      // TODO: Check statement is not null
      statements.push_back(std::move(*s));
    } catch (const ParseException &e) {
      // Error while parsing this statement. Continue parsing the next statement
      // so the user knows all errors in their code.
      errs.push_back(e);
      tokStream.skipPastSemiColon();
    }
  }
  return statements;
}

} // namespace treewalk
} // namespace plox