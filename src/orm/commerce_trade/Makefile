###############################################################################
# Commerce Trade ORM Region Build Configuration
# 3 databases: taiwan_prod, taiwan_cain_auction_cera, taiwan_cain_auction_gold
# 69 tables total
###############################################################################

REGION = commerce_trade
DATABASES = taiwan_prod taiwan_cain_auction_cera taiwan_cain_auction_gold
BUILDROOT = ../../../build/orm/$(REGION)
LIBROOT = ../../lib

# Compiler configuration
CC = gcc
CFLAGS = -m32 -O2 -g -fPIC -pipe -Wall -Wextra -std=c99
INCLUDES = -I../../../include -I../../../include/common \
           -I../../../include/database/headers

# Auto-detect all ORM source files in this region
SOURCES = $(wildcard taiwan_prod/sources/*.c taiwan_cain_auction_cera/sources/*.c \
                     taiwan_cain_auction_gold/sources/*.c)

# Generate object file paths
OBJECTS = $(patsubst %.c,$(BUILDROOT)/%.o,$(notdir $(SOURCES)))

# Target library
TARGET = $(LIBROOT)/liborm_$(REGION).a

.PHONY: all clean verify

all: $(TARGET)
	@echo "[SUCCESS] Commerce trade ORM library created: $(TARGET)"

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
$(BUILDROOT)/%.o: taiwan_prod/sources/%.c | $(BUILDROOT)
	@echo "[CC] Compiling taiwan_prod/$<"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDROOT)/%.o: taiwan_cain_auction_cera/sources/%.c | $(BUILDROOT)
	@echo "[CC] Compiling taiwan_cain_auction_cera/$<"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDROOT)/%.o: taiwan_cain_auction_gold/sources/%.c | $(BUILDROOT)
	@echo "[CC] Compiling taiwan_cain_auction_gold/$<"
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
