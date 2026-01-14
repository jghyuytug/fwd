#!/bin/bash
# 编译监控脚本 - 早期检测失败并终止无意义编译
# 用法: ./monitor_compilation.sh <log_file> <docker_container_id> <threshold_errors>

LOG_FILE="$1"
CONTAINER_ID="$2"
THRESHOLD="${3:-20000}"  # 默认阈值20000错误

if [ -z "$LOG_FILE" ] || [ -z "$CONTAINER_ID" ]; then
    echo "用法: $0 <log_file> <container_id> [threshold_errors]"
    echo "示例: $0 logs/compile.log abc123 20000"
    exit 1
fi

echo "============================================"
echo "编译监控启动"
echo "============================================"
echo "日志文件: $LOG_FILE"
echo "容器ID: $CONTAINER_ID"
echo "错误阈值: $THRESHOLD"
echo "检查间隔: 30秒"
echo ""

# 初始化变量
last_size=0
stall_count=0
last_error_count=0
check_interval=30

# 失败检测标准
MAX_STALL_COUNT=3      # 连续3次不增长判定为卡死
ERROR_GROWTH_RATE=1000 # 每30秒增长>1000错误判定为雪崩
NO_OBJ_AFTER_MIN=10    # 10分钟后仍无.o文件判定为失败

start_time=$(date +%s)

while true; do
    sleep $check_interval

    current_time=$(date +%s)
    elapsed_min=$(( (current_time - start_time) / 60 ))

    # 检查1: 日志文件是否存在
    if [ ! -f "$LOG_FILE" ]; then
        echo "[$(date +%H:%M:%S)] ⚠️  日志文件不存在，等待..."
        continue
    fi

    # 检查2: 日志大小是否增长
    current_size=$(stat -c%s "$LOG_FILE" 2>/dev/null || stat -f%z "$LOG_FILE" 2>/dev/null || wc -c < "$LOG_FILE")
    size_mb=$(echo "scale=1; $current_size / 1048576" | bc)

    if [ "$current_size" -eq "$last_size" ]; then
        stall_count=$((stall_count + 1))
        echo "[$(date +%H:%M:%S)] ⏸️  日志停止增长 (${stall_count}/${MAX_STALL_COUNT}) - ${size_mb}MB"

        if [ $stall_count -ge $MAX_STALL_COUNT ]; then
            echo ""
            echo "❌ 检测到编译卡死（日志${MAX_STALL_COUNT}次检查无变化）"

            # 统计最终错误
            error_count=$(grep -c "error:" "$LOG_FILE" 2>/dev/null || echo 0)
            obj_count=$(docker exec "$CONTAINER_ID" ls /build/src_split/*.o 2>/dev/null | wc -l)

            echo "   - 最终错误数: $error_count"
            echo "   - 目标文件数: $obj_count"
            echo "   - 运行时长: ${elapsed_min}分钟"
            echo ""
            echo "🛑 建议操作: docker stop $CONTAINER_ID"
            exit 1
        fi
    else
        stall_count=0
        growth=$((current_size - last_size))
        growth_kb=$((growth / 1024))
        echo "[$(date +%H:%M:%S)] ✅ 编译进行中 - 日志${size_mb}MB (+${growth_kb}KB)"
    fi
    last_size=$current_size

    # 检查3: 错误数是否超过阈值
    error_count=$(grep -c "error:" "$LOG_FILE" 2>/dev/null || echo 0)
    error_growth=$((error_count - last_error_count))

    if [ $error_count -gt 0 ]; then
        echo "           错误数: $error_count (+$error_growth)"

        # 3a: 错误数超过阈值
        if [ $error_count -gt $THRESHOLD ]; then
            echo ""
            echo "❌ 错误数超过阈值 ($error_count > $THRESHOLD)"
            echo "   - 这可能导致编译器效率极低"
            echo "   - 继续编译可能无法生成目标文件"
            echo ""
            echo "🛑 建议操作: docker stop $CONTAINER_ID"
            exit 2
        fi

        # 3b: 错误增长过快（雪崩）
        if [ $error_growth -gt $ERROR_GROWTH_RATE ] && [ $last_error_count -gt 0 ]; then
            echo "           ⚠️  错误快速增长 (+$error_growth/${check_interval}秒)"
        fi
    fi
    last_error_count=$error_count

    # 检查4: 是否生成了目标文件
    if [ $elapsed_min -ge $NO_OBJ_AFTER_MIN ]; then
        obj_count=$(docker exec "$CONTAINER_ID" ls /build/src_split/*.o 2>/dev/null | wc -l)

        if [ $obj_count -eq 0 ]; then
            echo ""
            echo "❌ ${NO_OBJ_AFTER_MIN}分钟后仍无目标文件生成"
            echo "   - 错误数: $error_count"
            echo "   - 编译器可能已放弃生成.o文件"
            echo "   - 只在输出错误信息"
            echo ""
            echo "🛑 建议操作: docker stop $CONTAINER_ID"
            exit 3
        else
            echo "           目标文件: $obj_count/24"
        fi
    fi

    # 检查5: 容器是否还在运行
    if ! docker ps --format '{{.ID}}' | grep -q "^${CONTAINER_ID:0:12}"; then
        echo ""
        echo "✅ 容器已停止，编译可能完成"

        # 显示最终统计
        error_count=$(grep -c "error:" "$LOG_FILE" 2>/dev/null || echo 0)
        warning_count=$(grep -c "warning:" "$LOG_FILE" 2>/dev/null || echo 0)

        echo ""
        echo "============================================"
        echo "编译完成统计"
        echo "============================================"
        echo "错误数: $error_count"
        echo "警告数: $warning_count"
        echo "运行时长: ${elapsed_min}分钟"
        echo "日志大小: ${size_mb}MB"
        echo ""

        # 判断成功或失败
        if [ $error_count -eq 0 ]; then
            echo "🎉 编译成功！"
            exit 0
        else
            echo "❌ 编译失败（$error_count 错误）"
            exit 4
        fi
    fi

    echo ""
done
