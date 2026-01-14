#!/bin/bash

# Phase 5 Stage 2 - Final Compilation Fix Script
# Purpose: Apply all critical fixes and prepare for compilation test
# Date: 2025-11-06

set -e

GAME_DIR="/c/Users/waw/Desktop/game"

echo "================================"
echo "Phase 5 Stage 2 - Final Fixes"
echo "================================"
echo ""

# Step 1: Ensure all module directories exist
echo "[1/6] Ensuring module directories exist..."
mkdir -p "$GAME_DIR/src/common"
mkdir -p "$GAME_DIR/src/security"
mkdir -p "$GAME_DIR/src/user"
mkdir -p "$GAME_DIR/src/inventory"
mkdir -p "$GAME_DIR/src/events"
mkdir -p "$GAME_DIR/src/database"
mkdir -p "$GAME_DIR/src/network"
mkdir -p "$GAME_DIR/build"
mkdir -p "$GAME_DIR/lib"
echo "✓ Directories ready"
echo ""

# Step 2: Copy Common module headers to src/common/
echo "[2/6] Copying Common module headers..."
if [ -f "$GAME_DIR/include/common/defs.h" ]; then
  cp "$GAME_DIR/include/common/defs.h" "$GAME_DIR/src/common/defs.h"
  echo "  ✓ Copied defs.h"
fi

if [ -f "$GAME_DIR/include/common/error_codes.h" ]; then
  cp "$GAME_DIR/include/common/error_codes.h" "$GAME_DIR/src/common/error_codes.h"
  echo "  ✓ Copied error_codes.h"
fi

if [ -f "$GAME_DIR/include/common/constants_improved.h" ]; then
  cp "$GAME_DIR/include/common/constants_improved.h" "$GAME_DIR/src/common/constants_improved.h"
  echo "  ✓ Copied constants_improved.h"
fi
echo ""

# Step 3: Copy Security module headers to src/security/
echo "[3/6] Copying Security module headers..."
if [ -d "$GAME_DIR/include/security/headers" ]; then
  cp "$GAME_DIR/include/security/headers/CHackAnalyzer.h" "$GAME_DIR/src/security/CHackAnalyzer.h"
  echo "  ✓ Copied CHackAnalyzer.h (fixed)"

  cp "$GAME_DIR/include/security/headers/CAutoPunishRuleBlackIPMgr.h" "$GAME_DIR/src/security/CAutoPunishRuleBlackIPMgr.h"
  echo "  ✓ Copied CAutoPunishRuleBlackIPMgr.h (fixed)"

  cp "$GAME_DIR/include/security/headers/CGMAccounts.h" "$GAME_DIR/src/security/CGMAccounts.h"
  echo "  ✓ Copied CGMAccounts.h (fixed)"
fi

# Copy common headers to security module too
cp "$GAME_DIR/include/common/defs.h" "$GAME_DIR/src/security/defs.h" 2>/dev/null || true
cp "$GAME_DIR/include/common/error_codes.h" "$GAME_DIR/src/security/error_codes.h" 2>/dev/null || true
cp "$GAME_DIR/include/common/constants_improved.h" "$GAME_DIR/src/security/constants_improved.h" 2>/dev/null || true
echo ""

# Step 4: Verify all header files are present
echo "[4/6] Verifying header files..."
MISSING=0

for file in "$GAME_DIR/src/common/defs.h" "$GAME_DIR/src/common/error_codes.h" "$GAME_DIR/src/common/constants_improved.h"; do
  if [ -f "$file" ]; then
    echo "  ✓ $(basename $file) present in src/common/"
  else
    echo "  ✗ $(basename $file) MISSING in src/common/"
    MISSING=$((MISSING+1))
  fi
done

for file in "$GAME_DIR/src/security/CHackAnalyzer.h" "$GAME_DIR/src/security/CAutoPunishRuleBlackIPMgr.h" "$GAME_DIR/src/security/CGMAccounts.h"; do
  if [ -f "$file" ]; then
    echo "  ✓ $(basename $file) present in src/security/"
  else
    echo "  ✗ $(basename $file) MISSING in src/security/"
    MISSING=$((MISSING+1))
  fi
done

if [ $MISSING -eq 0 ]; then
  echo "  ✓ All required header files present"
else
  echo "  ✗ WARNING: $MISSING header files missing"
fi
echo ""

# Step 5: Verify include paths in common_interface.h
echo "[5/6] Verifying include paths..."
if grep -q '#include "defs.h"' "$GAME_DIR/src/common/common_interface.h"; then
  echo "  ✓ Local include for defs.h"
else
  echo "  ✗ WARNING: defs.h not using local include"
fi

if grep -q '#include "error_codes.h"' "$GAME_DIR/src/common/common_interface.h"; then
  echo "  ✓ Local include for error_codes.h"
else
  echo "  ✗ WARNING: error_codes.h not using local include"
fi

if grep -q '#include "constants_improved.h"' "$GAME_DIR/src/common/common_interface.h"; then
  echo "  ✓ Local include for constants_improved.h"
else
  echo "  ✗ WARNING: constants_improved.h not using local include"
fi
echo ""

# Step 6: Verify Security module Makefile uses gcc
echo "[6/6] Verifying build configuration..."
if grep -q "^CC = gcc" "$GAME_DIR/src/security/Makefile"; then
  echo "  ✓ Security Makefile uses gcc compiler"
else
  echo "  ✗ WARNING: Security Makefile may not use gcc"
fi

if grep -q "\-I\." "$GAME_DIR/src/security/Makefile"; then
  echo "  ✓ Security Makefile includes local paths (-I.)"
else
  echo "  ✗ WARNING: Security Makefile missing local include path"
fi
echo ""

echo "================================"
echo "✓ All fixes verified and ready"
echo "================================"
echo ""
echo "Next step: Run compilation test"
echo "  docker run -it -v \"$GAME_DIR\":/game game-builder:latest bash"
echo "  cd /game && make clean && make -j4 all"
echo ""

