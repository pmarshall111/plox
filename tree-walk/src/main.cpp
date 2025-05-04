#include <CLI/CLI.hpp>
#include <fstream>
#include <iostream>
#include <optional>

#include <ast_printer.h>
#include <interpreter.h>
#include <parser.h>
#include <scanner.h>

namespace plox {
namespace treewalk {

int run(const std::string &buff) {
  // Scan
  std::vector<SyntaxError> syntErrs;
  auto tokens = scanTokens(buff, syntErrs);
  if (syntErrs.size()) {
    for (auto &err : syntErrs) {
      std::cout << "Syntax error: " << err << std::endl;
    }
    return -1;
  }

  // Parse
  std::vector<ParseError> parsErrs;
  auto ast = parse(tokens, parsErrs);
  if (parsErrs.size()) {
    for (auto &err : parsErrs) {
      std::cout << "Parse error: " << err << std::endl;
    }
    return -2;
  }

  // Interpret
  std::vector<InterpretError> interpErrs;
  auto val = interpret(ast, interpErrs);
  if (interpErrs.size()) {
    for (auto &err : interpErrs) {
      std::cout << "Interpreter error: " << err << std::endl;
    }
    return -3;
  }

  std::cout << interpretutils::valueToString(val) << std::endl;
  return 0;
}

int runFile(const std::string &script) {
  std::ifstream file(script);
  if (!file) {
    std::cerr << "Could not open file: " << script << std::endl;
    return 1;
  }

  std::stringstream ss;
  ss << file.rdbuf();
  int rc = 0;
  try {
    rc = run(ss.str());
  } catch (const std::exception &ex) {
    // TODO: error handling. Print?
    return 65;
  }
  return rc;
}

int runRepl() {
  while (true) {
    std::string userInput;
    getline(std::cin, userInput);
    try {
      run(userInput);
    } catch (const std::exception &ex) {
      // TODO: error handling. Print?
    }
  }
  return 0;
}

} // namespace treewalk
} // namespace plox

int main(int argc, char **argv) {
  // Setup CLI
  CLI::App app{"Lox - Tree walk Implementation"};
  std::optional<std::string> script;
  app.add_option("script", script, "A path to a lox script");
  CLI11_PARSE(app, argc, argv);

  // Route to desired behaviour
  using namespace plox::treewalk;
  int rc = 0;
  if (script) {
    rc = runFile(script.value());
  } else {
    rc = runRepl();
  }

  return rc;
}