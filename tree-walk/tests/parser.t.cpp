#include <gtest/gtest.h>

#include <parser.h>
#include <stmt_printer.h>

namespace plox {
namespace treewalk {
namespace test {

TEST(Parser, smoke) {
  // Given
  std::vector<ParseError> errs;
  // (5/1+2)*--8;
  std::string expected = "((group ((5/1)+2))*(-(-8)))";
  std::vector<Token> toks{
      {TokenType::LEFT_PAREN, "(", 0},  {TokenType::NUMBER, "5", 0},
      {TokenType::SLASH, "/", 0},       {TokenType::NUMBER, "1", 0},
      {TokenType::PLUS, "+", 0},        {TokenType::NUMBER, "2", 0},
      {TokenType::RIGHT_PAREN, ")", 0}, {TokenType::STAR, "*", 0},
      {TokenType::MINUS, "-", 0},       {TokenType::MINUS, "-", 0},
      {TokenType::NUMBER, "8", 0},      {TokenType::SEMICOLON, ";", 0}};

  // When
  auto stmts = parse(toks, errs);

  // Then
  ASSERT_EQ(0, errs.size());
  ASSERT_EQ(1, stmts.size());
  ASSERT_EQ(expected, std::visit(stmt::PrinterVisitor{}, stmts[0]));
}

TEST(Parser, SmokeMultiStmt) {
  // Given
  std::vector<ParseError> errs;
  // (5+1;
  // 2-0;
  std::string expected = "(2-0)";
  std::vector<Token> toks{
      {TokenType::LEFT_PAREN, "(", 0}, {TokenType::NUMBER, "5", 0},
      {TokenType::PLUS, "+", 0},       {TokenType::NUMBER, "1", 0},
      {TokenType::SEMICOLON, ";", 0},  {TokenType::NUMBER, "2", 1},
      {TokenType::MINUS, "-", 1},      {TokenType::NUMBER, "0", 1},
      {TokenType::SEMICOLON, ";", 1}};

  // When
  auto stmts = parse(toks, errs);

  // Then
  ASSERT_EQ(1, errs.size());
  ASSERT_EQ(1, stmts.size());
  ASSERT_EQ(expected, std::visit(stmt::PrinterVisitor{}, stmts[0]));
}

TEST(Parser, SmokeError) {
  // Given
  std::vector<ParseError> errs;
  // (5+2*8;
  std::vector<Token> toks{
      {TokenType::LEFT_PAREN, "(", 0}, {TokenType::NUMBER, "5", 0},
      {TokenType::PLUS, "+", 0},       {TokenType::NUMBER, "2", 0},
      {TokenType::STAR, "*", 0},       {TokenType::NUMBER, "8", 0},
      {TokenType::SEMICOLON, ";", 0}};

  // When
  auto astRoot = parse(toks, errs);

  // Then
  ASSERT_EQ(1, errs.size());
}

} // namespace test
} // namespace treewalk
} // namespace plox
