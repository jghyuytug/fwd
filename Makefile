###############################################################################
# Master Makefile for DNF Game Server Modularization Project
# Coordinates building of all modules
###############################################################################

# Project Directories
SRC_DIR = src
BUILD_DIR = build
LIB_DIR = src/lib
BIN_DIR = bin

# Compiler Configuration
CC = gcc
CFLAGS = -m32 -O2 -g -fPIC -pipe -Wall -Wextra -std=c99
LDFLAGS = -m32
INCLUDES = -Iinclude -Iinclude/common -Iinclude/common/headers \
           -Iinclude/security -Iinclude/database -Iinclude/database/headers \
           -Iinclude/network -Iinclude/network/headers \
           -Iinclude/user -Iinclude/user/headers \
           -Iinclude/inventory -Iinclude/inventory/headers \
           -Iinclude/events -Iinclude/events/headers \
           -Iinclude/combat -Iinclude/dungeon -Iinclude/party \
           -Iinclude/quest -Iinclude/skill -Iinclude/guild \
           -Iinclude/pvp -Iinclude/trade -Iinclude/mail \
           -Iinclude/shop -Iinclude/enhancement -Iinclude/achievement \
           -Isrc

# Main Program
MAIN_SRC = $(SRC_DIR)/main.c
MAIN_OBJ = $(BUILD_DIR)/main.o
TARGET = $(BIN_DIR)/df_game_server

# Module Libraries (order matters for linking)
MODULE_LIBS = \
    $(LIB_DIR)/libachievement.a \
    $(LIB_DIR)/libenhancement.a \
    $(LIB_DIR)/libshop.a \
    $(LIB_DIR)/libmail.a \
    $(LIB_DIR)/libtrade.a \
    $(LIB_DIR)/libpvp.a \
    $(LIB_DIR)/libguild.a \
    $(LIB_DIR)/libquest.a \
    $(LIB_DIR)/libskill.a \
    $(LIB_DIR)/libparty.a \
    $(LIB_DIR)/libdungeon.a \
    $(LIB_DIR)/libcombat.a \
    $(LIB_DIR)/libevents.a \
    $(LIB_DIR)/libinventory.a \
    $(LIB_DIR)/libuser.a \
    $(LIB_DIR)/libnetwork.a \
    $(LIB_DIR)/libdatabase.a \
    $(LIB_DIR)/libsecurity.a \
    $(LIB_DIR)/libcommon.a

# System Libraries
SYS_LIBS = -lpthread -lrt -ldl -lm
# Note: -lmysqlclient -lGeoIP are optional in stub mode
# TODO: Link with 32-bit MySQL libraries when available

# Modules to Build
MODULES = common security database orm network user inventory events combat dungeon party quest skill guild pvp trade mail shop enhancement achievement

# Phony Targets
.PHONY: all clean verify show-config help run rebuild $(MODULES) server

# Default Target: Build All Modules and Server
all: $(MODULES) server

# Server Target: Build the main executable
server: $(TARGET)

# Compile main.c
$(MAIN_OBJ): $(MAIN_SRC) | $(BUILD_DIR)
	@echo ""
	@echo "[CC] Compiling main program: $<"
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Link final executable
$(TARGET): $(MODULES) $(MAIN_OBJ) | $(BIN_DIR)
	@echo ""
	@echo "=========================================="
	@echo "Linking final executable..."
	@echo "=========================================="
	@echo "[LD] Linking: $@"
	$(CC) $(LDFLAGS) -o $@ $(MAIN_OBJ) $(MODULE_LIBS) $(SYS_LIBS)
	@echo ""
	@echo "=========================================="
	@echo "Build successful!"
	@echo "=========================================="
	@echo "Executable: $(TARGET)"
	@ls -lh $(TARGET) 2>/dev/null || echo "[Info] File created: $(TARGET)"
	@file $(TARGET) 2>/dev/null || echo "[Info] Binary format check skipped"
	@echo ""

# Individual Module Build Targets
common:
	@echo "[Building Common Module]"
	$(MAKE) -C $(SRC_DIR)/common all

security: common
	@echo "[Building Security Module]"
	$(MAKE) -C $(SRC_DIR)/security all

database: common security
	@echo "[Building Database Module]"
	$(MAKE) -C $(SRC_DIR)/database all

orm: common security database
	@echo "[Building ORM Layer - 973 modules]"
	$(MAKE) -C $(SRC_DIR)/orm all

network: common security database orm
	@echo "[Building Network Module]"
	$(MAKE) -C $(SRC_DIR)/network all

user: common security database
	@echo "[Building User Module]"
	$(MAKE) -C $(SRC_DIR)/user all

inventory: common user database
	@echo "[Building Inventory Module]"
	$(MAKE) -C $(SRC_DIR)/inventory all

events: common network user
	@echo "[Building Events Module]"
	$(MAKE) -C $(SRC_DIR)/events all

combat: common user
	@echo "[Building Combat Module]"
	$(MAKE) -C $(SRC_DIR)/combat all

dungeon: common user
	@echo "[Building Dungeon Module]"
	$(MAKE) -C $(SRC_DIR)/dungeon all

party: common user
	@echo "[Building Party Module]"
	$(MAKE) -C $(SRC_DIR)/party all

quest: common user
	@echo "[Building Quest Module]"
	$(MAKE) -C $(SRC_DIR)/quest all

skill: common user
	@echo "[Building Skill Module]"
	$(MAKE) -C $(SRC_DIR)/skill all

guild: common user
	@echo "[Building Guild Module]"
	$(MAKE) -C $(SRC_DIR)/guild all

pvp: common user
	@echo "[Building PvP Module]"
	$(MAKE) -C $(SRC_DIR)/pvp all

trade: common user
	@echo "[Building Trade Module]"
	$(MAKE) -C $(SRC_DIR)/trade all

mail: common user
	@echo "[Building Mail Module]"
	$(MAKE) -C $(SRC_DIR)/mail all

shop: common user
	@echo "[Building Shop Module]"
	$(MAKE) -C $(SRC_DIR)/shop all

enhancement: common user
	@echo "[Building Enhancement Module]"
	$(MAKE) -C $(SRC_DIR)/enhancement all

achievement: common user
	@echo "[Building Achievement Module]"
	$(MAKE) -C $(SRC_DIR)/achievement all

# Create Necessary Directories
$(BUILD_DIR) $(BIN_DIR):
	@mkdir -p $@

init-dirs: $(BUILD_DIR) $(BIN_DIR)
	@$(foreach module,$(MODULES),mkdir -p $(BUILD_DIR)/$(module);)

# Clean All Modules and Executables
clean:
	@echo "[Cleaning All Modules and Executables]"
	@$(foreach module,$(MODULES),$(MAKE) -C $(SRC_DIR)/$(module) clean;)
	@rm -rf $(BUILD_DIR)
	@rm -rf $(BIN_DIR)
	@echo "[CLEAN] Cleanup complete"

# Rebuild from scratch
rebuild: clean all

# Verification Target
verify: all
	@echo "=========================================="
	@echo "Build Verification Report"
	@echo "=========================================="
	@echo ""
	@echo "Module Libraries:"
	@ls -lh $(LIB_DIR)/*.a 2>/dev/null || echo "[Warning] No libraries found"
	@echo ""
	@echo "Executable:"
	@ls -lh $(TARGET) 2>/dev/null || echo "[Warning] Executable not built"
	@echo ""
	@echo "File type:"
	@file $(TARGET) 2>/dev/null || echo "[Info] File check skipped"
	@echo ""
	@$(foreach module,$(MODULES),$(MAKE) -C $(SRC_DIR)/$(module) verify 2>/dev/null || true;)
	@echo ""
	@echo "=========================================="

# Run the server
run: $(TARGET)
	@echo "=========================================="
	@echo "Starting DNF Game Server..."
	@echo "=========================================="
	@$(TARGET)

# Show Build Configuration
show-config:
	@echo "=========================================="
	@echo "DNF Game Server Build Configuration"
	@echo "=========================================="
	@echo "Compiler: $(CC)"
	@echo "Compiler Flags: $(CFLAGS)"
	@echo "Linker Flags: $(LDFLAGS)"
	@echo ""
	@echo "Directories:"
	@echo "  Source:      $(SRC_DIR)"
	@echo "  Build:       $(BUILD_DIR)"
	@echo "  Libraries:   $(LIB_DIR)"
	@echo "  Binaries:    $(BIN_DIR)"
	@echo ""
	@echo "Modules: $(MODULES)"
	@echo ""
	@echo "Main Program:"
	@echo "  Source:  $(MAIN_SRC)"
	@echo "  Object:  $(MAIN_OBJ)"
	@echo "  Target:  $(TARGET)"
	@echo ""
	@echo "System Libraries: $(SYS_LIBS)"
	@echo "=========================================="

# Help Information
help:
	@echo "=========================================="
	@echo "DNF Game Server Build System"
	@echo "=========================================="
	@echo ""
	@echo "Build Targets:"
	@echo "  make all          - Build all modules and server (default)"
	@echo "  make server       - Build only the server executable"
	@echo "  make clean        - Remove all build artifacts"
	@echo "  make rebuild      - Clean and rebuild from scratch"
	@echo ""
	@echo "Module Targets:"
	@$(foreach module,$(MODULES),echo "  make $(module)        - Build $(module) module";)
	@echo ""
	@echo "Testing & Info:"
	@echo "  make verify       - Verify build and show details"
	@echo "  make run          - Build and run the server"
	@echo "  make show-config  - Display build configuration"
	@echo "  make help         - Show this help message"
	@echo ""
	@echo "Build Order (with dependencies):"
	@echo "  Layer 0: Common"
	@echo "  Layer 1: Security → Common"
	@echo "  Layer 1: Database → Common, Security"
	@echo "  Layer 1: Network → Common, Security, Database"
	@echo "  Layer 2: User → Common, Security, Database"
	@echo "  Layer 2: Inventory → Common, User, Database"
	@echo "  Layer 2: Events → Common, Network, User"
	@echo "  Layer 3: Combat → Common, User"
	@echo "  Layer 3: Dungeon → Common, User"
	@echo "  Layer 3: Party → Common, User"
	@echo "  Layer 3: Quest → Common, User"
	@echo "  Layer 3: Skill → Common, User"
	@echo "  Layer 3: Guild → Common, User"
	@echo "  Layer 3: PvP → Common, User"
	@echo "  Layer 3: Trade → Common, User"
	@echo "  Layer 3: Mail → Common, User"
	@echo "  Layer 3: Shop → Common, User"
	@echo "  Layer 3: Enhancement → Common, User"
	@echo "  Layer 3: Achievement → Common, User"
	@echo ""
	@echo "Example Workflow:"
	@echo "  1. make clean      # Clean previous builds"
	@echo "  2. make all        # Build everything"
	@echo "  3. make verify     # Verify the build"
	@echo "  4. make run        # Run the server"
	@echo ""
	@echo "=========================================="