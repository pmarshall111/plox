#include <CLI/CLI.hpp>
#include <fstream>
#include <iostream>
#include <optional>

int run(const std::string& buff) {
    std::cout << "This was your command! " << buff << std::endl;
    return 0;
}

int runFile(const std::string& script) {
    std::ifstream file(script);
    if (!file) {
        std::cerr << "Could not open file: " << script << std::endl;
        return 1;
    }
    
    std::stringstream ss;
    ss << file.rdbuf();
    return run(ss.str());
}

int runRepl() {
    while (true) {
        std::string userInput;
        getline(std::cin, userInput);
        if (-999 == run(userInput)) {
            // Sentinel value to exit repl
            return 0;
        }
    }
}

int main(int argc, char** argv) {
    // Setup CLI
    CLI::App app{"Lox - Tree walk Implementation"};
    std::optional<std::string> script;
    app.add_option("script", script, "A path to a lox script");
    CLI11_PARSE(app, argc, argv);

    // Route to desired behaviour
    int rc = 0;
    if (script) {
        rc = runFile(script.value());
    } else {
        rc = runRepl();
    }

    return rc;
}
