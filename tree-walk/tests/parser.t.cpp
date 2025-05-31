#include <parser.h>

#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

#include <stmt_printer.h>

using ::testing::HasSubstr;

namespace plox {
namespace treewalk {
namespace test {

TEST(Parser, smoke) {
  // Given
  std::vector<ParseException> errs;
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
  std::vector<ParseException> errs;
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
  std::vector<ParseException> errs;
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
  std::vector<ParseException> errs;
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
  std::vector<ParseException> errs;
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
  std::vector<ParseException> errs;
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
  std::vector<ParseException> errs;
  // (5+2*8;
  std::vector<Token> toks{
      {TokenType::LEFT_PAREN, "(", 0}, {TokenType::NUMBER, "5", 0},
      {TokenType::PLUS, "+", 0},       {TokenType::NUMBER, "2", 0},
      {TokenType::STAR, "*", 0},       {TokenType::NUMBER, "8", 0},
      {TokenType::SEMICOLON, ";", 0}};

  // When
  auto stmts = parse(toks, errs);

  // Then
  ASSERT_EQ(1, errs.size());
  ASSERT_EQ(0, stmts.size());
}

TEST(Parser, AddrOutOfRangeNoSemiColon) {
  // Given
  std::vector<ParseException> errs;
  // var a
  std::vector<Token> toks{{TokenType::VAR, "var", 0},
                          {TokenType::IDENTIFIER, "a", 0}};

  // When
  auto stmts = parse(toks, errs);

  // Then
  ASSERT_EQ(1, errs.size());
  ASSERT_THAT(errs[0].what(), ::HasSubstr("Incomplete statement"));
  ASSERT_EQ(0, stmts.size());
}

TEST(Parser, AddrOutOfRangeIncompleteStatement) {
  // Given
  std::vector<ParseException> errs;
  // 1+
  std::vector<Token> toks{{TokenType::NUMBER, "1", 0},
                          {TokenType::PLUS, "+", 0}};

  // When
  auto stmts = parse(toks, errs);

  // Then
  ASSERT_EQ(1, errs.size());
  ASSERT_THAT(errs[0].what(), ::HasSubstr("Incomplete statement"));
  ASSERT_EQ(0, stmts.size());
}

TEST(Parser, Assign) {
  // Given
  std::vector<ParseException> errs;
  // var a = 1;
  // a = 2;
  std::string expected1 = "var a = 1";
  std::string expected2 = "(a=2)";
  std::vector<Token> toks{
      {TokenType::VAR, "var", 0},      {TokenType::IDENTIFIER, "a", 0},
      {TokenType::EQUAL, "=", 0},      {TokenType::NUMBER, "1", 0},
      {TokenType::SEMICOLON, ";", 0},

      {TokenType::IDENTIFIER, "a", 1}, {TokenType::EQUAL, "=", 1},
      {TokenType::NUMBER, "2", 1},     {TokenType::SEMICOLON, ";", 1},
  };

  // When
  auto stmts = parse(toks, errs);

  // Then
  ASSERT_EQ(0, errs.size());
  ASSERT_EQ(2, stmts.size());
  ASSERT_TRUE(std::holds_alternative<stmt::VarDecl>(stmts[0]));
  ASSERT_EQ(expected1, std::visit(stmt::PrinterVisitor{}, stmts[0]));
  ASSERT_TRUE(std::holds_alternative<stmt::Expression>(stmts[1]));
  ASSERT_EQ(expected2, std::visit(stmt::PrinterVisitor{}, stmts[1]));
}

TEST(Parser, AssignToRVal) {
  // Given
  std::vector<ParseException> errs;
  // 2*3 = 2;
  std::string expected = "((2*3)=2)";
  std::vector<Token> toks{
      {TokenType::NUMBER, "2", 0}, {TokenType::STAR, "*", 0},
      {TokenType::NUMBER, "3", 0}, {TokenType::EQUAL, "=", 0},
      {TokenType::NUMBER, "2", 0}, {TokenType::SEMICOLON, ";", 0},
  };

  // When
  auto stmts = parse(toks, errs);

  // Then
  ASSERT_EQ(1, errs.size());
  ASSERT_THAT(errs[0].what(), ::HasSubstr("r-value"));
  ASSERT_EQ(0, stmts.size());
}

} // namespace test
} // namespace treewalk
} // namespace plox
