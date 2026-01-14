#!/bin/bash
###############################################################################
# DNF游戏服务器 - 远程服务器数据库测试脚本
#
# 功能: 上传代码到生产服务器并编译运行数据库连接测试
# 用法: ./test_db_remote.sh
#
# 服务器信息:
#   IP: 192.168.200.131
#   User: root
#   Password: 12345
###############################################################################

set -e

# 服务器配置
SERVER_IP="192.168.200.131"
SERVER_USER="root"
SERVER_PASS="12345"
SERVER_PATH="/tmp/dnf_db_test"

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m'

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

echo ""
print_info "DNF数据库连接测试 - 远程服务器模式"
echo "======================================"
echo ""

# 检查必要文件
print_info "检查必要文件..."
if [ ! -f "tests/test_db_connection.c" ]; then
    print_error "测试程序源文件不存在"
    exit 1
fi

if [ ! -f "src/database/database_init.c" ]; then
    print_error "数据库初始化源文件不存在"
    exit 1
fi

if [ ! -f "config/database.cfg" ]; then
    print_error "数据库配置文件不存在"
    exit 1
fi
print_success "所有必要文件存在"

# 创建临时目录结构
print_info "准备上传文件..."
mkdir -p /tmp/dnf_upload/{include/database/headers,include/common/headers,src/database,config,tests}

# 复制文件
cp tests/test_db_connection.c /tmp/dnf_upload/tests/
cp src/database/database_init.c /tmp/dnf_upload/src/database/
cp config/database.cfg /tmp/dnf_upload/config/
cp include/database/headers/db_manager.h /tmp/dnf_upload/include/database/headers/
cp include/database/headers/database_interface.h /tmp/dnf_upload/include/database/headers/ 2>/dev/null || true
cp include/database/headers/db_types.h /tmp/dnf_upload/include/database/headers/ 2>/dev/null || true
cp include/database/headers/db_enums.h /tmp/dnf_upload/include/database/headers/ 2>/dev/null || true
cp include/common/headers/error_codes.h /tmp/dnf_upload/include/common/headers/ 2>/dev/null || true
cp include/common/defs.h /tmp/dnf_upload/include/common/ 2>/dev/null || true

# 创建简化的Makefile
cat > /tmp/dnf_upload/tests/Makefile << 'EOF'
CC = gcc
CFLAGS = -g -Wall -Wextra -std=c99
INCLUDES = -I../include -I../include/database/headers -I../include/common -I../include/common/headers
LIBS = -lmysqlclient -lpthread -lm

all: test_db_connection

test_db_connection: test_db_connection.c ../src/database/database_init.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

clean:
	rm -f test_db_connection

test: test_db_connection
	./test_db_connection ../config/database.cfg
EOF

print_success "文件准备完成"

# 打包文件
print_info "打包文件..."
cd /tmp
tar czf dnf_upload.tar.gz dnf_upload/
print_success "文件打包完成"

# 上传到服务器
print_info "上传文件到服务器 ${SERVER_IP}..."
sshpass -p "${SERVER_PASS}" scp \
    -o KexAlgorithms=+diffie-hellman-group1-sha1,diffie-hellman-group14-sha1 \
    -o HostKeyAlgorithms=+ssh-rsa,ssh-dss \
    -o StrictHostKeyChecking=no \
    /tmp/dnf_upload.tar.gz ${SERVER_USER}@${SERVER_IP}:/tmp/

print_success "文件上传完成"

# 在服务器上解压、编译和测试
print_info "在服务器上编译和运行测试..."
echo ""

sshpass -p "${SERVER_PASS}" ssh \
    -o KexAlgorithms=+diffie-hellman-group1-sha1,diffie-hellman-group14-sha1 \
    -o HostKeyAlgorithms=+ssh-rsa,ssh-dss \
    -o StrictHostKeyChecking=no \
    ${SERVER_USER}@${SERVER_IP} << 'ENDSSH'

# 解压文件
cd /tmp
rm -rf dnf_upload
tar xzf dnf_upload.tar.gz

# 编译测试程序
cd dnf_upload/tests
echo "=========================================="
echo "编译测试程序..."
echo "=========================================="
make clean
make all

if [ $? -eq 0 ]; then
    echo ""
    echo "=========================================="
    echo "运行数据库连接测试..."
    echo "=========================================="
    echo ""
    ./test_db_connection ../config/database.cfg
else
    echo "编译失败!"
    exit 1
fi

ENDSSH

echo ""
print_success "测试完成"

# 清理本地临时文件
rm -rf /tmp/dnf_upload /tmp/dnf_upload.tar.gz

echo ""
print_info "完成!"
