###############################################################################
# DNF游戏服务器 - 数据库测试程序 Makefile
###############################################################################

# 编译器配置
CC = gcc
CXX = g++
CFLAGS = -m32 -g -Wall -Wextra -std=c99
INCLUDES = -I../include -I../include/database/headers -I../include/common -I../include/common/headers

# 库配置
LIBS = -lpthread -lm -ldl -lssl -lcrypto -lstdc++
LDFLAGS = -m32 -L/usr/lib/mysql -lmysqlclient

# 源文件和目标
DB_TEST_SRC = test_db_connection.c ../src/database/db_connection_manager.c
DB_TEST_TARGET = test_db_connection

ACCOUNTS_TEST_SRC = test_accounts_orm.c ../src/database/db_connection_manager.c ../src/database/accounts_orm.c
ACCOUNTS_TEST_TARGET = test_accounts_orm

CHARAC_TEST_SRC = test_charac_info_orm.c ../src/database/db_connection_manager.c ../src/database/charac_info_orm.c
CHARAC_TEST_TARGET = test_charac_info_orm

CARGO_TEST_SRC = test_account_cargo_orm.c ../src/database/db_connection_manager.c ../src/database/account_cargo_orm.c
CARGO_TEST_TARGET = test_account_cargo_orm

ADMIN_TEST_SRC = test_admin_member_orm.c ../src/database/db_connection_manager.c ../src/database/admin_member_orm.c
ADMIN_TEST_TARGET = test_admin_member_orm

# 默认目标
.PHONY: all clean test test-db test-accounts test-charac test-cargo test-admin help

all: $(DB_TEST_TARGET) $(ACCOUNTS_TEST_TARGET) $(CHARAC_TEST_TARGET) $(CARGO_TEST_TARGET) $(ADMIN_TEST_TARGET)

# 编译数据库连接测试程序
$(DB_TEST_TARGET): $(DB_TEST_SRC)
	@echo "[CC] 编译数据库连接测试程序..."
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ $(LDFLAGS) $(LIBS)
	@echo "[SUCCESS] 测试程序编译完成: $@"

# 编译账号ORM测试程序
$(ACCOUNTS_TEST_TARGET): $(ACCOUNTS_TEST_SRC)
	@echo "[CC] 编译账号ORM测试程序..."
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ $(LDFLAGS) $(LIBS)
	@echo "[SUCCESS] 测试程序编译完成: $@"

# 编译角色ORM测试程序
$(CHARAC_TEST_TARGET): $(CHARAC_TEST_SRC)
	@echo "[CC] 编译角色ORM测试程序..."
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ $(LDFLAGS) $(LIBS)
	@echo "[SUCCESS] 测试程序编译完成: $@"

# 编译账号仓库ORM测试程序
$(CARGO_TEST_TARGET): $(CARGO_TEST_SRC)
	@echo "[CC] 编译账号仓库ORM测试程序..."
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ $(LDFLAGS) $(LIBS)
	@echo "[SUCCESS] 测试程序编译完成: $@"

# 编译管理员ORM测试程序
$(ADMIN_TEST_TARGET): $(ADMIN_TEST_SRC)
	@echo "[CC] 编译管理员ORM测试程序..."
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ $(LDFLAGS) $(LIBS)
	@echo "[SUCCESS] 测试程序编译完成: $@"

# 运行所有测试
test: test-db test-accounts test-charac test-cargo test-admin

# 运行数据库连接测试
test-db: $(DB_TEST_TARGET)
	@echo "[TEST] 运行数据库连接测试..."
	@echo ""
	./$(DB_TEST_TARGET) ../config/database.cfg

# 运行账号ORM测试
test-accounts: $(ACCOUNTS_TEST_TARGET)
	@echo "[TEST] 运行账号ORM测试..."
	@echo ""
	./$(ACCOUNTS_TEST_TARGET) ../config/database.cfg

# 运行角色ORM测试
test-charac: $(CHARAC_TEST_TARGET)
	@echo "[TEST] 运行角色ORM测试..."
	@echo ""
	./$(CHARAC_TEST_TARGET) ../config/database.cfg

# 运行账号仓库ORM测试
test-cargo: $(CARGO_TEST_TARGET)
	@echo "[TEST] 运行账号仓库ORM测试..."
	@echo ""
	./$(CARGO_TEST_TARGET) ../config/database.cfg

# 运行管理员ORM测试
test-admin: $(ADMIN_TEST_TARGET)
	@echo "[TEST] 运行管理员ORM测试..."
	@echo ""
	./$(ADMIN_TEST_TARGET) ../config/database.cfg

# 清理编译产物
clean:
	@echo "[CLEAN] 清理测试程序..."
	rm -f $(DB_TEST_TARGET) $(ACCOUNTS_TEST_TARGET) $(CHARAC_TEST_TARGET) $(CARGO_TEST_TARGET) $(ADMIN_TEST_TARGET)
	@echo "[CLEAN] 清理完成"

# 帮助信息
help:
	@echo "DNF数据库测试程序 - 可用目标:"
	@echo "  make all            - 编译所有测试程序 (默认)"
	@echo "  make test           - 运行所有测试"
	@echo "  make test-db        - 运行数据库连接测试"
	@echo "  make test-accounts  - 运行账号ORM测试"
	@echo "  make test-charac    - 运行角色ORM测试"
	@echo "  make test-cargo     - 运行账号仓库ORM测试"
	@echo "  make test-admin     - 运行管理员ORM测试"
	@echo "  make clean          - 清理编译产物"
	@echo "  make help           - 显示此帮助信息"
	@echo ""
	@echo "编译要求:"
	@echo "  - gcc with -m32 support"
	@echo "  - libmysqlclient-dev (32-bit)"
	@echo "  - 可访问的MySQL数据库服务器 (192.168.200.131:3306)"
