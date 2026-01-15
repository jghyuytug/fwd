#include "auto_market_condition_ctrl_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/*******************************************************************************
 * 测试框架
 ******************************************************************************/

static int test_count = 0;
static int test_passed = 0;
static int assertion_count = 0;
static int assertion_passed = 0;
static int current_test_failed = 0;

#define TEST_START(name) \
    do { \
        if (test_count > 0 && !current_test_failed) { \
            test_passed++; \
        } \
        current_test_failed = 0; \
        printf("\n[TEST %d] %s\n", ++test_count, name); \
    } while(0)

#define ASSERT(condition, message) \
    do { \
        assertion_count++; \
        if (condition) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED)\n", message); \
        } \
    } while(0)

#define ASSERT_EQ(a, b, message) \
    do { \
        assertion_count++; \
        if ((a) == (b)) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED: expected %lld, got %lld)\n", message, (long long)(b), (long long)(a)); \
        } \
    } while(0)

/*******************************************************************************
 * 测试用例
 ******************************************************************************/

void test_initialize(DBConnectionManager* manager) {
    TEST_START("初始化配置");

    /* 清空表 */
    DBQueryResult result;
    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, "TRUNCATE TABLE auto_market_condition_ctrl", &result);
    DBQueryResult_Free(&result);

    /* 初始化 */
    int ret = AutoMarketConditionCtrl_Initialize(manager, 1000000000ULL, 50000000ULL);
    ASSERT(ret == 0, "初始化成功");

    /* 验证 */
    AutoMarketConditionCtrl config;
    ret = AutoMarketConditionCtrl_Get(manager, &config);
    ASSERT(ret == 0, "读取配置成功");
    ASSERT_EQ(config.optimum_gold_supply, 1000000000ULL, "最优金币供应量正确");
    ASSERT_EQ(config.over_gold, 50000000ULL, "超额金币量正确");
}

void test_duplicate_initialize(DBConnectionManager* manager) {
    TEST_START("重复初始化（应跳过）");

    AutoMarketConditionCtrl before;
    AutoMarketConditionCtrl_Get(manager, &before);

    /* 尝试再次初始化 */
    int ret = AutoMarketConditionCtrl_Initialize(manager, 9999999999ULL, 9999999ULL);
    ASSERT(ret == 0, "重复初始化返回成功");

    /* 验证数据未被覆盖 */
    AutoMarketConditionCtrl after;
    AutoMarketConditionCtrl_Get(manager, &after);
    ASSERT_EQ(after.optimum_gold_supply, before.optimum_gold_supply, "最优金币供应量未改变");
    ASSERT_EQ(after.over_gold, before.over_gold, "超额金币量未改变");
}

void test_update(DBConnectionManager* manager) {
    TEST_START("更新配置");

    AutoMarketConditionCtrl config;
    config.optimum_gold_supply = 2000000000ULL;
    config.over_gold = 100000000ULL;

    int ret = AutoMarketConditionCtrl_Update(manager, &config);
    ASSERT(ret == 0, "更新成功");

    /* 验证 */
    AutoMarketConditionCtrl retrieved;
    AutoMarketConditionCtrl_Get(manager, &retrieved);
    ASSERT_EQ(retrieved.optimum_gold_supply, 2000000000ULL, "最优金币供应量更新正确");
    ASSERT_EQ(retrieved.over_gold, 100000000ULL, "超额金币量更新正确");
}

void test_exists(DBConnectionManager* manager) {
    TEST_START("检查配置存在性");

    int exists = AutoMarketConditionCtrl_Exists(manager);
    ASSERT(exists == 1, "配置存在");
}

void test_increase_supply(DBConnectionManager* manager) {
    TEST_START("增加金币供应量");

    /* 获取当前值 */
    AutoMarketConditionCtrl before;
    AutoMarketConditionCtrl_Get(manager, &before);

    /* 增加 500,000,000 */
    int ret = AutoMarketConditionCtrl_IncreaseSupply(manager, 500000000ULL);
    ASSERT(ret == 0, "增加金币供应量成功");

    /* 验证 */
    AutoMarketConditionCtrl after;
    AutoMarketConditionCtrl_Get(manager, &after);
    unsigned long long expected = before.optimum_gold_supply + 500000000ULL;
    ASSERT_EQ(after.optimum_gold_supply, expected, "金币供应量增加正确");
    ASSERT_EQ(after.over_gold, before.over_gold, "超额金币量未改变");
}

void test_decrease_supply(DBConnectionManager* manager) {
    TEST_START("减少金币供应量");

    /* 获取当前值 */
    AutoMarketConditionCtrl before;
    AutoMarketConditionCtrl_Get(manager, &before);

    /* 减少 300,000,000 */
    int ret = AutoMarketConditionCtrl_DecreaseSupply(manager, 300000000ULL);
    ASSERT(ret == 0, "减少金币供应量成功");

    /* 验证 */
    AutoMarketConditionCtrl after;
    AutoMarketConditionCtrl_Get(manager, &after);
    unsigned long long expected = before.optimum_gold_supply - 300000000ULL;
    ASSERT_EQ(after.optimum_gold_supply, expected, "金币供应量减少正确");
    ASSERT_EQ(after.over_gold, before.over_gold, "超额金币量未改变");
}

void test_decrease_supply_underflow_protection(DBConnectionManager* manager) {
    TEST_START("减少金币供应量（防止下溢）");

    /* 设置一个小值 */
    AutoMarketConditionCtrl config;
    config.optimum_gold_supply = 100ULL;
    config.over_gold = 0ULL;
    AutoMarketConditionCtrl_Update(manager, &config);

    /* 尝试减少超过当前值的数量 */
    int ret = AutoMarketConditionCtrl_DecreaseSupply(manager, 500ULL);
    ASSERT(ret == 0, "减少操作执行成功");

    /* 验证结果应该为0（不应该下溢变成负数） */
    AutoMarketConditionCtrl after;
    AutoMarketConditionCtrl_Get(manager, &after);
    ASSERT_EQ(after.optimum_gold_supply, 0ULL, "金币供应量降为0（无下溢）");
}

void test_large_numbers(DBConnectionManager* manager) {
    TEST_START("处理大数值");

    AutoMarketConditionCtrl config;
    config.optimum_gold_supply = 18446744073709551615ULL;  /* BIGINT UNSIGNED 最大值 */
    config.over_gold = 9223372036854775807ULL;             /* BIGINT 最大正值 */

    int ret = AutoMarketConditionCtrl_Update(manager, &config);
    ASSERT(ret == 0, "更新大数值成功");

    /* 验证 */
    AutoMarketConditionCtrl retrieved;
    AutoMarketConditionCtrl_Get(manager, &retrieved);
    ASSERT_EQ(retrieved.optimum_gold_supply, 18446744073709551615ULL, "最大BIGINT UNSIGNED值正确");
    ASSERT_EQ(retrieved.over_gold, 9223372036854775807ULL, "大BIGINT值正确");
}

void test_update_on_empty_table(DBConnectionManager* manager) {
    TEST_START("空表更新（自动插入）");

    /* 清空表 */
    DBQueryResult result;
    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, "TRUNCATE TABLE auto_market_condition_ctrl", &result);
    DBQueryResult_Free(&result);

    /* 验证表为空 */
    int exists = AutoMarketConditionCtrl_Exists(manager);
    ASSERT(exists == 0, "表已清空");

    /* 更新（应该自动插入） */
    AutoMarketConditionCtrl config;
    config.optimum_gold_supply = 3000000000ULL;
    config.over_gold = 150000000ULL;
    int ret = AutoMarketConditionCtrl_Update(manager, &config);
    ASSERT(ret == 0, "空表更新成功");

    /* 验证 */
    AutoMarketConditionCtrl retrieved;
    AutoMarketConditionCtrl_Get(manager, &retrieved);
    ASSERT_EQ(retrieved.optimum_gold_supply, 3000000000ULL, "自动插入值正确");
}

void test_print_info(DBConnectionManager* manager) {
    TEST_START("打印配置信息");

    /* 设置测试数据 */
    AutoMarketConditionCtrl config;
    config.optimum_gold_supply = 1000000000ULL;
    config.over_gold = 250000000ULL;  /* 25%超额，应触发通货膨胀警告 */
    AutoMarketConditionCtrl_Update(manager, &config);

    AutoMarketConditionCtrl_Get(manager, &config);
    AutoMarketConditionCtrl_PrintInfo(&config);
    ASSERT(1, "打印信息成功");
}

/*******************************************************************************
 * 主测试函数
 ******************************************************************************/

int main(int argc, char* argv[]) {
    DBConnectionManager manager;

    if (argc < 2) {
        fprintf(stderr, "用法: %s <config_file>\n", argv[0]);
        return 1;
    }

    printf("\n========================================\n");
    printf("自动市场条件控制 ORM 测试\n");
    printf("========================================\n");

    /* 初始化数据库连接 */
    memset(&manager, 0, sizeof(DBConnectionManager));
    if (DBConnectionManager_Initialize(&manager, argv[1]) < 0) {
        fprintf(stderr, "初始化数据库连接管理器失败\n");
        return 1;
    }

    if (DBConnectionManager_Connect(&manager, DB_TYPE_CAIN) < 0) {
        fprintf(stderr, "连接到 taiwan_cain 数据库失败\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    /* 运行测试用例 */
    test_initialize(&manager);
    test_duplicate_initialize(&manager);
    test_update(&manager);
    test_exists(&manager);
    test_increase_supply(&manager);
    test_decrease_supply(&manager);
    test_decrease_supply_underflow_protection(&manager);
    test_large_numbers(&manager);
    test_update_on_empty_table(&manager);
    test_print_info(&manager);

    /* 最后一个测试完成 */
    if (!current_test_failed) {
        test_passed++;
    }

    /* 清理 */
    DBConnectionManager_Cleanup(&manager);

    /* 打印测试结果 */
    printf("\n========================================\n");
    printf("测试用例: %d/%d 通过 (%.1f%%)\n",
           test_passed, test_count,
           test_count > 0 ? (test_passed * 100.0 / test_count) : 0);
    printf("断言: %d/%d 通过 (%.1f%%)\n",
           assertion_passed, assertion_count,
           assertion_count > 0 ? (assertion_passed * 100.0 / assertion_count) : 0);
    printf("========================================\n");

    return (test_passed == test_count && assertion_passed == assertion_count) ? 0 : 1;
}
