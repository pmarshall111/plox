#include <gtest/gtest.h>

#include <scanner.h>

namespace plox {
namespace treewalk {
namespace test {

TEST(Scanner, smoke) {
  // Given
  std::vector<SyntaxError> errors;
  std::string code = R"(var a = "hi")";
  std::vector<Token> expected{
      Token{TokenType::VAR, "var", 0}, Token{TokenType::IDENTIFIER, "a", 0},
      Token{TokenType::EQUAL, "=", 0}, Token{TokenType::STRING, "hi", 0}};

  // When
  auto vec = scanTokens(code, errors);

  // Then
  ASSERT_EQ(vec, expected);
}

TEST(Scanner, SmokeError) {
  // Given
  std::vector<SyntaxError> errors;
  std::string code = R"(var a == "hi)";

  // When
  scanTokens(code, errors);

  // Then
  ASSERT_EQ(1, errors.size());
}

} // namespace test
} // namespace treewalk
} // namespace plox
