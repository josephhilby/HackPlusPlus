CC             := cc
CXX            := c++
AR             := ar
RANLIB         := ranlib

SRC_DIR        := src
INC_DIR        := include
SHARED_DIR     := ../shared
OBJ_DIR        := build

LIB_A          := $(OBJ_DIR)/libvm.a

TEST_DIR       := test/unit
TEST_BIN       := $(OBJ_DIR)/vm_tests

GTEST_INC      ?= /opt/homebrew/include
GTEST_LIB      ?= /opt/homebrew/lib
GTEST_LIBS     := -lgtest -lgtest_main

CFLAGS         := -std=c2x -Wall -Wextra -Wpedantic
CFLAGS         += -I$(INC_DIR) -I$(SRC_DIR) -I$(SHARED_DIR)

CXXFLAGS       := -std=c++20

SRCS           := $(wildcard $(SRC_DIR)/*.c) \
                  $(wildcard $(SHARED_DIR)/*.c)

OBJS           := $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRCS))

SAN_CFLAGS     := -g -O0 -fno-omit-frame-pointer
SAN_FLAGS      := -fsanitize=address,undefined

LLVM_BINDIR    ?= /opt/homebrew/opt/llvm/bin
CLANG          := $(LLVM_BINDIR)/clang
CLANGXX        := $(LLVM_BINDIR)/clang++

LEAK_CXXWARN   := -Wno-character-conversion
LEAK_BIN       := $(OBJ_DIR)/vm_tests_leak
LEAK_LIB_A     := $(OBJ_DIR)/libvm_leak.a
LEAK_OBJ_DIR   := $(OBJ_DIR)/leak
LEAK_OBJS      := $(patsubst %.c,$(LEAK_OBJ_DIR)/%.o,$(SRCS))

ASAN_OPTIONS   ?= detect_leaks=1:abort_on_error=1:strict_string_checks=1:check_initialization_order=1
UBSAN_OPTIONS  ?= halt_on_error=1:print_stacktrace=1

.PHONY: all clean test debug leak

all: $(LIB_A)

$(LIB_A): $(OBJS)
	$(AR) rcs $@ $(OBJS)
	$(RANLIB) $@

$(OBJ_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

test: CFLAGS   += -g -O0
test: CXXFLAGS += -g -O0
test: $(TEST_BIN)
	./$(TEST_BIN)

$(TEST_BIN): $(LIB_A) $(TEST_DIR)/vm_tests.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) \
	  -I$(INC_DIR) -I$(SRC_DIR) -I$(SHARED_DIR) -I$(GTEST_INC) \
	  $(TEST_DIR)/vm_tests.cpp \
	  $(LIB_A) \
	  -L$(GTEST_LIB) $(GTEST_LIBS) \
	  -o $@

debug: CFLAGS   += -g -O0
debug: CXXFLAGS += -g -O0
debug: $(TEST_BIN)

$(LEAK_LIB_A): $(LEAK_OBJS)
	mkdir -p $(dir $@)
	$(AR) rcs $@ $(LEAK_OBJS)
	$(RANLIB) $@

$(LEAK_OBJ_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CLANG) $(CFLAGS) $(SAN_CFLAGS) $(SAN_FLAGS) -c $< -o $@

$(LEAK_BIN): $(LEAK_LIB_A) $(TEST_DIR)/vm_tests.cpp
	mkdir -p $(dir $@)
	$(CLANGXX) $(CXXFLAGS) $(LEAK_CXXWARN) $(SAN_CFLAGS) $(SAN_FLAGS) \
	  -I$(INC_DIR) -I$(SRC_DIR) -I$(SHARED_DIR) -I$(GTEST_INC) \
	  $(TEST_DIR)/vm_tests.cpp \
	  $(LEAK_LIB_A) \
	  -L$(GTEST_LIB) $(GTEST_LIBS) \
	  -o $@

leak: $(LEAK_BIN)
	ASAN_OPTIONS="$(ASAN_OPTIONS)" UBSAN_OPTIONS="$(UBSAN_OPTIONS)" ./$(LEAK_BIN)