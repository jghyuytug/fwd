#!/bin/bash
# 第二轮完整修复脚本
# 日期: 2025-10-29
# 用途: 从Round 2备份完整恢复到干净状态

set -e

DECL_FILE="src_split/df_game_r_decl.h"

echo "[*] 第二轮完整修复开始..."
echo "[*] 目标文件: $DECL_FILE"

# 备份当前文件
BACKUP_FILE="${DECL_FILE}.backup_round2_complete_$(date +%Y%m%d_%H%M%S)"
cp "$DECL_FILE" "$BACKUP_FILE"
echo "[*] 已备份到: $BACKUP_FILE"

# Step 1: 移除所有 @<register> 符号
echo "[*] Step 1: 移除 @ 符号..."
sed -i 's/@<e[abcd]x>//g; s/@<e[sd]i>//g; s/@<ebp>//g; s/@<esp>//g; s/@//g' "$DECL_FILE"

# Step 2: 移除反引号
echo "[*] Step 2: 移除反引号..."
sed -i 's/`//g' "$DECL_FILE"

# Step 3: 修复 thunk to' 模式
echo "[*] Step 3: 修复 thunk 模式..."
sed -i "s/thunk to'/thunk to /g" "$DECL_FILE"

# Step 4: 注释掉第70737行的 keyed to' 错误
echo "[*] Step 4: 注释问题行 (line 70737)..."
sed -i "70737s/^int global constructor keyed to'/\/\/ int global constructor keyed to'/" "$DECL_FILE"

# 验证修复结果
echo ""
echo "[*] 修复完成! 验证结果:"
echo -n "  @ 符号剩余: "
grep -c "@" "$DECL_FILE" || echo "0"
echo -n "  未注释的 thunk to' 剩余: "
grep -v "^//" "$DECL_FILE" | grep -c "thunk to'" || echo "0"
echo -n "  未注释的 keyed to' 剩余: "
grep -v "^//" "$DECL_FILE" | grep -c "keyed to'" || echo "0"

echo ""
echo "备份文件: $BACKUP_FILE"
echo ""
echo "下一步: 运行编译验证"
echo "docker run --rm -v \"C:\\Users\\waw\\Desktop\\game:/build\" df_game_r:latest bash -c \"cd /build/src_split && make clean && make -j12 all 2>&1 | tee /build/compile_round2_clean_\$(date +%Y%m%d_%H%M%S).log\""
