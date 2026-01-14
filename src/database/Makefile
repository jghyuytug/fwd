###############################################################################
# Database Module Build Configuration
# Compiles the database access layer (MySQL connection pool, query execution)
###############################################################################

MODULE = database
BUILDROOT = ../build
LIBROOT = ../lib
SRCDIR = .
ORMDIR = $(SRCDIR)/sources

# Compiler configuration (32-bit CentOS 5/7 target)
CC = gcc
CFLAGS = -m32 -O2 -g -fPIC -pipe -Wall -Wextra -std=c99
LDFLAGS = -m32
INCLUDES = -I../../include -I../../include/common -I../../include/database -I../../include/database/headers

# Source files - Database module components
BASE_SOURCES = $(SRCDIR)/database_init.c \
	$(SRCDIR)/database.c \
	$(SRCDIR)/database_pool_advanced.c \
	$(SRCDIR)/database_query_advanced.c \
	$(SRCDIR)/database_transaction_advanced.c \
	$(SRCDIR)/database_interface_stub.c

# Automatically include every generated ORM source under sources/
ORM_SOURCES = $(shell find $(ORMDIR) -name '*.c' -print)

SOURCES = $(BASE_SOURCES) $(ORM_SOURCES)
OBJECTS = $(addprefix $(BUILDROOT)/$(MODULE)/, $(SOURCES:$(SRCDIR)/%.c=%.o))

# Target library
TARGET = $(LIBROOT)/lib$(MODULE).a

# Phony targets
.PHONY: all clean verify show-config

# Default target: build library
all: $(TARGET)

# Link object files into static library
$(TARGET): $(OBJECTS) | $(LIBROOT)
	@echo "[AR] Creating static library: $@"
	ar rcs $@ $^
	@echo "[SUCCESS] Database library created: $@"

# Create output directories if they don't exist
$(BUILDROOT)/$(MODULE) $(LIBROOT):
	@mkdir -p $@

# Compile source files to object files
$(BUILDROOT)/$(MODULE)/%.o: $(SRCDIR)/%.c | $(BUILDROOT)/$(MODULE)
	@echo "[CC] Compiling: $<"
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Verify build configuration
verify:
	@echo "=== Database Module Build Verification ==="
	@echo "Module: $(MODULE)"
	@echo "Source Files: $(SOURCES)"
	@echo "Target: $(TARGET)"
	@echo "Compiler: $(CC) $(CFLAGS)"
	@echo "Objects: $(OBJECTS)"
	@echo "Build Directory: $(BUILDROOT)/$(MODULE)"
	@echo "Library Directory: $(LIBROOT)"

# Show configuration
show-config:
	@echo "Database Module Configuration:"
	@echo "  - Module: $(MODULE)"
	@echo "  - Build Target: CentOS 5/7 32-bit (i386)"
	@echo "  - Compiler Flags: $(CFLAGS)"
	@echo "  - Include Paths: $(INCLUDES)"
	@echo "  - Source Files: $(SOURCES)"
	@echo "  - Output Library: $(TARGET)"

# Clean build artifacts
clean:
	@echo "[CLEAN] Removing database module build artifacts..."
	rm -rf $(BUILDROOT)/$(MODULE)
	rm -f $(TARGET)
	@echo "[CLEAN] Database module cleanup complete"

# Help information
help:
	@echo "Database Module Build Targets:"
	@echo "  make all       - Build database module library (default)"
	@echo "  make clean     - Remove build artifacts"
	@echo "  make verify    - Show build configuration details"
	@echo "  make show-config - Display module configuration"
	@echo "  make help      - Show this help message"
