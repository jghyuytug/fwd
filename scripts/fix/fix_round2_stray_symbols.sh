#!/bin/bash
# 第二轮修复: 清除残留的Hex-Rays符号
# 日期: 2025-10-29
# 目标: 修复26,904个stray '@' 和 stray '`' 错误

set -e

DECL_FILE="src_split/df_game_r_decl.h"

echo "[*] 第二轮修复开始..."
echo "[*] 目标文件: $DECL_FILE"

# 备份
BACKUP_FILE="${DECL_FILE}.backup_round2_$(date +%Y%m%d_%H%M%S)"
cp "$DECL_FILE" "$BACKUP_FILE"
echo "[*] 已备份到: $BACKUP_FILE"

# 1. 移除 @<register> 注释 (Hex-Rays calling convention)
echo "[*] 修复1: 移除 @<eax>, @<edx> 等寄存器注释..."
sed -i 's/@<e[abcd]x>//g' "$DECL_FILE"
sed -i 's/@<e[sd]i>//g' "$DECL_FILE"
sed -i 's/@<ebp>//g' "$DECL_FILE"
sed -i 's/@<esp>//g' "$DECL_FILE"

# 2. 移除其他 @ 符号 (IDA通配符或其他注释)
echo "[*] 修复2: 移除残留的 @ 符号..."
sed -i 's/@//g' "$DECL_FILE"

# 3. 修复 backtick 符号 (可能来自某些反引号注释)
echo "[*] 修复3: 移除 backtick 符号..."
sed -i "s/\`//g" "$DECL_FILE"

# 统计修复结果
echo ""
echo "[*] 修复完成!"
echo ""
echo "验证结果:"
grep -c "@" "$DECL_FILE" && echo "  警告: 仍有 @ 符号残留" || echo "  ✓ 所有 @ 符号已清除"
grep -c "\`" "$DECL_FILE" && echo "  警告: 仍有 backtick 残留" || echo "  ✓ 所有 backtick 已清除"
echo ""
echo "备份文件: $BACKUP_FILE"
echo ""
echo "下一步: cd src_split && make clean && make -j12 all 2>&1 | tee ../compile_round3_\$(date +%Y%m%d_%H%M%S).log"
