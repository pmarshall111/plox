#include <scanner.h>

#include <map>
#include <sstream>
#include <strings.h>

namespace plox {
namespace treewalk {

namespace {
// All scanXXX() methods are to leave pos at the last char of the token
std::optional<SyntaxError> scanNumber(std::string_view code, int &pos,
                                      std::string_view &out, int line) {
  int numDots = 0;
  int start = pos;
  for (; pos < code.size(); pos++) {
    // Dot handling
    if (code.at(pos) == '.') {
      if (numDots > 0) {
        return SyntaxError("More than 1 dot found in number", line);
      }

      numDots++;

      // There must be a number after the dot.
      if (pos + 1 == code.size() || !isdigit(code.at(pos + 1))) {
        return SyntaxError("Trailing dot found in number", line);
      }
    }

    // Continue only if a digit
    if (!isdigit(code.at(pos))) {
      break;
    }
  }

  int size = pos - start;
  out = std::string_view(&code.at(start), size);
  pos--; // Reset pos to the last char of the number
  return std::nullopt;
}

std::optional<SyntaxError> scanString(std::string_view code, int &pos,
                                      std::string_view &out, int &line) {
  int start = pos;
  pos++; // Skip initial open quotes
  for (; pos < code.size(); pos++) {
    const char &c = code.at(pos);
    if (c == '\n') {
      line++;
    } else if (c == '"') {
      int size = pos - 1 - start; // skip start and end quotes
      out = std::string_view(&code.at(start + 1), size);
      return std::nullopt;
    }
  }

  return SyntaxError("Unterminated string!", line);
}

void scanLiteral(std::string_view code, int &pos, std::string_view &out) {
  int start = pos;
  for (; pos < code.size(); pos++) {
    const char &c = code.at(pos);
    if (!isalnum(c) && c != '_') {
      break;
    }
  }

  int size = pos - start;
  out = std::string_view(&code.at(start), size);
  pos--; // Reset pos to the last char of the literal
}

struct CaseInsensCompare {
  bool operator()(const std::string_view &a, const std::string_view &b) const {
    // Compare functions in cpp assume true means a<b. Elements are considered
    // equal if !(a<b) && !(b<a)
    if (a.size() != b.size()) {
      return true;
    }
    return strncasecmp(a.data(), b.data(), a.size()) < 0;
  }
};
const std::map<std::string_view, TokenType, CaseInsensCompare> g_keywords{
    {"and", TokenType::AND},       {"class", TokenType::CLASS},
    {"else", TokenType::ELSE},     {"false", TokenType::FALSE},
    {"fun", TokenType::FUN},       {"for", TokenType::FOR},
    {"if", TokenType::IF},         {"nul", TokenType::NUL},
    {"or", TokenType::OR},         {"print", TokenType::PRINT},
    {"return", TokenType::RETURN}, {"super", TokenType::SUPER},
    {"this", TokenType::THIS},     {"true", TokenType::TRUE},
    {"var", TokenType::VAR},       {"while", TokenType::WHILE}};

bool nextCharEquals(std::string_view code, int pos, char c) {
  return pos + 1 < code.size() && code.at(pos + 1) == c;
}
} // namespace

Token::Token(TokenType type, std::string_view val, int line)
    : d_type(type), d_val(val), d_line(line) {}

std::vector<Token> scanTokens(const std::string_view code,
                              std::vector<SyntaxError> &errors) {
  std::vector<Token> tokens;

  int line = 0;
  for (int pos = 0; pos < code.size(); pos++) {
    const char &c = code.at(pos);

    // Single char tokens
    if (c == '(') {
      tokens.emplace_back(TokenType::LEFT_PAREN, std::string_view(&c, 1), line);
    } else if (c == ')') {
      tokens.emplace_back(TokenType::RIGHT_PAREN, std::string_view(&c, 1),
                          line);
    } else if (c == '{') {
      tokens.emplace_back(TokenType::LEFT_BRACE, std::string_view(&c, 1), line);
    } else if (c == '}') {
      tokens.emplace_back(TokenType::RIGHT_BRACE, std::string_view(&c, 1),
                          line);
    } else if (c == ',') {
      tokens.emplace_back(TokenType::COMMA, std::string_view(&c, 1), line);
    } else if (c == '.') {
      tokens.emplace_back(TokenType::DOT, std::string_view(&c, 1), line);
    } else if (c == '-') {
      tokens.emplace_back(TokenType::MINUS, std::string_view(&c, 1), line);
    } else if (c == '+') {
      tokens.emplace_back(TokenType::PLUS, std::string_view(&c, 1), line);
    } else if (c == ';') {
      tokens.emplace_back(TokenType::SEMICOLON, std::string_view(&c, 1), line);
    } else if (c == '/') {
      tokens.emplace_back(TokenType::SLASH, std::string_view(&c, 1), line);
    } else if (c == '*') {
      tokens.emplace_back(TokenType::STAR, std::string_view(&c, 1), line);
    }
    // 1 or 2 char tokens
    else if (c == '!') {
      if (nextCharEquals(code, pos, '=')) {
        tokens.emplace_back(TokenType::BANG_EQUAL, std::string_view(&c, 2),
                            line);
        pos++; // 2 char token
      } else {
        tokens.emplace_back(TokenType::BANG, std::string_view(&c, 1), line);
      }
    } else if (c == '=') {
      if (nextCharEquals(code, pos, '=')) {
        tokens.emplace_back(TokenType::EQUAL_EQUAL, std::string_view(&c, 2),
                            line);
        pos++; // 2 char token
      } else {
        tokens.emplace_back(TokenType::EQUAL, std::string_view(&c, 1), line);
      }
    } else if (c == '>') {
      if (nextCharEquals(code, pos, '=')) {
        tokens.emplace_back(TokenType::LESS_EQUAL, std::string_view(&c, 2),
                            line);
        pos++; // 2 char token
      } else {
        tokens.emplace_back(TokenType::LESS, std::string_view(&c, 1), line);
      }
    } else if (c == '<') {
      if (nextCharEquals(code, pos, '=')) {
        tokens.emplace_back(TokenType::GREATER_EQUAL, std::string_view(&c, 2),
                            line);
        pos++; // 2 char token
      } else {
        tokens.emplace_back(TokenType::GREATER, std::string_view(&c, 1), line);
      }
    }
    // literals
    else if (isdigit(c)) {
      std::string_view num;
      auto syntaxError = scanNumber(code, pos, num, line);
      if (syntaxError) {
        errors.push_back(syntaxError.value());
      } else {
        tokens.emplace_back(TokenType::NUMBER, num, line);
      }
    } else if (c == '"') {
      std::string_view str;
      auto syntaxError = scanString(code, pos, str, line);
      if (syntaxError) {
        errors.push_back(syntaxError.value());
      } else {
        tokens.emplace_back(TokenType::STRING, str, line);
      }
    } else if (isalpha(c) || c == '_') {
      std::string_view literal;
      scanLiteral(code, pos, literal);
      if (g_keywords.contains(literal)) {
        tokens.emplace_back(g_keywords.at(literal), literal, line);
      } else {
        tokens.emplace_back(TokenType::IDENTIFIER, literal, line);
      }
    } else {
      if (c == '\r' || c == ' ' || c == '\t') {
        // ignore whitespace
      } else if (c == '\n') {
        line++;
      } else {
        std::ostringstream ss;
        ss << "Unknown symbol: " << c;
        errors.emplace_back(ss.str(), line);
      }
    }
  }

  return tokens;
}

TokenType Token::getType() const { return d_type; }

std::string_view Token::getVal() const { return d_val; }

int Token::getLine() const { return d_line; }

std::ostream &operator<<(std::ostream &os, const Token &tok) {
  os << tokenutils::tokenTypeToStr(tok.getType()) << ":" << tok.getVal() << ":"
     << tok.getLine();
  return os;
}

namespace tokenutils {
std::string tokenTypeToStr(TokenType tt) {
  switch (tt) {
  // Single char tokens
  case TokenType::LEFT_PAREN:
    return "LEFT_PAREN";
  case TokenType::RIGHT_PAREN:
    return "RIGHT_PAREN";
  case TokenType::LEFT_BRACE:
    return "LEFT_BRACE";
  case TokenType::RIGHT_BRACE:
    return "RIGHT_BRACE";
  case TokenType::COMMA:
    return "COMMA";
  case TokenType::DOT:
    return "DOT";
  case TokenType::MINUS:
    return "MINUS";
  case TokenType::PLUS:
    return "PLUS";
  case TokenType::SEMICOLON:
    return "SEMICOLON";
  case TokenType::SLASH:
    return "SLASH";
  case TokenType::STAR:
    return "STAR";

  // 1 or 2 char tokens
  case TokenType::BANG:
    return "BANG";
  case TokenType::BANG_EQUAL:
    return "BANG_EQUAL";
  case TokenType::EQUAL:
    return "EQUAL";
  case TokenType::EQUAL_EQUAL:
    return "EQUAL_EQUAL";
  case TokenType::GREATER:
    return "GREATER";
  case TokenType::GREATER_EQUAL:
    return "GREATER_EQUAL";
  case TokenType::LESS:
    return "LESS";
  case TokenType::LESS_EQUAL:
    return "LESS_EQUAL";

  // Literals
  case TokenType::IDENTIFIER:
    return "IDENTIFIER";
  case TokenType::STRING:
    return "STRING";
  case TokenType::NUMBER:
    return "NUMBER";

  // Keywords
  case TokenType::AND:
    return "AND";
  case TokenType::CLASS:
    return "CLASS";
  case TokenType::ELSE:
    return "ELSE";
  case TokenType::FALSE:
    return "FALSE";
  case TokenType::FUN:
    return "FUN";
  case TokenType::FOR:
    return "FOR";
  case TokenType::IF:
    return "IF";
  case TokenType::NUL:
    return "NUL";
  case TokenType::OR:
    return "OR";
  case TokenType::PRINT:
    return "PRINT";
  case TokenType::RETURN:
    return "RETURN";
  case TokenType::SUPER:
    return "SUPER";
  case TokenType::THIS:
    return "THIS";
  case TokenType::TRUE:
    return "TRUE";
  case TokenType::VAR:
    return "VAR";
  case TokenType::WHILE:
    return "WHILE";
  default:
    return "__unknown__";
  }
}
} // namespace tokenutils

} // namespace treewalk
} // namespace plox