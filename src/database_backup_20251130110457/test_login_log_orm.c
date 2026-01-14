/**
 * DNF游戏服务器 - LoginLog ORM 测试程序
 *
 * 功能:
 * 1. 测试登录日志CRUD操作
 * 2. 测试登录日志查询功能
 * 3. 测试登出时间更新
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db_connection_manager.h"
#include "login_log_orm.h"

// ANSI颜色代码
#define COLOR_RESET   "\033[0m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_RED     "\033[31m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_CYAN    "\033[36m"

void print_separator() {
    printf("%s================================================================================\n%s",
           COLOR_CYAN, COLOR_RESET);
}

void print_success(const char* msg) {
    printf("%s✓ %s%s\n", COLOR_GREEN, msg, COLOR_RESET);
}

void print_error(const char* msg) {
    printf("%s✗ %s%s\n", COLOR_RED, msg, COLOR_RESET);
}

void print_info(const char* msg) {
    printf("%s● %s%s\n", COLOR_BLUE, msg, COLOR_RESET);
}

void print_warning(const char* msg) {
    printf("%s⚠ %s%s\n", COLOR_YELLOW, msg, COLOR_RESET);
}

// 测试创建登录日志
int test_create_login_log(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试1: 创建登录日志%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    LoginLog log;
    int i;

    // 创建记录1
    print_info("创建登录记录1 - 账号10001");
    LoginLog_Init(&log);
    log.account_id = 10001;
    strcpy(log.login_ip, "192.168.1.100");
    log.server_id = 1;

    if (LoginLog_Create(manager, &log) == 0) {
        print_success("登录记录创建成功");
        printf("  账号ID: %d, IP: %s, 服务器ID: %d\n",
               log.account_id, log.login_ip, log.server_id);
    } else {
        print_error("登录记录创建失败");
        return -1;
    }

    // 创建记录2 - 同一账号不同时间
    print_info("创建登录记录2 - 账号10001 (第二次登录)");
    LoginLog_Init(&log);
    log.account_id = 10001;
    strcpy(log.login_ip, "192.168.1.101");
    log.server_id = 2;

    if (LoginLog_Create(manager, &log) == 0) {
        print_success("登录记录创建成功");
    } else {
        print_error("登录记录创建失败");
        return -1;
    }

    // 批量创建不同账号的登录记录
    print_info("批量创建登录记录 (账号10002-10004)");
    for (i = 2; i <= 4; i++) {
        LoginLog_Init(&log);
        log.account_id = 10000 + i;
        snprintf(log.login_ip, sizeof(log.login_ip), "192.168.1.%d", 100 + i);
        log.server_id = i % 3 + 1;
        LoginLog_Create(manager, &log);
    }
    print_success("批量创建完成 (3个登录记录)");

    printf("\n");
    return 0;
}

// 测试查询登录日志
int test_query_login_log(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试2: 查询登录日志%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    LoginLog logs[10];
    int count;

    // 按账号ID查询
    print_info("查询账号10001的登录记录");
    count = LoginLog_GetByAccountId(manager, 10001, logs, 10);
    if (count > 0) {
        print_success("查询成功");
        printf("  找到 %d 条记录\n", count);
        printf("  最近一次: IP=%s, 服务器=%d, 时间=%s\n",
               logs[0].login_ip, logs[0].server_id, logs[0].login_time);
    } else {
        print_error("查询失败");
        return -1;
    }

    // 查询不存在的账号
    print_info("查询不存在的账号99999");
    count = LoginLog_GetByAccountId(manager, 99999, logs, 10);
    if (count == -1) {
        print_success("正确返回查询失败");
    } else if (count == 0) {
        print_success("正确返回0条记录");
    } else {
        print_warning("不存在的账号返回了记录");
    }

    // 获取记录总数
    print_info("获取登录日志总数");
    int total = LoginLog_GetCount(manager);
    if (total > 0) {
        print_success("获取成功");
        printf("  记录总数: %d\n", total);
    } else {
        print_error("获取失败");
        return -1;
    }

    // 获取某账号的登录次数
    print_info("获取账号10001的登录次数");
    int login_count = LoginLog_GetCountByAccountId(manager, 10001);
    if (login_count > 0) {
        print_success("获取成功");
        printf("  登录次数: %d\n", login_count);
    } else {
        print_error("获取失败");
        return -1;
    }

    printf("\n");
    return 0;
}

// 测试更新登出时间
int test_update_logout_time(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试3: 更新登出时间%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    LoginLog logs[10];
    int count;

    // 获取账号10001的最近登录记录
    count = LoginLog_GetByAccountId(manager, 10001, logs, 1);
    if (count <= 0) {
        print_error("无法找到测试记录");
        return -1;
    }

    long long seq = logs[0].seq;
    printf("  记录序列号: %lld\n", seq);
    printf("  登录时间: %s\n", logs[0].login_time);
    printf("  原登出时间: %s\n", logs[0].logout_time);

    // 更新登出时间
    print_info("更新登出时间");
    if (LoginLog_UpdateLogoutTime(manager, seq, "2025-11-12 12:00:00") == 0) {
        print_success("更新成功");

        // 验证更新
        LoginLog verify_log;
        if (LoginLog_GetBySeq(manager, seq, &verify_log) == 0) {
            if (strcmp(verify_log.logout_time, "2025-11-12 12:00:00") == 0) {
                print_success("验证: 登出时间更新正确");
                printf("  新登出时间: %s\n", verify_log.logout_time);
            } else {
                print_error("验证失败: 登出时间不匹配");
                return -1;
            }
        }
    } else {
        print_error("更新失败");
        return -1;
    }

    printf("\n");
    return 0;
}

// 测试按IP查询
int test_query_by_ip(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试4: 按IP查询登录日志%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    LoginLog logs[10];
    int count;

    // 按IP查询
    print_info("查询IP '192.168.1.100' 的登录记录");
    count = LoginLog_GetByIP(manager, "192.168.1.100", logs, 10);
    if (count > 0) {
        print_success("查询成功");
        printf("  找到 %d 条记录\n", count);
        printf("  账号ID: %d, 服务器ID: %d\n",
               logs[0].account_id, logs[0].server_id);
    } else {
        print_error("查询失败");
        return -1;
    }

    printf("\n");
    return 0;
}

// 测试按时间范围查询
int test_query_by_time_range(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试5: 按时间范围查询%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    LoginLog logs[20];
    int count;
    int i;

    // 按时间范围查询
    print_info("查询2025-11-01到2025-12-31的登录记录");
    count = LoginLog_GetByTimeRange(manager, "2025-11-01 00:00:00",
                                    "2025-12-31 23:59:59", logs, 20);
    if (count >= 0) {
        print_success("查询成功");
        printf("  找到 %d 条记录\n", count);

        int display = (count > 3) ? 3 : count;
        for (i = 0; i < display; i++) {
            printf("  [%d] 账号=%d, IP=%s, 时间=%s\n",
                   i + 1, logs[i].account_id, logs[i].login_ip, logs[i].login_time);
        }
    } else {
        print_error("查询失败");
        return -1;
    }

    printf("\n");
    return 0;
}

// 测试查询活跃登录
int test_active_logins(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试6: 查询活跃登录%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    LoginLog logs[20];
    int count;
    int i;

    // 查询活跃登录（未登出的）
    print_info("查询当前在线的登录记录");
    count = LoginLog_GetActiveLogins(manager, logs, 20);
    if (count >= 0) {
        print_success("查询成功");
        printf("  找到 %d 条活跃登录\n", count);

        int display = (count > 5) ? 5 : count;
        for (i = 0; i < display; i++) {
            printf("  [%d] 账号=%d, IP=%s, 服务器=%d\n",
                   i + 1, logs[i].account_id, logs[i].login_ip, logs[i].server_id);
        }
    } else {
        print_error("查询失败");
        return -1;
    }

    printf("\n");
    return 0;
}

// 测试按服务器ID查询
int test_query_by_server(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试7: 按服务器ID查询%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    LoginLog logs[20];
    int count;
    int i;

    // 按服务器ID查询
    print_info("查询服务器1的登录记录");
    count = LoginLog_GetByServerId(manager, 1, logs, 20);
    if (count >= 0) {
        print_success("查询成功");
        printf("  找到 %d 条记录\n", count);

        int display = (count > 3) ? 3 : count;
        for (i = 0; i < display; i++) {
            printf("  [%d] 账号=%d, IP=%s\n",
                   i + 1, logs[i].account_id, logs[i].login_ip);
        }
    } else {
        print_error("查询失败");
        return -1;
    }

    printf("\n");
    return 0;
}

// 测试删除日志
int test_delete_login_log(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试8: 删除登录日志%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    LoginLog logs[10];
    int count;

    // 获取账号10004的登录记录
    count = LoginLog_GetByAccountId(manager, 10004, logs, 1);
    if (count <= 0) {
        print_error("无法找到测试记录");
        return -1;
    }

    long long seq = logs[0].seq;
    printf("  准备删除序列号: %lld\n", seq);

    // 删除记录
    print_info("删除登录日志");
    if (LoginLog_Delete(manager, seq) == 0) {
        print_success("删除成功");

        // 验证删除
        if (!LoginLog_Exists(manager, seq)) {
            print_success("验证: 记录已被删除");
        } else {
            print_error("验证失败: 记录仍然存在");
            return -1;
        }
    } else {
        print_error("删除失败");
        return -1;
    }

    // 尝试删除不存在的记录
    print_info("删除不存在的记录 (seq=999999)");
    if (LoginLog_Delete(manager, 999999) == 0) {
        print_success("返回成功(没有行被删除)");
    } else {
        print_warning("返回失败");
    }

    printf("\n");
    return 0;
}

int main(int argc, char* argv[]) {
    printf("\n");
    print_separator();
    printf("%s  DNF游戏服务器 - LoginLog表ORM测试  %s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();
    printf("\n");

    if (argc < 2) {
        print_error("用法: test_login_log_orm <配置文件路径>");
        printf("  示例: ./test_login_log_orm ../config/database.cfg\n");
        return 1;
    }

    const char* config_file = argv[1];
    printf("配置文件: %s\n\n", config_file);

    // 初始化数据库连接
    DBConnectionManager manager;
    memset(&manager, 0, sizeof(DBConnectionManager));

    print_info("初始化数据库连接...");
    if (DBConnectionManager_Initialize(&manager, config_file) != 0) {
        print_error("初始化失败");
        return 1;
    }
    print_success("初始化成功");

    // 连接到登录数据库
    print_info("连接到登录数据库 (test_taiwan_login)...");
    if (DBConnectionManager_Connect(&manager, DB_TYPE_LOGIN) != 0) {
        print_error("连接失败");
        return 1;
    }
    print_success("连接成功");
    printf("\n");

    // 清理旧测试数据
    print_info("清理旧测试数据...");
    char cleanup_query[256];
    snprintf(cleanup_query, sizeof(cleanup_query),
             "DELETE FROM login_log WHERE account_id >= 10001 AND account_id <= 10004");
    DBQueryResult cleanup_result;
    memset(&cleanup_result, 0, sizeof(DBQueryResult));
    DBConnectionManager_ExecuteQuery(&manager, DB_TYPE_LOGIN, cleanup_query, &cleanup_result);
    DBQueryResult_Free(&cleanup_result);
    print_success("清理完成");
    printf("\n");

    // 运行测试
    int success_count = 0;
    int total_tests = 8;

    if (test_create_login_log(&manager) == 0) success_count++;
    if (test_query_login_log(&manager) == 0) success_count++;
    if (test_update_logout_time(&manager) == 0) success_count++;
    if (test_query_by_ip(&manager) == 0) success_count++;
    if (test_query_by_time_range(&manager) == 0) success_count++;
    if (test_active_logins(&manager) == 0) success_count++;
    if (test_query_by_server(&manager) == 0) success_count++;
    if (test_delete_login_log(&manager) == 0) success_count++;

    // 清理测试数据
    print_separator();
    print_info("清理测试数据...");
    memset(&cleanup_result, 0, sizeof(DBQueryResult));
    DBConnectionManager_ExecuteQuery(&manager, DB_TYPE_LOGIN, cleanup_query, &cleanup_result);
    DBQueryResult_Free(&cleanup_result);
    print_success("清理完成");
    printf("\n");

    // 清理连接
    DBConnectionManager_Cleanup(&manager);

    // 输出测试结果
    print_separator();
    printf("\n");
    printf("%s测试结果汇总:%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();
    printf("  通过: %s%d%s\n", COLOR_GREEN, success_count, COLOR_RESET);
    printf("  失败: %s%d%s\n", COLOR_RED, total_tests - success_count, COLOR_RESET);
    printf("  总计: %d\n", total_tests);
    printf("  成功率: %.1f%%\n", (success_count * 100.0) / total_tests);
    print_separator();
    printf("\n");

    return (success_count == total_tests) ? 0 : 1;
}
