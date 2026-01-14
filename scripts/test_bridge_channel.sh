#!/bin/bash
#
# test_bridge_channel.sh - 测试 Bridge 和 Channel 服务启动及通信
#
# 用法: ./test_bridge_channel.sh
#

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"

BRIDGE_DIR="$PROJECT_DIR/src/bridge_cpp"
CHANNEL_DIR="$PROJECT_DIR/src/channel_cpp"

BRIDGE_BIN="$BRIDGE_DIR/df_bridge_r"
CHANNEL_BIN="$CHANNEL_DIR/df_channel_r"

BRIDGE_PORT=7000
CHANNEL_PORT=7001

# 颜色
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

log_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

cleanup() {
    log_info "Cleaning up..."

    # 停止 Channel
    if [ -f "$CHANNEL_DIR/pid/channel.pid" ]; then
        PID=$(cat "$CHANNEL_DIR/pid/channel.pid" 2>/dev/null)
        if [ -n "$PID" ] && kill -0 "$PID" 2>/dev/null; then
            kill "$PID" 2>/dev/null || true
            sleep 1
        fi
        rm -f "$CHANNEL_DIR/pid/channel.pid"
    fi

    # 停止 Bridge
    if [ -f "$BRIDGE_DIR/pid/bridge.pid" ]; then
        PID=$(cat "$BRIDGE_DIR/pid/bridge.pid" 2>/dev/null)
        if [ -n "$PID" ] && kill -0 "$PID" 2>/dev/null; then
            kill "$PID" 2>/dev/null || true
            sleep 1
        fi
        rm -f "$BRIDGE_DIR/pid/bridge.pid"
    fi

    log_info "Cleanup complete"
}

trap cleanup EXIT

# 检查二进制文件
check_binaries() {
    log_info "Checking binaries..."

    if [ ! -f "$BRIDGE_BIN" ]; then
        log_error "Bridge binary not found: $BRIDGE_BIN"
        return 1
    fi

    if [ ! -f "$CHANNEL_BIN" ]; then
        log_error "Channel binary not found: $CHANNEL_BIN"
        return 1
    fi

    log_info "Binaries found:"
    ls -la "$BRIDGE_BIN"
    ls -la "$CHANNEL_BIN"

    return 0
}

# 创建必要目录
create_dirs() {
    log_info "Creating directories..."

    mkdir -p "$BRIDGE_DIR/log" "$BRIDGE_DIR/pid"
    mkdir -p "$CHANNEL_DIR/log" "$CHANNEL_DIR/pid"
}

# 启动 Bridge
start_bridge() {
    log_info "Starting Bridge on port $BRIDGE_PORT..."

    cd "$BRIDGE_DIR"

    # 检查端口是否已被占用
    if netstat -tuln 2>/dev/null | grep -q ":$BRIDGE_PORT "; then
        log_warn "Port $BRIDGE_PORT already in use"
        return 1
    fi

    # 启动
    ./df_bridge_r bridge &
    BRIDGE_PID=$!

    sleep 2

    # 检查是否运行
    if kill -0 "$BRIDGE_PID" 2>/dev/null; then
        log_info "Bridge started (PID: $BRIDGE_PID)"
        return 0
    else
        log_error "Bridge failed to start"
        return 1
    fi
}

# 启动 Channel
start_channel() {
    log_info "Starting Channel on port $CHANNEL_PORT..."

    cd "$CHANNEL_DIR"

    # 检查端口是否已被占用
    if netstat -tuln 2>/dev/null | grep -q ":$CHANNEL_PORT "; then
        log_warn "Port $CHANNEL_PORT already in use"
        return 1
    fi

    # 启动
    ./df_channel_r channel &
    CHANNEL_PID=$!

    sleep 2

    # 检查是否运行
    if kill -0 "$CHANNEL_PID" 2>/dev/null; then
        log_info "Channel started (PID: $CHANNEL_PID)"
        return 0
    else
        log_error "Channel failed to start"
        return 1
    fi
}

# 检查端口监听
check_ports() {
    log_info "Checking port bindings..."

    local bridge_ok=0
    local channel_ok=0

    # 等待几秒让服务绑定端口
    sleep 2

    if netstat -tuln 2>/dev/null | grep -q ":$BRIDGE_PORT "; then
        log_info "Bridge listening on port $BRIDGE_PORT"
        bridge_ok=1
    else
        log_warn "Bridge NOT listening on port $BRIDGE_PORT"
    fi

    if netstat -tuln 2>/dev/null | grep -q ":$CHANNEL_PORT "; then
        log_info "Channel listening on port $CHANNEL_PORT"
        channel_ok=1
    else
        log_warn "Channel NOT listening on port $CHANNEL_PORT"
    fi

    if [ "$bridge_ok" = "1" ] && [ "$channel_ok" = "1" ]; then
        return 0
    else
        return 1
    fi
}

# 测试连接
test_connection() {
    log_info "Testing TCP connection..."

    # 测试 Bridge
    if timeout 5 bash -c "echo '' | nc -v localhost $BRIDGE_PORT" 2>&1 | grep -q "Connected"; then
        log_info "Bridge connection: OK"
    else
        log_warn "Bridge connection: FAILED (might be protocol issue)"
    fi

    # 测试 Channel
    if timeout 5 bash -c "echo '' | nc -v localhost $CHANNEL_PORT" 2>&1 | grep -q "Connected"; then
        log_info "Channel connection: OK"
    else
        log_warn "Channel connection: FAILED (might be protocol issue)"
    fi
}

# 显示日志
show_logs() {
    log_info "Recent logs:"

    echo "=== Bridge Log ==="
    tail -20 "$BRIDGE_DIR/log/bridge.log" 2>/dev/null || echo "(no log)"

    echo ""
    echo "=== Channel Log ==="
    tail -20 "$CHANNEL_DIR/log/channel.log" 2>/dev/null || echo "(no log)"
}

# 主测试
main() {
    echo "========================================"
    echo "  Bridge/Channel Service Test"
    echo "========================================"
    echo ""

    check_binaries || exit 1
    create_dirs

    echo ""
    log_info "Starting services..."

    start_bridge
    start_channel

    echo ""
    check_ports

    echo ""
    test_connection

    echo ""
    show_logs

    echo ""
    log_info "Test complete. Services running. Press Ctrl+C to stop."

    # 等待
    wait
}

main "$@"
