find_package(GTest CONFIG REQUIRED)
enable_testing()

add_executable(tree-walk-tst interpreter.t.cpp parser.t.cpp scanner.t.cpp)
target_link_libraries(
  tree-walk-tst PRIVATE tree-walk-lib GTest::gtest GTest::gtest_main
                        GTest::gmock GTest::gmock_main)
