#!/bin/bash
###############################################################################
# DNF游戏服务器 - Docker环境数据库测试脚本
#
# 功能: 在Docker容器中编译和运行数据库连接测试程序
# 用法: ./test_db_docker.sh
###############################################################################

set -e  # 遇到错误立即退出

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# 打印带颜色的消息
print_info() {
    echo -e "${CYAN}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[✓]${NC} $1"
}

print_error() {
    echo -e "${RED}[✗]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[⚠]${NC} $1"
}

# 检查Docker是否可用
if ! command -v docker &> /dev/null; then
    print_error "Docker未安装或不可用"
    exit 1
fi

print_info "开始数据库连接测试..."
echo ""

# 构建Docker镜像（如果需要）
print_info "检查Docker镜像..."
if ! docker images | grep -q dnf_game_builder; then
    print_warning "Docker镜像不存在，开始构建..."
    docker build -t dnf_game_builder .
    print_success "Docker镜像构建完成"
else
    print_success "Docker镜像已存在"
fi

echo ""
print_info "在Docker容器中编译测试程序..."

# 在Docker容器中编译测试程序
docker run --rm \
    -v "$(pwd):/build" \
    -w /build/tests \
    dnf_game_builder \
    make clean all

print_success "编译完成"

echo ""
print_info "运行数据库连接测试..."
echo ""

# 运行测试（需要网络访问以连接到MySQL服务器）
docker run --rm \
    -v "$(pwd):/build" \
    -w /build/tests \
    --network host \
    dnf_game_builder \
    ./test_db_connection ../config/database.cfg

echo ""
print_success "测试完成"
