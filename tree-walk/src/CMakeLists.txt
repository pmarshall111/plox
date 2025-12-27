find_package(CLI11 CONFIG REQUIRED)

add_library(
  tree-walk-lib
  ast_printer.cpp
  class.cpp
  environment.cpp
  errs.cpp
  func_native.cpp
  func.cpp
  interpreter.cpp
  parser.cpp
  scanner.cpp
  stmt_printer.cpp
  value_printer.cpp)
target_include_directories(tree-walk-lib PUBLIC .)

add_executable(tree-walk main.cpp)
target_link_libraries(tree-walk PRIVATE tree-walk-lib CLI11::CLI11)
