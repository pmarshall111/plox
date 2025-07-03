#include <gtest/gtest.h>

#include <scanner.h>

namespace plox {
namespace treewalk {
namespace test {

TEST(Scanner, Assignment) {
  // Given
  std::vector<SyntaxException> errors;
  std::string code = R"(var a = "hi")";
  std::vector<Token> expected{
      Token{TokenType::VAR, "var", 1}, Token{TokenType::IDENTIFIER, "a", 1},
      Token{TokenType::EQUAL, "=", 1}, Token{TokenType::STRING, "hi", 1},
      Token{TokenType::EOF_, "", 1}};

  // When
  auto vec = scanTokens(code, errors);

  // Then
  ASSERT_EQ(expected, vec);
  ASSERT_EQ(0, errors.size());
}

TEST(Scanner, UnterminatedStringErr) {
  // Given
  std::vector<SyntaxException> errors;
  std::string code = R"(var a = "hi)"; // unterminated string

  // When
  scanTokens(code, errors);

  // Then
  ASSERT_EQ(1, errors.size());
}

TEST(Scanner, Addition) {
  // Given
  std::vector<SyntaxException> errors;
  std::string code = "1+2";
  std::vector<Token> expected{
      Token{TokenType::NUMBER, "1", 1}, Token{TokenType::PLUS, "+", 1},
      Token{TokenType::NUMBER, "2", 1}, Token{TokenType::EOF_, "", 1}};

  // When
  auto vec = scanTokens(code, errors);

  // Then
  ASSERT_EQ(expected, vec);
  ASSERT_EQ(0, errors.size());
}

TEST(Scanner, NoSpaces) {
  // Given
  std::vector<SyntaxException> errors;
  std::string code = "var B=1";
  std::vector<Token> expected{
      Token{TokenType::VAR, "var", 1}, Token{TokenType::IDENTIFIER, "B", 1},
      Token{TokenType::EQUAL, "=", 1}, Token{TokenType::NUMBER, "1", 1},
      Token{TokenType::EOF_, "", 1}};

  // When
  auto vec = scanTokens(code, errors);

  // Then
  ASSERT_EQ(expected, vec);
  ASSERT_EQ(0, errors.size());
}

TEST(Scanner, Print) {
  // Given
  std::vector<SyntaxException> errors;
  std::string code = "print 1;";
  std::vector<Token> expected{
      Token{TokenType::PRINT, "print", 1}, Token{TokenType::NUMBER, "1", 1},
      Token{TokenType::SEMICOLON, ";", 1}, Token{TokenType::EOF_, "", 1}};

  // When
  auto vec = scanTokens(code, errors);

  // Then
  ASSERT_EQ(expected, vec);
  ASSERT_EQ(0, errors.size());
}

} // namespace test
} // namespace treewalk
} // namespace plox
