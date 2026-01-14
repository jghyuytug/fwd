#include "charac_link_bonus_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

/* 测试统计 */
static int test_count = 0;
static int test_passed = 0;
static int assertion_count = 0;
static int assertion_passed = 0;
static int current_test_failed = 0;

/* 测试宏 */
#define TEST_START(name) \
    do { \
        if (test_count > 0 && !current_test_failed) { \
            test_passed++; \
        } \
        current_test_failed = 0; \
        printf("\n[TEST %d] %s\n", ++test_count, name); \
    } while(0)

#define ASSERT_EQ(a, b, message) \
    do { \
        assertion_count++; \
        if ((a) == (b)) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED: %d != %d)\n", message, (int)(a), (int)(b)); \
        } \
    } while(0)

#define ASSERT_TRUE(cond, message) \
    do { \
        assertion_count++; \
        if (cond) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED)\n", message); \
        } \
    } while(0)

/* 测试用例 */
void test_add_and_get(DBConnectionManager* manager) {
    TEST_START("添加和获取连接奖励");

    CharacLinkBonus record, retrieved;
    memset(&record, 0, sizeof(CharacLinkBonus));

    record.charac_no = 700001;
    record.exp = 1000;
    record.gold = 5000;
    record.mercenary_start_time = 1609459200;  /* 2021-01-01 00:00:00 */
    record.mercenary_finish_time = 1640995200;  /* 2022-01-01 00:00:00 */
    record.mercenary_area = 1;
    record.mercenary_period = 30;

    ASSERT_EQ(CharacLinkBonus_Add(manager, &record), 0, "添加记录成功");
    ASSERT_EQ(CharacLinkBonus_Get(manager, record.charac_no, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.charac_no, record.charac_no, "角色编号匹配");
    ASSERT_EQ(retrieved.exp, record.exp, "经验匹配");
    ASSERT_EQ(retrieved.gold, record.gold, "金币匹配");
    ASSERT_EQ(retrieved.mercenary_area, record.mercenary_area, "雇佣兵区域匹配");
}

void test_update(DBConnectionManager* manager) {
    TEST_START("更新连接奖励");

    CharacLinkBonus record, retrieved;
    ASSERT_EQ(CharacLinkBonus_Get(manager, 700001, &record), 0, "获取已存在记录");

    record.exp = 2000;
    record.gold = 10000;
    record.mercenary_period = 60;

    ASSERT_EQ(CharacLinkBonus_Update(manager, &record), 0, "更新记录成功");
    ASSERT_EQ(CharacLinkBonus_Get(manager, 700001, &retrieved), 0, "获取更新后记录");
    ASSERT_EQ(retrieved.exp, 2000, "经验已更新");
    ASSERT_EQ(retrieved.gold, 10000, "金币已更新");
    ASSERT_EQ(retrieved.mercenary_period, 60, "雇佣兵周期已更新");
}

void test_exists(DBConnectionManager* manager) {
    TEST_START("检查连接奖励是否存在");

    ASSERT_EQ(CharacLinkBonus_Exists(manager, 700001), 1, "记录存在");
    ASSERT_EQ(CharacLinkBonus_Exists(manager, 999999), 0, "记录不存在");
}

void test_update_exp(DBConnectionManager* manager) {
    TEST_START("更新经验奖励");

    CharacLinkBonus retrieved;

    ASSERT_EQ(CharacLinkBonus_UpdateExp(manager, 700001, 3000), 0, "更新经验成功");
    ASSERT_EQ(CharacLinkBonus_Get(manager, 700001, &retrieved), 0, "获取记录");
    ASSERT_EQ(retrieved.exp, 3000, "经验已更新为3000");
}

void test_add_exp(DBConnectionManager* manager) {
    TEST_START("增加经验奖励");

    CharacLinkBonus retrieved;

    ASSERT_EQ(CharacLinkBonus_AddExp(manager, 700001, 500), 0, "增加经验成功");
    ASSERT_EQ(CharacLinkBonus_Get(manager, 700001, &retrieved), 0, "获取记录");
    ASSERT_EQ(retrieved.exp, 3500, "经验已增加到3500");
}

void test_update_gold(DBConnectionManager* manager) {
    TEST_START("更新金币奖励");

    CharacLinkBonus retrieved;

    ASSERT_EQ(CharacLinkBonus_UpdateGold(manager, 700001, 15000), 0, "更新金币成功");
    ASSERT_EQ(CharacLinkBonus_Get(manager, 700001, &retrieved), 0, "获取记录");
    ASSERT_EQ(retrieved.gold, 15000, "金币已更新为15000");
}

void test_add_gold(DBConnectionManager* manager) {
    TEST_START("增加金币奖励");

    CharacLinkBonus retrieved;

    ASSERT_EQ(CharacLinkBonus_AddGold(manager, 700001, 2000), 0, "增加金币成功");
    ASSERT_EQ(CharacLinkBonus_Get(manager, 700001, &retrieved), 0, "获取记录");
    ASSERT_EQ(retrieved.gold, 17000, "金币已增加到17000");
}

void test_update_mercenary_info(DBConnectionManager* manager) {
    TEST_START("更新雇佣兵信息");

    CharacLinkBonus retrieved;
    int start_time = (int)time(NULL) - 86400;  /* 昨天 */
    int finish_time = (int)time(NULL) + 86400 * 30;  /* 30天后 */

    ASSERT_EQ(CharacLinkBonus_UpdateMercenaryInfo(manager, 700001, start_time, finish_time, 2, 45), 0,
              "更新雇佣兵信息成功");
    ASSERT_EQ(CharacLinkBonus_Get(manager, 700001, &retrieved), 0, "获取记录");
    ASSERT_EQ(retrieved.mercenary_start_time, start_time, "雇佣兵开始时间已更新");
    ASSERT_EQ(retrieved.mercenary_finish_time, finish_time, "雇佣兵结束时间已更新");
    ASSERT_EQ(retrieved.mercenary_area, 2, "雇佣兵区域已更新为2");
    ASSERT_EQ(retrieved.mercenary_period, 45, "雇佣兵周期已更新为45");
}

void test_is_mercenary_active(DBConnectionManager* manager) {
    TEST_START("检查雇佣兵是否有效");

    /* 上一个测试已设置为当前有效期内 */
    ASSERT_EQ(CharacLinkBonus_IsMercenaryActive(manager, 700001), 1, "雇佣兵有效");

    /* 设置为过期 */
    CharacLinkBonus_UpdateMercenaryInfo(manager, 700001, 1000000000, 1100000000, 2, 30);
    ASSERT_EQ(CharacLinkBonus_IsMercenaryActive(manager, 700001), 0, "雇佣兵已过期");
}

void test_clear_mercenary_info(DBConnectionManager* manager) {
    TEST_START("清除雇佣兵信息");

    CharacLinkBonus retrieved;

    ASSERT_EQ(CharacLinkBonus_ClearMercenaryInfo(manager, 700001), 0, "清除雇佣兵信息成功");
    ASSERT_EQ(CharacLinkBonus_Get(manager, 700001, &retrieved), 0, "获取记录");
    ASSERT_EQ(retrieved.mercenary_start_time, 0, "雇佣兵开始时间已清除");
    ASSERT_EQ(retrieved.mercenary_finish_time, 0, "雇佣兵结束时间已清除");
    ASSERT_EQ(retrieved.mercenary_area, -1, "雇佣兵区域已清除");
    ASSERT_EQ(retrieved.mercenary_period, -1, "雇佣兵周期已清除");
}

void test_total_statistics(DBConnectionManager* manager) {
    TEST_START("总计统计");

    /* 添加第二条记录用于统计 */
    CharacLinkBonus record2;
    memset(&record2, 0, sizeof(CharacLinkBonus));
    record2.charac_no = 700002;
    record2.exp = 5000;
    record2.gold = 20000;
    record2.mercenary_start_time = 0;
    record2.mercenary_finish_time = 0;
    record2.mercenary_area = -1;
    record2.mercenary_period = -1;

    CharacLinkBonus_Add(manager, &record2);

    unsigned long long total_exp = CharacLinkBonus_GetTotalExp(manager);
    unsigned long long total_gold = CharacLinkBonus_GetTotalGold(manager);

    ASSERT_TRUE(total_exp >= 8500, "总经验至少8500（3500+5000）");
    ASSERT_TRUE(total_gold >= 37000, "总金币至少37000（17000+20000）");
}

void test_active_mercenary_count(DBConnectionManager* manager) {
    TEST_START("统计当前有效雇佣兵数量");

    /* 设置700001为有效雇佣兵 */
    int start_time = (int)time(NULL) - 86400;
    int finish_time = (int)time(NULL) + 86400 * 30;
    CharacLinkBonus_UpdateMercenaryInfo(manager, 700001, start_time, finish_time, 1, 30);

    int count = CharacLinkBonus_GetActiveMercenaryCount(manager);
    ASSERT_TRUE(count >= 1, "至少有1个有效雇佣兵");
}

void test_delete(DBConnectionManager* manager) {
    TEST_START("删除连接奖励");

    ASSERT_EQ(CharacLinkBonus_Delete(manager, 700001), 0, "删除记录成功");
    ASSERT_EQ(CharacLinkBonus_Exists(manager, 700001), 0, "记录已删除");

    ASSERT_EQ(CharacLinkBonus_Delete(manager, 700002), 0, "删除记录成功");
}

void test_print_info(DBConnectionManager* manager) {
    TEST_START("打印连接奖励信息");

    CharacLinkBonus record;
    memset(&record, 0, sizeof(CharacLinkBonus));
    record.charac_no = 700003;
    record.exp = 999;
    record.gold = 888;
    record.mercenary_start_time = 1609459200;
    record.mercenary_finish_time = 1640995200;
    record.mercenary_area = 3;
    record.mercenary_period = 15;

    CharacLinkBonus_Add(manager, &record);

    printf("\n");
    CharacLinkBonus_PrintInfo(&record);

    CharacLinkBonus_Delete(manager, 700003);

    assertion_count++;
    assertion_passed++;
    printf("  ✓ 打印信息成功\n");
}

int main(int argc, char* argv[]) {
    DBConnectionManager manager;

    if (argc < 2) {
        fprintf(stderr, "用法: %s <config_file>\n", argv[0]);
        return 1;
    }

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

    printf("========================================\n");
    printf("角色连接奖励 ORM 测试\n");
    printf("========================================\n");

    /* 运行测试 */
    test_add_and_get(&manager);
    test_update(&manager);
    test_exists(&manager);
    test_update_exp(&manager);
    test_add_exp(&manager);
    test_update_gold(&manager);
    test_add_gold(&manager);
    test_update_mercenary_info(&manager);
    test_is_mercenary_active(&manager);
    test_clear_mercenary_info(&manager);
    test_total_statistics(&manager);
    test_active_mercenary_count(&manager);
    test_delete(&manager);
    test_print_info(&manager);

    /* 最后一个测试也要计入 */
    if (!current_test_failed) {
        test_passed++;
    }

    /* 清理 */
    DBConnectionManager_Cleanup(&manager);

    /* 打印测试结果 */
    printf("\n========================================\n");
    printf("测试用例: %d/%d 通过\n", test_passed, test_count);
    printf("断言: %d/%d 通过\n", assertion_passed, assertion_count);
    printf("========================================\n");

    return (test_passed == test_count && assertion_passed == assertion_count) ? 0 : 1;
}
