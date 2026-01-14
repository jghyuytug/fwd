###############################################################################
# Security Monitoring ORM Region Build Configuration
# 2 databases: d_taiwan_secu, d_technical_report
# 50 tables total
###############################################################################

REGION = security_monitoring
DATABASES = d_taiwan_secu d_technical_report
BUILDROOT = ../../../build/orm/$(REGION)
LIBROOT = ../../lib

# Compiler configuration
CC = gcc
CFLAGS = -m32 -O2 -g -fPIC -pipe -Wall -Wextra -std=c99
INCLUDES = -I../../../include -I../../../include/common \
           -I../../../include/database/headers

# Auto-detect all ORM source files in this region
SOURCES = $(wildcard d_taiwan_secu/sources/*.c d_technical_report/sources/*.c)

# Generate object file paths
OBJECTS = $(patsubst %.c,$(BUILDROOT)/%.o,$(notdir $(SOURCES)))

# Target library
TARGET = $(LIBROOT)/liborm_$(REGION).a

.PHONY: all clean verify

all: $(TARGET)
	@echo "[SUCCESS] Security monitoring ORM library created: $(TARGET)"

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
$(BUILDROOT)/%.o: d_taiwan_secu/sources/%.c | $(BUILDROOT)
	@echo "[CC] Compiling d_taiwan_secu/$<"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDROOT)/%.o: d_technical_report/sources/%.c | $(BUILDROOT)
	@echo "[CC] Compiling d_technical_report/$<"
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
