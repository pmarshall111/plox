#include <gtest/gtest.h>

#include <ast_printer.h>
#include <parser.h>

namespace plox {
namespace treewalk {
namespace test {

TEST(Parser, smoke) {
  // Given
  std::vector<ParseError> errs;
  // (5/1+2)*--8
  std::string expected = "((group ((5/1)+2))*(-(-8)))";
  std::vector<Token> toks{
      Token{TokenType::LEFT_PAREN, "(", 0},  Token{TokenType::NUMBER, "5", 0},
      Token{TokenType::SLASH, "/", 0},       Token{TokenType::NUMBER, "1", 0},
      Token{TokenType::PLUS, "+", 0},        Token{TokenType::NUMBER, "2", 0},
      Token{TokenType::RIGHT_PAREN, ")", 0}, Token{TokenType::STAR, "*", 0},
      Token{TokenType::MINUS, "-", 0},       Token{TokenType::MINUS, "-", 0},
      Token{TokenType::NUMBER, "8", 0},
  };

  // When
  auto astRoot = parse(toks, errs);

  // Then
  ASSERT_EQ(expected, std::visit(ast::PrinterVisitor{}, *astRoot));
  ASSERT_EQ(errs.size(), 0);
}

TEST(Parser, SmokeError) {
  // Given
  std::vector<ParseError> errs;
  // (5+2*8
  std::vector<Token> toks{
      Token{TokenType::LEFT_PAREN, "(", 0}, Token{TokenType::NUMBER, "5", 0},
      Token{TokenType::PLUS, "+", 0},       Token{TokenType::NUMBER, "2", 0},
      Token{TokenType::STAR, "*", 0},       Token{TokenType::NUMBER, "8", 0},
  };

  // When
  auto astRoot = parse(toks, errs);

  // Then
  ASSERT_EQ(errs.size(), 1);
}

} // namespace test
} // namespace treewalk
} // namespace plox
