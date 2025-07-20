#include <CLI/CLI.hpp>
#include <fstream>
#include <iostream>
#include <optional>

#include <ast_printer.h>
#include <func_native.h>
#include <interpreter.h>
#include <parser.h>
#include <scanner.h>

namespace plox {
namespace treewalk {

namespace {
auto s_globals = std::make_shared<Environment>();
}

void initNativeFuncs(std::shared_ptr<Environment> env) {
  nativefunc::addClock(env);
}

int run(const std::string &buff) {
  // Scan
  std::vector<SyntaxException> syntErrs;
  auto tokens = scanTokens(buff, syntErrs);
  if (syntErrs.size()) {
    for (auto &err : syntErrs) {
      std::cout << "Syntax error: " << err << std::endl;
    }
    return -1;
  }

  // Parse
  std::vector<ParseException> parsErrs;
  auto stmts = parse(tokens, parsErrs);
  if (parsErrs.size()) {
    for (auto &err : parsErrs) {
      std::cout << "Parse error: " << err << std::endl;
    }
    return -2;
  }

  // Interpret
  std::vector<InterpretException> interpErrs;
  interpret(stmts, s_globals, interpErrs);
  if (interpErrs.size()) {
    for (auto &err : interpErrs) {
      std::cout << "Interpreter error: " << err << std::endl;
    }
    return -3;
  }

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

int runCmds(const std::string &cmds) {
  int rc = 0;
  try {
    rc = run(cmds);
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
  auto script_option =
      app.add_option("-s,--script", script, "A path to a lox script");
  std::optional<std::string> commands;
  auto cmds_option = app.add_option("-c,--commands", commands, "Lox commands");

  // Allow script *or* command to be passed in - not both
  script_option->excludes(cmds_option);
  cmds_option->excludes(script_option);

  CLI11_PARSE(app, argc, argv);

  // Route to desired behaviour
  using namespace plox::treewalk;
  initNativeFuncs(s_globals);
  int rc = 0;
  if (script) {
    rc = runFile(script.value());
  } else if (commands) {
    rc = runCmds(commands.value());
  } else {
    rc = runRepl();
  }

  return rc;
}