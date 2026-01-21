CC      := cc
TARGET  := hackemu
OBJ_DIR := build

# Source files (relative to src/)
SRCS := \
  main.c \
  server/server.c \
  emu/mem.c \
  vendor/mongoose.c

# Map foo/bar.c -> build/foo/bar.o
OBJS := $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRCS))

CFLAGS := -std=c2x -Wall -Wextra -Wpedantic \
  -D_POSIX_C_SOURCE=200809L \
  -D_DEFAULT_SOURCE \
  -D_GNU_SOURCE \
  -I. \
  -Iemu \
  -Iserver \
  -Iutil \
  -Ivendor

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@

$(OBJ_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)
