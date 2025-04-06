find_package(CLI11 CONFIG REQUIRED)

add_library(tree-walk-lib ast_printer.cpp errs.cpp parser.cpp scanner.cpp)
target_include_directories(tree-walk-lib PUBLIC .)
target_compile_options(tree-walk-lib PRIVATE -ggdb)

add_executable(tree-walk main.cpp)
target_link_libraries(tree-walk PRIVATE tree-walk-lib CLI11::CLI11)
