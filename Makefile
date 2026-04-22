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

# compile shared library from ALL files in src folder into build/lib folder
$(LIB): $(SRC_FILES)
	mkdir -p $(LIB_DIR)
	$(CC) -shared -fPIC $(SRC_FILES) -o $(LIB) $(LDFLAGS)

# all test source files
TEST_FILES = $(wildcard $(TEST_DIR)/*.c)

# corresponding binaries
TEST_BINS = $(patsubst $(TEST_DIR)/%.c,$(BIN_DIR)/%,$(TEST_FILES))

test: $(TEST_BINS)

# compile all files in test folder into build/bin folder
$(BIN_DIR)/%: $(TEST_DIR)/%.c
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $<

# run all tests with LD_PRELOAD
run: all test
	for test in $(TEST_BINS); do \
		echo "Running $$test"; \
		LD_PRELOAD=$(LIB) $$test; \
		echo ""; \
	done

# clean build folder
clean:
	rm -rf $(BUILD_DIR)