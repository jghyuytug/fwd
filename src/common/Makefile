# Common Module Makefile
# Builds the Common module which provides:
# - Core initialization
# - Error handling
# - Logging utilities
# - Global state management

CC = gcc
CFLAGS = -m32 -O2 -g -fPIC -pipe -I. -I../../include
LDFLAGS = -m32
LIBS = -lpthread -lrt -ldl -lm

# Directories
SRCROOT = ../..
INCROOT = $(SRCROOT)/include
BUILDROOT = $(SRCROOT)/build
LIBROOT = $(SRCROOT)/src/lib

# Module name
MODULE = common
MODULE_LIB = $(LIBROOT)/lib$(MODULE).a

# Source files
SOURCES = \
    common_init.c

# Object files
OBJECTS = $(SOURCES:.c=.o)
BUILD_OBJECTS = $(addprefix $(BUILDROOT)/$(MODULE)/,$(OBJECTS))

# Build targets
.PHONY: all clean verify show-config

all: $(MODULE_LIB)

$(BUILDROOT)/$(MODULE):
	@mkdir -p $@

$(BUILDROOT)/$(MODULE)/%.o: %.c | $(BUILDROOT)/$(MODULE)
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) -c $< -o $@

$(MODULE_LIB): $(BUILD_OBJECTS)
	@mkdir -p $(LIBROOT)
	@echo "Archiving $(MODULE) library..."
	@ar rcs $@ $^
	@echo "Built: $@"

clean:
	@echo "Cleaning $(MODULE) module..."
	@rm -rf $(BUILDROOT)/$(MODULE)
	@rm -f $(MODULE_LIB)
	@echo "Clean complete"

verify:
	@echo "Verifying $(MODULE) module..."
	@test -f $(MODULE_LIB) && echo "[OK] $(MODULE_LIB) exists" || echo "[ERROR] $(MODULE_LIB) not found"
	@echo "Source files: $(SOURCES)"
	@echo "Build objects: $(BUILD_OBJECTS)"

show-config:
	@echo "Common Module Configuration"
	@echo "  CC: $(CC)"
	@echo "  CFLAGS: $(CFLAGS)"
	@echo "  MODULE_LIB: $(MODULE_LIB)"
	@echo "  SOURCES: $(SOURCES)"
