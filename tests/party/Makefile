###############################################################################
# Party Module Unit Tests Makefile
# Compiles and runs unit tests for party system
###############################################################################

TESTDIR = .
SRCDIR = ../../src/party
BUILDDIR = ./build
BINDIR = ./bin

# Compiler configuration (32-bit CentOS 5/7 target)
CC = gcc
CFLAGS = -m32 -O2 -g -fPIC -pipe -Wall -Wextra -std=c99
LDFLAGS = -m32
INCLUDES = -I../../include -I../../include/common -I../../include/party

# Test source files
TEST_SOURCES = test_party_manager.c

# Party module source files (needed for linking)
PARTY_SOURCES = $(SRCDIR)/party_manager.c \
                $(SRCDIR)/party_exp.c \
                $(SRCDIR)/quick_party.c

# Test executables
TEST_EXECUTABLES = $(addprefix $(BINDIR)/, $(TEST_SOURCES:.c=))

# Phony targets
.PHONY: all clean run test help

# Default target: build all tests
all: $(BINDIR) $(TEST_EXECUTABLES)

# Create output directories
$(BINDIR):
	@mkdir -p $@

# Build test_party_manager executable
$(BINDIR)/test_party_manager: test_party_manager.c $(PARTY_SOURCES)
	@echo "[CC] Building: $@"
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@ -lm
	@echo "[SUCCESS] $@ created"

# Run all tests
test: all
	@echo ""
	@echo "=== Running Party Module Unit Tests ==="
	@echo ""
	@for test in $(TEST_EXECUTABLES); do \
		echo "Running: $$test"; \
		$$test; \
		echo ""; \
	done

# Run individual test
test-manager: $(BINDIR)/test_party_manager
	@echo "=== Running Party Manager Tests ==="
	$<

# Clean build artifacts
clean:
	@echo "[CLEAN] Removing test build artifacts..."
	rm -rf $(BINDIR)
	rm -rf $(BUILDDIR)
	rm -f *.gcov *.gcda *.gcno
	@echo "[CLEAN] Party test cleanup complete"

# Help information
help:
	@echo "Party Module Unit Tests Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  make all             - Build all tests (default)"
	@echo "  make test            - Build and run all tests"
	@echo "  make test-manager    - Run party manager tests only"
	@echo "  make clean           - Remove build artifacts"
	@echo "  make help            - Show this help message"
	@echo ""
	@echo "Test Files:"
	@echo "  test_party_manager.c - Party system unit tests"
