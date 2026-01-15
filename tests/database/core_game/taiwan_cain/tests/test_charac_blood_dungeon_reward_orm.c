#include "charac_blood_dungeon_reward_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

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

#define ASSERT_STR_EQ(a, b, message) \
    do { \
        assertion_count++; \
        if (strcmp(a, b) == 0) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED: '%s' != '%s')\n", message, a, b); \
        } \
    } while(0)

#define ASSERT_GT(a, b, message) \
    do { \
        assertion_count++; \
        if ((a) > (b)) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED: %lld <= %lld)\n", message, (long long)(a), (long long)(b)); \
        } \
    } while(0)

/* 测试用例 */
void test_add_and_get(DBConnectionManager* manager) {
    TEST_START("添加和获取血色副本奖励记录");

    CharacBloodDungeonReward record, retrieved;
    memset(&record, 0, sizeof(CharacBloodDungeonReward));

    record.charac_no = 100001;
    strcpy(record.week_occ_date, "2025-01-06");
    record.week_point = 5000;
    record.week_enter_count = 10;
    record.week_use_gold = 100000;
    strcpy(record.last_play_date, "2025-01-12");
    record.enter_count = 25;
    record.rank = 5;
    record.reward = 0;
    record.reward_item_id = 50001;
    record.reward_gold = 50000;

    ASSERT_EQ(CharacBloodDungeonReward_Add(manager, &record), 0, "添加记录成功");
    ASSERT_EQ(CharacBloodDungeonReward_Get(manager, record.charac_no, record.week_occ_date, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.charac_no, record.charac_no, "角色编号匹配");
    ASSERT_STR_EQ(retrieved.week_occ_date, record.week_occ_date, "周日期匹配");
    ASSERT_EQ(retrieved.week_point, record.week_point, "周积分匹配");
    ASSERT_EQ(retrieved.reward, record.reward, "奖励状态匹配");
}

void test_update(DBConnectionManager* manager) {
    TEST_START("更新血色副本奖励记录");

    CharacBloodDungeonReward record, retrieved;

    ASSERT_EQ(CharacBloodDungeonReward_Get(manager, 100001, "2025-01-06", &record), 0, "获取已存在记录");

    record.week_point = 8000;
    record.reward = 1;  /* 标记为已领取 */
    record.rank = 3;    /* 排名上升 */

    ASSERT_EQ(CharacBloodDungeonReward_Update(manager, &record), 0, "更新记录成功");
    ASSERT_EQ(CharacBloodDungeonReward_Get(manager, record.charac_no, record.week_occ_date, &retrieved), 0, "获取更新后记录");
    ASSERT_EQ(retrieved.week_point, 8000, "周积分已更新");
    ASSERT_EQ(retrieved.reward, 1, "奖励状态已更新");
    ASSERT_EQ(retrieved.rank, 3, "排名已更新");
}

void test_exists(DBConnectionManager* manager) {
    TEST_START("检查血色副本奖励记录是否存在");

    ASSERT_EQ(CharacBloodDungeonReward_Exists(manager, 100001, "2025-01-06"), 1, "记录存在");
    ASSERT_EQ(CharacBloodDungeonReward_Exists(manager, 999999, "2025-01-06"), 0, "记录不存在");
}

void test_get_by_charac(DBConnectionManager* manager) {
    TEST_START("获取角色的所有周记录");

    /* 添加第二周记录 */
    CharacBloodDungeonReward record2;
    memset(&record2, 0, sizeof(CharacBloodDungeonReward));
    record2.charac_no = 100001;
    strcpy(record2.week_occ_date, "2025-01-13");
    record2.week_point = 6000;
    record2.week_enter_count = 8;
    record2.week_use_gold = 80000;
    strcpy(record2.last_play_date, "2025-01-19");
    record2.enter_count = 33;
    record2.rank = 2;
    record2.reward = 0;
    record2.reward_item_id = 50002;
    record2.reward_gold = 60000;

    CharacBloodDungeonReward_Add(manager, &record2);

    CharacBloodDungeonReward records[10];
    int count = CharacBloodDungeonReward_GetByCharac(manager, 100001, records, 10);

    ASSERT_EQ(count >= 2, 1, "至少获取到2条记录");
    /* 按 week_occ_date DESC 排序，最新的在前 */
    ASSERT_STR_EQ(records[0].week_occ_date, "2025-01-13", "第一条是最新周");
    ASSERT_STR_EQ(records[1].week_occ_date, "2025-01-06", "第二条是上一周");
}

void test_get_by_week_occ_date(DBConnectionManager* manager) {
    TEST_START("获取指定周的所有记录");

    /* 添加另一个角色的同一周记录 */
    CharacBloodDungeonReward record3;
    memset(&record3, 0, sizeof(CharacBloodDungeonReward));
    record3.charac_no = 100002;
    strcpy(record3.week_occ_date, "2025-01-06");
    record3.week_point = 7000;
    record3.week_enter_count = 12;
    record3.week_use_gold = 120000;
    strcpy(record3.last_play_date, "2025-01-12");
    record3.enter_count = 30;
    record3.rank = 4;
    record3.reward = 1;
    record3.reward_item_id = 50003;
    record3.reward_gold = 55000;

    CharacBloodDungeonReward_Add(manager, &record3);

    CharacBloodDungeonReward records[10];
    int count = CharacBloodDungeonReward_GetByWeekOccDate(manager, "2025-01-06", records, 10);

    ASSERT_EQ(count >= 2, 1, "该周至少有2条记录");
    /* 按 week_point DESC 排序，积分高的在前 */
    ASSERT_EQ(records[0].week_point >= records[1].week_point, 1, "按周积分降序排列");
}

void test_get_by_last_play_date(DBConnectionManager* manager) {
    TEST_START("获取指定最后游玩日期的记录");

    CharacBloodDungeonReward records[10];
    int count = CharacBloodDungeonReward_GetByLastPlayDate(manager, "2025-01-12", records, 10);

    ASSERT_EQ(count >= 2, 1, "该日期至少有2条记录");
}

void test_get_unrewarded_by_charac(DBConnectionManager* manager) {
    TEST_START("获取角色未领取奖励的记录");

    CharacBloodDungeonReward records[10];
    int count = CharacBloodDungeonReward_GetUnrewardedByCharac(manager, 100001, records, 10);

    ASSERT_EQ(count >= 1, 1, "至少有1条未领取记录");
    ASSERT_EQ(records[0].reward, 0, "奖励状态为未领取");
}

void test_get_top_by_week_point(DBConnectionManager* manager) {
    TEST_START("获取指定周的周积分排行榜");

    CharacBloodDungeonReward records[5];
    int count = CharacBloodDungeonReward_GetTopByWeekPoint(manager, "2025-01-06", 5, records, 5);

    ASSERT_EQ(count >= 2, 1, "获取到排行榜数据");
    /* 验证降序排列 */
    if (count >= 2) {
        ASSERT_EQ(records[0].week_point >= records[1].week_point, 1, "按周积分降序排列");
    }
}

void test_get_total_week_point(DBConnectionManager* manager) {
    TEST_START("获取角色总周积分");

    long long total = CharacBloodDungeonReward_GetTotalWeekPoint(manager, 100001);

    ASSERT_GT(total, 0, "总周积分大于0");
    printf("  角色100001总周积分: %lld\n", total);
}

void test_get_rewarded_count(DBConnectionManager* manager) {
    TEST_START("获取已领取奖励的记录数");

    int count = CharacBloodDungeonReward_GetRewardedCount(manager, 100001);

    ASSERT_EQ(count >= 1, 1, "至少有1条已领取记录");
}

void test_count_by_charac(DBConnectionManager* manager) {
    TEST_START("统计角色的周记录数量");

    int count = CharacBloodDungeonReward_CountByCharac(manager, 100001);

    ASSERT_EQ(count >= 2, 1, "角色至少有2周记录");
}

void test_count_by_week_occ_date(DBConnectionManager* manager) {
    TEST_START("统计指定周的记录数量");

    int count = CharacBloodDungeonReward_CountByWeekOccDate(manager, "2025-01-06");

    ASSERT_EQ(count >= 2, 1, "该周至少有2条记录");
}

void test_count_by_reward_status(DBConnectionManager* manager) {
    TEST_START("统计指定奖励状态的记录数量");

    int unrewarded = CharacBloodDungeonReward_CountByRewardStatus(manager, 0);
    int rewarded = CharacBloodDungeonReward_CountByRewardStatus(manager, 1);

    ASSERT_EQ(unrewarded >= 1, 1, "至少有1条未领取记录");
    ASSERT_EQ(rewarded >= 1, 1, "至少有1条已领取记录");
    printf("  未领取: %d, 已领取: %d\n", unrewarded, rewarded);
}

void test_delete(DBConnectionManager* manager) {
    TEST_START("删除血色副本奖励记录");

    ASSERT_EQ(CharacBloodDungeonReward_Delete(manager, 100002, "2025-01-06"), 0, "删除记录成功");
    ASSERT_EQ(CharacBloodDungeonReward_Exists(manager, 100002, "2025-01-06"), 0, "记录已删除");
}

void test_print_info(DBConnectionManager* manager) {
    TEST_START("打印血色副本奖励记录信息");

    CharacBloodDungeonReward record;
    ASSERT_EQ(CharacBloodDungeonReward_Get(manager, 100001, "2025-01-06", &record), 0, "获取记录");

    printf("\n");
    CharacBloodDungeonReward_PrintInfo(&record);

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
    printf("血色副本奖励记录 ORM 测试\n");
    printf("========================================\n");

    /* 运行测试 */
    test_add_and_get(&manager);
    test_update(&manager);
    test_exists(&manager);
    test_get_by_charac(&manager);
    test_get_by_week_occ_date(&manager);
    test_get_by_last_play_date(&manager);
    test_get_unrewarded_by_charac(&manager);
    test_get_top_by_week_point(&manager);
    test_get_total_week_point(&manager);
    test_get_rewarded_count(&manager);
    test_count_by_charac(&manager);
    test_count_by_week_occ_date(&manager);
    test_count_by_reward_status(&manager);
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
