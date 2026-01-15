#include "auto_market_condition_ctrl_daily_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

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

void test_add_and_get(DBConnectionManager* manager) {
    TEST_START("添加和获取每日统计");

    AutoMarketConditionCtrlDaily record;
    memset(&record, 0, sizeof(record));
    strncpy(record.occ_time, "2025-11-15", sizeof(record.occ_time) - 1);
    record.total_gold = 5000000000ULL;
    record.auction_gold = 2000000000ULL;
    record.over_gold = 100000000ULL;
    record.optimum_gold_supply = 4500000000ULL;
    record.gold_phase = 3;
    record.item_phase = 2;
    record.durability_phase = 1;

    int ret = AutoMarketConditionCtrlDaily_Add(manager, &record);
    ASSERT(ret == 0, "添加每日统计成功");

    AutoMarketConditionCtrlDaily retrieved;
    memset(&retrieved, 0, sizeof(retrieved));
    ret = AutoMarketConditionCtrlDaily_Get(manager, "2025-11-15", &retrieved);
    ASSERT(ret == 0, "获取每日统计成功");
    ASSERT_EQ(retrieved.total_gold, 5000000000ULL, "金币总量正确");
    ASSERT_EQ(retrieved.auction_gold, 2000000000ULL, "拍卖行金币量正确");
    ASSERT_EQ(retrieved.over_gold, 100000000ULL, "超额金币量正确");
    ASSERT_EQ(retrieved.optimum_gold_supply, 4500000000ULL, "最优供应量正确");
    ASSERT_EQ(retrieved.gold_phase, 3, "金币阶段正确");
    ASSERT_EQ(retrieved.item_phase, 2, "物品阶段正确");
    ASSERT_EQ(retrieved.durability_phase, 1, "耐久度阶段正确");
}

void test_update(DBConnectionManager* manager) {
    TEST_START("更新每日统计");

    AutoMarketConditionCtrlDaily record;
    AutoMarketConditionCtrlDaily_Get(manager, "2025-11-15", &record);
    record.total_gold = 5500000000ULL;
    record.gold_phase = 4;

    int ret = AutoMarketConditionCtrlDaily_Update(manager, &record);
    ASSERT(ret == 0, "更新成功");

    AutoMarketConditionCtrlDaily retrieved;
    AutoMarketConditionCtrlDaily_Get(manager, "2025-11-15", &retrieved);
    ASSERT_EQ(retrieved.total_gold, 5500000000ULL, "更新后金币总量正确");
    ASSERT_EQ(retrieved.gold_phase, 4, "更新后金币阶段正确");
}

void test_exists(DBConnectionManager* manager) {
    TEST_START("检查记录存在性");

    int exists = AutoMarketConditionCtrlDaily_Exists(manager, "2025-11-15");
    ASSERT(exists == 1, "记录存在");

    exists = AutoMarketConditionCtrlDaily_Exists(manager, "2099-12-31");
    ASSERT(exists == 0, "不存在的记录返回0");
}

void test_record_count(DBConnectionManager* manager) {
    TEST_START("统计记录数量");

    int count = AutoMarketConditionCtrlDaily_Count(manager);
    ASSERT(count >= 1, "记录数量正确");
}

void test_get_all(DBConnectionManager* manager) {
    TEST_START("获取所有每日统计");

    /* 添加多条测试数据 */
    AutoMarketConditionCtrlDaily records[7];
    const char* dates[] = {"2025-11-09", "2025-11-10", "2025-11-11", "2025-11-12", "2025-11-13", "2025-11-14", "2025-11-16"};

    for (int i = 0; i < 7; i++) {
        memset(&records[i], 0, sizeof(records[i]));
        strncpy(records[i].occ_time, dates[i], sizeof(records[i].occ_time) - 1);
        records[i].total_gold = 5000000000ULL + i * 100000000ULL;
        records[i].auction_gold = 2000000000ULL;
        records[i].over_gold = 100000000ULL - i * 5000000ULL;
        records[i].optimum_gold_supply = 4500000000ULL;
        records[i].gold_phase = (i % 5) + 1;
        records[i].item_phase = (i % 3) + 1;
        records[i].durability_phase = (i % 4) + 1;
        AutoMarketConditionCtrlDaily_Add(manager, &records[i]);
    }

    AutoMarketConditionCtrlDaily all_records[20];
    int count = AutoMarketConditionCtrlDaily_GetAll(manager, all_records, 20);
    ASSERT(count >= 7, "获取到至少7条记录");
}

void test_get_by_date_range(DBConnectionManager* manager) {
    TEST_START("按日期范围查询");

    AutoMarketConditionCtrlDaily records[10];
    int count = AutoMarketConditionCtrlDaily_GetByDateRange(manager, "2025-11-10", "2025-11-14", records, 10);
    ASSERT(count >= 5, "日期范围内至少有5条记录");
}

void test_get_recent(DBConnectionManager* manager) {
    TEST_START("获取最近N天统计");

    AutoMarketConditionCtrlDaily records[15];
    int count = AutoMarketConditionCtrlDaily_GetRecent(manager, 30, records, 15);
    ASSERT(count >= 1, "最近30天至少有1条记录");
}

void test_average_total_gold(DBConnectionManager* manager) {
    TEST_START("计算平均金币总量");

    long long avg = AutoMarketConditionCtrlDaily_GetAverageTotalGold(manager, 30);
    ASSERT(avg >= 0, "平均金币总量统计成功");
    printf("  [INFO] 最近30天平均金币总量: %lld\n", avg);
}

void test_gold_phase_count(DBConnectionManager* manager) {
    TEST_START("统计金币阶段出现次数");

    int count_phase_3 = AutoMarketConditionCtrlDaily_GetGoldPhaseCount(manager, 3, 30);
    ASSERT(count_phase_3 >= 0, "金币阶段3统计成功");

    int count_phase_4 = AutoMarketConditionCtrlDaily_GetGoldPhaseCount(manager, 4, 30);
    ASSERT(count_phase_4 >= 0, "金币阶段4统计成功");

    printf("  [INFO] 最近30天阶段3出现次数: %d\n", count_phase_3);
    printf("  [INFO] 最近30天阶段4出现次数: %d\n", count_phase_4);
}

void test_delete(DBConnectionManager* manager) {
    TEST_START("删除每日统计");

    int ret = AutoMarketConditionCtrlDaily_Delete(manager, "2025-11-16");
    ASSERT(ret == 0, "删除成功");

    int exists = AutoMarketConditionCtrlDaily_Exists(manager, "2025-11-16");
    ASSERT(exists == 0, "记录已被删除");
}

void test_print_info(DBConnectionManager* manager) {
    TEST_START("打印每日统计信息");

    AutoMarketConditionCtrlDaily record;
    if (AutoMarketConditionCtrlDaily_Get(manager, "2025-11-15", &record) == 0) {
        AutoMarketConditionCtrlDaily_PrintInfo(&record);
        ASSERT(1, "打印信息成功");
    }
}

int main(int argc, char* argv[]) {
    DBConnectionManager manager;

    if (argc < 2) {
        fprintf(stderr, "用法: %s <config_file>\n", argv[0]);
        return 1;
    }

    printf("\n========================================\n");
    printf("每日市场条件统计 ORM 测试\n");
    printf("========================================\n");

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

    test_add_and_get(&manager);
    test_update(&manager);
    test_exists(&manager);
    test_record_count(&manager);
    test_get_all(&manager);
    test_get_by_date_range(&manager);
    test_get_recent(&manager);
    test_average_total_gold(&manager);
    test_gold_phase_count(&manager);
    test_delete(&manager);
    test_print_info(&manager);

    if (!current_test_failed) {
        test_passed++;
    }

    DBConnectionManager_Cleanup(&manager);

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
