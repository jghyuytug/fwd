#!/bin/bash
# 回滚 Phase 4 的修复
# 恢复到 Phase 3 完成后的状态

set -e

WORKDIR="C:/Users/waw/Desktop/game"
SRC_DIR="$WORKDIR/src_split"
INCLUDE_DIR="$WORKDIR/include"
LOGS_DIR="$WORKDIR/logs"

# 确保日志目录存在
mkdir -p "$LOGS_DIR"

echo "========================================"
echo "Phase 4 回滚"
echo "========================================"
echo ""
echo "警告: 即将回滚所有Phase 4修复"
echo "这将恢复到Phase 3完成后的状态"
echo ""
echo "操作列表:"
echo "  1. 恢复 df_game_r_decl.h 到Phase 3状态"
echo "  2. 删除Phase 4创建的新头文件"
echo "  3. 清理编译产物"
echo ""
echo "按Ctrl+C取消，或在5秒后继续..."
echo ""

# 等待5秒
for i in {5..1}; do
    echo -n "  $i "
    sleep 1
done
echo ""
echo ""

# ============================================================
# 步骤1: 检查备份
# ============================================================
echo "[1/4] 检查备份文件..."

if [ ! -f "$SRC_DIR/df_game_r_decl.h.before_phase4" ]; then
    echo "✗ 错误: Phase 4备份不存在"
    echo "  位置: $SRC_DIR/df_game_r_decl.h.before_phase4"
    echo ""
    echo "无法执行回滚!"
    exit 1
fi

if [ ! -f "$SRC_DIR/df_game_r_decl.h.before_phase3" ]; then
    echo "⚠ 警告: Phase 3备份不存在"
    echo "  将使用Phase 4备份作为基准"
fi

echo "✓ 备份文件存在"
echo ""

# ============================================================
# 步骤2: 恢复 df_game_r_decl.h
# ============================================================
echo "[2/4] 恢复 df_game_r_decl.h..."

# 备份当前版本 (以防万一)
TIMESTAMP=$(date +%Y%m%d_%H%M%S)
cp "$SRC_DIR/df_game_r_decl.h" "$LOGS_DIR/df_game_r_decl.h.backup_$TIMESTAMP"
echo "  当前版本已备份: logs/df_game_r_decl.h.backup_$TIMESTAMP"

# 恢复到Phase 3状态
if [ -f "$SRC_DIR/df_game_r_decl.h.before_phase3" ]; then
    cp "$SRC_DIR/df_game_r_decl.h.before_phase3" "$SRC_DIR/df_game_r_decl.h"
    echo "✓ 恢复到Phase 3状态 (before_phase3)"
else
    # 使用Phase 4备份
    cp "$SRC_DIR/df_game_r_decl.h.before_phase4" "$SRC_DIR/df_game_r_decl.h"
    echo "✓ 恢复到Phase 4之前 (before_phase4)"
fi

echo ""

# ============================================================
# 步骤3: 删除Phase 4创建的文件
# ============================================================
echo "[3/4] 删除Phase 4新增的头文件..."

# Phase 4创建的新文件列表
PHASE4_FILES=(
    "$INCLUDE_DIR/item/CItem.h"
    "$INCLUDE_DIR/network/PacketDispatcher.h"
    "$INCLUDE_DIR/network/InterDispatcher.h"
    "$INCLUDE_DIR/network/CNetwork.h"
    "$INCLUDE_DIR/database/MySQL.h"
    "$INCLUDE_DIR/game/CDungeon.h"
    "$INCLUDE_DIR/user/CUserCharacInfo.h"
    "$INCLUDE_DIR/party/CParty.h"
)

DELETED_COUNT=0
for file in "${PHASE4_FILES[@]}"; do
    if [ -f "$file" ]; then
        rm -f "$file"
        echo "  删除: $(basename $file)"
        DELETED_COUNT=$((DELETED_COUNT + 1))
    fi
done

if [ $DELETED_COUNT -eq 0 ]; then
    echo "  没有找到Phase 4文件"
else
    echo "  已删除 $DELETED_COUNT 个文件"
fi

echo ""

# ============================================================
# 步骤4: 清理编译产物
# ============================================================
echo "[4/4] 清理编译产物..."

cd "$SRC_DIR"

# 删除.o文件
OBJ_COUNT=$(find . -name "*.o" -type f 2>/dev/null | wc -l)
find . -name "*.o" -type f -delete 2>/dev/null || true
echo "  已删除 $OBJ_COUNT 个.o文件"

# 删除最终二进制 (如果存在)
if [ -f "df_game_r" ]; then
    rm -f "df_game_r"
    echo "  已删除 df_game_r 二进制"
fi

# 删除Makefile生成的临时文件
make clean > /dev/null 2>&1 || true
echo "  已执行 make clean"

echo ""

# ============================================================
# 完成
# ============================================================
echo "========================================"
echo "回滚完成"
echo "========================================"
echo ""
echo "状态:"
echo "  ✓ df_game_r_decl.h 已恢复"
echo "  ✓ Phase 4新增文件已删除"
echo "  ✓ 编译产物已清理"
echo ""
echo "下一步:"
echo "  1. 验证源代码状态:"
echo "     tail -20 $SRC_DIR/df_game_r_decl.h | head -10"
echo ""
echo "  2. 重新编译:"
echo "     cd $SRC_DIR"
echo "     make clean && make -j8 all"
echo ""
echo "  3. 分析结果:"
echo "     python3 scripts/analyze_final_results.py <log_file>"
echo ""
echo "备份:"
echo "  当前Phase 4版本: logs/df_game_r_decl.h.backup_$TIMESTAMP"
echo ""
