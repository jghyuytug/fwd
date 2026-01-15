###############################################################################
# Guild Module Test Configuration
# Compiles and runs guild module tests
###############################################################################

MODULE = guild
TESTDIR = .
SRCDIR = ../../src/guild
INCLUDEDIR = ../../include

# Compiler configuration (32-bit CentOS 5/7 target)
CC = gcc
CFLAGS = -m32 -O2 -g -fPIC -pipe -Wall -Wextra -std=c99
LDFLAGS = -m32
INCLUDES = -I$(INCLUDEDIR) -I$(INCLUDEDIR)/common -I$(INCLUDEDIR)/guild \
           -I../../src/orm/core_game/d_guild/headers

# Source files
TEST_SOURCES = $(TESTDIR)/test_guild_manager.c
GUILD_SOURCES = $(SRCDIR)/guild_manager.c \
                $(SRCDIR)/guild_level.c \
                $(SRCDIR)/guild_skill.c

# Test executable
TEST_EXEC = test_guild_manager

# Phony targets
.PHONY: all test clean help

# Default target: build and run tests
all: test

# Build test executable
$(TEST_EXEC): $(TEST_SOURCES) $(GUILD_SOURCES)
	@echo "[CC] Building guild tests..."
	$(CC) $(CFLAGS) $(INCLUDES) $(TEST_SOURCES) $(GUILD_SOURCES) -o $(TEST_EXEC) $(LDFLAGS)
	@echo "[SUCCESS] Test executable created: $(TEST_EXEC)"

# Run tests
test: $(TEST_EXEC)
	@echo ""
	@echo "=== Running Guild Module Tests ==="
	@./$(TEST_EXEC)
	@echo ""

# Clean build artifacts
clean:
	@echo "[CLEAN] Removing test artifacts..."
	rm -f $(TEST_EXEC)
	@echo "[CLEAN] Cleanup complete"

# Help information
help:
	@echo "Guild Module Test Targets:"
	@echo "  make all   - Build and run tests (default)"
	@echo "  make test  - Build and run tests"
	@echo "  make clean - Remove test artifacts"
	@echo "  make help  - Show this help message"
