.PHONY: all clean

CC        = clang
CFLAGS    = -std=c11 -Wall -Wextra -Wpedantic -O0 -Iinclude

BUILD_SRC = src/runtime.c src/utils.c
BUILD_DIR = build

FORMATTER = clang-format
SOURCES = $(shell find . -o -name "*.c" -o -name "*.h")

LAB = byebye
PRJ = freec
LIB = $(LAB)_$(PRJ)

all:
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -shared -fPIC -o $(BUILD_DIR)/$(LIB).so $(BUILD_SRC) -ldl

tests/double_free_basic:
	$(CC) $(CFLAGS) -o tests/exe_double_free_basic tests/double_free_basic.c
	LD_PRELOAD=./$(BUILD_DIR)/$(LIB).so ./tests/exe_double_free_basic || true

format:
	$(FORMATTER) -i $(SOURCES)

clean:
	rm -rf $(BUILD_DIR)
