CC         := cc
CXX        := c++
AR         := ar
RANLIB     := ranlib

SRC_DIR    := src
OBJ_DIR    := build
INC_DIR    := include
LIB_A      := $(OBJ_DIR)/libassembler.a

TEST_DIR   := test/unit
TEST_BIN   := $(OBJ_DIR)/assembler_tests

GTEST_INC  := /opt/homebrew/include
GTEST_LIB  := /opt/homebrew/lib
GTEST_LIBS := -lgtest -lgtest_main

CFLAGS     := -std=c2x -Wall -Wextra -Wpedantic -I$(INC_DIR) -I$(SRC_DIR)
CXXFLAGS   := -std=c++20

SRCS       := $(wildcard $(SRC_DIR)/*.c)
OBJS       := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

.PHONY: all clean test debug

all: $(LIB_A)

$(LIB_A): $(OBJS) | $(OBJ_DIR)
	$(AR) rcs $@ $(OBJS)
	$(RANLIB) $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR)

test: CFLAGS   += -g -O0
test: CXXFLAGS += -g -O0
test: $(TEST_BIN)
	./$(TEST_BIN)

$(TEST_BIN): $(LIB_A) $(TEST_DIR)/assembler_tests.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) \
	  -I$(INC_DIR) -I$(SRC_DIR) -I$(GTEST_INC) \
	  $(TEST_DIR)/assembler_tests.cpp \
	  $(LIB_A) \
	  -L$(GTEST_LIB) $(GTEST_LIBS) \
	  -o $@

debug: CFLAGS   += -g -O0
debug: CXXFLAGS += -g -O0
debug: $(TEST_BIN)