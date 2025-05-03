#include <interpreter.h>

#include <gtest/gtest.h>

#include <ast_printer.h>

namespace plox {
namespace treewalk {
namespace test {

using namespace ast;

TEST(Interpreter, smoke) {
  // Given
  // (5/1+2)*--8
  std::unique_ptr<Expr> expr = std::make_unique<Expr>(Binary{
      std::make_unique<Expr>(Grouping{std::make_unique<Expr>(Binary{
          std::make_unique<Expr>(
              Binary{std::make_unique<Expr>(Literal{"5", TokenType::NUMBER}),
                     Token{TokenType::SLASH, "/", 0},
                     std::make_unique<Expr>(Literal{"1", TokenType::NUMBER})}),
          Token{TokenType::PLUS, "+", 0},
          std::make_unique<Expr>(Literal{"2", TokenType::NUMBER})})}),
      Token{TokenType::STAR, "*", 0},
      std::make_unique<Expr>(Unary{
          Token{TokenType::MINUS, "-", 0},
          std::make_unique<Expr>(Unary{
              Token{TokenType::MINUS, "-", 0},
              std::make_unique<Expr>(Literal{"8", TokenType::NUMBER})})})});

  ASSERT_EQ("((group ((5/1)+2))*(-(-8)))",
            std::visit(ast::PrinterVisitor{}, *expr));
  std::vector<InterpretError> errs;

  // When
  auto val = interpret(expr, errs);

  // Then
  ASSERT_TRUE(std::holds_alternative<double>(val));
  ASSERT_EQ(56.0, std::get<double>(val));
}

TEST(Interpreter, SmokeError) {
  // Given
  // -true
  std::unique_ptr<Expr> expr = std::make_unique<Expr>(
      Unary{Token{TokenType::MINUS, "-", 0},
            std::make_unique<Expr>(Literal{"true", TokenType::TRUE})});
  std::vector<InterpretError> errs;

  ASSERT_EQ("(-true)", std::visit(ast::PrinterVisitor{}, *expr));

  // When
  auto val = interpret(expr, errs);

  // Then
  ASSERT_TRUE(std::holds_alternative<std::monostate>(val));
  ASSERT_EQ(1, errs.size());
}

} // namespace test
} // namespace treewalk
} // namespace plox
