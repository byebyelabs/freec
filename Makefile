.PHONY: all clean tests

CC        = clang
CFLAGS    = -std=c11 -Wall -Wextra -Wpedantic -g -O0 -Iinclude

BUILD_SRC = src/runtime/runtime.c src/runtime/log.c
BUILD_DIR = build

FORMATTER = clang-format
SOURCES = $(shell find . -name "*.cpp" -o -name "*.c" -o -name "*.h")

LAB = byebye
PRJ = freec
LIB = $(LAB)_$(PRJ)

# for tests
export CC
export CFLAGS
export BUILD_DIR
export LIB

all:
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -shared -fPIC -o $(BUILD_DIR)/$(LIB).so $(BUILD_SRC) -ldl

tests:
	@python3 tests "tests/*.c"

tests/double_free:
	@python3 tests "tests/double_free_*"

tests/use_after_free:
	@python3 tests "tests/use_after_free_*"

tests/invalid_free:
	@python3 tests "tests/invalid_free_*"

format:
	$(FORMATTER) -i $(SOURCES)

clean:
	@rm -rf $(BUILD_DIR)
