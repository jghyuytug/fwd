###############################################################################
# Extended Features ORM Region Build Configuration
# 5 databases: taiwan_cain_2nd, taiwan_mng_manager, taiwan_cain_web, qq_bot, gm_gui
# 85 tables total
###############################################################################

REGION = extended_features
DATABASES = taiwan_cain_2nd taiwan_mng_manager taiwan_cain_web qq_bot gm_gui
BUILDROOT = ../../../build/orm/$(REGION)
LIBROOT = ../../lib

# Compiler configuration
CC = gcc
CFLAGS = -m32 -O2 -g -fPIC -pipe -Wall -Wextra -std=c99
INCLUDES = -I../../../include -I../../../include/common \
           -I../../../include/database/headers

# Auto-detect all ORM source files in this region
SOURCES = $(wildcard taiwan_cain_2nd/sources/*.c taiwan_mng_manager/sources/*.c \
                     taiwan_cain_web/sources/*.c qq_bot/sources/*.c \
                     gm_gui/sources/*.c)

# Generate object file paths
OBJECTS = $(patsubst %.c,$(BUILDROOT)/%.o,$(notdir $(SOURCES)))

# Target library
TARGET = $(LIBROOT)/liborm_$(REGION).a

.PHONY: all clean verify

all: $(TARGET)
	@echo "[SUCCESS] Extended features ORM library created: $(TARGET)"

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
$(BUILDROOT)/%.o: taiwan_cain_2nd/sources/%.c | $(BUILDROOT)
	@echo "[CC] Compiling taiwan_cain_2nd/$<"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDROOT)/%.o: taiwan_mng_manager/sources/%.c | $(BUILDROOT)
	@echo "[CC] Compiling taiwan_mng_manager/$<"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDROOT)/%.o: taiwan_cain_web/sources/%.c | $(BUILDROOT)
	@echo "[CC] Compiling taiwan_cain_web/$<"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDROOT)/%.o: qq_bot/sources/%.c | $(BUILDROOT)
	@echo "[CC] Compiling qq_bot/$<"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDROOT)/%.o: gm_gui/sources/%.c | $(BUILDROOT)
	@echo "[CC] Compiling gm_gui/$<"
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
