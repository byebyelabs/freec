.PHONY: all clean tests setup

CC        = clang

# addr2line requires -g flag
# Citation: https://stackoverflow.com/a/7648826
CFLAGS    = -std=c11 -Wall -Wextra -Wpedantic -O0 -Iinclude -pthread -g

BUILD_SRC = src/runtime.c src/utils.c src/tracing.c src/alist.c
BUILD_DIR = build

FORMATTER = clang-format
SOURCES = $(shell find . -name "*.c" -o -name "*.h")

LAB = byebye
PRJ = freec
LIB = $(LAB)_$(PRJ)

# for tests
export CC
export CFLAGS
export BUILD_DIR
export LIB

all:
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -shared -fPIC -o $(BUILD_DIR)/$(LIB).so $(BUILD_SRC) -ldl

setup:
	@make all
	@echo "alias freec='$(shell pwd)/$(BUILD_DIR)/$(LIB).so'" >> ~/.bash_rc

tests:
	@python3 tests "tests/*.c"

tests/double_free:
	@python3 tests "tests/double_free_*"

tests/use_after_free:
	@python3 tests "tests/use_after_free_*"

tests/invalid_free:
	@python3 tests "tests/invalid_free_*"

tests/memory_leak:
	@python3 tests "tests/memory_leak_*"

format:
	$(FORMATTER) -i $(SOURCES)

clean:
	rm -rf $(BUILD_DIR)
