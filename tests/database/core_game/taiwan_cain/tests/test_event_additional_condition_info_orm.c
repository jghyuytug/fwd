#include "event_additional_condition_info_orm.h"
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

#define ASSERT_TRUE(expr, message) \
    do { \
        assertion_count++; \
        if (expr) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED)\n", message); \
        } \
    } while(0)

#define ASSERT_GTE(a, b, message) \
    do { \
        assertion_count++; \
        if ((a) >= (b)) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED: %d < %d)\n", message, (int)(a), (int)(b)); \
        } \
    } while(0)

/* 测试用例 */

void test_add_and_get(DBConnectionManager* manager) {
    TEST_START("添加和获取活动额外条件信息");

    EventAdditionalConditionInfo record;
    memset(&record, 0, sizeof(EventAdditionalConditionInfo));
    record.charac_no = 10001;
    record.current_step = 5;
    record.reward_step = 3;
    strcpy(record.update_time, "2024-01-01 10:00:00");

    ASSERT_EQ(EventAdditionalConditionInfo_Add(manager, &record), 0, "添加记录成功");

    EventAdditionalConditionInfo retrieved;
    memset(&retrieved, 0, sizeof(EventAdditionalConditionInfo));
    ASSERT_EQ(EventAdditionalConditionInfo_Get(manager, 10001, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.charac_no, 10001, "角色编号匹配");
    ASSERT_EQ(retrieved.current_step, 5, "当前步骤匹配");
    ASSERT_EQ(retrieved.reward_step, 3, "奖励步骤匹配");
}

void test_update(DBConnectionManager* manager) {
    TEST_START("更新活动额外条件信息");

    EventAdditionalConditionInfo record;
    memset(&record, 0, sizeof(EventAdditionalConditionInfo));
    ASSERT_EQ(EventAdditionalConditionInfo_Get(manager, 10001, &record), 0, "获取已存在记录");

    record.current_step = 10;
    record.reward_step = 5;
    strcpy(record.update_time, "2024-01-01 11:00:00");

    ASSERT_EQ(EventAdditionalConditionInfo_Update(manager, &record), 0, "更新记录成功");

    EventAdditionalConditionInfo retrieved;
    memset(&retrieved, 0, sizeof(EventAdditionalConditionInfo));
    ASSERT_EQ(EventAdditionalConditionInfo_Get(manager, 10001, &retrieved), 0, "获取更新后记录");
    ASSERT_EQ(retrieved.current_step, 10, "当前步骤已更新");
    ASSERT_EQ(retrieved.reward_step, 5, "奖励步骤已更新");
}

void test_exists(DBConnectionManager* manager) {
    TEST_START("检查记录是否存在");

    ASSERT_TRUE(EventAdditionalConditionInfo_Exists(manager, 10001), "记录存在");
    ASSERT_TRUE(!EventAdditionalConditionInfo_Exists(manager, 99999), "记录不存在");
}

void test_add_multiple_records(DBConnectionManager* manager) {
    TEST_START("添加多条活动额外条件信息");

    EventAdditionalConditionInfo record2;
    memset(&record2, 0, sizeof(EventAdditionalConditionInfo));
    record2.charac_no = 10002;
    record2.current_step = 15;
    record2.reward_step = 15;  /* 奖励已全部领取 */
    strcpy(record2.update_time, "2024-01-01 12:00:00");
    ASSERT_EQ(EventAdditionalConditionInfo_Add(manager, &record2), 0, "添加记录2成功");

    EventAdditionalConditionInfo record3;
    memset(&record3, 0, sizeof(EventAdditionalConditionInfo));
    record3.charac_no = 10003;
    record3.current_step = 20;
    record3.reward_step = 10;  /* 有未领取奖励 */
    strcpy(record3.update_time, "2024-01-01 13:00:00");
    ASSERT_EQ(EventAdditionalConditionInfo_Add(manager, &record3), 0, "添加记录3成功");

    EventAdditionalConditionInfo record4;
    memset(&record4, 0, sizeof(EventAdditionalConditionInfo));
    record4.charac_no = 10004;
    record4.current_step = 8;
    record4.reward_step = 5;
    strcpy(record4.update_time, "2024-01-01 14:00:00");
    ASSERT_EQ(EventAdditionalConditionInfo_Add(manager, &record4), 0, "添加记录4成功");
}

void test_increment_current_step(DBConnectionManager* manager) {
    TEST_START("增加当前步骤");

    ASSERT_EQ(EventAdditionalConditionInfo_IncrementCurrentStep(manager, 10001, 5), 0, "增加当前步骤成功");

    EventAdditionalConditionInfo retrieved;
    memset(&retrieved, 0, sizeof(EventAdditionalConditionInfo));
    ASSERT_EQ(EventAdditionalConditionInfo_Get(manager, 10001, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.current_step, 15, "当前步骤已增加（10+5=15）");
}

void test_update_current_step(DBConnectionManager* manager) {
    TEST_START("更新当前步骤");

    ASSERT_EQ(EventAdditionalConditionInfo_UpdateCurrentStep(manager, 10001, 25), 0, "更新当前步骤成功");

    EventAdditionalConditionInfo retrieved;
    memset(&retrieved, 0, sizeof(EventAdditionalConditionInfo));
    ASSERT_EQ(EventAdditionalConditionInfo_Get(manager, 10001, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.current_step, 25, "当前步骤已更新为25");
}

void test_update_reward_step(DBConnectionManager* manager) {
    TEST_START("更新奖励步骤");

    ASSERT_EQ(EventAdditionalConditionInfo_UpdateRewardStep(manager, 10001, 15), 0, "更新奖励步骤成功");

    EventAdditionalConditionInfo retrieved;
    memset(&retrieved, 0, sizeof(EventAdditionalConditionInfo));
    ASSERT_EQ(EventAdditionalConditionInfo_Get(manager, 10001, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.reward_step, 15, "奖励步骤已更新为15");
}

void test_claim_reward(DBConnectionManager* manager) {
    TEST_START("领取奖励");

    ASSERT_EQ(EventAdditionalConditionInfo_ClaimReward(manager, 10001), 0, "领取奖励成功");

    EventAdditionalConditionInfo retrieved;
    memset(&retrieved, 0, sizeof(EventAdditionalConditionInfo));
    ASSERT_EQ(EventAdditionalConditionInfo_Get(manager, 10001, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.reward_step, 25, "奖励步骤已更新为当前步骤（25）");
}

void test_reset_progress(DBConnectionManager* manager) {
    TEST_START("重置进度");

    ASSERT_EQ(EventAdditionalConditionInfo_ResetProgress(manager, 10002), 0, "重置进度成功");

    EventAdditionalConditionInfo retrieved;
    memset(&retrieved, 0, sizeof(EventAdditionalConditionInfo));
    ASSERT_EQ(EventAdditionalConditionInfo_Get(manager, 10002, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.current_step, 0, "当前步骤已重置为0");
    ASSERT_EQ(retrieved.reward_step, 0, "奖励步骤已重置为0");
}

void test_batch_reset_progress(DBConnectionManager* manager) {
    TEST_START("批量重置所有角色进度");

    ASSERT_EQ(EventAdditionalConditionInfo_BatchResetProgress(manager), 0, "批量重置成功");

    EventAdditionalConditionInfo retrieved;
    memset(&retrieved, 0, sizeof(EventAdditionalConditionInfo));
    ASSERT_EQ(EventAdditionalConditionInfo_Get(manager, 10001, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.current_step, 0, "记录1当前步骤已重置为0");
    ASSERT_EQ(retrieved.reward_step, 0, "记录1奖励步骤已重置为0");

    memset(&retrieved, 0, sizeof(EventAdditionalConditionInfo));
    ASSERT_EQ(EventAdditionalConditionInfo_Get(manager, 10003, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.current_step, 0, "记录3当前步骤已重置为0");
    ASSERT_EQ(retrieved.reward_step, 0, "记录3奖励步骤已重置为0");
}

void test_get_by_current_step(DBConnectionManager* manager) {
    TEST_START("按当前步骤精确查询");

    /* 先设置一些不同的步骤 */
    EventAdditionalConditionInfo_UpdateCurrentStep(manager, 10001, 50);
    EventAdditionalConditionInfo_UpdateCurrentStep(manager, 10002, 50);
    EventAdditionalConditionInfo_UpdateCurrentStep(manager, 10003, 30);

    EventAdditionalConditionInfo records[10];
    memset(records, 0, sizeof(records));
    int count = EventAdditionalConditionInfo_GetByCurrentStep(manager, 50, records, 10);

    ASSERT_GTE(count, 2, "至少找到2条当前步骤为50的记录");
    if (count > 0) {
        ASSERT_EQ(records[0].current_step, 50, "第一条记录当前步骤为50");
    }
}

void test_get_by_min_current_step(DBConnectionManager* manager) {
    TEST_START("按最小当前步骤查询");

    EventAdditionalConditionInfo records[10];
    memset(records, 0, sizeof(records));
    int count = EventAdditionalConditionInfo_GetByMinCurrentStep(manager, 40, records, 10);

    ASSERT_GTE(count, 1, "至少找到1条当前步骤>=40的记录");
    if (count > 0) {
        ASSERT_GTE(records[0].current_step, 40, "第一条记录当前步骤>=40");
    }
    if (count > 1) {
        ASSERT_GTE(records[0].current_step, records[1].current_step, "按当前步骤降序排列");
    }
}

void test_get_top_by_current_step(DBConnectionManager* manager) {
    TEST_START("获取当前步骤TOP N");

    EventAdditionalConditionInfo records[10];
    memset(records, 0, sizeof(records));
    int count = EventAdditionalConditionInfo_GetTopByCurrentStep(manager, 3, records, 10);

    ASSERT_GTE(count, 1, "至少找到1条记录");
    if (count > 1) {
        ASSERT_GTE(records[0].current_step, records[1].current_step, "按当前步骤降序排列");
    }
}

void test_get_unclaimed_rewards(DBConnectionManager* manager) {
    TEST_START("查询未领取奖励的角色");

    /* 设置一些有未领取奖励的记录 */
    EventAdditionalConditionInfo_UpdateCurrentStep(manager, 10001, 20);
    EventAdditionalConditionInfo_UpdateRewardStep(manager, 10001, 10);

    EventAdditionalConditionInfo records[10];
    memset(records, 0, sizeof(records));
    int count = EventAdditionalConditionInfo_GetUnclaimedRewards(manager, records, 10);

    ASSERT_GTE(count, 1, "至少找到1条有未领取奖励的记录");
    if (count > 0) {
        ASSERT_TRUE(records[0].current_step > records[0].reward_step,
                    "第一条记录当前步骤>奖励步骤");
    }
}

void test_get_by_updated_after(DBConnectionManager* manager) {
    TEST_START("按更新时间查询");

    EventAdditionalConditionInfo records[10];
    memset(records, 0, sizeof(records));
    /* 使用过去的日期，避免NOW()导致的问题 */
    int count = EventAdditionalConditionInfo_GetByUpdatedAfter(manager, "2020-01-01 00:00:00",
                                                               records, 10);

    ASSERT_GTE(count, 1, "至少找到1条在指定时间之后更新的记录");
}

void test_count_by_current_step(DBConnectionManager* manager) {
    TEST_START("统计指定步骤的角色数量");

    int count = EventAdditionalConditionInfo_CountByCurrentStep(manager, 50);

    ASSERT_GTE(count, 0, "统计当前步骤为50的角色数量");
}

void test_count_unclaimed_rewards(DBConnectionManager* manager) {
    TEST_START("统计未领取奖励的角色数量");

    int count = EventAdditionalConditionInfo_CountUnclaimedRewards(manager);

    ASSERT_GTE(count, 0, "统计未领取奖励的角色数量");
}

void test_get_average_progress(DBConnectionManager* manager) {
    TEST_START("获取平均步骤进度");

    double average = EventAdditionalConditionInfo_GetAverageProgress(manager);

    ASSERT_GTE((int)average, 0, "平均步骤进度>=0");
}

void test_delete(DBConnectionManager* manager) {
    TEST_START("删除记录");

    ASSERT_EQ(EventAdditionalConditionInfo_Delete(manager, 10001), 0, "删除记录成功");
    ASSERT_TRUE(!EventAdditionalConditionInfo_Exists(manager, 10001), "记录已删除");

    ASSERT_EQ(EventAdditionalConditionInfo_Delete(manager, 10002), 0, "删除记录2成功");
    ASSERT_EQ(EventAdditionalConditionInfo_Delete(manager, 10003), 0, "删除记录3成功");
    ASSERT_EQ(EventAdditionalConditionInfo_Delete(manager, 10004), 0, "删除记录4成功");
}

void test_print_info(DBConnectionManager* manager) {
    TEST_START("打印活动额外条件信息");

    EventAdditionalConditionInfo record;
    memset(&record, 0, sizeof(EventAdditionalConditionInfo));
    record.charac_no = 99999;
    record.current_step = 100;
    record.reward_step = 80;
    strcpy(record.update_time, "2024-01-15 15:00:00");

    EventAdditionalConditionInfo_Add(manager, &record);

    EventAdditionalConditionInfo retrieved;
    memset(&retrieved, 0, sizeof(EventAdditionalConditionInfo));
    if (EventAdditionalConditionInfo_Get(manager, 99999, &retrieved) == 0) {
        EventAdditionalConditionInfo_PrintInfo(&retrieved);
        ASSERT_TRUE(1, "打印信息成功");
        EventAdditionalConditionInfo_Delete(manager, 99999);
    } else {
        printf("  ⚠ 无记录可打印\n");
    }
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
        fprintf(stderr, "连接到数据库失败\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("========================================\n");
    printf("活动额外条件信息 ORM 测试\n");
    printf("========================================\n");

    /* 运行测试用例 */
    test_add_and_get(&manager);
    test_update(&manager);
    test_exists(&manager);
    test_add_multiple_records(&manager);
    test_increment_current_step(&manager);
    test_update_current_step(&manager);
    test_update_reward_step(&manager);
    test_claim_reward(&manager);
    test_reset_progress(&manager);
    test_batch_reset_progress(&manager);
    test_get_by_current_step(&manager);
    test_get_by_min_current_step(&manager);
    test_get_top_by_current_step(&manager);
    test_get_unclaimed_rewards(&manager);
    test_get_by_updated_after(&manager);
    test_count_by_current_step(&manager);
    test_count_unclaimed_rewards(&manager);
    test_get_average_progress(&manager);
    test_delete(&manager);
    test_print_info(&manager);

    /* 最后一个测试也要算进去 */
    if (!current_test_failed) {
        test_passed++;
    }

    /* 清理数据库连接 */
    DBConnectionManager_Cleanup(&manager);

    /* 打印测试结果 */
    printf("\n========================================\n");
    printf("测试用例: %d/%d 通过\n", test_passed, test_count);
    printf("断言: %d/%d 通过\n", assertion_passed, assertion_count);
    printf("========================================\n");

    return (test_passed == test_count && assertion_passed == assertion_count) ? 0 : 1;
}
