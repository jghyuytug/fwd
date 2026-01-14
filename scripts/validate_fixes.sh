#!/bin/bash
# Day 3 Phase 5 - 验证所有修复
# 智能编译验证脚本，包含监控和回滚机制

set -e

WORKDIR="C:/Users/waw/Desktop/game"
SCRIPTS_DIR="$WORKDIR/scripts"
LOGS_DIR="$WORKDIR/logs"
TOOLS_DIR="$WORKDIR/tools"

# 确保目录存在
mkdir -p "$LOGS_DIR"
mkdir -p "$SCRIPTS_DIR"

echo "========================================"
echo "Day 3 Phase 5 - 智能编译验证"
echo "========================================"
echo ""
echo "启动时间: $(date '+%Y-%m-%d %H:%M:%S')"
echo "工作目录: $WORKDIR"
echo ""

# ============================================================
# 步骤1: 前置检查
# ============================================================
echo "[1/6] 前置条件检查..."

BACKUP_COUNT=0
if [ -f "$WORKDIR/src_split/df_game_r_decl.h.before_phase3" ]; then
    BACKUP_COUNT=$((BACKUP_COUNT + 1))
    echo "  ✓ Phase 3备份存在"
fi

if [ -f "$WORKDIR/src_split/df_game_r_decl.h.before_phase4" ]; then
    BACKUP_COUNT=$((BACKUP_COUNT + 1))
    echo "  ✓ Phase 4备份存在"
fi

if [ $BACKUP_COUNT -lt 1 ]; then
    echo "  ⚠ 警告: 未找到备份文件"
    echo "    建议在继续前创建备份"
fi

# 检查src_split目录
if [ ! -d "$WORKDIR/src_split" ]; then
    echo "  ✗ 错误: src_split目录不存在"
    exit 1
fi
echo "  ✓ src_split目录存在"

# 检查include目录
if [ ! -d "$WORKDIR/include" ]; then
    echo "  ✗ 错误: include目录不存在"
    exit 1
fi
echo "  ✓ include目录存在"

# ============================================================
# 步骤2: 快速语法检查 (仅头文件)
# ============================================================
echo ""
echo "[2/6] 快速语法检查 (头文件)..."

SYNTAX_LOG="$LOGS_DIR/syntax_check_$(date +%Y%m%d_%H%M%S).log"

cd "$WORKDIR/src_split"

# 检查df_game_r_decl.h的基本语法
echo "  检查 df_game_r_decl.h..."
if head -1000 df_game_r_decl.h | grep -E "^(#include|class|struct|typedef)" > /dev/null 2>&1; then
    echo "  ✓ df_game_r_decl.h 头部有效"
else
    echo "  ⚠ df_game_r_decl.h 结构不清晰，但继续"
fi

# 快速验证第一个part
echo "  快速编译检查 (df_game_r_part_1.c)..."
g++ -m32 -fsyntax-only -std=c++11 -x c++ -I. -I../include \
  df_game_r_part_1.c > "$SYNTAX_LOG" 2>&1 || true

SYNTAX_ERRORS=$(grep -c "^df_game_r_part_1.c:[0-9]*:.*error:" "$SYNTAX_LOG" || echo 0)
echo "  第一个part文件语法错误: $SYNTAX_ERRORS"

if [ $SYNTAX_ERRORS -gt 50 ]; then
    echo "  ⚠ 警告: 语法错误较多"
    echo "  显示前20个错误:"
    head -20 "$SYNTAX_LOG" | grep error:
fi

# ============================================================
# 步骤3: 预编译检查 (编译统计)
# ============================================================
echo ""
echo "[3/6] 预编译估算..."

# 查看当前源文件状态
PART_COUNT=$(ls df_game_r_part_*.c 2>/dev/null | wc -l)
echo "  源文件部分数: $PART_COUNT"

DECL_SIZE=$(wc -l < df_game_r_decl.h)
echo "  df_game_r_decl.h 行数: $DECL_SIZE"

# ============================================================
# 步骤4: 启动完整编译 (后台)
# ============================================================
echo ""
echo "[4/6] 启动完整编译..."

TIMESTAMP=$(date +%Y%m%d_%H%M%S)
COMPILE_LOG="$LOGS_DIR/day3_phase5_compile_$TIMESTAMP.log"

cd "$WORKDIR/src_split"

# 检查是否有Docker
if command -v docker &> /dev/null; then
    echo "  使用Docker编译..."

    # 清理旧目标文件
    make clean > /dev/null 2>&1 || true

    # 启动Docker编译 (后台)
    nohup docker run --rm -v "$WORKDIR:/build" df_game_r:latest \
      bash -c "cd /build/src_split && make clean && make -j8 all" \
      > "$COMPILE_LOG" 2>&1 &

    COMPILE_PID=$!
    echo "  Docker编译启动 (PID: $COMPILE_PID)"
    echo "  日志: $COMPILE_LOG"

else
    echo "  本地编译..."

    # 本地编译 (后台)
    nohup bash -c "cd '$WORKDIR/src_split' && make clean && make -j8 all" \
      > "$COMPILE_LOG" 2>&1 &

    COMPILE_PID=$!
    echo "  本地编译启动 (PID: $COMPILE_PID)"
    echo "  日志: $COMPILE_LOG"
fi

echo "  $COMPILE_PID" > "$LOGS_DIR/compile.pid"

# ============================================================
# 步骤5: 启动智能监控
# ============================================================
echo ""
echo "[5/6] 启动智能监控..."

# 等待日志文件创建
WAIT_COUNT=0
while [ ! -f "$COMPILE_LOG" ] && [ $WAIT_COUNT -lt 30 ]; do
    sleep 1
    WAIT_COUNT=$((WAIT_COUNT + 1))
done

if [ ! -f "$COMPILE_LOG" ]; then
    echo "  ⚠ 日志文件未创建，编译可能失败"
    echo "  请手动检查编译进度"
else
    echo "  ✓ 日志文件已创建"

    # 启动监控脚本 (后台)
    if [ -f "$SCRIPTS_DIR/monitor_compilation.py" ]; then
        nohup python3 "$SCRIPTS_DIR/monitor_compilation.py" \
          "$COMPILE_LOG" \
          --threshold 15000 \
          --interval 30 \
          > "$LOGS_DIR/monitor.log" 2>&1 &

        MONITOR_PID=$!
        echo "  监控脚本启动 (PID: $MONITOR_PID)"
    else
        echo "  ⚠ 监控脚本不存在，跳过"
    fi
fi

# ============================================================
# 步骤6: 等待编译完成
# ============================================================
echo ""
echo "[6/6] 等待编译完成..."
echo ""
echo "编译可能需要30-60分钟，请继续监控日志:"
echo "  tail -f $COMPILE_LOG"
echo ""
echo "或运行分析脚本:"
echo "  python3 $SCRIPTS_DIR/analyze_final_results.py $COMPILE_LOG"
echo ""

# 等待编译进程
wait $COMPILE_PID 2>/dev/null || true

echo ""
echo "========================================"
echo "编译完成！分析结果..."
echo "========================================"
echo ""

# ============================================================
# 结果分析
# ============================================================

if [ ! -f "$COMPILE_LOG" ]; then
    echo "⚠ 错误: 编译日志不存在"
    exit 1
fi

# 统计错误
FINAL_ERRORS=$(grep -c "^[^:]*:[0-9]*:.*error:" "$COMPILE_LOG" || echo 0)
FINAL_WARNINGS=$(grep -c "^[^:]*:[0-9]*:.*warning:" "$COMPILE_LOG" || echo 0)

echo "编译统计:"
echo "  错误数:    $FINAL_ERRORS"
echo "  警告数:    $FINAL_WARNINGS"
echo ""

# 与Day 2对比
DAY2_ERRORS=17772
REDUCTION=$((DAY2_ERRORS - FINAL_ERRORS))
REDUCTION_PCT=$((REDUCTION * 100 / DAY2_ERRORS))

echo "与Day 2对比:"
echo "  Day 2 (Round B): $DAY2_ERRORS 错误"
echo "  Day 3 (Phase 5): $FINAL_ERRORS 错误"
echo "  削减: $REDUCTION 错误 ($REDUCTION_PCT%)"
echo ""

# 评估
if [ $FINAL_ERRORS -lt 12000 ]; then
    echo "✓ 达到Week 1调整目标 (<12,000)"
    EXIT_CODE=0
elif [ $FINAL_ERRORS -lt 15000 ]; then
    echo "~ 接近目标 (<15,000)"
    EXIT_CODE=0
else
    echo "⚠ 未达目标，需要额外修复"
    EXIT_CODE=1
fi

# 检查.o文件生成
OBJ_COUNT=$(find "$WORKDIR/src_split" -name "*.o" -type f 2>/dev/null | wc -l)
echo ""
echo "生成的目标文件: $OBJ_COUNT/24"

if [ $OBJ_COUNT -gt 0 ]; then
    echo "✓ 成功生成目标文件"
else
    echo "⚠ 未生成目标文件"
fi

# ============================================================
# 生成完整报告
# ============================================================
echo ""
echo "详细日志: $COMPILE_LOG"
echo ""

# 如果有分析脚本，运行它
if [ -f "$SCRIPTS_DIR/analyze_final_results.py" ]; then
    echo "运行详细分析..."
    python3 "$SCRIPTS_DIR/analyze_final_results.py" "$COMPILE_LOG" || true
fi

echo ""
echo "========================================"
echo "验证完成"
echo "========================================"
echo ""

exit $EXIT_CODE
