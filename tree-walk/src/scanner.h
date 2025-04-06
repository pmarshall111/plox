#ifndef TREEWALK_SCANNER_H
#define TREEWALK_SCANNER_H

#include <optional>
#include <string>
#include <vector>

#include <errs.h>

namespace plox {
namespace treewalk {

enum class TokenType {
  // Single char tokens
  LEFT_PAREN,
  RIGHT_PAREN,
  LEFT_BRACE,
  RIGHT_BRACE,
  COMMA,
  DOT,
  MINUS,
  PLUS,
  SEMICOLON,
  SLASH,
  STAR,

  // 1 or 2 char tokens
  BANG,
  BANG_EQUAL,
  EQUAL,
  EQUAL_EQUAL,
  GREATER,
  GREATER_EQUAL,
  LESS,
  LESS_EQUAL,

  // Literals
  IDENTIFIER,
  STRING,
  NUMBER,

  // Keywords
  AND,
  CLASS,
  ELSE,
  FALSE,
  FUN,
  FOR,
  IF,
  NUL,
  OR,
  PRINT,
  RETURN,
  SUPER,
  THIS,
  TRUE,
  VAR,
  WHILE
};

class Token {
public:
  Token(TokenType type, std::string_view val, int line);

  bool operator==(const Token &) const = default;

  TokenType getType() const;
  std::string_view getVal() const;
  int getLine() const;

private:
  TokenType d_type;
  std::string_view d_val;
  int d_line;
};

std::ostream &operator<<(std::ostream &os, const Token &tok);

std::vector<Token> scanTokens(const std::string_view code,
                              std::vector<SyntaxError> &errors);

namespace tokenutils {
std::string tokenTypeToStr(TokenType tt);
} // namespace tokenutils

} // namespace treewalk
} // namespace plox

#endif