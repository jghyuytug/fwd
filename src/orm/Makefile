###############################################################################
# ORM Layer Build Configuration
# Compiles all ORM modules across 7 functional regions
# Integrated from local_orm project (2025-11-23)
###############################################################################

MODULE = orm
BUILDROOT = ../../build
LIBROOT = ../lib

# Compiler configuration (32-bit CentOS 5/7 target)
CC = gcc
CFLAGS = -m32 -O2 -g -fPIC -pipe -Wall -Wextra -std=c99
LDFLAGS = -m32
INCLUDES = -I../../include -I../../include/common -I../../include/database/headers

# ORM Regions
REGIONS = core_game security_monitoring audit_logging event_operations \
          commerce_trade extended_features channel_system web_community

# Default target: build all ORM modules
.PHONY: all clean verify $(REGIONS) help

all: intro $(REGIONS) summary

intro:
	@echo "========================================="
	@echo "Building ORM Layer (973 modules)"
	@echo "========================================="

summary:
	@echo ""
	@echo "========================================="
	@echo "ORM Layer Build Complete"
	@echo "Libraries: lib/liborm_*.a"
	@echo "========================================="

# Build individual regions
core_game:
	@echo "[ORM] Building core_game (288 tables)..."
	@$(MAKE) -C core_game all

security_monitoring:
	@echo "[ORM] Building security_monitoring (50 tables)..."
	@$(MAKE) -C security_monitoring all

audit_logging:
	@echo "[ORM] Building audit_logging (101 tables)..."
	@$(MAKE) -C audit_logging all

event_operations:
	@echo "[ORM] Building event_operations (64 tables)..."
	@$(MAKE) -C event_operations all

commerce_trade:
	@echo "[ORM] Building commerce_trade (69 tables)..."
	@$(MAKE) -C commerce_trade all

extended_features:
	@echo "[ORM] Building extended_features (85 tables)..."
	@$(MAKE) -C extended_features all

channel_system:
	@echo "[ORM] Building channel_system (8 tables)..."
	@$(MAKE) -C channel_system all

web_community:
	@echo "[ORM] Building web_community (476 tables)..."
	@$(MAKE) -C web_community all

# Clean all regions
clean:
	@echo "[CLEAN] Cleaning ORM layer..."
	@for region in $(REGIONS); do \
		$(MAKE) -C $$region clean 2>/dev/null || true; \
	done
	@rm -f $(LIBROOT)/liborm_*.a
	@echo "[CLEAN] ORM layer cleanup complete"

# Verify build
verify:
	@echo "=== ORM Layer Verification ==="
	@echo "Regions: $(REGIONS)"
	@echo "Libraries:"
	@ls -lh $(LIBROOT)/liborm_*.a 2>/dev/null || echo "  (No libraries built yet)"

# Help information
help:
	@echo "ORM Layer Build Targets:"
	@echo "  make all       - Build all 973 ORM modules (default)"
	@echo "  make core_game - Build core_game region only"
	@echo "  make security_monitoring - Build security_monitoring region only"
	@echo "  make audit_logging - Build audit_logging region only"
	@echo "  make event_operations - Build event_operations region only"
	@echo "  make commerce_trade - Build commerce_trade region only"
	@echo "  make extended_features - Build extended_features region only"
	@echo "  make channel_system - Build channel_system region only"
	@echo "  make web_community - Build web_community region only"
	@echo "  make clean     - Remove all ORM build artifacts"
	@echo "  make verify    - Show build status"
	@echo "  make help      - Show this help message"
