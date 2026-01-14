###############################################################################
# Mail Module Build Configuration
# Compiles the in-game mail system
###############################################################################

MODULE = mail
BUILDROOT = ../build
LIBROOT = ../lib
SRCDIR = .

# Compiler configuration (32-bit CentOS 5/7 target)
CC = gcc
CFLAGS = -m32 -O2 -g -fPIC -pipe -Wall -Wextra -std=c99
LDFLAGS = -m32
INCLUDES = -I../../include -I../../include/common -I../../include/mail

# Source files - Mail module components
# Using mail_manager.c (full implementation) instead of mail_init.c (stub)
SOURCES = $(SRCDIR)/mail_manager.c
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
	@echo "[SUCCESS] Mail library created: $@"

# Create output directories if they don't exist
$(BUILDROOT)/$(MODULE) $(LIBROOT):
	@mkdir -p $@

# Compile source files to object files
$(BUILDROOT)/$(MODULE)/%.o: $(SRCDIR)/%.c | $(BUILDROOT)/$(MODULE)
	@echo "[CC] Compiling: $<"
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Verify build configuration
verify:
	@echo "=== Mail Module Build Verification ==="
	@echo "Module: $(MODULE)"
	@echo "Source Files: $(SOURCES)"
	@echo "Target: $(TARGET)"
	@echo "Compiler: $(CC) $(CFLAGS)"
	@echo "Objects: $(OBJECTS)"
	@echo "Build Directory: $(BUILDROOT)/$(MODULE)"
	@echo "Library Directory: $(LIBROOT)"

# Show configuration
show-config:
	@echo "Mail Module Configuration:"
	@echo "  - Module: $(MODULE)"
	@echo "  - Build Target: CentOS 5/7 32-bit (i386)"
	@echo "  - Compiler Flags: $(CFLAGS)"
	@echo "  - Include Paths: $(INCLUDES)"
	@echo "  - Source Files: $(SOURCES)"
	@echo "  - Output Library: $(TARGET)"

# Clean build artifacts
clean:
	@echo "[CLEAN] Removing mail module build artifacts..."
	rm -rf $(BUILDROOT)/$(MODULE)
	rm -f $(TARGET)
	@echo "[CLEAN] Mail module cleanup complete"

# Help information
help:
	@echo "Mail Module Build Targets:"
	@echo "  make all       - Build mail module library (default)"
	@echo "  make clean     - Remove build artifacts"
	@echo "  make verify    - Show build configuration details"
	@echo "  make show-config - Display module configuration"
	@echo "  make help      - Show this help message"
