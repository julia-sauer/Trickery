CC = gcc
CFLAGS = -Wall -Wextra -O0 -fno-builtin -fPIC
LDFLAGS = -ldl

SRC_DIR = src
TEST_DIR = tests
BUILD_DIR = build
LIB_DIR = $(BUILD_DIR)/lib
BIN_DIR = $(BUILD_DIR)/bin

LIB = $(LIB_DIR)/hook.so

# all source files
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)

# default target
all: $(LIB)

# build shared library from ALL src/*.c
$(LIB): $(SRC_FILES)
	mkdir -p $(LIB_DIR)
	$(CC) -shared -fPIC $(SRC_FILES) -o $(LIB) $(LDFLAGS)

# build a test (example: printtest)
TEST = $(BIN_DIR)/printtest

test: $(TEST)

$(TEST): $(TEST_DIR)/printtest.c
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(TEST) $<

# run test with LD_PRELOAD
run: all test
	LD_PRELOAD=$(LIB) $(TEST)

# clean build folder
clean:
	rm -rf $(BUILD_DIR)