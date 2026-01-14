#!/bin/bash
###############################################################################
# DNF游戏服务器 - 简化数据库测试脚本
#
# 功能: 手动上传文件到服务器并编译测试
# 用法: bash test_db_simple.sh
###############################################################################

SERVER="root@192.168.200.131"
SSH_OPTS="-o KexAlgorithms=+diffie-hellman-group1-sha1,diffie-hellman-group14-sha1 -o HostKeyAlgorithms=+ssh-rsa,ssh-dss"

echo "============================================"
echo "  DNF数据库连接测试 - 简化版本"
echo "============================================"
echo ""
echo "服务器: 192.168.200.131"
echo "用户: root"
echo "密码: 12345"
echo ""
echo "步骤 1: 准备文件..."
echo "============================================"

# 创建临时目录
rm -rf /tmp/dnf_test
mkdir -p /tmp/dnf_test/{include/database/headers,src/database,config,tests}

# 复制必要文件
echo "复制源文件..."
cp tests/test_db_connection.c /tmp/dnf_test/tests/ 2>/dev/null || echo "警告: test_db_connection.c 不存在"
cp src/database/database_init.c /tmp/dnf_test/src/database/ 2>/dev/null || echo "警告: database_init.c 不存在"
cp config/database.cfg /tmp/dnf_test/config/ 2>/dev/null || echo "警告: database.cfg 不存在"
cp include/database/headers/db_manager.h /tmp/dnf_test/include/database/headers/ 2>/dev/null || echo "警告: db_manager.h 不存在"

# 创建Makefile
cat > /tmp/dnf_test/tests/Makefile << 'EOF'
CC = gcc
CFLAGS = -g -Wall -Wextra
INCLUDES = -I../include -I../include/database/headers
LIBS = -lmysqlclient -lpthread

all: test_db_connection

test_db_connection: test_db_connection.c ../src/database/database_init.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

clean:
	rm -f test_db_connection

test: test_db_connection
	./test_db_connection ../config/database.cfg
EOF

# 打包
echo "打包文件..."
cd /tmp
tar czf dnf_test.tar.gz dnf_test/

echo ""
echo "步骤 2: 上传文件到服务器"
echo "============================================"
echo "请输入密码: 12345"
echo ""

scp ${SSH_OPTS} /tmp/dnf_test.tar.gz ${SERVER}:/tmp/

echo ""
echo "步骤 3: 在服务器上编译和测试"
echo "============================================"
echo "请输入密码: 12345"
echo ""

ssh ${SSH_OPTS} ${SERVER} << 'ENDSSH'
cd /tmp
tar xzf dnf_test.tar.gz
cd dnf_test/tests

echo "编译测试程序..."
make clean
make all

if [ $? -eq 0 ]; then
    echo ""
    echo "========================================"
    echo "  运行数据库连接测试"
    echo "========================================"
    echo ""
    ./test_db_connection ../config/database.cfg
    TEST_RESULT=$?

    echo ""
    if [ $TEST_RESULT -eq 0 ]; then
        echo "✓ 测试成功!"
    else
        echo "✗ 测试失败 (退出码: $TEST_RESULT)"
    fi
else
    echo "✗ 编译失败!"
    exit 1
fi
ENDSSH

echo ""
echo "完成!"

# 清理
rm -rf /tmp/dnf_test /tmp/dnf_test.tar.gz
