#include "dnf_game_message_orm.h"
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

#define ASSERT_NEQ(a, b, message) \
    do { \
        assertion_count++; \
        if ((a) != (b)) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED: %d == %d)\n", message, (int)(a), (int)(b)); \
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

int main(int argc, char* argv[]) {
    DBConnectionManager manager;
    DnfGameMessage record, retrieved;
    DnfGameMessage* records = NULL;
    int ret, count;

    if (argc < 2) {
        printf("Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    /* 初始化数据库连接 */
    memset(&manager, 0, sizeof(DBConnectionManager));
    if (DBConnectionManager_Initialize(&manager, argv[1]) < 0) {
        printf("Failed to initialize database connection manager\n");
        return 1;
    }

    if (DBConnectionManager_Connect(&manager, DB_TYPE_TAIWAN) < 0) {
        printf("Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    /* 清理测试数据 */
    DnfGameMessage_Clear(&manager);

    /* 测试 1: 添加游戏消息 */
    TEST_START("Add - 添加游戏消息");
    memset(&record, 0, sizeof(DnfGameMessage));
    record.no = 0; /* 自增主键 */
    strcpy(record.message, "Welcome to DNF Server!");
    record.display_type = 1;
    record.start_h = 0;
    record.end_h = 23;
    strcpy(record.occ_date, "2025-11-16");

    ret = DnfGameMessage_Add(&manager, &record);
    ASSERT_EQ(ret, 0, "添加记录成功");

    count = DnfGameMessage_Count(&manager);
    ASSERT_EQ(count, 1, "总记录数为1");

    /* 测试 2: 获取游戏消息 (需要知道自增ID) */
    TEST_START("Get - 获取游戏消息");
    /* 通过日期查询获取刚才插入的记录 */
    ret = DnfGameMessage_GetByDate(&manager, "2025-11-16", &records, &count);
    ASSERT_EQ(ret, 0, "按日期查询成功");
    ASSERT_EQ(count, 1, "找到1条记录");

    if (count > 0 && records) {
        unsigned int msg_no = records[0].no;
        free(records);
        records = NULL;

        memset(&retrieved, 0, sizeof(DnfGameMessage));
        ret = DnfGameMessage_Get(&manager, msg_no, &retrieved);
        ASSERT_EQ(ret, 0, "获取数据成功");
        ASSERT_STR_EQ(retrieved.message, "Welcome to DNF Server!", "消息内容正确");
        ASSERT_EQ(retrieved.display_type, 1, "显示类型正确");
        ASSERT_EQ(retrieved.start_h, 0, "开始小时正确");
        ASSERT_EQ(retrieved.end_h, 23, "结束小时正确");
        ASSERT_STR_EQ(retrieved.occ_date, "2025-11-16", "日期正确");

        DnfGameMessage_PrintInfo(&retrieved);

        /* 测试 3: Exists - 检查记录是否存在 */
        TEST_START("Exists - 检查记录是否存在");
        ret = DnfGameMessage_Exists(&manager, msg_no);
        ASSERT_EQ(ret, 1, "记录存在");

        ret = DnfGameMessage_Exists(&manager, 9999);
        ASSERT_EQ(ret, 0, "不存在的记录");

        /* 测试 4: UpdateMessage - 更新消息内容 */
        TEST_START("UpdateMessage - 更新消息内容");
        ret = DnfGameMessage_UpdateMessage(&manager, msg_no, "Server maintenance at 3:00 AM");
        ASSERT_EQ(ret, 0, "更新消息成功");

        memset(&retrieved, 0, sizeof(DnfGameMessage));
        ret = DnfGameMessage_Get(&manager, msg_no, &retrieved);
        ASSERT_STR_EQ(retrieved.message, "Server maintenance at 3:00 AM", "消息已更新");

        /* 测试 5: UpdateDisplayType - 更新显示类型 */
        TEST_START("UpdateDisplayType - 更新显示类型");
        ret = DnfGameMessage_UpdateDisplayType(&manager, msg_no, 2);
        ASSERT_EQ(ret, 0, "更新显示类型成功");

        memset(&retrieved, 0, sizeof(DnfGameMessage));
        ret = DnfGameMessage_Get(&manager, msg_no, &retrieved);
        ASSERT_EQ(retrieved.display_type, 2, "显示类型已更新为2");

        /* 测试 6: UpdateTimeRange - 更新时间范围 */
        TEST_START("UpdateTimeRange - 更新时间范围");
        ret = DnfGameMessage_UpdateTimeRange(&manager, msg_no, 3, 5);
        ASSERT_EQ(ret, 0, "更新时间范围成功");

        memset(&retrieved, 0, sizeof(DnfGameMessage));
        ret = DnfGameMessage_Get(&manager, msg_no, &retrieved);
        ASSERT_EQ(retrieved.start_h, 3, "开始小时已更新为3");
        ASSERT_EQ(retrieved.end_h, 5, "结束小时已更新为5");

        /* 测试 7: Update - 更新整个记录 */
        TEST_START("Update - 更新整个记录");
        retrieved.display_type = 1;
        strcpy(retrieved.message, "Event starts at 6:00 PM!");
        retrieved.start_h = 18;
        retrieved.end_h = 22;

        ret = DnfGameMessage_Update(&manager, &retrieved);
        ASSERT_EQ(ret, 0, "更新记录成功");

        memset(&retrieved, 0, sizeof(DnfGameMessage));
        ret = DnfGameMessage_Get(&manager, msg_no, &retrieved);
        ASSERT_STR_EQ(retrieved.message, "Event starts at 6:00 PM!", "更新后消息正确");
        ASSERT_EQ(retrieved.start_h, 18, "更新后开始小时正确");
        ASSERT_EQ(retrieved.end_h, 22, "更新后结束小时正确");
    }

    /* 测试 8: SetMessageSchedule - 幂等设置消息计划 */
    TEST_START("SetMessageSchedule - 幂等设置消息计划");
    ret = DnfGameMessage_SetMessageSchedule(&manager, 0, "New Year Event!", 1, 0, 23, "2026-01-01");
    ASSERT_EQ(ret, 0, "设置新消息成功");

    ret = DnfGameMessage_GetByDate(&manager, "2026-01-01", &records, &count);
    ASSERT_EQ(count, 1, "新年消息已添加");
    if (count > 0 && records) {
        unsigned int new_year_no = records[0].no;
        free(records);
        records = NULL;

        /* 幂等性测试 - 用相同no更新 */
        ret = DnfGameMessage_SetMessageSchedule(&manager, new_year_no, "Happy New Year 2026!", 1, 0, 23, "2026-01-01");
        ASSERT_EQ(ret, 0, "幂等更新成功");

        memset(&retrieved, 0, sizeof(DnfGameMessage));
        ret = DnfGameMessage_Get(&manager, new_year_no, &retrieved);
        ASSERT_STR_EQ(retrieved.message, "Happy New Year 2026!", "幂等更新后消息正确");
    }

    count = DnfGameMessage_Count(&manager);
    ASSERT_EQ(count, 2, "总记录数为2");

    /* 测试 9: 批量添加不同日期的消息 */
    TEST_START("Batch Add - 批量添加不同日期消息");
    DnfGameMessage msgs[3];

    memset(&msgs[0], 0, sizeof(DnfGameMessage));
    msgs[0].no = 0;
    strcpy(msgs[0].message, "Weekend Event");
    msgs[0].display_type = 1;
    msgs[0].start_h = 10;
    msgs[0].end_h = 20;
    strcpy(msgs[0].occ_date, "2025-11-16");

    memset(&msgs[1], 0, sizeof(DnfGameMessage));
    msgs[1].no = 0;
    strcpy(msgs[1].message, "Daily Bonus");
    msgs[1].display_type = 2;
    msgs[1].start_h = 0;
    msgs[1].end_h = 23;
    strcpy(msgs[1].occ_date, "2025-11-17");

    memset(&msgs[2], 0, sizeof(DnfGameMessage));
    msgs[2].no = 0;
    strcpy(msgs[2].message, "Maintenance Notice");
    msgs[2].display_type = 1;
    msgs[2].start_h = 3;
    msgs[2].end_h = 5;
    strcpy(msgs[2].occ_date, "2025-11-18");

    int i;
    for (i = 0; i < 3; i++) {
        ret = DnfGameMessage_Add(&manager, &msgs[i]);
        ASSERT_EQ(ret, 0, "批量添加成功");
    }

    count = DnfGameMessage_Count(&manager);
    ASSERT_EQ(count, 5, "总记录数为5");

    /* 测试 10: GetByDisplayType - 按显示类型查询 */
    TEST_START("GetByDisplayType - 按显示类型查询");
    ret = DnfGameMessage_GetByDisplayType(&manager, 1, &records, &count);
    ASSERT_EQ(ret, 0, "按显示类型查询成功");
    ASSERT_TRUE(count >= 3, "找到至少3条type=1的记录");
    if (records) {
        free(records);
        records = NULL;
    }

    ret = DnfGameMessage_GetByDisplayType(&manager, 2, &records, &count);
    ASSERT_EQ(count, 1, "找到1条type=2的记录");
    if (records) {
        free(records);
        records = NULL;
    }

    /* 测试 11: GetByDateRange - 按日期范围查询 */
    TEST_START("GetByDateRange - 按日期范围查询");
    ret = DnfGameMessage_GetByDateRange(&manager, "2025-11-16", "2025-11-18", &records, &count);
    ASSERT_EQ(ret, 0, "按日期范围查询成功");
    ASSERT_TRUE(count >= 3, "找到至少3条记录（2025-11-16到11-18）");
    if (records) {
        free(records);
        records = NULL;
    }

    /* 测试 12: GetActiveMessages - 获取当前活跃消息 */
    TEST_START("GetActiveMessages - 获取当前活跃消息");
    ret = DnfGameMessage_GetActiveMessages(&manager, 1, "2025-11-16", 15, &records, &count);
    ASSERT_EQ(ret, 0, "获取活跃消息成功");
    /* 应该找到Weekend Event (10-20小时范围，15点在范围内) */
    ASSERT_TRUE(count >= 1, "找到至少1条活跃消息");
    if (records) {
        free(records);
        records = NULL;
    }

    /* 测试13: DeleteByDate - 按日期删除 */
    TEST_START("DeleteByDate - 按日期删除");
    ret = DnfGameMessage_DeleteByDate(&manager, "2025-11-17");
    ASSERT_EQ(ret, 0, "按日期删除成功");

    ret = DnfGameMessage_GetByDate(&manager, "2025-11-17", &records, &count);
    ASSERT_EQ(count, 0, "2025-11-17的记录已删除");
    if (records) {
        free(records);
        records = NULL;
    }

    count = DnfGameMessage_Count(&manager);
    ASSERT_EQ(count, 4, "删除后剩余4条记录");

    /* 测试 14: Clear - 清空所有数据 */
    TEST_START("Clear - 清空所有数据");
    ret = DnfGameMessage_Clear(&manager);
    ASSERT_EQ(ret, 0, "清空成功");

    count = DnfGameMessage_Count(&manager);
    ASSERT_EQ(count, 0, "所有记录已清空");

    /* 测试 15: 获取不存在的记录 */
    TEST_START("Get Non-Existent - 获取不存在的记录");
    memset(&retrieved, 0, sizeof(DnfGameMessage));
    ret = DnfGameMessage_Get(&manager, 9999, &retrieved);
    ASSERT_NEQ(ret, 0, "获取不存在的记录应失败");

    /* 清理 */
    DnfGameMessage_Clear(&manager);
    DBConnectionManager_Cleanup(&manager);

    /* 计算最后一个测试是否通过 */
    if (!current_test_failed) {
        test_passed++;
    }

    /* 打印测试结果 */
    printf("\n========================================\n");
    printf("测试用例: %d/%d 通过\n", test_passed, test_count);
    printf("断言: %d/%d 通过\n", assertion_passed, assertion_count);
    printf("========================================\n");

    return (test_passed == test_count && assertion_passed == assertion_count) ? 0 : 1;
}
