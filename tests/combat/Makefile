###############################################################################
# Combat Module Unit Tests Makefile
# Compiles and runs unit tests for combat system
###############################################################################

TESTDIR = .
SRCDIR = ../../src/combat
BUILDDIR = ./build
BINDIR = ./bin

# Compiler configuration (32-bit CentOS 5/7 target)
CC = gcc
CFLAGS = -m32 -O2 -g -fPIC -pipe -Wall -Wextra -std=c99
LDFLAGS = -m32
INCLUDES = -I../../include -I../../include/common -I../../include/combat

# Test source files
TEST_SOURCES = test_combat_damage.c \
               test_combat_buff.c \
               test_combat_log.c \
               benchmark_combat.c

# Combat module source files (needed for linking)
COMBAT_SOURCES = $(SRCDIR)/combat_damage.c \
                 $(SRCDIR)/combat_buff.c \
                 $(SRCDIR)/combat_log.c \
                 $(SRCDIR)/combat_data_loader.c

# Test executables
TEST_EXECUTABLES = $(addprefix $(BINDIR)/, $(TEST_SOURCES:.c=))

# Phony targets
.PHONY: all clean run test help

# Default target: build all tests
all: $(BINDIR) $(TEST_EXECUTABLES)

# Create output directories
$(BINDIR):
	@mkdir -p $@

# Build individual test executables
$(BINDIR)/test_combat_damage: test_combat_damage.c $(SRCDIR)/combat_damage.c $(SRCDIR)/combat_data_loader.c
	@echo "[CC] Building: $@"
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@ -lm
	@echo "[SUCCESS] $@ created"

$(BINDIR)/test_combat_buff: test_combat_buff.c $(SRCDIR)/combat_buff.c
	@echo "[CC] Building: $@"
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@ -lm
	@echo "[SUCCESS] $@ created"

$(BINDIR)/test_combat_log: test_combat_log.c $(SRCDIR)/combat_log.c
	@echo "[CC] Building: $@"
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@ -lm
	@echo "[SUCCESS] $@ created"

# Run all tests
test: all
	@echo ""
	@echo "=== Running Combat Module Unit Tests ==="
	@echo ""
	@for test in $(TEST_EXECUTABLES); do \
		echo "Running: $$test"; \
		$$test; \
		echo ""; \
	done

# Run individual tests
test-damage: $(BINDIR)/test_combat_damage
	@echo "=== Running Damage Calculation Tests ==="
	$<

test-buff: $(BINDIR)/test_combat_buff
	@echo "=== Running Buff System Tests ==="
	$<

test-log: $(BINDIR)/test_combat_log
	@echo "=== Running Combat Log Tests ==="
	$<

# Run performance benchmark
benchmark: $(BINDIR)/benchmark_combat
	@echo "=== Running Combat System Performance Benchmark ==="
	$<

# Build benchmark executable
$(BINDIR)/benchmark_combat: benchmark_combat.c $(SRCDIR)/combat_damage.c $(SRCDIR)/combat_buff.c $(SRCDIR)/combat_log.c $(SRCDIR)/combat_data_loader.c
	@echo "[CC] Building: $@"
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@ -lm -lrt
	@echo "[SUCCESS] $@ created"

# Run tests and generate coverage report (requires gcov)
coverage: CFLAGS += --coverage
coverage: clean all
	@echo "=== Running Tests with Coverage ==="
	@for test in $(TEST_EXECUTABLES); do \
		$$test; \
	done
	@echo ""
	@echo "=== Generating Coverage Report ==="
	gcov $(COMBAT_SOURCES)
	@echo "Coverage report generated: *.gcov"

# Clean build artifacts
clean:
	@echo "[CLEAN] Removing test build artifacts..."
	rm -rf $(BINDIR)
	rm -rf $(BUILDDIR)
	rm -f *.gcov *.gcda *.gcno
	@echo "[CLEAN] Combat test cleanup complete"

# Help information
help:
	@echo "Combat Module Unit Tests Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  make all             - Build all tests (default)"
	@echo "  make test            - Build and run all tests"
	@echo "  make test-damage     - Run damage calculation tests only"
	@echo "  make test-buff       - Run buff system tests only"
	@echo "  make test-log        - Run combat log tests only"
	@echo "  make coverage        - Run tests with coverage analysis"
	@echo "  make clean           - Remove build artifacts"
	@echo "  make help            - Show this help message"
	@echo ""
	@echo "Test Files:"
	@echo "  test_combat_damage.c - Damage calculation tests"
	@echo "  test_combat_buff.c   - Buff/debuff system tests"
	@echo "  test_combat_log.c    - Combat logging tests"
