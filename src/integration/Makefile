# Integration Module Makefile
# Game Systems Integration Layer

CC = gcc
CFLAGS = -m32 -Wall -Wextra -g -fPIC -I../../include -I..
LDFLAGS = -m32

# Source files
SRCS = game_integration.c
OBJS = $(SRCS:.c=.o)

# Target library
TARGET = libintegration.a

# Build directory
BUILD_DIR = ../../build/integration

.PHONY: all clean verify

all: $(BUILD_DIR) $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/$(TARGET): $(addprefix $(BUILD_DIR)/, $(OBJS))
	ar rcs $@ $^
	@echo "[Integration] Static library built: $@"

$(BUILD_DIR)/%.o: %.c game_integration.h
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "[Integration] Compiled: $<"

clean:
	rm -rf $(BUILD_DIR)
	@echo "[Integration] Clean complete"

verify:
	@echo "[Integration] Verifying build..."
	@test -f $(BUILD_DIR)/$(TARGET) && echo "Library OK" || echo "Library MISSING"
