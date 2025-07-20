find_package(CLI11 CONFIG REQUIRED)

add_library(
  tree-walk-lib
  ast_printer.cpp
  environment.cpp
  errs.cpp
  func_native.cpp
  func.cpp
  interpreter_visitor.cpp
  interpreter.cpp
  parser.cpp
  scanner.cpp
  stmt_printer.cpp
  value.cpp)
target_include_directories(tree-walk-lib PUBLIC .)
target_compile_options(tree-walk-lib PRIVATE -ggdb)
target_compile_options(tree-walk-lib PRIVATE -fno-inline -fno-inline-functions
                                             -fno-default-inline)

add_executable(tree-walk main.cpp)
target_link_libraries(tree-walk PRIVATE tree-walk-lib CLI11::CLI11)
