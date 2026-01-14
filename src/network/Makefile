###############################################################################
# Network Module Build Configuration
# Compiles the network layer (epoll-based connection manager, packet handling)
###############################################################################

MODULE = network
BUILDROOT = ../build
LIBROOT = ../lib
SRCDIR = .

# Compiler configuration (default 32-bit CentOS 5/7 target)
USE_32BIT ?= 1
CC = gcc

ARCHFLAGS =
ifeq ($(USE_32BIT),1)
  ARCHFLAGS += -m32
endif

CFLAGS = $(ARCHFLAGS) -O2 -g -fPIC -pipe -Wall -Wextra -std=c99
LDFLAGS = $(ARCHFLAGS)
INCLUDES = -I. -I../../include -I../../include/common -I../../include/network -I../../include/network/headers

# Source files - Network module components
SOURCES = $(SRCDIR)/network_init.c \
          $(SRCDIR)/network.c \
          $(SRCDIR)/session_state.c \
          $(SRCDIR)/session_timer.c \
          $(SRCDIR)/packet_handlers.c \
          $(SRCDIR)/dispatcher.c \
          $(SRCDIR)/db_service.c \
          $(SRCDIR)/game_server.c
OBJECTS = $(addprefix $(BUILDROOT)/$(MODULE)/, $(SOURCES:$(SRCDIR)/%.c=%.o))

# Target library
TARGET = $(LIBROOT)/lib$(MODULE).a

# Standalone test server sources (subset for standalone build)
SERVER_SOURCES = $(SRCDIR)/game_server.c \
                 $(SRCDIR)/dispatcher.c \
                 $(SRCDIR)/db_service.c \
                 $(SRCDIR)/server_main.c
SERVER_OBJECTS = $(addprefix $(BUILDROOT)/$(MODULE)/, $(SERVER_SOURCES:$(SRCDIR)/%.c=%.o))
SERVER_TARGET = $(BUILDROOT)/game_server

# Phony targets
.PHONY: all clean verify show-config server

# Default target: build library
all: $(TARGET)

# Build standalone test server
server: $(SERVER_TARGET)
	@echo "[SUCCESS] Standalone game server built: $(SERVER_TARGET)"

# Link object files into static library
$(TARGET): $(OBJECTS) | $(LIBROOT)
	@echo "[AR] Creating static library: $@"
	ar rcs $@ $^
	@echo "[SUCCESS] Network library created: $@"

# Link standalone game server executable
$(SERVER_TARGET): $(SERVER_OBJECTS)
	@echo "[LD] Linking game server: $@"
	$(CC) $(LDFLAGS) -o $@ $^ -lpthread -lrt
	@echo "[SUCCESS] Game server linked: $@"

# Create output directories if they don't exist
$(BUILDROOT)/$(MODULE) $(LIBROOT):
	@mkdir -p $@

# Compile source files to object files
$(BUILDROOT)/$(MODULE)/%.o: $(SRCDIR)/%.c | $(BUILDROOT)/$(MODULE)
	@echo "[CC] Compiling: $<"
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Verify build configuration
verify:
	@echo "=== Network Module Build Verification ==="
	@echo "Module: $(MODULE)"
	@echo "Source Files: $(SOURCES)"
	@echo "Target: $(TARGET)"
	@echo "Compiler: $(CC) $(CFLAGS)"
	@echo "Objects: $(OBJECTS)"
	@echo "Build Directory: $(BUILDROOT)/$(MODULE)"
	@echo "Library Directory: $(LIBROOT)"

# Show configuration
show-config:
	@echo "Network Module Configuration:"
	@echo "  - Module: $(MODULE)"
	@echo "  - Build Target: CentOS 5/7 32-bit (i386)"
	@echo "  - Compiler Flags: $(CFLAGS)"
	@echo "  - Include Paths: $(INCLUDES)"
	@echo "  - Source Files: $(SOURCES)"
	@echo "  - Output Library: $(TARGET)"

# Clean build artifacts
clean:
	@echo "[CLEAN] Removing network module build artifacts..."
	rm -rf $(BUILDROOT)/$(MODULE)
	rm -f $(TARGET)
	rm -f $(SERVER_TARGET)
	@echo "[CLEAN] Network module cleanup complete"

# Help information
help:
	@echo "Network Module Build Targets:"
	@echo "  make all       - Build network module library (default)"
	@echo "  make server    - Build standalone test server"
	@echo "  make clean     - Remove build artifacts"
	@echo "  make verify    - Show build configuration details"
	@echo "  make show-config - Display module configuration"
	@echo "  make help      - Show this help message"
