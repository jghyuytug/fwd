#!/bin/bash
# 回滚 Phase 3 的修复
# 恢复到 Phase 2 完成后的状态 (常量应用前)

set -e

WORKDIR="C:/Users/waw/Desktop/game"
SRC_DIR="$WORKDIR/src_split"
INCLUDE_DIR="$WORKDIR/include"
LOGS_DIR="$WORKDIR/logs"

# 确保日志目录存在
mkdir -p "$LOGS_DIR"

echo "========================================"
echo "Phase 3 回滚"
echo "========================================"
echo ""
echo "警告: 即将回滚Phase 3修复"
echo "这将恢复到Phase 2完成后的状态"
echo "常量应用将被撤销，代码可读性可能降低"
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
echo "[1/3] 检查备份文件..."

# 查找原始备份
PHASE2_BACKUP="$SRC_DIR/df_game_r_decl.h.original"
PHASE3_BACKUP="$SRC_DIR/df_game_r_decl.h.before_phase3"

if [ ! -f "$PHASE2_BACKUP" ] && [ ! -f "$PHASE3_BACKUP" ]; then
    echo "⚠ 警告: 找不到Phase 2或Phase 3的备份"
    echo "  位置1: $PHASE2_BACKUP"
    echo "  位置2: $PHASE3_BACKUP"
    echo ""
    echo "无法执行回滚!"
    exit 1
fi

if [ -f "$PHASE3_BACKUP" ]; then
    echo "✓ 找到Phase 3备份"
elif [ -f "$PHASE2_BACKUP" ]; then
    echo "✓ 找到原始备份"
fi

echo ""

# ============================================================
# 步骤2: 恢复 df_game_r_decl.h
# ============================================================
echo "[2/3] 恢复 df_game_r_decl.h..."

# 备份当前版本 (以防万一)
TIMESTAMP=$(date +%Y%m%d_%H%M%S)
cp "$SRC_DIR/df_game_r_decl.h" "$LOGS_DIR/df_game_r_decl.h.backup_$TIMESTAMP"
echo "  当前版本已备份: logs/df_game_r_decl.h.backup_$TIMESTAMP"

# 恢复
if [ -f "$PHASE3_BACKUP" ]; then
    cp "$PHASE3_BACKUP" "$SRC_DIR/df_game_r_decl.h"
    echo "✓ 恢复到Phase 3之前 (before_phase3)"
elif [ -f "$PHASE2_BACKUP" ]; then
    cp "$PHASE2_BACKUP" "$SRC_DIR/df_game_r_decl.h"
    echo "✓ 恢复到原始状态 (original)"
fi

echo ""

# ============================================================
# 步骤3: 清理编译产物
# ============================================================
echo "[3/3] 清理编译产物..."

cd "$SRC_DIR"

# 删除.o文件
OBJ_COUNT=$(find . -name "*.o" -type f 2>/dev/null | wc -l)
find . -name "*.o" -type f -delete 2>/dev/null || true
echo "  已删除 $OBJ_COUNT 个.o文件"

# 删除最终二进制
if [ -f "df_game_r" ]; then
    rm -f "df_game_r"
    echo "  已删除 df_game_r 二进制"
fi

# 清理编译临时文件
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
echo "  ✓ 编译产物已清理"
echo ""
echo "警告:"
echo "  Phase 3常量应用已撤销"
echo "  代码可读性会降低 (从60%回到25%左右)"
echo ""
echo "下一步:"
echo "  1. 验证恢复:"
echo "     wc -l $SRC_DIR/df_game_r_decl.h"
echo ""
echo "  2. 重新编译:"
echo "     cd $SRC_DIR"
echo "     make clean && make -j8 all"
echo ""
echo "  3. 分析结果:"
echo "     python3 scripts/analyze_final_results.py <log_file>"
echo ""
echo "备份:"
echo "  Phase 3版本: logs/df_game_r_decl.h.backup_$TIMESTAMP"
echo ""
