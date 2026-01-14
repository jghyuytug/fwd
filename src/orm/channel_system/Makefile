###############################################################################
# Channel System ORM Region Build Configuration
# 2 databases: d_channel, taiwan_login_play
# 8 tables total
###############################################################################

REGION = channel_system
DATABASES = d_channel taiwan_login_play
BUILDROOT = ../../../build/orm/$(REGION)
LIBROOT = ../../lib

# Compiler configuration
CC = gcc
CFLAGS = -m32 -O2 -g -fPIC -pipe -Wall -Wextra -std=c99
INCLUDES = -I../../../include -I../../../include/common \
           -I../../../include/database/headers

# Auto-detect all ORM source files in this region
SOURCES = $(wildcard d_channel/sources/*.c taiwan_login_play/sources/*.c)

# Generate object file paths
OBJECTS = $(patsubst %.c,$(BUILDROOT)/%.o,$(notdir $(SOURCES)))

# Target library
TARGET = $(LIBROOT)/liborm_$(REGION).a

.PHONY: all clean verify

all: $(TARGET)
	@echo "[SUCCESS] Channel system ORM library created: $(TARGET)"

# Create the library
$(TARGET): $(OBJECTS) | $(LIBROOT)
	@echo "[AR] Creating ORM library for $(REGION)..."
	ar rcs $@ $^
	@ranlib $@ 2>/dev/null || true

# Create output directories
$(BUILDROOT):
	@mkdir -p $@

$(LIBROOT):
	@mkdir -p $@

# Compile source files
$(BUILDROOT)/%.o: d_channel/sources/%.c | $(BUILDROOT)
	@echo "[CC] Compiling d_channel/$<"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDROOT)/%.o: taiwan_login_play/sources/%.c | $(BUILDROOT)
	@echo "[CC] Compiling taiwan_login_play/$<"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Clean build artifacts
clean:
	@echo "[CLEAN] Removing $(REGION) ORM build artifacts..."
	@rm -rf $(BUILDROOT)
	@rm -f $(TARGET)

# Verify configuration
verify:
	@echo "Region: $(REGION)"
	@echo "Databases: $(DATABASES)"
	@echo "Sources: $(words $(SOURCES)) files"
	@echo "Target: $(TARGET)"
	@echo "Compiler: $(CC) $(CFLAGS)"
