#include <parser.h>

// clang-format off

// program        → statement* EOF ;
//
// statement      → blockStmt
//                | exprStmt
//                | funcStmt
//                | ifStmt
//                | printStmt
//                | varStmt
//                | whileStmt ;
//
// blockStmt      → "{" statement* "}" ;
// exprStmt       → expression ";" ;
// funcStmt       → "fun" function ;
// forStmt        → "for" "(" (varStmt | exprStmt | ";") expression? ";" expression? ")" statement ";" ;
// ifStmt         → "if" "(" expression ")" statement ("else" statement)? ";" ;
// printStmt      → "print" expression ";" ;
// varStmt        → "var" IDENTIFIER ( "=" expression )? ";" ;
// whileStmt      → "while" "(" expression ")" statement ";" ;

// expression     → assignment ;
// assignment     → IDENTIFIER "=" assignment | equality ;
// equality       → comparison ( ( "!=" | "==" ) comparison )* ;
// comparison     → term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
// term           → factor ( ( "-" | "+" ) factor )* ;
// factor         → unary ( ( "/" | "*" ) unary )* ;
// unary          → ( "!" | "-" ) unary
//                | call ;
// call           → primary ( "(" arguments? ")" )* ;
// primary        → NUMBER | STRING | "true" | "false" | "nil"
//                | "(" expression ")" | IDENTIFIER ;

// function       → INDENTIFIER "(" parameters? ")" blockStmt ;
// parameters     → IDENTIFIER ( "," IDENTIFIER )* ;
// arguments      → expression ( "," expression )* ;

// clang-format on

namespace plox {
namespace treewalk {

namespace {

class TokenStream {
public:
  TokenStream(const std::vector<Token> &toks) : d_pos(0), d_toks(toks){};

  const Token &peek() const {
    if (d_pos >= d_toks.size()) {
      throw ParseException("Incomplete statement - expected more tokens!",
                           d_toks[d_pos - 1].line);
    }
    return d_toks[d_pos];
  };
  void next() { d_pos++; };
  bool hasNext() { return d_pos < d_toks.size() - 1; }
  void skipPastSemiColons() {
    while (hasNext() && peek().type != TokenType::SEMICOLON) {
      // Keep on iterating until the first semi-colon
      next();
    }

    while (hasNext() && peek().type == TokenType::SEMICOLON) {
      // Current char is a semi-colon. Continue until the next char is not a
      // semi-colon
      next();
    }
  }

private:
  int d_pos;
  const std::vector<Token> &d_toks;
};

std::unique_ptr<ast::Expr> expression(TokenStream &tokStream);
std::unique_ptr<stmt::Stmt> statement(TokenStream &tokStream);
std::unique_ptr<stmt::Stmt> varStatement(TokenStream &tokStream);

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
    throw ParseException("No closing paren found!", tokStream.peek().line);
  }
  default:
    throw ParseException("Unknown token!", tokStream.peek().line);
  }
}

std::unique_ptr<ast::Expr> call(TokenStream &tokStream) {
  auto expr = primary(tokStream);
  if (TokenType::LEFT_PAREN != tokStream.peek().type) {
    // Not a function call
    return expr;
  }

  // Loop to support for multiple call levels i.e. fn(1)(2)(3);
  while (TokenType::LEFT_PAREN == tokStream.peek().type) {
    tokStream.next();
    expr = std::make_unique<ast::Expr>(ast::Call{std::move(expr)});

    // Loop to construct arguments
    while (TokenType::RIGHT_PAREN != tokStream.peek().type) {
      // Args can be expressions that later need to be evaluated i.e. fn(1+2);
      auto arg = expression(tokStream);
      std::get<ast::Call>(*expr).args.emplace_back(std::move(arg));

      if (TokenType::COMMA == tokStream.peek().type) {
        tokStream.next();
        if (TokenType::RIGHT_PAREN == tokStream.peek().type) {
          throw ParseException("Comma must not directly precede closing paren!",
                               tokStream.peek().line);
        }
      } else if (TokenType::RIGHT_PAREN != tokStream.peek().type) {
        throw ParseException("Argument list must be comma separated!",
                             tokStream.peek().line);
      }
    }
    tokStream.next();
  }

  return expr;
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
    return call(tokStream);
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

std::unique_ptr<ast::Expr> assignment(TokenStream &tokStream) {
  std::unique_ptr<ast::Expr> exp = equality(tokStream);

  if (TokenType::EQUAL == tokStream.peek().type) {
    tokStream.next();
    if (!std::holds_alternative<ast::Variable>(*exp)) {
      throw ParseException("Cannot assign to r-value", tokStream.peek().line);
    }

    std::string_view name = std::get<ast::Variable>(*exp).name;
    return std::make_unique<ast::Expr>(
        ast::Assign{name, assignment(tokStream)});
  }

  return exp;
}

std::unique_ptr<ast::Expr> expression(TokenStream &tokStream) {
  return assignment(tokStream);
}

std::unique_ptr<stmt::Stmt> blockStatement(TokenStream &tokStream) {
  int blockStart = tokStream.peek().line;
  auto blk = std::make_unique<stmt::Stmt>(stmt::Block());
  while (tokStream.hasNext()) {
    if (TokenType::RIGHT_BRACE == tokStream.peek().type) {
      tokStream.next();
      return blk;
    }
    std::get<stmt::Block>(*blk).stmts.push_back(
        std::move(statement(tokStream)));
  }

  throw ParseException("Block has no closing brace.", blockStart);
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
    throw ParseException("No ending semi colon found!", tokStream.peek().line);
  }
}

std::unique_ptr<stmt::Stmt> forStatement(TokenStream &tokStream) {
  if (TokenType::LEFT_PAREN != tokStream.peek().type) {
    throw ParseException("For needs to be followed by an opening paren.",
                         tokStream.peek().line);
  }
  tokStream.next();
  auto forStmt = std::make_unique<stmt::Stmt>(stmt::For());

  // Read initialiser
  if (TokenType::VAR == tokStream.peek().type) {
    tokStream.next();
    std::get<stmt::For>(*forStmt).initialiser = varStatement(tokStream);
  } else if (TokenType::SEMICOLON == tokStream.peek().type) {
    tokStream.next();
  } else {
    // Must be an expression statement
    std::get<stmt::For>(*forStmt).initialiser = exprStatement(tokStream);
  }

  // Read condition
  if (TokenType::SEMICOLON != tokStream.peek().type) {
    std::get<stmt::For>(*forStmt).condition = expression(tokStream);
  }
  if (TokenType::SEMICOLON != tokStream.peek().type) {
    throw ParseException("Expected semi-colon after for loop condition.",
                         tokStream.peek().line);
  }
  tokStream.next();

  // Read incrementer
  if (TokenType::RIGHT_PAREN != tokStream.peek().type) {
    std::get<stmt::For>(*forStmt).incrementer = expression(tokStream);
  }
  if (TokenType::RIGHT_PAREN != tokStream.peek().type) {
    throw ParseException(
        "Expected for loop specifiers to be closed with parenthesis",
        tokStream.peek().line);
  }
  tokStream.next();

  // Read body
  std::get<stmt::For>(*forStmt).body = statement(tokStream);

  if (TokenType::SEMICOLON == tokStream.peek().type) {
    tokStream.next();
    return forStmt;
  }
  throw ParseException("No ending semi colon found for 'for' stmt!",
                       tokStream.peek().line);
}

std::unique_ptr<stmt::Stmt> funStatement(TokenStream &tokStream) {
  const Token &funStart = tokStream.peek();

  // Get function name
  if (TokenType::IDENTIFIER != tokStream.peek().type) {
    throw ParseException("fun declaration not followed by identifier!",
                         tokStream.peek().line);
  }
  auto funStmt =
      std::make_unique<stmt::Stmt>(stmt::Fun{tokStream.peek().value});
  tokStream.next();

  if (TokenType::LEFT_PAREN != tokStream.peek().type) {
    throw ParseException("fun identifier needs to be followed by arguments! "
                         "No opening paren found.",
                         tokStream.peek().line);
  }
  tokStream.next();

  // Construct params
  while (TokenType::RIGHT_PAREN != tokStream.peek().type) {
    if (TokenType::IDENTIFIER != tokStream.peek().type) {
      throw ParseException("fun argument must be an identifier!",
                           tokStream.peek().line);
    }
    std::get<stmt::Fun>(*funStmt).params.push_back(tokStream.peek().value);
    tokStream.next();

    if (TokenType::COMMA == tokStream.peek().type) {
      tokStream.next();
      if (TokenType::RIGHT_PAREN == tokStream.peek().type) {
        throw ParseException("Comma must not directly precede closing paren!",
                             tokStream.peek().line);
      }
    } else if (TokenType::RIGHT_PAREN != tokStream.peek().type) {
      throw ParseException("Argument list must be comma separated!",
                           tokStream.peek().line);
    }
  }
  tokStream.next();

  // Construct body
  if (TokenType::LEFT_BRACE != tokStream.peek().type) {
    throw ParseException("Function must be followed by an opening curly brace!",
                         tokStream.peek().line);
  }
  tokStream.next();

  while (tokStream.hasNext()) {
    if (TokenType::RIGHT_BRACE == tokStream.peek().type) {
      tokStream.next();
      return funStmt;
    }
    std::get<stmt::Fun>(*funStmt).stmts.push_back(
        std::move(statement(tokStream)));
  }

  throw ParseException("Function block has no closing brace.", funStart.line);
}

std::unique_ptr<stmt::Stmt> ifStatement(TokenStream &tokStream) {
  if (TokenType::LEFT_PAREN != tokStream.peek().type) {
    throw ParseException("If conditions need to be surrounded by parentheses! "
                         "No opening paren found.",
                         tokStream.peek().line);
  }
  tokStream.next();

  // Read condition
  auto ifStmt = std::make_unique<stmt::Stmt>(stmt::If());
  std::get<stmt::If>(*ifStmt).condition = expression(tokStream);
  if (TokenType::RIGHT_PAREN != tokStream.peek().type) {
    throw ParseException("If conditions need to be surrounded by parentheses! "
                         "No closing paren found.",
                         tokStream.peek().line);
  }
  tokStream.next();

  // Read if branch
  std::get<stmt::If>(*ifStmt).ifBranch = statement(tokStream);

  // Read optional else branch
  if (TokenType::ELSE == tokStream.peek().type) {
    tokStream.next();
    std::get<stmt::If>(*ifStmt).elseBranch = statement(tokStream);
  }

  if (TokenType::SEMICOLON == tokStream.peek().type) {
    tokStream.next();
    return ifStmt;
  }
  throw ParseException("No ending semi colon found for if stmt!",
                       tokStream.peek().line);
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
    throw ParseException("No ending semi colon found!", tokStream.peek().line);
  }
}

std::unique_ptr<stmt::Stmt> varStatement(TokenStream &tokStream) {
  const Token &varName = tokStream.peek();
  tokStream.next();
  if (varName.type != TokenType::IDENTIFIER) {
    throw ParseException("Variable declaration not followed by identifier!",
                         tokStream.peek().line);
  }

  switch (tokStream.peek().type) {
  case TokenType::SEMICOLON: {
    tokStream.next();
    return std::make_unique<stmt::Stmt>(stmt::VarDecl{varName.value, {}});
  }
  case TokenType::EQUAL: {
    tokStream.next();
    auto expr = expression(tokStream);
    if (tokStream.peek().type == TokenType::SEMICOLON) {
      tokStream.next();
      return std::make_unique<stmt::Stmt>(
          stmt::VarDecl{varName.value, std::move(expr)});
    }
  }
  default:
    throw ParseException("Invalid token following var decl!",
                         tokStream.peek().line);
  }
}

std::unique_ptr<stmt::Stmt> whileStatement(TokenStream &tokStream) {
  if (TokenType::LEFT_PAREN != tokStream.peek().type) {
    throw ParseException(
        "While conditions need to be surrounded by parentheses! "
        "No opening paren found.",
        tokStream.peek().line);
  }
  tokStream.next();

  // Read condition
  auto whileStmt = std::make_unique<stmt::Stmt>(stmt::While());
  std::get<stmt::While>(*whileStmt).condition = expression(tokStream);
  if (TokenType::RIGHT_PAREN != tokStream.peek().type) {
    throw ParseException(
        "While conditions need to be surrounded by parentheses! "
        "No closing paren found.",
        tokStream.peek().line);
  }
  tokStream.next();

  // Read body
  std::get<stmt::While>(*whileStmt).body = statement(tokStream);

  if (TokenType::SEMICOLON == tokStream.peek().type) {
    tokStream.next();
    return whileStmt;
  }
  throw ParseException("No ending semi colon found for while stmt!",
                       tokStream.peek().line);
}

std::unique_ptr<stmt::Stmt> statement(TokenStream &tokStream) {
  switch (tokStream.peek().type) {
  case TokenType::LEFT_BRACE: {
    tokStream.next();
    return blockStatement(tokStream);
  }
  case TokenType::FOR: {
    tokStream.next();
    return forStatement(tokStream);
  }
  case TokenType::FUN: {
    tokStream.next();
    return funStatement(tokStream);
  }
  case TokenType::IF: {
    tokStream.next();
    return ifStatement(tokStream);
  }
  case TokenType::PRINT: {
    tokStream.next();
    return printStatement(tokStream);
  }
  case TokenType::VAR: {
    tokStream.next();
    return varStatement(tokStream);
  }
  case TokenType::WHILE: {
    tokStream.next();
    return whileStatement(tokStream);
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
      tokStream.skipPastSemiColons();
    }
  }
  return statements;
}

} // namespace treewalk
} // namespace plox