SRC_FILES=$(shell find tree-walk -name "*.cpp" -o -name "*.h")
CMAKE_FILES=$(shell find tree-walk -name "*CMakeLists.txt")

.PHONY: format
format:
	clang-format -i $(SRC_FILES)
	cmake-format -i $(CMAKE_FILES)