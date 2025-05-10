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
  ASSERT_TRUE(std::holds_alternative<stmt::Expression>(stmts[0]));
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
  ASSERT_TRUE(std::holds_alternative<stmt::Expression>(stmts[0]));
  ASSERT_EQ(expected, std::visit(stmt::PrinterVisitor{}, stmts[0]));
}

TEST(Parser, VarDecl) {
  // Given
  std::vector<ParseError> errs;
  // var a;
  std::string expected = "var a";
  std::vector<Token> toks{{TokenType::VAR, "var", 0},
                          {TokenType::IDENTIFIER, "a", 0},
                          {TokenType::SEMICOLON, ";", 0}};

  // When
  auto stmts = parse(toks, errs);

  // Then
  ASSERT_EQ(0, errs.size());
  ASSERT_EQ(1, stmts.size());
  ASSERT_TRUE(std::holds_alternative<stmt::VarDecl>(stmts[0]));
  ASSERT_EQ(expected, std::visit(stmt::PrinterVisitor{}, stmts[0]));
}

TEST(Parser, VarDef) {
  // Given
  std::vector<ParseError> errs;
  // var a = true;
  std::string expected = "var a = true";
  std::vector<Token> toks{{TokenType::VAR, "var", 0},
                          {TokenType::IDENTIFIER, "a", 0},
                          {TokenType::EQUAL, "=", 0},
                          {TokenType::TRUE, "true", 0},
                          {TokenType::SEMICOLON, ";", 0}};

  // When
  auto stmts = parse(toks, errs);

  // Then
  ASSERT_EQ(0, errs.size());
  ASSERT_EQ(1, stmts.size());
  ASSERT_TRUE(std::holds_alternative<stmt::VarDecl>(stmts[0]));
  ASSERT_EQ(expected, std::visit(stmt::PrinterVisitor{}, stmts[0]));
}

TEST(Parser, VarUsage) {
  // Given
  std::vector<ParseError> errs;
  // var a = b;
  std::string expected = "var a = (var b)";
  std::vector<Token> toks{{TokenType::VAR, "var", 0},
                          {TokenType::IDENTIFIER, "a", 0},
                          {TokenType::EQUAL, "=", 0},
                          {TokenType::IDENTIFIER, "b", 0},
                          {TokenType::SEMICOLON, ";", 0}};

  // When
  auto stmts = parse(toks, errs);

  // Then
  ASSERT_EQ(0, errs.size());
  ASSERT_EQ(1, stmts.size());
  ASSERT_TRUE(std::holds_alternative<stmt::VarDecl>(stmts[0]));
  ASSERT_EQ(expected, std::visit(stmt::PrinterVisitor{}, stmts[0]));
}

TEST(Parser, PrintStmt) {
  // Given
  std::vector<ParseError> errs;
  // print 1;
  std::string expected = "print 1";
  std::vector<Token> toks{{TokenType::PRINT, "print", 0},
                          {TokenType::NUMBER, "1", 0},
                          {TokenType::SEMICOLON, ";", 0}};

  // When
  auto stmts = parse(toks, errs);

  // Then
  ASSERT_EQ(0, errs.size());
  ASSERT_EQ(1, stmts.size());
  ASSERT_TRUE(std::holds_alternative<stmt::Print>(stmts[0]));
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
