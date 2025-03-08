find_package(CLI11 CONFIG REQUIRED)

add_executable(tree-walk main.cpp)

target_link_libraries(tree-walk PRIVATE CLI11::CLI11)