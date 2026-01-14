#!/bin/bash
# extract_flex_tables.sh - 提取 Flex 词法表
# 用法: bash extract_flex_tables.sh

set -e

SRC_DIR="src_split"
OUT_FILE="data/flex_tables.c"

echo "正在提取 Flex 表..."

# 创建输出文件头
cat > "$OUT_FILE" << 'EOF'
/*
 * flex_tables.c - Flex 词法分析器状态表
 *
 * 自动生成自: df_game_r_part01.c, df_game_r_part05.c
 * 生成时间: 2025-10-22
 *
 * 注意: 此文件包含大型静态数据表，仅供只读访问
 */

#include <data/flex_tables.h>

/* ========================================================================
 * 主 Flex 表 (DNF 脚本词法器)
 * 来源: src_split/df_game_r_part01.c
 * ======================================================================== */

EOF

# 提取 yy_accept[10000] (行 12-10014)
echo "提取 yy_accept[10000]..."
sed -n '12,10014p' "$SRC_DIR/df_game_r_part01.c" >> "$OUT_FILE"
echo "" >> "$OUT_FILE"

# 提取 yy_ec[256] (行 10015-10273)
echo "提取 yy_ec[256]..."
sed -n '10015,10273p' "$SRC_DIR/df_game_r_part01.c" >> "$OUT_FILE"
echo "" >> "$OUT_FILE"

# 提取 yy_meta[200] (行 10274-10476)
echo "提取 yy_meta[200]..."
sed -n '10274,10476p' "$SRC_DIR/df_game_r_part01.c" >> "$OUT_FILE"
echo "" >> "$OUT_FILE"

# 提取 yy_base[10000] (行 10477-20479)
echo "提取 yy_base[10000]..."
sed -n '10477,20479p' "$SRC_DIR/df_game_r_part01.c" >> "$OUT_FILE"
echo "" >> "$OUT_FILE"

# 提取 yy_def[10000] (行 20480-30482)
echo "提取 yy_def[10000]..."
sed -n '20480,30482p' "$SRC_DIR/df_game_r_part01.c" >> "$OUT_FILE"
echo "" >> "$OUT_FILE"

# 添加次级表说明
cat >> "$OUT_FILE" << 'EOF'

/* ========================================================================
 * 次级 Flex 表 (SlangFilter 词法器)
 * 来源: src_split/df_game_r_part05.c
 * ======================================================================== */

EOF

# 提取小型表
echo "提取 yy_accept_slang[57]..."
sed -n '84147,84206p' "$SRC_DIR/df_game_r_part05.c" | sed 's/yy_accept/yy_accept_slang/' >> "$OUT_FILE"
echo "" >> "$OUT_FILE"

echo "提取 yy_def_slang[64]..."
sed -n '84207,84273p' "$SRC_DIR/df_game_r_part05.c" | sed 's/yy_def/yy_def_slang/' >> "$OUT_FILE"
echo "" >> "$OUT_FILE"

echo "提取 yy_base_slang[64]..."
sed -n '84274,84340p' "$SRC_DIR/df_game_r_part05.c" | sed 's/yy_base/yy_base_slang/' >> "$OUT_FILE"
echo "" >> "$OUT_FILE"

echo "✅ Flex 表提取完成: $OUT_FILE"
ls -lh "$OUT_FILE"
