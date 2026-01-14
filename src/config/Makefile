# Configuration Module Makefile
# Phase: 6.7 - Configuration Abstraction (Stage 3)
# Date: 2025-11-25

# Compiler settings
CC = gcc
CFLAGS = -m32 -O2 -g -Wall -Wextra -fPIC -pipe -I../../include

# Directories
SRC_DIR = .
BUILD_DIR = ../build/config
OBJ_DIR = $(BUILD_DIR)/obj

# Source files
SOURCES = game_config.c etc_parser.c
OBJECTS = $(patsubst %.c,$(OBJ_DIR)/%.o,$(SOURCES))

# Target library
TARGET = $(BUILD_DIR)/libconfig.a

# Build rules
all: directories $(TARGET)

directories:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(OBJ_DIR)

$(TARGET): $(OBJECTS)
	@echo "[AR] Creating $@"
	@ar rcs $@ $^
	@echo "[Config] Build complete"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "[CC] $<"
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "[Config] Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR)
	@echo "[Config] Clean complete"

.PHONY: all directories clean
