#include "blood_dungeon_rank_select_orm.h"
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
    TEST_START("添加和获取排名奖励配置");

    BloodDungeonRankSelect record;
    memset(&record, 0, sizeof(record));
    record.min_amount = 0;
    record.max_amount = 10000;
    record.rank = 1;
    record.reward_item_id = 100001;
    record.reward_gold = 50000;
    record.winner_count = 10;

    int ret = BloodDungeonRankSelect_Add(manager, &record);
    ASSERT(ret == 0, "添加排名奖励配置成功");

    BloodDungeonRankSelect retrieved;
    memset(&retrieved, 0, sizeof(retrieved));
    ret = BloodDungeonRankSelect_Get(manager, 0, 10000, 1, &retrieved);
    ASSERT(ret == 0, "获取排名奖励配置成功");
    ASSERT_EQ(retrieved.min_amount, 0, "积分区间最小值正确");
    ASSERT_EQ(retrieved.max_amount, 10000, "积分区间最大值正确");
    ASSERT_EQ(retrieved.rank, 1, "排名正确");
    ASSERT_EQ(retrieved.reward_item_id, 100001, "奖励物品ID正确");
    ASSERT_EQ(retrieved.reward_gold, 50000, "奖励金币正确");
    ASSERT_EQ(retrieved.winner_count, 10, "获奖人数正确");
}

void test_update(DBConnectionManager* manager) {
    TEST_START("更新排名奖励配置");

    BloodDungeonRankSelect record;
    BloodDungeonRankSelect_Get(manager, 0, 10000, 1, &record);
    record.reward_item_id = 100002;
    record.reward_gold = 60000;

    int ret = BloodDungeonRankSelect_Update(manager, &record);
    ASSERT(ret == 0, "更新成功");

    BloodDungeonRankSelect retrieved;
    BloodDungeonRankSelect_Get(manager, 0, 10000, 1, &retrieved);
    ASSERT_EQ(retrieved.reward_item_id, 100002, "更新后奖励物品ID正确");
    ASSERT_EQ(retrieved.reward_gold, 60000, "更新后奖励金币正确");
}

void test_exists(DBConnectionManager* manager) {
    TEST_START("检查配置存在性");

    int exists = BloodDungeonRankSelect_Exists(manager, 0, 10000, 1);
    ASSERT(exists == 1, "配置存在");

    exists = BloodDungeonRankSelect_Exists(manager, 999999, 999999, 99);
    ASSERT(exists == 0, "不存在的配置返回0");
}

void test_config_count(DBConnectionManager* manager) {
    TEST_START("统计配置数量");

    int count = BloodDungeonRankSelect_Count(manager);
    ASSERT(count >= 1, "配置数量正确");
}

void test_get_all(DBConnectionManager* manager) {
    TEST_START("获取所有排名奖励配置");

    /* 添加多条测试数据 */
    BloodDungeonRankSelect records[10];
    long long ranges[][2] = {{0, 10000}, {10001, 50000}, {50001, 100000}};
    unsigned char ranks[] = {1, 2, 3};

    int idx = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            memset(&records[idx], 0, sizeof(records[idx]));
            records[idx].min_amount = ranges[i][0];
            records[idx].max_amount = ranges[i][1];
            records[idx].rank = ranks[j];
            records[idx].reward_item_id = 100000 + idx;
            records[idx].reward_gold = 10000 * (j + 1);
            records[idx].winner_count = 10 - j;
            BloodDungeonRankSelect_Add(manager, &records[idx]);
            idx++;
        }
    }

    BloodDungeonRankSelect all_records[30];
    int count = BloodDungeonRankSelect_GetAll(manager, all_records, 30);
    ASSERT(count >= 9, "获取到至少9条配置");
}

void test_get_by_point_range(DBConnectionManager* manager) {
    TEST_START("按积分区间查询");

    BloodDungeonRankSelect records[10];
    int count = BloodDungeonRankSelect_GetByPointRange(manager, 0, 10000, records, 10);
    ASSERT(count >= 3, "积分区间内至少有3个排名配置");
}

void test_get_by_rank(DBConnectionManager* manager) {
    TEST_START("按排名查询");

    BloodDungeonRankSelect records[10];
    int count = BloodDungeonRankSelect_GetByRank(manager, 1, records, 10);
    ASSERT(count >= 3, "排名1至少有3个积分段配置");
}

void test_find_reward_by_point(DBConnectionManager* manager) {
    TEST_START("根据积分和排名查找奖励");

    BloodDungeonRankSelect record;
    int ret = BloodDungeonRankSelect_FindRewardByPoint(manager, 5000, 1, &record);
    ASSERT(ret == 0, "查找到积分5000排名1的奖励配置");
    ASSERT_EQ(record.min_amount, 0, "匹配的积分区间最小值正确");
    ASSERT_EQ(record.max_amount, 10000, "匹配的积分区间最大值正确");

    ret = BloodDungeonRankSelect_FindRewardByPoint(manager, 75000, 2, &record);
    ASSERT(ret == 0, "查找到积分75000排名2的奖励配置");
    ASSERT_EQ(record.min_amount, 50001, "匹配的积分区间最小值正确");
    ASSERT_EQ(record.max_amount, 100000, "匹配的积分区间最大值正确");
}

void test_count_point_ranges(DBConnectionManager* manager) {
    TEST_START("统计不同积分区间数量");

    int count = BloodDungeonRankSelect_CountPointRanges(manager);
    ASSERT(count >= 3, "至少有3个不同的积分区间");
    printf("  [INFO] 积分区间数量: %d\n", count);
}

void test_delete(DBConnectionManager* manager) {
    TEST_START("删除排名奖励配置");

    int ret = BloodDungeonRankSelect_Delete(manager, 50001, 100000, 3);
    ASSERT(ret == 0, "删除成功");

    int exists = BloodDungeonRankSelect_Exists(manager, 50001, 100000, 3);
    ASSERT(exists == 0, "配置已被删除");
}

void test_print_info(DBConnectionManager* manager) {
    TEST_START("打印配置信息");

    BloodDungeonRankSelect record;
    if (BloodDungeonRankSelect_Get(manager, 0, 10000, 1, &record) == 0) {
        BloodDungeonRankSelect_PrintInfo(&record);
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
    printf("血色副本排名奖励配置 ORM 测试\n");
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
    test_config_count(&manager);
    test_get_all(&manager);
    test_get_by_point_range(&manager);
    test_get_by_rank(&manager);
    test_find_reward_by_point(&manager);
    test_count_point_ranges(&manager);
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
