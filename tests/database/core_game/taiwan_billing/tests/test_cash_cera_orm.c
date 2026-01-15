/**
 * DNF游戏服务器 - CashCera ORM 测试程序
 *
 * 功能:
 * 1. 测试点券CRUD操作
 * 2. 测试点券增减操作
 * 3. 测试点券查询功能
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db_connection_manager.h"
#include "cash_cera_orm.h"

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

// 测试创建点券记录
int test_create_cash(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试1: 创建点券记录%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    CashCera cash;
    int i;

    // 创建记录1 - 新账号
    print_info("创建记录1 - 账号ID=10001");
    CashCera_Init(&cash);
    cash.account_id = 10001;
    cash.cera_point = 1000;

    if (CashCera_Create(manager, &cash) == 0) {
        print_success("记录创建成功");
        printf("  账号ID: %d, 点券: %lld\n", cash.account_id, cash.cera_point);
    } else {
        print_error("记录创建失败");
        return -1;
    }

    // 创建记录2 - 富豪账号
    print_info("创建记录2 - 账号ID=10002");
    CashCera_Init(&cash);
    cash.account_id = 10002;
    cash.cera_point = 50000;

    if (CashCera_Create(manager, &cash) == 0) {
        print_success("记录创建成功");
        printf("  账号ID: %d, 点券: %lld\n", cash.account_id, cash.cera_point);
    } else {
        print_error("记录创建失败");
        return -1;
    }

    // 批量创建测试账号
    print_info("批量创建测试账号 (10003-10005)");
    for (i = 3; i <= 5; i++) {
        CashCera_Init(&cash);
        cash.account_id = 10000 + i;
        cash.cera_point = i * 1000;
        CashCera_Create(manager, &cash);
    }
    print_success("批量创建完成 (3个账号)");

    printf("\n");
    return 0;
}

// 测试查询点券
int test_query_cash(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试2: 查询点券信息%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    CashCera cash;

    // 按账号ID查询
    print_info("按账号ID查询: 10001");
    if (CashCera_GetByAccountId(manager, 10001, &cash) == 0) {
        print_success("查询成功");
        printf("  账号ID: %d\n", cash.account_id);
        printf("  点券: %lld\n", cash.cera_point);
        printf("  更新时间: %s\n", cash.update_time);
    } else {
        print_error("查询失败");
        return -1;
    }

    // 查询不存在的账号
    print_info("查询不存在的账号ID: 99999");
    if (CashCera_GetByAccountId(manager, 99999, &cash) == -1) {
        print_success("正确返回查询失败");
    } else {
        print_warning("不存在的账号返回了结果");
    }

    // 获取点券数量
    print_info("获取账号10002的点券数量");
    long long cera_amount = CashCera_GetCeraAmount(manager, 10002);
    if (cera_amount > 0) {
        print_success("获取成功");
        printf("  点券数量: %lld\n", cera_amount);
    } else {
        print_error("获取失败");
        return -1;
    }

    printf("\n");
    return 0;
}

// 测试更新点券
int test_update_cash(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试3: 更新点券信息%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    CashCera cash;

    // 查询当前信息
    if (CashCera_GetByAccountId(manager, 10001, &cash) != 0) {
        print_error("无法找到测试账号");
        return -1;
    }

    printf("  原始点券: %lld\n", cash.cera_point);

    // 直接更新点券数量
    print_info("更新点券数量为2000");
    if (CashCera_UpdateCera(manager, 10001, 2000) == 0) {
        print_success("更新成功");

        // 验证更新
        if (CashCera_GetByAccountId(manager, 10001, &cash) == 0) {
            if (cash.cera_point == 2000) {
                print_success("验证: 点券更新正确");
                printf("  新点券数量: %lld\n", cash.cera_point);
            } else {
                print_error("验证失败: 点券不匹配");
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

// 测试点券增减操作
int test_cera_operations(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试4: 点券增减操作%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    CashCera cash;

    // 获取初始余额
    if (CashCera_GetByAccountId(manager, 10002, &cash) != 0) {
        print_error("无法找到测试账号");
        return -1;
    }

    long long initial_cera = cash.cera_point;
    printf("  初始余额: %lld\n", initial_cera);

    // 增加点券
    print_info("增加5000点券");
    if (CashCera_AddCera(manager, 10002, 5000) == 0) {
        print_success("增加成功");

        // 验证
        long long current = CashCera_GetCeraAmount(manager, 10002);
        if (current == initial_cera + 5000) {
            print_success("验证: 点券增加正确");
            printf("  当前余额: %lld\n", current);
        } else {
            print_error("验证失败: 余额不匹配");
            return -1;
        }
    } else {
        print_error("增加失败");
        return -1;
    }

    // 扣除点券
    print_info("扣除3000点券");
    if (CashCera_SubtractCera(manager, 10002, 3000) == 0) {
        print_success("扣除成功");

        // 验证
        long long current = CashCera_GetCeraAmount(manager, 10002);
        if (current == initial_cera + 5000 - 3000) {
            print_success("验证: 点券扣除正确");
            printf("  当前余额: %lld\n", current);
        } else {
            print_error("验证失败: 余额不匹配");
            return -1;
        }
    } else {
        print_error("扣除失败");
        return -1;
    }

    // 测试余额不足
    print_info("尝试扣除超额点券（应该失败）");
    int ret = CashCera_SubtractCera(manager, 10002, 999999);
    if (ret == -2) {
        print_success("正确检测到余额不足");
    } else if (ret == 0) {
        print_error("错误: 允许了透支");
        return -1;
    } else {
        print_warning("返回了其他错误");
    }

    printf("\n");
    return 0;
}

// 测试存在性检查
int test_existence_check(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试5: 存在性检查%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    // 测试存在的账号
    print_info("检查账号10001是否存在");
    if (CashCera_Exists(manager, 10001)) {
        print_success("已存在的账号返回true");
    } else {
        print_error("已存在的账号返回false");
        return -1;
    }

    // 测试不存在的账号
    print_info("检查不存在的账号 (ID=99999)");
    if (!CashCera_Exists(manager, 99999)) {
        print_success("不存在的账号返回false");
    } else {
        print_error("不存在的账号返回true");
        return -1;
    }

    // 获取记录总数
    print_info("获取点券记录总数");
    int total = CashCera_GetCount(manager);
    if (total > 0) {
        print_success("获取成功");
        printf("  记录总数: %d\n", total);
    } else {
        print_error("获取失败");
        return -1;
    }

    printf("\n");
    return 0;
}

// 测试批量查询
int test_batch_query(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试6: 批量查询%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    CashCera records[100];
    int count;
    int i;

    // 获取所有记录
    print_info("获取所有点券记录");
    count = CashCera_GetAll(manager, records, 100);
    if (count > 0) {
        print_success("查询成功");
        printf("  获取到 %d 条记录\n", count);

        // 显示前3条
        int display = (count > 3) ? 3 : count;
        for (i = 0; i < display; i++) {
            printf("  [%d] 账号ID=%d, 点券=%lld\n",
                   i + 1, records[i].account_id, records[i].cera_point);
        }
    } else {
        print_error("查询失败或无数据");
        return -1;
    }

    printf("\n");
    return 0;
}

// 测试按范围查询
int test_range_query(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试7: 按范围查询%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    CashCera records[50];
    int count;
    int i;

    // 查询点券在2000-10000之间的记录
    print_info("查询点券2000-10000的记录");
    count = CashCera_GetByRange(manager, 2000, 10000, records, 50);
    if (count >= 0) {
        print_success("查询成功");
        printf("  找到 %d 条记录\n", count);

        for (i = 0; i < count && i < 5; i++) {
            printf("  [%d] 账号ID=%d, 点券=%lld\n",
                   i + 1, records[i].account_id, records[i].cera_point);
        }
    } else {
        print_error("查询失败");
        return -1;
    }

    printf("\n");
    return 0;
}

// 测试删除记录
int test_delete_cash(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试8: 删除点券记录%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    // 确认记录存在
    if (!CashCera_Exists(manager, 10003)) {
        print_error("测试账号不存在");
        return -1;
    }

    printf("  准备删除: 账号ID=10003\n");

    // 删除记录
    print_info("删除点券记录");
    if (CashCera_Delete(manager, 10003) == 0) {
        print_success("删除成功");

        // 验证删除
        if (!CashCera_Exists(manager, 10003)) {
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
    print_info("删除不存在的记录 (ID=99999)");
    if (CashCera_Delete(manager, 99999) == 0) {
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
    printf("%s  DNF游戏服务器 - CashCera表ORM测试  %s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();
    printf("\n");

    if (argc < 2) {
        print_error("用法: test_cash_cera_orm <配置文件路径>");
        printf("  示例: ./test_cash_cera_orm ../config/database.cfg\n");
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

    // 连接到计费数据库 (cash_cera表在test_taiwan_billing)
    print_info("连接到计费数据库 (test_taiwan_billing)...");
    if (DBConnectionManager_Connect(&manager, DB_TYPE_BILLING) != 0) {
        print_error("连接失败");
        return 1;
    }
    print_success("连接成功");
    printf("\n");

    // 清理旧测试数据
    print_info("清理旧测试数据...");
    char cleanup_query[256];
    snprintf(cleanup_query, sizeof(cleanup_query),
             "DELETE FROM cash_cera WHERE account_id >= 10001 AND account_id <= 10005");
    DBQueryResult cleanup_result;
    memset(&cleanup_result, 0, sizeof(DBQueryResult));
    DBConnectionManager_ExecuteQuery(&manager, DB_TYPE_BILLING, cleanup_query, &cleanup_result);
    DBQueryResult_Free(&cleanup_result);
    print_success("清理完成");
    printf("\n");

    // 运行测试
    int success_count = 0;
    int total_tests = 8;

    if (test_create_cash(&manager) == 0) success_count++;
    if (test_query_cash(&manager) == 0) success_count++;
    if (test_update_cash(&manager) == 0) success_count++;
    if (test_cera_operations(&manager) == 0) success_count++;
    if (test_existence_check(&manager) == 0) success_count++;
    if (test_batch_query(&manager) == 0) success_count++;
    if (test_range_query(&manager) == 0) success_count++;
    if (test_delete_cash(&manager) == 0) success_count++;

    // 清理测试数据
    print_separator();
    print_info("清理测试数据...");
    memset(&cleanup_result, 0, sizeof(DBQueryResult));
    DBConnectionManager_ExecuteQuery(&manager, DB_TYPE_BILLING, cleanup_query, &cleanup_result);
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
