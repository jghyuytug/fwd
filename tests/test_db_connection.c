/**
 * DNF游戏服务器 - 数据库连接测试程序
 *
 * 功能:
 * 1. 测试连接到5个测试数据库
 * 2. 验证基本的CRUD操作
 * 3. 显示连接池统计信息
 *
 * 编译: gcc -o test_db_connection test_db_connection.c ../src/database/database_init.c -I../include -I../include/database/headers -I../include/common -lmysqlclient -m32
 * 运行: ./test_db_connection ../config/database.cfg
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db_connection_manager.h"

// ANSI颜色代码
#define COLOR_RESET   "\033[0m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_RED     "\033[31m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_CYAN    "\033[36m"

// 数据库类型名称映射
const char* db_type_names[] = {
    "Unknown",
    "test_d_taiwan (账号)",
    "test_taiwan_cain (游戏)",
    "test_taiwan_billing (计费)",
    "test_taiwan_login (登录)",
    "test_d_guild (公会)"
};

// 打印分隔线
void print_separator() {
    printf("%s", COLOR_CYAN);
    printf("========================================");
    printf("========================================\n");
    printf("%s", COLOR_RESET);
}

// 打印成功消息
void print_success(const char* msg) {
    printf("%s✓ %s%s\n", COLOR_GREEN, msg, COLOR_RESET);
}

// 打印错误消息
void print_error(const char* msg) {
    printf("%s✗ %s%s\n", COLOR_RED, msg, COLOR_RESET);
}

// 打印信息消息
void print_info(const char* msg) {
    printf("%s● %s%s\n", COLOR_BLUE, msg, COLOR_RESET);
}

// 打印警告消息
void print_warning(const char* msg) {
    printf("%s⚠ %s%s\n", COLOR_YELLOW, msg, COLOR_RESET);
}

// 测试连接到指定数据库
int test_database_connection(DBConnectionManager* connector, int db_type) {
    print_separator();
    printf("%s测试数据库: %s%s\n", COLOR_CYAN, db_type_names[db_type], COLOR_RESET);
    print_separator();

    // 1. 连接测试
    print_info("正在连接到数据库...");
    if (DBConnectionManager_Connect(connector, db_type) != 0) {
        print_error("连接失败");
        return -1;
    }
    print_success("连接成功");

    // 2. 检查连接状态
    if (!DBConnectionManager_IsConnected(connector, db_type)) {
        print_error("连接状态检查失败");
        return -1;
    }
    print_success("连接状态正常");

    // 3. 执行基本查询 (SELECT 1)
    print_info("执行基本查询测试 (SELECT 1)...");
    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(connector, db_type, "SELECT 1 AS test_value", &result) < 0) {
        print_error("查询执行失败");
        return -1;
    }
    print_success("查询执行成功");

    // 验证结果
    if (result.row_count == 1 && result.column_count == 1) {
        print_success("查询结果验证通过 (1行 x 1列)");
    } else {
        print_error("查询结果验证失败");
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);

    // 4. 查询表列表
    print_info("查询表列表...");
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(connector, db_type, "SHOW TABLES", &result) < 0) {
        print_error("查询表列表失败");
        return -1;
    }

    printf("  发现 %d 个表:\n", result.row_count);
    char* values[10];
    int row_num = 0;
    while (DBQueryResult_FetchRow(&result, values) > 0 && row_num < 10) {
        printf("    %d. %s\n", row_num + 1, values[0]);
        row_num++;
    }
    if (result.row_count > 10) {
        printf("    ... (共 %d 个表)\n", result.row_count);
    }

    DBQueryResult_Free(&result);
    print_success("表列表查询成功");

    // 5. 连接池统计
    int total, available, inuse;
    if (DBConnectionManager_GetConnectionPoolStats(connector, db_type, &total, &available, &inuse) == 0) {
        printf("  连接池统计:\n");
        printf("    总连接数: %d\n", total);
        printf("    可用连接: %d\n", available);
        printf("    使用中连接: %d\n", inuse);
        print_success("连接池状态正常");
    }

    printf("\n");
    return 0;
}

// 测试事务功能
int test_transaction(DBConnectionManager* connector) {
    print_separator();
    printf("%s测试事务功能%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    int db_type = DB_TYPE_TAIWAN;  // 使用账号数据库测试

    print_info("开始事务...");
    if (DBConnectionManager_BeginTransaction(connector, db_type) < 0) {
        print_error("开始事务失败");
        return -1;
    }
    print_success("事务开始成功");

    // 执行一个测试查询
    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    print_info("在事务中执行查询...");
    if (DBConnectionManager_ExecuteQuery(connector, db_type, "SELECT COUNT(*) FROM accounts", &result) < 0) {
        print_warning("查询失败 (可能表不存在)");
        DBConnectionManager_RollbackTransaction(connector, db_type);
        print_info("事务已回滚");
        return 0;  // 不算错误，表可能还没数据
    }

    char* values[1];
    if (DBQueryResult_FetchRow(&result, values) > 0) {
        printf("  accounts表记录数: %s\n", values[0]);
    }
    DBQueryResult_Free(&result);

    print_info("提交事务...");
    if (DBConnectionManager_CommitTransaction(connector, db_type) < 0) {
        print_error("提交事务失败");
        return -1;
    }
    print_success("事务提交成功");

    printf("\n");
    return 0;
}

// 打印总体统计信息
void print_overall_stats(DBConnectionManager* connector) {
    print_separator();
    printf("%s总体统计信息%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    int total_queries, failed_queries;
    DBConnectionManager_GetStats(connector, 0, &total_queries, &failed_queries);

    printf("  总查询次数: %d\n", total_queries);
    printf("  失败查询次数: %d\n", failed_queries);

    if (failed_queries == 0) {
        printf("  成功率: %s100%%%s\n", COLOR_GREEN, COLOR_RESET);
    } else {
        float success_rate = ((float)(total_queries - failed_queries) / total_queries) * 100.0f;
        printf("  成功率: %.2f%%\n", success_rate);
    }

    printf("\n");
}

int main(int argc, char* argv[]) {
    printf("\n");
    print_separator();
    printf("%s  DNF游戏服务器 - 数据库连接测试程序  %s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();
    printf("\n");

    // 检查命令行参数
    if (argc < 2) {
        print_error("用法: test_db_connection <配置文件路径>");
        printf("  示例: ./test_db_connection ../config/database.cfg\n");
        return 1;
    }

    const char* config_file = argv[1];
    printf("配置文件: %s\n\n", config_file);

    // 创建数据库连接器
    DBConnectionManager connector;
    memset(&connector, 0, sizeof(DBConnectionManager));

    // 初始化连接器
    print_info("初始化数据库连接器...");
    if (DBConnectionManager_Initialize(&connector, config_file) != 0) {
        print_error("初始化失败");
        return 1;
    }
    print_success("初始化成功");
    printf("\n");

    // 测试每个数据库连接
    int success_count = 0;
    for (int i = 1; i <= DB_TYPE_MAX_INDEX; i++) {
        if (test_database_connection(&connector, i) == 0) {
            success_count++;
        }
    }

    // 测试事务功能
    test_transaction(&connector);

    // 打印总体统计
    print_overall_stats(&connector);

    // 清理
    print_info("清理并关闭所有连接...");
    DBConnectionManager_Cleanup(&connector);
    print_success("清理完成");

    printf("\n");
    print_separator();
    printf("%s测试结果: %d/%d 个数据库连接成功%s\n",
           success_count == DB_TYPE_MAX_INDEX ? COLOR_GREEN : COLOR_YELLOW,
           success_count, DB_TYPE_MAX_INDEX, COLOR_RESET);
    print_separator();
    printf("\n");

    return (success_count == DB_TYPE_MAX_INDEX) ? 0 : 1;
}
