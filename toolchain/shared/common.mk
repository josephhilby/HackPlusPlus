# Compiler/toolchain commands
CC             := cc
CXX            := c++
AR             := ar
RANLIB         := ranlib

# Project layout
SRC_DIR        := src
INC_DIR        := include
SHARED_DIR     := ../shared
OBJ_DIR        := build

# Output paths, supplied by each component Makefile
BIN            := $(OBJ_DIR)/$(BIN_NAME)
LIB_A          := $(OBJ_DIR)/$(LIB_NAME)
TEST_BIN       := $(OBJ_DIR)/$(TEST_NAME)

# main.c builds the executable, but is excluded from the static library
APP_SRC        := $(SRC_DIR)/main.c

# Component source files and shared source files
LOCAL_SRCS     := $(filter-out $(APP_SRC),$(wildcard $(SRC_DIR)/*.c))
SHARED_SRCS    := $(wildcard $(SHARED_DIR)/*.c)

# Object files go under build/
# src/foo.c        -> build/foo.o
# ../shared/foo.c  -> build/shared/foo.o
LOCAL_OBJS     := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(LOCAL_SRCS))
SHARED_OBJS    := $(patsubst $(SHARED_DIR)/%.c,$(OBJ_DIR)/shared/%.o,$(SHARED_SRCS))

# Library objects exclude main.c
LIB_OBJS       := $(LOCAL_OBJS) $(SHARED_OBJS)

# Executable entry-point object
APP_OBJ        := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(APP_SRC))

# GoogleTest paths
GTEST_INC      ?= /opt/homebrew/include
GTEST_LIB      ?= /opt/homebrew/lib
GTEST_LIBS     := -lgtest -lgtest_main

# Normal C/C++ build flags
CFLAGS         := -std=c2x -Wall -Wextra -Wpedantic
CFLAGS         += -I$(INC_DIR) -I$(SRC_DIR) -I$(SHARED_DIR)

CXXFLAGS       := -std=c++20

# Sanitizer/debug flags for leak target
SAN_CFLAGS     := -g -O0 -fno-omit-frame-pointer
SAN_FLAGS      := -fsanitize=address,undefined

# Clang is used for sanitizer builds
LLVM_BINDIR    ?= /opt/homebrew/opt/llvm/bin
CLANG          := $(LLVM_BINDIR)/clang
CLANGXX        := $(LLVM_BINDIR)/clang++

# Leak/sanitizer build outputs
LEAK_CXXWARN   := -Wno-character-conversion
LEAK_BIN       := $(OBJ_DIR)/$(TEST_NAME)_leak
LEAK_LIB_A     := $(OBJ_DIR)/$(basename $(LIB_NAME))_leak.a
LEAK_OBJ_DIR   := $(OBJ_DIR)/leak

# Sanitizer object files mirror normal object layout under build/leak/
LEAK_LOCAL_OBJS  := $(patsubst $(SRC_DIR)/%.c,$(LEAK_OBJ_DIR)/%.o,$(LOCAL_SRCS))
LEAK_SHARED_OBJS := $(patsubst $(SHARED_DIR)/%.c,$(LEAK_OBJ_DIR)/shared/%.o,$(SHARED_SRCS))
LEAK_OBJS        := $(LEAK_LOCAL_OBJS) $(LEAK_SHARED_OBJS)

# Runtime sanitizer options
ASAN_OPTIONS   ?= detect_leaks=1:abort_on_error=1:strict_string_checks=1:check_initialization_order=1
UBSAN_OPTIONS  ?= halt_on_error=1:print_stacktrace=1

.PHONY: all clean test debug leak

# Default build: static library + runnable CLI binary
all: $(LIB_A) $(BIN)

# Build the component static library from library objects
$(LIB_A): $(LIB_OBJS)
	$(AR) rcs $@ $(LIB_OBJS)
	$(RANLIB) $@

# Build the CLI executable from main.o + static library
$(BIN): $(APP_OBJ) $(LIB_A)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(APP_OBJ) $(LIB_A) -o $@

# Compile component source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile shared source files into build/shared/
$(OBJ_DIR)/shared/%.o: $(SHARED_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Remove all generated build artifacts
clean:
	rm -rf $(OBJ_DIR)

# Build and run GoogleTest unit tests
test: CFLAGS   += -g -O0
test: CXXFLAGS += -g -O0
test: $(TEST_BIN)
	./$(TEST_BIN)

# Link test binary against the component static library
$(TEST_BIN): $(LIB_A) $(TEST_SRC)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) \
	  -I$(INC_DIR) -I$(SRC_DIR) -I$(SHARED_DIR) -I$(GTEST_INC) \
	  $(TEST_SRC) \
	  $(LIB_A) \
	  -L$(GTEST_LIB) $(GTEST_LIBS) \
	  -o $@

# Build tests with debug symbols, but do not run them
debug: CFLAGS   += -g -O0
debug: CXXFLAGS += -g -O0
debug: $(TEST_BIN)

# Build sanitizer-enabled static library
$(LEAK_LIB_A): $(LEAK_OBJS)
	mkdir -p $(dir $@)
	$(AR) rcs $@ $(LEAK_OBJS)
	$(RANLIB) $@

# Compile component source files with sanitizer flags
$(LEAK_OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CLANG) $(CFLAGS) $(SAN_CFLAGS) $(SAN_FLAGS) -c $< -o $@

# Compile shared source files with sanitizer flags
$(LEAK_OBJ_DIR)/shared/%.o: $(SHARED_DIR)/%.c
	mkdir -p $(dir $@)
	$(CLANG) $(CFLAGS) $(SAN_CFLAGS) $(SAN_FLAGS) -c $< -o $@

# Link sanitizer-enabled test binary
$(LEAK_BIN): $(LEAK_LIB_A) $(TEST_SRC)
	mkdir -p $(dir $@)
	$(CLANGXX) $(CXXFLAGS) $(LEAK_CXXWARN) $(SAN_CFLAGS) $(SAN_FLAGS) \
	  -I$(INC_DIR) -I$(SRC_DIR) -I$(SHARED_DIR) -I$(GTEST_INC) \
	  $(TEST_SRC) \
	  $(LEAK_LIB_A) \
	  -L$(GTEST_LIB) $(GTEST_LIBS) \
	  -o $@

# Run sanitizer-enabled tests
leak: $(LEAK_BIN)
	ASAN_OPTIONS="$(ASAN_OPTIONS)" UBSAN_OPTIONS="$(UBSAN_OPTIONS)" ./$(LEAK_BIN)