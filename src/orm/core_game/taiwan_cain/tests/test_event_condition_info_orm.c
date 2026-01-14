/**
 * 活动条件信息 ORM 单元测试
 *
 * 编译:
 * gcc -m32 -std=c99 -I../headers \
 *     -I/usr/include/mysql \
 *     -L/usr/lib/mysql \
 *     ../sources/db_connection_manager.c \
 *     ../sources/event_condition_info_orm.c \
 *     test_event_condition_info_orm.c \
 *     -o test_event_condition_info_orm \
 *     -lmysqlclient -lpthread
 *
 * 运行:
 * ./test_event_condition_info_orm ../config/database.cfg
 */

#include "../headers/event_condition_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* 测试统计 */
static int tests_run = 0;
static int tests_passed = 0;
static int assertions_run = 0;

#define TEST_ASSERT(cond) do { \
    assertions_run++; \
    if (!(cond)) { \
        printf("  ✗ 断言失败: %s (行%d)\n", #cond, __LINE__); \
        return 0; \
    } \
} while(0)

#define RUN_TEST(test_func) do { \
    tests_run++; \
    printf("\n测试 %d: %s\n", tests_run, #test_func); \
    if (test_func(manager)) { \
        tests_passed++; \
        printf("  ✓ 通过\n"); \
    } else { \
        printf("  ✗ 失败\n"); \
    } \
} while(0)

/* 测试用例 */

/**
 * 测试1: 添加活动条件信息
 */
int test_add(DBConnectionManager* manager) {
    EventConditionInfo record = {0};
    record.charac_no = 1001;
    record.current_step = 5;
    record.reward_step = 3;

    int ret = EventConditionInfo_Add(manager, &record);
    TEST_ASSERT(ret == 0);

    return 1;
}

/**
 * 测试2: 获取活动条件信息
 */
int test_get(DBConnectionManager* manager) {
    EventConditionInfo record = {0};

    int ret = EventConditionInfo_Get(manager, 1001, &record);
    TEST_ASSERT(ret == 0);
    TEST_ASSERT(record.charac_no == 1001);
    TEST_ASSERT(record.current_step == 5);
    TEST_ASSERT(record.reward_step == 3);
    TEST_ASSERT(strlen(record.update_time) > 0);

    return 1;
}

/**
 * 测试3: 检查存在性
 */
int test_exists(DBConnectionManager* manager) {
    int exists = EventConditionInfo_Exists(manager, 1001);
    TEST_ASSERT(exists == 1);

    exists = EventConditionInfo_Exists(manager, 9999999);
    TEST_ASSERT(exists == 0);

    return 1;
}

/**
 * 测试4: 更新活动条件信息
 */
int test_update(DBConnectionManager* manager) {
    EventConditionInfo record = {0};
    record.charac_no = 1001;
    record.current_step = 10;
    record.reward_step = 8;

    int ret = EventConditionInfo_Update(manager, &record);
    TEST_ASSERT(ret == 0);

    ret = EventConditionInfo_Get(manager, 1001, &record);
    TEST_ASSERT(ret == 0);
    TEST_ASSERT(record.current_step == 10);
    TEST_ASSERT(record.reward_step == 8);

    return 1;
}

/**
 * 测试5: 增加当前步骤
 */
int test_increment_current_step(DBConnectionManager* manager) {
    EventConditionInfo record = {0};

    int ret = EventConditionInfo_IncrementCurrentStep(manager, 1001, 2);
    TEST_ASSERT(ret == 0);

    ret = EventConditionInfo_Get(manager, 1001, &record);
    TEST_ASSERT(ret == 0);
    TEST_ASSERT(record.current_step == 12);  // 10 + 2

    return 1;
}

/**
 * 测试6: 更新当前步骤
 */
int test_update_current_step(DBConnectionManager* manager) {
    EventConditionInfo record = {0};

    int ret = EventConditionInfo_UpdateCurrentStep(manager, 1001, 15);
    TEST_ASSERT(ret == 0);

    ret = EventConditionInfo_Get(manager, 1001, &record);
    TEST_ASSERT(ret == 0);
    TEST_ASSERT(record.current_step == 15);
    TEST_ASSERT(record.reward_step == 8);  // 奖励步骤不变

    return 1;
}

/**
 * 测试7: 更新奖励步骤
 */
int test_update_reward_step(DBConnectionManager* manager) {
    EventConditionInfo record = {0};

    int ret = EventConditionInfo_UpdateRewardStep(manager, 1001, 12);
    TEST_ASSERT(ret == 0);

    ret = EventConditionInfo_Get(manager, 1001, &record);
    TEST_ASSERT(ret == 0);
    TEST_ASSERT(record.current_step == 15);  // 当前步骤不变
    TEST_ASSERT(record.reward_step == 12);

    return 1;
}

/**
 * 测试8: 领取奖励
 */
int test_claim_reward(DBConnectionManager* manager) {
    EventConditionInfo record = {0};

    int ret = EventConditionInfo_ClaimReward(manager, 1001);
    TEST_ASSERT(ret == 0);

    ret = EventConditionInfo_Get(manager, 1001, &record);
    TEST_ASSERT(ret == 0);
    TEST_ASSERT(record.current_step == 15);
    TEST_ASSERT(record.reward_step == 15);  // reward_step == current_step

    return 1;
}

/**
 * 测试9: 添加多个记录用于查询测试
 */
int test_add_multiple_records(DBConnectionManager* manager) {
    EventConditionInfo records[] = {
        {1002, 20, 18, ""},
        {1003, 30, 30, ""},
        {1004, 5, 5, ""},
        {1005, 50, 45, ""}
    };

    for (int i = 0; i < 4; i++) {
        int ret = EventConditionInfo_Add(manager, &records[i]);
        TEST_ASSERT(ret == 0);
    }

    return 1;
}

/**
 * 测试10: 按当前步骤查询
 */
int test_get_by_current_step(DBConnectionManager* manager) {
    EventConditionInfo records[10] = {0};

    int count = EventConditionInfo_GetByCurrentStep(manager, 15, records, 10);
    TEST_ASSERT(count == 1);
    TEST_ASSERT(records[0].charac_no == 1001);

    return 1;
}

/**
 * 测试11: 按最小当前步骤查询
 */
int test_get_by_min_current_step(DBConnectionManager* manager) {
    EventConditionInfo records[10] = {0};

    int count = EventConditionInfo_GetByMinCurrentStep(manager, 20, records, 10);
    TEST_ASSERT(count >= 2);  // 至少1002(20)和1005(50)

    // 验证排序：第一条应该是步骤最大的
    TEST_ASSERT(records[0].current_step >= records[1].current_step);

    return 1;
}

/**
 * 测试12: 获取TOP N
 */
int test_get_top_by_current_step(DBConnectionManager* manager) {
    EventConditionInfo records[3] = {0};

    int count = EventConditionInfo_GetTopByCurrentStep(manager, 3, records, 3);
    TEST_ASSERT(count == 3);

    // 验证排序：降序
    TEST_ASSERT(records[0].current_step >= records[1].current_step);
    TEST_ASSERT(records[1].current_step >= records[2].current_step);

    return 1;
}

/**
 * 测试13: 查询未领取奖励的角色
 */
int test_get_unclaimed_rewards(DBConnectionManager* manager) {
    EventConditionInfo records[10] = {0};

    int count = EventConditionInfo_GetUnclaimedRewards(manager, records, 10);
    TEST_ASSERT(count >= 2);  // 至少1002和1005有未领取奖励

    // 验证查询结果
    for (int i = 0; i < count; i++) {
        TEST_ASSERT(records[i].current_step > records[i].reward_step);
    }

    return 1;
}

/**
 * 测试14: 按更新时间查询
 */
int test_get_by_updated_after(DBConnectionManager* manager) {
    EventConditionInfo records[10] = {0};

    // 查询2020年之后更新的（所有测试记录都应该在这之后）
    int count = EventConditionInfo_GetByUpdatedAfter(manager, "2020-01-01 00:00:00", records, 10);
    TEST_ASSERT(count >= 5);  // 至少5条记录

    return 1;
}

/**
 * 测试15: 统计指定步骤的角色数量
 */
int test_count_by_current_step(DBConnectionManager* manager) {
    int count = EventConditionInfo_CountByCurrentStep(manager, 15);
    TEST_ASSERT(count == 1);  // 只有1001

    count = EventConditionInfo_CountByCurrentStep(manager, 5);
    TEST_ASSERT(count == 1);  // 只有1004

    return 1;
}

/**
 * 测试16: 统计未领取奖励的角色数量
 */
int test_count_unclaimed_rewards(DBConnectionManager* manager) {
    int count = EventConditionInfo_CountUnclaimedRewards(manager);
    TEST_ASSERT(count >= 2);  // 至少1002和1005

    return 1;
}

/**
 * 测试17: 获取平均步骤进度
 */
int test_get_average_progress(DBConnectionManager* manager) {
    double avg = EventConditionInfo_GetAverageProgress(manager);
    TEST_ASSERT(avg >= 0.0);
    TEST_ASSERT(avg > 10.0);  // 平均值应该大于10

    printf("  平均进度: %.2f\n", avg);

    return 1;
}

/**
 * 测试18: 重置单个角色进度
 */
int test_reset_progress(DBConnectionManager* manager) {
    EventConditionInfo record = {0};

    int ret = EventConditionInfo_ResetProgress(manager, 1002);
    TEST_ASSERT(ret == 0);

    ret = EventConditionInfo_Get(manager, 1002, &record);
    TEST_ASSERT(ret == 0);
    TEST_ASSERT(record.current_step == 0);
    TEST_ASSERT(record.reward_step == 0);

    return 1;
}

/**
 * 测试19: 打印信息
 */
int test_print_info(DBConnectionManager* manager) {
    EventConditionInfo record = {0};

    int ret = EventConditionInfo_Get(manager, 1001, &record);
    TEST_ASSERT(ret == 0);

    printf("\n");
    EventConditionInfo_PrintInfo(&record);

    return 1;
}

/**
 * 测试20: 删除活动条件信息
 */
int test_delete(DBConnectionManager* manager) {
    // 删除测试数据
    int ret = EventConditionInfo_Delete(manager, 1001);
    TEST_ASSERT(ret == 0);

    ret = EventConditionInfo_Delete(manager, 1002);
    TEST_ASSERT(ret == 0);

    ret = EventConditionInfo_Delete(manager, 1003);
    TEST_ASSERT(ret == 0);

    ret = EventConditionInfo_Delete(manager, 1004);
    TEST_ASSERT(ret == 0);

    ret = EventConditionInfo_Delete(manager, 1005);
    TEST_ASSERT(ret == 0);

    // 验证已删除
    int exists = EventConditionInfo_Exists(manager, 1001);
    TEST_ASSERT(exists == 0);

    return 1;
}

/**
 * 主函数
 */
int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("用法: %s <config_file>\n", argv[0]);
        printf("示例: %s ../config/database.cfg\n", argv[0]);
        return 1;
    }

    printf("===========================================\n");
    printf("活动条件信息 ORM 单元测试\n");
    printf("===========================================\n");

    /* 初始化数据库连接 */
    DBConnectionManager* manager = DBConnectionManager_Create(argv[1]);
    if (!manager) {
        printf("✗ 无法创建数据库连接管理器\n");
        return 1;
    }

    printf("✓ 数据库连接成功\n");

    /* 运行所有测试 */
    RUN_TEST(test_add);
    RUN_TEST(test_get);
    RUN_TEST(test_exists);
    RUN_TEST(test_update);
    RUN_TEST(test_increment_current_step);
    RUN_TEST(test_update_current_step);
    RUN_TEST(test_update_reward_step);
    RUN_TEST(test_claim_reward);
    RUN_TEST(test_add_multiple_records);
    RUN_TEST(test_get_by_current_step);
    RUN_TEST(test_get_by_min_current_step);
    RUN_TEST(test_get_top_by_current_step);
    RUN_TEST(test_get_unclaimed_rewards);
    RUN_TEST(test_get_by_updated_after);
    RUN_TEST(test_count_by_current_step);
    RUN_TEST(test_count_unclaimed_rewards);
    RUN_TEST(test_get_average_progress);
    RUN_TEST(test_reset_progress);
    RUN_TEST(test_print_info);
    RUN_TEST(test_delete);

    /* 清理 */
    DBConnectionManager_Destroy(manager);

    /* 输出测试结果 */
    printf("\n===========================================\n");
    printf("测试完成\n");
    printf("===========================================\n");
    printf("总测试数: %d\n", tests_run);
    printf("通过: %d\n", tests_passed);
    printf("失败: %d\n", tests_run - tests_passed);
    printf("总断言数: %d\n", assertions_run);
    printf("成功率: %.1f%%\n", tests_run > 0 ? (tests_passed * 100.0 / tests_run) : 0);
    printf("===========================================\n");

    return (tests_run == tests_passed) ? 0 : 1;
}
