#!/bin/bash
#
# fix_template_errors.sh - Fix template argument errors (Phase 5)
#
# This script fixes:
# 1. std::_Rb_tree wrong template argument errors
# 2. std::pair wrong template argument errors
# 3. Invalid template instantiations
#
# Expected error reduction: 40K-60K errors
#

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
SRC_DIR="$PROJECT_ROOT/src_split"
DECL_FILE="$SRC_DIR/df_game_r_decl.h"
BACKUP_DIR="$PROJECT_ROOT/backups"

echo -e "${GREEN}=== Template Error Fix Script ===${NC}"
echo "Target: $DECL_FILE"
echo ""

# Create backup
TIMESTAMP=$(date +%Y%m%d_%H%M%S)
mkdir -p "$BACKUP_DIR"
BACKUP_FILE="$BACKUP_DIR/df_game_r_decl_before_template_fix_${TIMESTAMP}.h"
cp "$DECL_FILE" "$BACKUP_FILE"
echo -e "${GREEN}✓ Backup created: $BACKUP_FILE${NC}"

# Count problematic patterns before
echo ""
echo -e "${YELLOW}=== Statistics Before ===${NC}"
echo "std::_Rb_tree declarations: $(grep -c "std::_Rb_tree<" "$DECL_FILE" || echo "0")"
echo "std::pair member declarations: $(grep -c "std::pair<.*>::" "$DECL_FILE" || echo "0")"
echo "std::map member declarations: $(grep -c "std::map<.*>::" "$DECL_FILE" || echo "0")"

# Fix 1: Comment out std::_Rb_tree member function declarations
# These have wrong template argument counts (3 args instead of 5)
echo ""
echo -e "${YELLOW}=== Fix 1: Fixing std::_Rb_tree declarations ===${NC}"

# Find and comment lines with std::_Rb_tree member functions (incomplete template args)
sed -i 's|^\(.*std::_Rb_tree<[^>]*,\s*[^>]*,\s*[^>]*>::\)|// \1|g' "$DECL_FILE"
echo -e "${GREEN}✓ std::_Rb_tree declarations commented${NC}"

# Fix 2: Comment out std::pair member function declarations
# std::pair doesn't have member functions that should be declared
echo ""
echo -e "${YELLOW}=== Fix 2: Fixing std::pair member declarations ===${NC}"
sed -i 's|^\(.*__cdecl std::pair<.*>::\)|// \1|g' "$DECL_FILE"
echo -e "${GREEN}✓ std::pair member declarations commented${NC}"

# Fix 3: Comment out std::map member function declarations with incomplete args
echo ""
echo -e "${YELLOW}=== Fix 3: Fixing std::map member declarations ===${NC}"
# Only comment if it's a direct member declaration (not already commented)
sed -i 's|^\(void __cdecl std::map<[^>]*>::\)|// \1|g' "$DECL_FILE"
sed -i 's|^\(int __cdecl std::map<[^>]*>::\)|// \1|g' "$DECL_FILE"
echo -e "${GREEN}✓ std::map member declarations commented${NC}"

# Fix 4: Comment out std::allocator member declarations
echo ""
echo -e "${YELLOW}=== Fix 4: Fixing std::allocator declarations ===${NC}"
sed -i 's|^\(.*__cdecl std::allocator<.*>::\)|// \1|g' "$DECL_FILE"
echo -e "${GREEN}✓ std::allocator declarations commented${NC}"

# Statistics after
echo ""
echo -e "${YELLOW}=== Statistics After ===${NC}"
echo "Uncommented std::_Rb_tree: $(grep -c "^[^/]*std::_Rb_tree<.*>::" "$DECL_FILE" || echo "0")"
echo "Uncommented std::pair:: : $(grep -c "^[^/]*std::pair<.*>::" "$DECL_FILE" || echo "0")"
echo "Uncommented std::map:: : $(grep -c "^void __cdecl std::map<" "$DECL_FILE" || echo "0")"

echo ""
echo -e "${GREEN}=== Fix Complete ===${NC}"
echo ""
echo "Expected error reduction: 40K-60K errors"
echo ""
echo "To verify:"
echo "  cd $SRC_DIR"
echo "  make clean"
echo "  make -j4 2>&1 | tee ../compile_after_template_fix.log"
echo ""
echo "To rollback:"
echo "  cp $BACKUP_FILE $DECL_FILE"
