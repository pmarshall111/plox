### Building

.PHONY: build
build: 
	mkdir -p build; cmake --build build --parallel 10

### Running

.PHONY: run
run: build
	@echo "Running binary in REPL mode..."
	./build/tree-walk/src/tree-walk	


### Testing

.PHONY: unit-test
unit-test: build
	./build/tree-walk/tests/tree-walk-tst

.PHONY: system-test
system-test: build
	pytest

.PHONY: test
test: unit-test system-test

### Developer Tooling

SRC_FILES=$(shell find tree-walk -name "*.cpp" -o -name "*.h")
CMAKE_FILES=$(shell find tree-walk -name "*CMakeLists.txt")

.PHONY: format
format:
	clang-format -i $(SRC_FILES)
	cmake-format -i $(CMAKE_FILES)
	black -q .

.PHONY: gen
gen:
	python3.12 tree-walk/tools/gen_ast_h.py