###############################################################################
# Quest Module Unit Tests Makefile
# Compiles and runs unit tests for quest system
###############################################################################

TESTDIR = .
SRCDIR = ../../src/quest
BUILDDIR = ./build
BINDIR = ./bin

# Compiler configuration (32-bit CentOS 5/7 target)
CC = gcc
CFLAGS = -m32 -O2 -g -fPIC -pipe -Wall -Wextra -std=c99
LDFLAGS = -m32
INCLUDES = -I../../include -I../../include/common -I../../include/quest

# Test source files
TEST_SOURCES = test_quest_manager.c

# Quest module source files (needed for linking)
QUEST_SOURCES = $(SRCDIR)/quest_config.c \
                $(SRCDIR)/quest_manager.c \
                $(SRCDIR)/quest_trigger.c \
                $(SRCDIR)/quest_reward.c

# Test executables
TEST_EXECUTABLES = $(addprefix $(BINDIR)/, $(TEST_SOURCES:.c=))

# Phony targets
.PHONY: all clean run test help

# Default target: build all tests
all: $(BINDIR) $(TEST_EXECUTABLES)

# Create output directories
$(BINDIR):
	@mkdir -p $@

# Build test_quest_manager executable
$(BINDIR)/test_quest_manager: test_quest_manager.c $(QUEST_SOURCES)
	@echo "[CC] Building: $@"
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@ -lm
	@echo "[SUCCESS] $@ created"

# Run all tests
test: all
	@echo ""
	@echo "=== Running Quest Module Unit Tests ==="
	@echo ""
	@for test in $(TEST_EXECUTABLES); do \
		echo "Running: $$test"; \
		$$test; \
		echo ""; \
	done

# Run individual test
test-manager: $(BINDIR)/test_quest_manager
	@echo "=== Running Quest Manager Tests ==="
	$<

# Clean build artifacts
clean:
	@echo "[CLEAN] Removing test build artifacts..."
	rm -rf $(BINDIR)
	rm -rf $(BUILDDIR)
	rm -f *.gcov *.gcda *.gcno
	@echo "[CLEAN] Quest test cleanup complete"

# Help information
help:
	@echo "Quest Module Unit Tests Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  make all             - Build all tests (default)"
	@echo "  make test            - Build and run all tests"
	@echo "  make test-manager    - Run quest manager tests only"
	@echo "  make clean           - Remove build artifacts"
	@echo "  make help            - Show this help message"
	@echo ""
	@echo "Test Files:"
	@echo "  test_quest_manager.c - Quest system unit tests"
