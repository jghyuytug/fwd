# Security Module Makefile
# Builds the Security module which provides:
# - Hack/cheat detection (CHackAnalyzer)
# - IP-based blacklist management (CAutoPunishRuleBlackIPMgr)
# - GM account verification (CGMAccounts)

CC = gcc
CFLAGS = -m32 -O2 -g -fPIC -pipe -I. -I../../include
LDFLAGS = -m32
LIBS = -lpthread -lrt -ldl -lm

SRCROOT = ../..
INCROOT = $(SRCROOT)/include
BUILDROOT = $(SRCROOT)/build
LIBROOT = $(SRCROOT)/src/lib

MODULE = security
MODULE_LIB = $(LIBROOT)/lib$(MODULE).a

# Source files for Security module
# Using security_manager.c (full implementation) instead of security_init.c (stub)
SOURCES = \
    security_manager.c

HEADERS = \
    $(INCROOT)/security/headers/*.h

OBJECTS = $(SOURCES:.c=.o)
BUILD_OBJECTS = $(addprefix $(BUILDROOT)/$(MODULE)/,$(OBJECTS))

.PHONY: all clean verify show-config

all: $(MODULE_LIB)

$(MODULE_LIB): $(BUILD_OBJECTS)
	@mkdir -p $(LIBROOT)
	@echo "Archiving $(MODULE) library..."
	@ar rcs $@ $^
	@echo "Built: $@"

$(BUILDROOT)/$(MODULE):
	@mkdir -p $@

$(BUILDROOT)/$(MODULE)/%.o: %.c | $(BUILDROOT)/$(MODULE)
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) -c $< -o $@

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
	@echo "Security Module Configuration"
	@echo "  CC: $(CC)"
	@echo "  CFLAGS: $(CFLAGS)"
	@echo "  MODULE_LIB: $(MODULE_LIB)"
	@echo "  SOURCES: $(SOURCES)"

