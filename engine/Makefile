# Game Engine Makefile
# GCC 4.1.2 compatible, Linux target

# Compiler settings
CC = gcc
CFLAGS = -O2 -g -Wall -Wextra -Wno-unused-parameter -D_GNU_SOURCE -fPIC -std=c99
LDFLAGS = -lpthread -lm

# Optional Lua support (set USE_LUA=1 to enable)
ifdef USE_LUA
CFLAGS += -DUSE_LUA
LDFLAGS += -llua
endif

# Directories
BUILD_DIR = build
LIB_DIR = lib
OBJ_DIR = $(BUILD_DIR)/obj

# Output library
ENGINE_LIB = $(LIB_DIR)/libengine.a

# Source files - Core layer
CORE_SOURCES = \
	core/network/epoll_reactor.c \
	core/network/tcp_listener.c \
	core/network/tcp_connection.c \
	core/network/buffer.c \
	core/event/event_bus.c \
	core/timer/timer_wheel.c \
	core/log/logger.c \
	core/config/config_loader.c \
	core/memory/pool_allocator.c

# Data layer
DATA_SOURCES = \
	data/orm/orm_wrapper.c \
	data/cache/lru_cache.c

# Session layer
SESSION_SOURCES = \
	session/session.c \
	session/session_manager.c

# Protocol layer
PROTOCOL_SOURCES = \
	protocol/message_router.c \
	protocol/codec/packet_codec.c

# IPC layer
IPC_SOURCES = \
	ipc/ipc_client.c

# Game layer
GAME_SOURCES = \
	game/entity/entity.c \
	game/combat/combat_engine.c \
	game/buff/buff_system.c \
	game/ai/monster_ai.c \
	game/dungeon/dungeon_manager.c

# Script layer
SCRIPT_SOURCES = \
	script/lua_engine.c \
	script/hot_reload.c \
	script/bindings/lua_combat.c \
	script/bindings/lua_entity.c \
	script/bindings/lua_event.c

# All sources
ALL_SOURCES = $(CORE_SOURCES) $(DATA_SOURCES) $(SESSION_SOURCES) \
              $(PROTOCOL_SOURCES) $(IPC_SOURCES) $(GAME_SOURCES) $(SCRIPT_SOURCES)

# Object files
OBJECTS = $(patsubst %.c,$(OBJ_DIR)/%.o,$(ALL_SOURCES))

# Include paths
INCLUDES = -I. -Icore

# Default target
all: dirs $(ENGINE_LIB)
	@echo "Engine library built: $(ENGINE_LIB)"
	@echo "Total source files: $(words $(ALL_SOURCES))"

# Create directories
dirs:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(OBJ_DIR)/core/network
	@mkdir -p $(OBJ_DIR)/core/event
	@mkdir -p $(OBJ_DIR)/core/timer
	@mkdir -p $(OBJ_DIR)/core/log
	@mkdir -p $(OBJ_DIR)/core/config
	@mkdir -p $(OBJ_DIR)/core/memory
	@mkdir -p $(OBJ_DIR)/data/orm
	@mkdir -p $(OBJ_DIR)/data/cache
	@mkdir -p $(OBJ_DIR)/session
	@mkdir -p $(OBJ_DIR)/protocol/codec
	@mkdir -p $(OBJ_DIR)/ipc
	@mkdir -p $(OBJ_DIR)/game/entity
	@mkdir -p $(OBJ_DIR)/game/combat
	@mkdir -p $(OBJ_DIR)/game/buff
	@mkdir -p $(OBJ_DIR)/game/ai
	@mkdir -p $(OBJ_DIR)/game/dungeon
	@mkdir -p $(OBJ_DIR)/script/bindings
	@mkdir -p $(LIB_DIR)

# Build static library
$(ENGINE_LIB): $(OBJECTS)
	@echo "Creating library: $@"
	ar rcs $@ $^

# Generic compile rule
$(OBJ_DIR)/%.o: %.c
	@echo "Compiling: $<"
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Test program
TEST_SRC = tests/test_engine.c
TEST_BIN = $(BUILD_DIR)/test_engine

test: all $(TEST_BIN)
	@echo "Running tests..."
	./$(TEST_BIN)

$(TEST_BIN): $(TEST_SRC) $(ENGINE_LIB)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $< -L$(LIB_DIR) -lengine $(LDFLAGS)

# Game server
GAME_SERVER_SRC = services/game/game_main.c
GAME_SERVER_BIN = $(BUILD_DIR)/game_server

game: all $(GAME_SERVER_BIN)
	@echo "Game server built: $(GAME_SERVER_BIN)"

$(GAME_SERVER_BIN): $(GAME_SERVER_SRC) $(ENGINE_LIB)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $< -L$(LIB_DIR) -lengine $(LDFLAGS)

# Clean
clean:
	rm -rf $(BUILD_DIR) $(LIB_DIR)

# Install to project lib directory
install: all
	@mkdir -p ../src/lib
	cp $(ENGINE_LIB) ../src/lib/
	@echo "Installed to ../src/lib/"

# Count lines of code
loc:
	@echo "Lines of code by layer:"
	@echo "  Core:     $$(wc -l core/*/*.c core/*/*/*.c 2>/dev/null | tail -1 | awk '{print $$1}')"
	@echo "  Data:     $$(wc -l data/*/*.c 2>/dev/null | tail -1 | awk '{print $$1}')"
	@echo "  Session:  $$(wc -l session/*.c 2>/dev/null | tail -1 | awk '{print $$1}')"
	@echo "  Protocol: $$(wc -l protocol/*.c protocol/*/*.c 2>/dev/null | tail -1 | awk '{print $$1}')"
	@echo "  IPC:      $$(wc -l ipc/*.c 2>/dev/null | tail -1 | awk '{print $$1}')"
	@echo "  Game:     $$(wc -l game/*/*.c 2>/dev/null | tail -1 | awk '{print $$1}')"
	@echo "  Script:   $$(wc -l script/*.c script/*/*.c 2>/dev/null | tail -1 | awk '{print $$1}')"
	@echo "  Total:    $$(wc -l $(ALL_SOURCES) | tail -1 | awk '{print $$1}')"

# Show help
help:
	@echo "Game Engine Build System"
	@echo ""
	@echo "Targets:"
	@echo "  all     - Build engine library (default)"
	@echo "  test    - Build and run tests"
	@echo "  game    - Build game server"
	@echo "  clean   - Remove build files"
	@echo "  install - Install to project lib directory"
	@echo "  loc     - Count lines of code"
	@echo "  help    - Show this help"
	@echo ""
	@echo "Options:"
	@echo "  USE_LUA=1 - Enable Lua support"
	@echo ""
	@echo "Output:"
	@echo "  $(ENGINE_LIB)"
	@echo "  $(GAME_SERVER_BIN)"

.PHONY: all dirs test game clean install loc help
