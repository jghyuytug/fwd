# Master Makefile for DNF Game Server Modules
# Coordinates building all modularized components
#
# Usage:
#   make all           - Build all modules
#   make clean         - Clean all modules
#   make verify        - Verify all modules
#   make &lt;module&gt;      - Build specific module (e.g., make common)

.PHONY: all clean verify common security database network user inventory events pvf integration help

MODULES = common security database network pvf integration

# Default target
all: $(MODULES)
	@echo ""
	@echo "============================================================"
	@echo "All modules built successfully!"
	@echo "============================================================"

# Individual module targets
common:
	@echo "Building Common module..."
	@$(MAKE) -C src/common all
	@$(MAKE) -C src/common verify

security:
	@echo "Building Security module..."
	@$(MAKE) -C src/security all
	@$(MAKE) -C src/security verify

database:
	@echo "Building Database module..."
	@$(MAKE) -C src/database all
	@$(MAKE) -C src/database verify

network:
	@echo "Building Network module..."
	@$(MAKE) -C src/network all
	ar rcs $(LIBROOT)/libnetwork.a $(patsubst %.c,%.o,$(wildcard src/network/*.c))

pvf:
	@echo "Building PVF module..."
	@$(MAKE) -C pvf all

pvf-verify:
	@$(MAKE) -C pvf verify

user:
	@echo "Building User module..."
	@$(MAKE) -C src/user all || echo "[INFO] User module not yet ready"

inventory:
	@echo "Building Inventory module..."
	@$(MAKE) -C src/inventory all || echo "[INFO] Inventory module not yet ready"

events:
	@echo "Building Events module..."
	@$(MAKE) -C src/events all || echo "[INFO] Events module not yet ready"

integration:
	@echo "Building Integration module..."
	@$(MAKE) -C integration all
	@echo "Integration module built"

# Clean target
clean:
	@echo "Cleaning all modules..."
	@for module in $(MODULES) user inventory events integration; do \
		if [ -f src/$$module/Makefile ] || [ -f $$module/Makefile ]; then \
			$(MAKE) -C src/$$module clean 2>/dev/null || $(MAKE) -C $$module clean 2>/dev/null || true; \
		fi; \
	done
	@echo "All modules cleaned"

# Verification target
verify:
	@echo "Verifying module builds..."
	@for module in $(MODULES); do \
		if [ -f src/$$module/Makefile ]; then \
			$(MAKE) -C src/$$module verify; \
		fi; \
	done

# Show configuration
show-config:
	@echo "DNF Game Server Module Build Configuration"
	@echo "==========================================="
	@for module in $(MODULES); do \
		if [ -f src/$$module/Makefile ]; then \
			echo ""; \
			$(MAKE) -C src/$$module show-config; \
		fi; \
	done

# Help target
help:
	@echo "DNF Game Server Module Build System"
	@echo "===================================="
	@echo ""
	@echo "Available targets:"
	@echo "  make all          - Build all ready modules"
	@echo "  make common       - Build Common module"
	@echo "  make security     - Build Security module"
	@echo "  make clean        - Clean all modules"
	@echo "  make verify       - Verify all module builds"
	@echo "  make show-config  - Show build configuration"
	@echo "  make help         - Show this help message"
	@echo ""
	@echo "Modules:"
	@for module in $(MODULES); do \
		echo "  $$module - Ready for building"; \
	done
	@echo ""
	@echo "Future modules (not yet ready):"
	@echo "  user      - User/character management"
	@echo "  inventory - Inventory system"
	@echo "  events    - Event system"