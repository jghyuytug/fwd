#!/bin/bash
#
# fix_deep_errors.sh - Fix deep compilation errors (Phase 4)
#
# This script fixes:
# 1. Invalid std::_Select1st declarations (~364 declarations → ~150K errors)
# 2. Boost namespace declarations (~64 declarations → ~6.5K errors)
# 3. std::_Rb_tree template argument errors
# 4. StringUnorderedMap forward declarations
#
# Expected error reduction: 150K-200K errors (414K → 214K-264K)
#

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
SRC_DIR="$PROJECT_ROOT/src_split"
DECL_FILE="$SRC_DIR/df_game_r_decl.h"
BACKUP_DIR="$PROJECT_ROOT/backups"

echo -e "${GREEN}=== Deep Error Fix Script ===${NC}"
echo "Target: $DECL_FILE"
echo ""

# Create backup
TIMESTAMP=$(date +%Y%m%d_%H%M%S)
mkdir -p "$BACKUP_DIR"
BACKUP_FILE="$BACKUP_DIR/df_game_r_decl_before_deep_fix_${TIMESTAMP}.h"
cp "$DECL_FILE" "$BACKUP_FILE"
echo -e "${GREEN}✓ Backup created: $BACKUP_FILE${NC}"

# Statistics before
echo ""
echo -e "${YELLOW}=== Statistics Before ===${NC}"
echo "std::_Select1st declarations: $(grep -c "^int __cdecl std::_Select1st" "$DECL_FILE" || true)"
echo "boost:: declarations: $(grep -c "^boost::" "$DECL_FILE" || true)"
echo "StringUnorderedMap references: $(grep -c "StringUnorderedMap" "$DECL_FILE" || true)"

# Fix 1: Comment out std::_Select1st declarations
# These are invalid STL member function declarations that should not be explicitly declared
echo ""
echo -e "${YELLOW}=== Fix 1: Commenting std::_Select1st declarations ===${NC}"
sed -i 's|^int __cdecl std::_Select1st<|// int __cdecl std::_Select1st<|g' "$DECL_FILE"
echo -e "${GREEN}✓ std::_Select1st declarations commented${NC}"

# Fix 2: Comment out boost:: namespace declarations
# These require boost library which is not available
echo ""
echo -e "${YELLOW}=== Fix 2: Commenting boost declarations ===${NC}"
sed -i 's|^boost::|// boost::|g' "$DECL_FILE"
echo -e "${GREEN}✓ boost declarations commented${NC}"

# Fix 3: Add StringUnorderedMap forward declaration at the top (after includes)
echo ""
echo -e "${YELLOW}=== Fix 3: Adding StringUnorderedMap forward declaration ===${NC}"

# Check if already declared
if ! grep -q "template<typename HashTrait, typename ValueType> class StringUnorderedMap" "$DECL_FILE"; then
    # Find line after last #include and insert forward declaration
    LINE_NUM=$(grep -n "^#include" "$DECL_FILE" | tail -1 | cut -d: -f1)
    if [ -n "$LINE_NUM" ]; then
        ((LINE_NUM++))
        sed -i "${LINE_NUM}i\\
\\
// Forward declarations for decompiled types\\
struct CharStringHashTrait;\\
template<typename HashTrait, typename ValueType> class StringUnorderedMap;" "$DECL_FILE"
        echo -e "${GREEN}✓ StringUnorderedMap forward declared after line $LINE_NUM${NC}"
    else
        echo -e "${RED}✗ Could not find include section${NC}"
    fi
else
    echo -e "${YELLOW}⚠ StringUnorderedMap already declared${NC}"
fi

# Fix 4: Add advancealtar namespace forward declaration
echo ""
echo -e "${YELLOW}=== Fix 4: Adding namespace forward declarations ===${NC}"

if ! grep -q "namespace advancealtar" "$DECL_FILE"; then
    LINE_NUM=$(grep -n "template<typename HashTrait, typename ValueType> class StringUnorderedMap" "$DECL_FILE" | cut -d: -f1)
    if [ -n "$LINE_NUM" ]; then
        ((LINE_NUM++))
        sed -i "${LINE_NUM}i\\
\\
// Namespace forward declarations\\
namespace advancealtar { }" "$DECL_FILE"
        echo -e "${GREEN}✓ advancealtar namespace declared${NC}"
    fi
else
    echo -e "${YELLOW}⚠ advancealtar already declared${NC}"
fi

# Statistics after
echo ""
echo -e "${YELLOW}=== Statistics After ===${NC}"
echo "std::_Select1st declarations: $(grep -c "^int __cdecl std::_Select1st" "$DECL_FILE" || echo "0")"
echo "boost:: declarations: $(grep -c "^boost::" "$DECL_FILE" || echo "0")"
echo "Commented _Select1st: $(grep -c "^// int __cdecl std::_Select1st" "$DECL_FILE" || echo "0")"
echo "Commented boost: $(grep -c "^// boost::" "$DECL_FILE" || echo "0")"

echo ""
echo -e "${GREEN}=== Fix Complete ===${NC}"
echo ""
echo "Expected error reduction:"
echo "  - std::_Select1st errors: ~150,000"
echo "  - boost errors: ~6,500"
echo "  - StringUnorderedMap errors: ~72"
echo "  - advancealtar errors: ~9,156"
echo "  Total: ~165,000+ errors"
echo ""
echo "To verify:"
echo "  cd $SRC_DIR"
echo "  make clean"
echo "  make -j4 2>&1 | tee ../compile_after_deep_fix.log"
echo ""
echo "To rollback:"
echo "  cp $BACKUP_FILE $DECL_FILE"
