#include <interpreter.h>

#include <gtest/gtest.h>

#include <stmt_printer.h>

namespace plox {
namespace treewalk {
namespace test {

using namespace ast;

TEST(Interpreter, smoke) {
  // Given
  // (5/1+2)*--8
  std::vector<stmt::Stmt> statements;
  statements.emplace_back(stmt::VarDecl{
      "myVar",
      std::make_unique<Expr>(Binary{
          std::make_unique<Expr>(Grouping{std::make_unique<Expr>(Binary{
              std::make_unique<Expr>(Binary{
                  std::make_unique<Expr>(Literal{"5", TokenType::NUMBER}),
                  Token{TokenType::SLASH, "/", 0},
                  std::make_unique<Expr>(Literal{"1", TokenType::NUMBER})}),
              Token{TokenType::PLUS, "+", 0},
              std::make_unique<Expr>(Literal{"2", TokenType::NUMBER})})}),
          Token{TokenType::STAR, "*", 0},
          std::make_unique<Expr>(Unary{
              Token{TokenType::MINUS, "-", 0},
              std::make_unique<Expr>(Unary{Token{TokenType::MINUS, "-", 0},
                                           std::make_unique<Expr>(Literal{
                                               "8", TokenType::NUMBER})})})})});

  ASSERT_EQ("var myVar = ((group ((5/1)+2))*(-(-8)))",
            std::visit(stmt::PrinterVisitor{}, statements[0]));
  auto env = Environment::create();
  std::vector<InterpretException> errs;

  // When
  interpret(statements, env, errs);

  // Then
  auto val = env->get("myVar");
  ASSERT_EQ(56.0, std::get<double>(val));
  ASSERT_EQ(0, errs.size());
}

TEST(Interpreter, SmokeError) {
  // Given
  // -true
  std::vector<stmt::Stmt> statements;
  statements.emplace_back(stmt::VarDecl{
      "myVar", std::make_unique<Expr>(Unary{
                   Token{TokenType::MINUS, "-", 0},
                   std::make_unique<Expr>(Literal{"true", TokenType::TRUE})})});
  std::vector<InterpretException> errs;
  auto env = Environment::create();

  ASSERT_EQ("var myVar = (-true)",
            std::visit(stmt::PrinterVisitor{}, statements[0]));

  // When
  interpret(statements, env, errs);

  // Then
  ASSERT_THROW(env->get("myVar"), InterpretException);
  ASSERT_EQ(1, errs.size());
}

TEST(Interpreter, UseVar) {
  // Given
  // var a = 3;
  // var b = 2 * a;
  std::vector<stmt::Stmt> statements;
  statements.emplace_back(stmt::VarDecl{
      "a", std::make_unique<Expr>(Literal{"3", TokenType::NUMBER})});
  statements.emplace_back(stmt::VarDecl{
      "b", std::make_unique<Expr>(
               Binary{std::make_unique<Expr>(Literal{"2", TokenType::NUMBER}),
                      Token{TokenType::STAR, "*", 1},
                      std::make_unique<Expr>(Variable{"a"})})});
  std::vector<InterpretException> errs;
  auto env = Environment::create();

  ASSERT_EQ("var a = 3", std::visit(stmt::PrinterVisitor{}, statements[0]));
  ASSERT_EQ("var b = (2*(var a))",
            std::visit(stmt::PrinterVisitor{}, statements[1]));

  // When
  interpret(statements, env, errs);

  // Then
  auto b = env->get("b");
  ASSERT_EQ(6, std::get<double>(b));
  ASSERT_EQ(0, errs.size());
}

TEST(Interpreter, ReassignVar) {
  // Given
  // var a = 3;
  // a = 2 * a;
  std::vector<stmt::Stmt> statements;
  statements.emplace_back(stmt::VarDecl{
      "a", std::make_unique<Expr>(Literal{"3", TokenType::NUMBER})});
  statements.emplace_back(stmt::Expression{std::make_unique<Expr>(
      Assign{"a", std::make_unique<Expr>(Binary{
                      std::make_unique<Expr>(Literal{"2", TokenType::NUMBER}),
                      Token{TokenType::STAR, "*", 1},
                      std::make_unique<Expr>(Variable{"a"})})})});
  std::vector<InterpretException> errs;
  auto env = Environment::create();

  ASSERT_EQ("var a = 3", std::visit(stmt::PrinterVisitor{}, statements[0]));
  ASSERT_EQ("(a=(2*(var a)))",
            std::visit(stmt::PrinterVisitor{}, statements[1]));

  // When
  interpret(statements, env, errs);

  // Then
  auto a = env->get("a");
  ASSERT_EQ(6, std::get<double>(a));
  ASSERT_EQ(0, errs.size());
}

} // namespace test
} // namespace treewalk
} // namespace plox
