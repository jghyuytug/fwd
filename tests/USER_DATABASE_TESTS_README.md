# User Database Module - Unit Tests Documentation
**Phase 6.1 Week 3 - Database Integration Testing**

## 概述

本文档说明User模块数据库功能的单元测试，涵盖所有CRUD操作和错误处理。

## 测试覆盖范围

### 1. Account数据库操作测试 (4个函数)
- `User_DB_CreateAccount` - 创建新账号
- `User_DB_LoadAccount` - 加载账号数据
- `User_DB_SaveAccount` - 保存账号更新
- `User_DB_DeleteAccount` - 删除账号（软删除）

### 2. Character数据库操作测试 (5个函数)
- `User_DB_CreateCharacter` - 创建新角色
- `User_DB_LoadCharacter` - 加载单个角色
- `User_DB_LoadCharacters` - 加载账号所有角色
- `User_DB_SaveCharacter` - 保存角色更新
- `User_DB_DeleteCharacter` - 删除角色（软删除，7天恢复期）

### 3. Session数据库操作测试 (3个函数)
- `User_DB_SaveSession` - 保存会话（使用REPLACE INTO实现upsert）
- `User_DB_LoadSession` - 加载会话数据
- `User_DB_DeleteSession` - 删除会话

### 4. 存储过程调用测试 (1个函数)
- `User_DB_AuthenticateAccount` - 账号认证（调用存储过程）

### 5. 错误处理测试
- NULL参数验证
- 不存在的记录查询
- 无效参数处理
- 数据库连接失败处理

## 测试环境准备

### 前提条件

1. **MySQL数据库服务器运行中**
   ```bash
   # 检查MySQL服务状态
   systemctl status mysqld
   ```

2. **测试数据库已创建并导入Schema**
   ```bash
   # 创建测试数据库
   mysql -u root -p -e "CREATE DATABASE IF NOT EXISTS testdb CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;"

   # 导入User模块Schema
   mysql -u root -p testdb < database/schema_user_module.sql
   ```

3. **数据库配置文件存在**

   文件位置: `config/database.cfg`

   示例配置:
   ```ini
   [Database_Account]
   host=localhost
   port=3306
   user=testuser
   password=testpass
   database=testdb
   charset=utf8mb4
   pool_size=5
   timeout=30

   [Database_Character]
   host=localhost
   port=3306
   user=testuser
   password=testpass
   database=testdb
   charset=utf8mb4
   pool_size=5
   timeout=30

   [Database_Game]
   host=localhost
   port=3306
   user=testuser
   password=testpass
   database=testdb
   charset=utf8mb4
   pool_size=5
   timeout=30

   [Database_Log]
   host=localhost
   port=3306
   user=testuser
   password=testpass
   database=testdb
   charset=utf8mb4
   pool_size=5
   timeout=30
   ```

4. **所有依赖模块已编译**
   ```bash
   # 编译Common模块
   make -C src/common all

   # 编译Database模块
   make -C src/database all

   # 编译User模块
   make -C src/user all
   ```

## 编译测试

### 方法1: 使用Makefile（推荐）

```bash
cd tests

# 验证测试环境
make -f Makefile_user_tests verify

# 编译所有依赖库 + 测试
make -f Makefile_user_tests full

# 仅编译测试（如果库已存在）
make -f Makefile_user_tests all
```

### 方法2: 手动编译

```bash
cd tests

gcc -m32 -O0 -g -fPIC -pipe -Wall -Wextra -std=c99 \
    -I../include -I../include/common -I../include/user -I../include/database \
    -o user_database_unit_tests \
    user_database_unit_tests.c \
    ../lib/libuser.a \
    ../lib/libdatabase.a \
    ../lib/libcommon.a \
    -lmysqlclient -lpthread -lrt -ldl -lm
```

### 方法3: 使用Docker（推荐用于CI）

```bash
# 创建并启动容器
docker create --name test_runner dnf_game_builder sleep 600
docker start test_runner

# 复制源码和配置到容器
docker cp src test_runner:/build/
docker cp include test_runner:/build/
docker cp lib test_runner:/build/
docker cp tests test_runner:/build/
docker cp config test_runner:/build/

# 在容器中编译并运行测试
docker exec test_runner bash -c "cd /build/tests && make -f Makefile_user_tests full && ./user_database_unit_tests"

# 清理
docker stop test_runner
docker rm test_runner
```

## 运行测试

### 快速运行

```bash
cd tests

# 编译并运行
make -f Makefile_user_tests run
```

### 直接执行

```bash
cd tests
./user_database_unit_tests
```

### 预期输出

```
╔═══════════════════════════════════════════════════════╗
║   User Database Module - Unit Tests                  ║
║   Phase 6.1 Week 3 - Database Integration Testing    ║
╚═══════════════════════════════════════════════════════╝

Initializing database system...
✓ Database system initialized

┌─────────────────────────────────────────────────────┐
│  ACCOUNT DATABASE TESTS                             │
└─────────────────────────────────────────────────────┘

=== Test: Account Create and Load ===
[PASS] User_DB_CreateAccount should succeed
[PASS] User_DB_LoadAccount should succeed
[PASS] Account name should match
[PASS] Password hash should match
[PASS] Email should match
[PASS] Account status should be active
✓ Account created and loaded successfully (account_id=123)

=== Test: Account Save ===
[PASS] Should load existing account
[PASS] User_DB_SaveAccount should succeed
[PASS] Should reload account after save
[PASS] VIP level should be updated
[PASS] VIP points should be updated
[PASS] Login count should be updated
✓ Account saved and verified successfully

=== Test: Account Error Handling ===
[PASS] Should reject NULL account_name
[PASS] Should reject NULL account pointer
[PASS] Should reject NULL account in save
[PASS] Should reject NULL account in create
[PASS] Should return error for non-existent account
✓ Account error handling validated

=== Test: Account Delete ===
[PASS] Should load account before delete
[PASS] User_DB_DeleteAccount should succeed
✓ Account deleted successfully (soft delete with status=3)

┌─────────────────────────────────────────────────────┐
│  CHARACTER DATABASE TESTS                           │
└─────────────────────────────────────────────────────┘

[... 类似的角色测试输出 ...]

┌─────────────────────────────────────────────────────┐
│  SESSION DATABASE TESTS                             │
└─────────────────────────────────────────────────────┘

[... 类似的会话测试输出 ...]

┌─────────────────────────────────────────────────────┐
│  STORED PROCEDURE TESTS                             │
└─────────────────────────────────────────────────────┘

[... 存储过程测试输出 ...]

Cleaning up database system...
✓ Database system cleanup complete

╔═══════════════════════════════════════════════════════╗
║  TEST SUMMARY                                         ║
╠═══════════════════════════════════════════════════════╣
║  Total Tests Run:     52                              ║
║  Tests Passed:        52   ✓                          ║
║  Tests Failed:        0    ✗                          ║
║  Success Rate:        100.0%                          ║
╚═══════════════════════════════════════════════════════╝

🎉 All tests PASSED! User Database module is working correctly.
```

## 测试结构

### 测试辅助函数

```c
// 初始化测试账号数据
void init_test_account(UserAccount* account);

// 初始化测试角色数据
void init_test_character(Character* character, uint32_t account_id);

// 初始化测试会话数据
void init_test_session(UserSession* session, uint32_t account_id);

// 断言宏（带详细错误报告）
#define TEST_ASSERT(condition, message)
```

### 测试函数命名约定

- `test_<entity>_<operation>()` - 功能测试
- `test_<entity>_error_handling()` - 错误处理测试

例如:
- `test_account_create_and_load()` - 测试账号创建和加载
- `test_character_error_handling()` - 测试角色错误处理

## 测试数据

### 测试账号

- **账号名**: `test_account_001`, `test_char_account`, `test_auth_account`
- **密码哈希**: `sha256_test_hash_123456`
- **邮箱**: `test@example.com`
- **IP地址**: `192.168.1.100`

### 测试角色

- **角色名**: `TestCharacter001`
- **职业**: 1 (Fighter)
- **等级**: 1 → 10 (在保存测试中更新)
- **初始金币**: 1000 → 50000 (在保存测试中更新)

### 测试会话

- **客户端版本**: 20250101
- **Socket FD**: 1234
- **客户端端口**: 5678

## 清理测试数据

测试完成后，可以手动清理测试数据：

```sql
-- 清理测试账号
DELETE FROM user_accounts WHERE account_name LIKE 'test_%';

-- 清理测试角色
DELETE FROM characters WHERE character_name LIKE 'Test%';

-- 清理测试会话
DELETE FROM user_sessions WHERE username LIKE 'test_%';
```

或者使用专门的清理脚本（待创建）：

```bash
./cleanup_test_data.sh
```

## 故障排查

### 问题1: 数据库连接失败

**错误**: `ERROR: Failed to initialize database system (code=12289)`

**解决方案**:
1. 检查MySQL服务是否运行
2. 验证config/database.cfg配置正确
3. 确认数据库用户权限
4. 检查防火墙设置

### 问题2: 编译错误 - 找不到库

**错误**: `fatal error: user/user_database.h: No such file or directory`

**解决方案**:
```bash
# 确保包含路径正确
make -f Makefile_user_tests verify

# 重新编译所有依赖库
make -f Makefile_user_tests build-libs
```

### 问题3: 链接错误 - undefined reference

**错误**: `undefined reference to 'MySQL_select_fmt'`

**解决方案**:
```bash
# 检查库文件是否存在
ls -lh ../lib/libdatabase.a
ls -lh ../lib/libuser.a
ls -lh ../lib/libcommon.a

# 重新编译Database模块
make -C ../src/database clean
make -C ../src/database all
```

### 问题4: 测试运行时错误 - ERR_DATABASE_QUERY_FAILED

**可能原因**:
1. 数据库Schema未导入
2. 表结构与代码不匹配
3. 存储过程未创建

**解决方案**:
```bash
# 重新导入Schema
mysql -u root -p testdb < database/schema_user_module.sql

# 验证表是否存在
mysql -u root -p testdb -e "SHOW TABLES;"

# 检查存储过程
mysql -u root -p testdb -e "SHOW PROCEDURE STATUS WHERE Db='testdb';"
```

## 持续集成(CI)配置

### GitHub Actions示例

```yaml
name: User Database Tests

on: [push, pull_request]

jobs:
  test:
    runs-on: ubuntu-latest

    services:
      mysql:
        image: mysql:5.7
        env:
          MYSQL_ROOT_PASSWORD: root
          MYSQL_DATABASE: testdb
        ports:
          - 3306:3306
        options: --health-cmd="mysqladmin ping" --health-interval=10s --health-timeout=5s --health-retries=3

    steps:
    - uses: actions/checkout@v2

    - name: Import database schema
      run: |
        mysql -h 127.0.0.1 -u root -proot testdb < database/schema_user_module.sql

    - name: Build Docker image
      run: docker build -t dnf_game_builder .

    - name: Run tests
      run: |
        docker run --network host \
          -v $(pwd):/build \
          dnf_game_builder \
          bash -c "cd /build/tests && make -f Makefile_user_tests full && ./user_database_unit_tests"
```

## 性能基准

### 预期测试执行时间

- **总测试数**: 约50-60个断言
- **执行时间**: < 5秒 (本地MySQL)
- **执行时间**: < 10秒 (Docker MySQL)

### 数据库操作性能

- Account CRUD: < 100ms per operation
- Character CRUD: < 150ms per operation
- Session CRUD: < 50ms per operation
- Authenticate: < 200ms (包含存储过程)

## 测试覆盖率

| 模块 | 函数数 | 测试覆盖 | 覆盖率 |
|------|--------|----------|--------|
| Account DB | 4 | 4 | 100% |
| Character DB | 5 | 5 | 100% |
| Session DB | 3 | 3 | 100% |
| Stored Proc | 1 | 1 | 100% |
| **总计** | **13** | **13** | **100%** |

## 下一步

- ✅ 单元测试完成
- ⏳ 集成测试（测试完整用户流程）
- ⏳ 压力测试（并发操作）
- ⏳ 性能测试（大数据量）

## 参考文档

- [User Database Implementation](../src/user/user_database.c)
- [User Database Interface](../include/user/user_database.h)
- [Database Schema](../database/schema_user_module.sql)
- [Error Codes](../include/common/error_codes.h)

---

**文档版本**: 1.0
**最后更新**: 2025-11-08
**维护者**: Phase 6.1 开发团队
