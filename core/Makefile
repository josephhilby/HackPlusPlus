EMCC        := emcc

SRC_DIR     := emu
UTIL_DIR    := util
BUILD_DIR   := build
WASM_DIR    := ../web/src/runtime/wasm

TARGET_NAME := computer
WASM_JS     := $(WASM_DIR)/$(TARGET_NAME).js

SRCS := \
  $(SRC_DIR)/computer.c

OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

CFLAGS := -std=c2x -Wall -Wextra -Wpedantic \
  -I$(SRC_DIR) \
  -I$(UTIL_DIR)

LDFLAGS := \
  -s WASM=1 \
  -s MODULARIZE=1 \
  -s EXPORT_ES6=1 \
  -s ENVIRONMENT=web \
  -s ALLOW_MEMORY_GROWTH=1 \
  -s EXPORTED_FUNCTIONS='["_init","_get_rom_ptr","_commit_rom","_run","_stop","_step","_reset","_set_keyboard","_get_state_ptr","_get_framebuffer_ptr"]' \
  -s EXPORTED_RUNTIME_METHODS='["HEAPU8"]'

.PHONY: all wasm clean

all: wasm

wasm: $(WASM_JS)

$(WASM_JS): $(OBJS)
	mkdir -p $(dir $@)
	$(EMCC) $(OBJS) $(LDFLAGS) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(EMCC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) \
	       $(WASM_DIR)/$(TARGET_NAME).js \
	       $(WASM_DIR)/$(TARGET_NAME).wasm