#include <CLI/CLI.hpp>
#include <fstream>
#include <iostream>
#include <optional>

namespace plox {
namespace treewalk {

/*
 * Throws SyntaxException
 */
int run(const std::string &buff) {
  std::cout << "This was your command! " << buff << std::endl;
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