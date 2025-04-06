#include <gtest/gtest.h>

#include <scanner.h>

namespace plox {
namespace treewalk {
namespace test {

TEST(Scanner, Assignment) {
  // Given
  std::vector<SyntaxError> errors;
  std::string code = R"(var a = "hi")";
  std::vector<Token> expected{
      Token{TokenType::VAR, "var", 0}, Token{TokenType::IDENTIFIER, "a", 0},
      Token{TokenType::EQUAL, "=", 0}, Token{TokenType::STRING, "hi", 0}};

  // When
  auto vec = scanTokens(code, errors);

  // Then
  ASSERT_EQ(expected, vec);
  ASSERT_EQ(0, errors.size());
}

TEST(Scanner, UnterminatedStringErr) {
  // Given
  std::vector<SyntaxError> errors;
  std::string code = R"(var a = "hi)"; // unterminated string

  // When
  scanTokens(code, errors);

  // Then
  ASSERT_EQ(1, errors.size());
}

TEST(Scanner, Addition) {
  // Given
  std::vector<SyntaxError> errors;
  std::string code = "1+2";
  std::vector<Token> expected{Token{TokenType::NUMBER, "1", 0},
                              Token{TokenType::PLUS, "+", 0},
                              Token{TokenType::NUMBER, "2", 0}};

  // When
  auto vec = scanTokens(code, errors);

  // Then
  ASSERT_EQ(expected, vec);
  ASSERT_EQ(0, errors.size());
}

TEST(Scanner, NoSpaces) {
  std::vector<SyntaxError> errors;
  std::string code = "var B=1";
  std::vector<Token> expected{
      Token{TokenType::VAR, "var", 0}, Token{TokenType::IDENTIFIER, "B", 0},
      Token{TokenType::EQUAL, "=", 0}, Token{TokenType::NUMBER, "1", 0}};

  // When
  auto vec = scanTokens(code, errors);

  // Then
  ASSERT_EQ(expected, vec);
  ASSERT_EQ(0, errors.size());
}

} // namespace test
} // namespace treewalk
} // namespace plox
