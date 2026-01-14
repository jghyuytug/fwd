###############################################################################
# Core Game ORM Region Build Configuration
# 5 databases: d_taiwan, taiwan_cain, taiwan_billing, taiwan_login, d_guild
# 288 tables total
###############################################################################

REGION = core_game
DATABASES = d_taiwan taiwan_cain taiwan_billing taiwan_login d_guild
BUILDROOT = ../../../build/orm/$(REGION)
LIBROOT = ../../lib

# Compiler configuration
CC = gcc
CFLAGS = -m32 -O2 -g -fPIC -pipe -Wall -Wextra -std=c99
INCLUDES = -I../../../include -I../../../include/common \
           -I../../../include/database/headers

# Auto-detect all ORM source files in this region
SOURCES = $(wildcard d_taiwan/sources/*.c taiwan_cain/sources/*.c \
                     taiwan_billing/sources/*.c taiwan_login/sources/*.c \
                     d_guild/sources/*.c)

# Generate object file paths
OBJECTS = $(patsubst %.c,$(BUILDROOT)/%.o,$(notdir $(SOURCES)))

# Target library
TARGET = $(LIBROOT)/liborm_$(REGION).a

.PHONY: all clean verify

all: $(TARGET)
	@echo "[SUCCESS] Core game ORM library created: $(TARGET)"

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
$(BUILDROOT)/%.o: d_taiwan/sources/%.c | $(BUILDROOT)
	@echo "[CC] Compiling d_taiwan/$<"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDROOT)/%.o: taiwan_cain/sources/%.c | $(BUILDROOT)
	@echo "[CC] Compiling taiwan_cain/$<"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDROOT)/%.o: taiwan_billing/sources/%.c | $(BUILDROOT)
	@echo "[CC] Compiling taiwan_billing/$<"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDROOT)/%.o: taiwan_login/sources/%.c | $(BUILDROOT)
	@echo "[CC] Compiling taiwan_login/$<"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDROOT)/%.o: d_guild/sources/%.c | $(BUILDROOT)
	@echo "[CC] Compiling d_guild/$<"
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
