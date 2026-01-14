# DNF Client Makefile
# Supports Windows (MinGW) and Linux

# Detect OS
ifeq ($(OS),Windows_NT)
    PLATFORM = windows
    EXE = .exe
    RM = del /Q
    MKDIR = mkdir
    SEP = \\
else
    PLATFORM = linux
    EXE =
    RM = rm -f
    MKDIR = mkdir -p
    SEP = /
endif

# Compiler
CC = gcc
CFLAGS = -Wall -Wextra -Wno-unused-parameter -std=c99 -O2 -g

# SDL2 flags
ifeq ($(PLATFORM),windows)
    # Windows (assumes SDL2 is installed via vcpkg or similar)
    SDL2_CFLAGS = -IC:/SDL2/include
    SDL2_LIBS = -LC:/SDL2/lib -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lws2_32
else
    # Linux
    SDL2_CFLAGS = $(shell pkg-config --cflags sdl2 SDL2_image SDL2_ttf SDL2_mixer)
    SDL2_LIBS = $(shell pkg-config --libs sdl2 SDL2_image SDL2_ttf SDL2_mixer)
endif

# Directories
SRC_DIR = src
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(BUILD_DIR)/bin

# Output
TARGET = $(BIN_DIR)/dnf_client$(EXE)

# Source files
SOURCES = \
    $(SRC_DIR)/core/client.c \
    $(SRC_DIR)/core/input_manager.c \
    $(SRC_DIR)/render/renderer.c \
    $(SRC_DIR)/render/map_renderer.c \
    $(SRC_DIR)/render/entity_renderer.c \
    $(SRC_DIR)/render/animation.c \
    $(SRC_DIR)/render/effects.c \
    $(SRC_DIR)/render/camera.c \
    $(SRC_DIR)/network/network.c \
    $(SRC_DIR)/network/packet_parser.c \
    $(SRC_DIR)/network/message_handler.c \
    $(SRC_DIR)/network/sync_position.c \
    $(SRC_DIR)/network/sync_combat.c \
    $(SRC_DIR)/network/sync_state.c \
    $(SRC_DIR)/network/lag_compensation.c \
    $(SRC_DIR)/game/entity.c \
    $(SRC_DIR)/game/player_controller.c \
    $(SRC_DIR)/game/skill_system.c \
    $(SRC_DIR)/game/interaction.c \
    $(SRC_DIR)/scene/scene_manager.c \
    $(SRC_DIR)/scene/scene_login.c \
    $(SRC_DIR)/scene/scene_charselect.c \
    $(SRC_DIR)/scene/scene_town.c \
    $(SRC_DIR)/scene/scene_dungeon.c \
    $(SRC_DIR)/ui/ui.c \
    $(SRC_DIR)/ui/ui_inventory.c \
    $(SRC_DIR)/ui/ui_skill.c \
    $(SRC_DIR)/ui/ui_character.c \
    $(SRC_DIR)/ui/ui_chat.c \
    $(SRC_DIR)/ui/ui_minimap.c \
    $(SRC_DIR)/ui/ui_hud.c \
    $(SRC_DIR)/resource/npk_parser.c \
    $(SRC_DIR)/resource/img_decoder.c \
    $(SRC_DIR)/resource/resource_manager.c \
    $(SRC_DIR)/resource/ani_loader.c \
    $(SRC_DIR)/resource/cache_manager.c

# Object files
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))

# Include paths
INCLUDES = -Iinclude -I$(PVF_INC_DIR) $(SDL2_CFLAGS)

# PVF library path
PVF_LIB_DIR = ../src/lib
PVF_INC_DIR = ../include/pvf

# Libraries
LIBS = $(SDL2_LIBS) -L$(PVF_LIB_DIR) -lpvf -lz -lm

# Default target
all: dirs $(TARGET)
	@echo "Build complete: $(TARGET)"

# Create directories
dirs:
	@$(MKDIR) $(OBJ_DIR)$(SEP)core 2>nul || true
	@$(MKDIR) $(OBJ_DIR)$(SEP)render 2>nul || true
	@$(MKDIR) $(OBJ_DIR)$(SEP)network 2>nul || true
	@$(MKDIR) $(OBJ_DIR)$(SEP)game 2>nul || true
	@$(MKDIR) $(OBJ_DIR)$(SEP)scene 2>nul || true
	@$(MKDIR) $(OBJ_DIR)$(SEP)ui 2>nul || true
	@$(MKDIR) $(OBJ_DIR)$(SEP)resource 2>nul || true
	@$(MKDIR) $(BIN_DIR) 2>nul || true

# Link
$(TARGET): $(OBJECTS)
	@echo "Linking: $@"
	$(CC) $(OBJECTS) -o $@ $(LIBS)

# Compile
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compiling: $<"
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Clean
clean:
ifeq ($(PLATFORM),windows)
	@if exist $(BUILD_DIR) rmdir /S /Q $(BUILD_DIR)
else
	@rm -rf $(BUILD_DIR)
endif

# Run
run: all
	$(TARGET)

# Debug build
debug: CFLAGS += -DDEBUG -O0
debug: all

# Release build
release: CFLAGS += -DNDEBUG -O3
release: all

# Install SDL2 (Linux only)
install-deps:
ifeq ($(PLATFORM),linux)
	sudo apt-get install -y libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev
else
	@echo "On Windows, install SDL2 manually or use vcpkg"
endif

# Help
help:
	@echo "DNF Client Build System"
	@echo ""
	@echo "Targets:"
	@echo "  all          - Build client (default)"
	@echo "  clean        - Remove build files"
	@echo "  run          - Build and run"
	@echo "  debug        - Debug build"
	@echo "  release      - Release build"
	@echo "  install-deps - Install SDL2 (Linux)"
	@echo "  help         - Show this help"
	@echo ""
	@echo "Platform: $(PLATFORM)"
	@echo "Output:   $(TARGET)"

.PHONY: all dirs clean run debug release install-deps help
