#include "charac_tower_scream_orm.h"
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

/* 测试用例 */
void test_add_and_get(DBConnectionManager* manager) {
    TEST_START("添加和获取惊叫之塔记录");

    CharacTowerScream record, retrieved;
    memset(&record, 0, sizeof(CharacTowerScream));

    record.charac_no = 6000001;
    strcpy(record.first_layer_start_date, "2025-11-16 10:00:00");
    record.today_enter_count = 5;
    record.last_clear_layer = 50;
    record.enter_count_by_week = 20;
    strcpy(record.m_date, "2025-11-16 15:00:00");
    strcpy(record.last_clear_date, "2025-11-16 14:00:00");

    ASSERT_EQ(CharacTowerScream_Add(manager, &record), 0, "添加记录成功");
    ASSERT_EQ(CharacTowerScream_Get(manager, record.charac_no, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.charac_no, record.charac_no, "角色编号匹配");
    ASSERT_EQ(retrieved.today_enter_count, record.today_enter_count, "今日进入次数匹配");
    ASSERT_EQ(retrieved.last_clear_layer, record.last_clear_layer, "最后通关层数匹配");
    ASSERT_EQ(retrieved.enter_count_by_week, record.enter_count_by_week, "本周进入次数匹配");
}

void test_update(DBConnectionManager* manager) {
    TEST_START("更新惊叫之塔记录");

    CharacTowerScream record, retrieved;
    ASSERT_EQ(CharacTowerScream_Get(manager, 6000001, &record), 0, "获取已存在记录");

    record.today_enter_count = 10;
    record.last_clear_layer = 60;
    record.enter_count_by_week = 30;

    ASSERT_EQ(CharacTowerScream_Update(manager, &record), 0, "更新记录成功");
    ASSERT_EQ(CharacTowerScream_Get(manager, 6000001, &retrieved), 0, "获取更新后记录");
    ASSERT_EQ(retrieved.today_enter_count, 10, "今日进入次数已更新");
    ASSERT_EQ(retrieved.last_clear_layer, 60, "最后通关层数已更新");
    ASSERT_EQ(retrieved.enter_count_by_week, 30, "本周进入次数已更新");
}

void test_exists(DBConnectionManager* manager) {
    TEST_START("检查记录是否存在");

    ASSERT_EQ(CharacTowerScream_Exists(manager, 6000001), 1, "记录存在");
    ASSERT_EQ(CharacTowerScream_Exists(manager, 9999999), 0, "记录不存在");
}

void test_add_multiple_records(DBConnectionManager* manager) {
    TEST_START("添加多条惊叫之塔记录");

    CharacTowerScream record2, record3;
    memset(&record2, 0, sizeof(CharacTowerScream));
    memset(&record3, 0, sizeof(CharacTowerScream));

    record2.charac_no = 6000002;
    strcpy(record2.first_layer_start_date, "2025-11-16 11:00:00");
    record2.today_enter_count = 3;
    record2.last_clear_layer = 40;
    record2.enter_count_by_week = 15;
    strcpy(record2.m_date, "2025-11-16 15:00:00");
    strcpy(record2.last_clear_date, "2025-11-16 14:30:00");

    record3.charac_no = 6000003;
    strcpy(record3.first_layer_start_date, "2025-11-16 12:00:00");
    record3.today_enter_count = 7;
    record3.last_clear_layer = 70;
    record3.enter_count_by_week = 25;
    strcpy(record3.m_date, "2025-11-16 15:00:00");
    strcpy(record3.last_clear_date, "2025-11-16 14:45:00");

    ASSERT_EQ(CharacTowerScream_Add(manager, &record2), 0, "添加记录2成功");
    ASSERT_EQ(CharacTowerScream_Add(manager, &record3), 0, "添加记录3成功");
}

void test_update_today_enter_count(DBConnectionManager* manager) {
    TEST_START("更新今日进入次数");

    ASSERT_EQ(CharacTowerScream_UpdateTodayEnterCount(manager, 6000001, 15), 0, "更新今日进入次数成功");

    CharacTowerScream retrieved;
    ASSERT_EQ(CharacTowerScream_Get(manager, 6000001, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.today_enter_count, 15, "今日进入次数已更新");
}

void test_increment_today_enter_count(DBConnectionManager* manager) {
    TEST_START("增加今日进入次数");

    CharacTowerScream before, after;
    ASSERT_EQ(CharacTowerScream_Get(manager, 6000001, &before), 0, "获取初始记录");

    ASSERT_EQ(CharacTowerScream_IncrementTodayEnterCount(manager, 6000001), 0, "增加今日进入次数成功");

    ASSERT_EQ(CharacTowerScream_Get(manager, 6000001, &after), 0, "获取增加后记录");
    ASSERT_EQ(after.today_enter_count, before.today_enter_count + 1, "今日进入次数增加1");
}

void test_reset_today_enter_count(DBConnectionManager* manager) {
    TEST_START("重置今日进入次数");

    ASSERT_EQ(CharacTowerScream_ResetTodayEnterCount(manager, 6000001), 0, "重置今日进入次数成功");

    CharacTowerScream retrieved;
    ASSERT_EQ(CharacTowerScream_Get(manager, 6000001, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.today_enter_count, 0, "今日进入次数已重置为0");
}

void test_update_last_clear_layer(DBConnectionManager* manager) {
    TEST_START("更新最后通关层数");

    ASSERT_EQ(CharacTowerScream_UpdateLastClearLayer(manager, 6000001, 80), 0, "更新最后通关层数成功");

    CharacTowerScream retrieved;
    ASSERT_EQ(CharacTowerScream_Get(manager, 6000001, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.last_clear_layer, 80, "最后通关层数已更新");
}

void test_update_week_enter_count(DBConnectionManager* manager) {
    TEST_START("更新本周进入次数");

    ASSERT_EQ(CharacTowerScream_UpdateEnterCountByWeek(manager, 6000001, 50), 0, "更新本周进入次数成功");

    CharacTowerScream retrieved;
    ASSERT_EQ(CharacTowerScream_Get(manager, 6000001, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.enter_count_by_week, 50, "本周进入次数已更新");
}

void test_increment_week_enter_count(DBConnectionManager* manager) {
    TEST_START("增加本周进入次数");

    CharacTowerScream before, after;
    ASSERT_EQ(CharacTowerScream_Get(manager, 6000001, &before), 0, "获取初始记录");

    ASSERT_EQ(CharacTowerScream_IncrementEnterCountByWeek(manager, 6000001), 0, "增加本周进入次数成功");

    ASSERT_EQ(CharacTowerScream_Get(manager, 6000001, &after), 0, "获取增加后记录");
    ASSERT_EQ(after.enter_count_by_week, before.enter_count_by_week + 1, "本周进入次数增加1");
}

void test_reset_week_enter_count(DBConnectionManager* manager) {
    TEST_START("重置本周进入次数");

    ASSERT_EQ(CharacTowerScream_ResetEnterCountByWeek(manager, 6000001), 0, "重置本周进入次数成功");

    CharacTowerScream retrieved;
    ASSERT_EQ(CharacTowerScream_Get(manager, 6000001, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.enter_count_by_week, 0, "本周进入次数已重置为0");
}

void test_update_last_clear_date(DBConnectionManager* manager) {
    TEST_START("更新最后通关日期");

    ASSERT_EQ(CharacTowerScream_UpdateLastClearDate(manager, 6000001, "2025-11-17 10:00:00"), 0,
              "更新最后通关日期成功");

    CharacTowerScream retrieved;
    ASSERT_EQ(CharacTowerScream_Get(manager, 6000001, &retrieved), 0, "获取记录成功");
    ASSERT_STR_EQ(retrieved.last_clear_date, "2025-11-17 10:00:00", "最后通关日期已更新");
}

void test_get_by_min_layer(DBConnectionManager* manager) {
    TEST_START("获取指定层数以上的角色列表");

    CharacTowerScream records[10];
    int count = CharacTowerScream_GetByMinLayer(manager, 50, records, 10);

    ASSERT_TRUE(count >= 1, "至少找到1条记录");
    if (count >= 1) {
        ASSERT_TRUE(records[0].last_clear_layer >= 50, "层数满足条件");
    }
}

void test_get_top_by_today_enter_count(DBConnectionManager* manager) {
    TEST_START("获取今日进入次数排行");

    /* 先设置一些记录的今日进入次数 */
    CharacTowerScream_UpdateTodayEnterCount(manager, 6000001, 10);
    CharacTowerScream_UpdateTodayEnterCount(manager, 6000002, 5);
    CharacTowerScream_UpdateTodayEnterCount(manager, 6000003, 15);

    CharacTowerScream records[5];
    int count = CharacTowerScream_GetTopByTodayEnterCount(manager, 5, records);

    ASSERT_TRUE(count >= 1, "至少找到1条记录");
    if (count >= 2) {
        ASSERT_TRUE(records[0].today_enter_count >= records[1].today_enter_count,
                    "按今日进入次数降序排列");
    }
}

void test_get_top_by_week_enter_count(DBConnectionManager* manager) {
    TEST_START("获取本周进入次数排行");

    /* 先设置一些记录的本周进入次数 */
    CharacTowerScream_UpdateEnterCountByWeek(manager, 6000001, 50);
    CharacTowerScream_UpdateEnterCountByWeek(manager, 6000002, 30);
    CharacTowerScream_UpdateEnterCountByWeek(manager, 6000003, 70);

    CharacTowerScream records[5];
    int count = CharacTowerScream_GetTopByWeekEnterCount(manager, 5, records);

    ASSERT_TRUE(count >= 1, "至少找到1条记录");
    if (count >= 2) {
        ASSERT_TRUE(records[0].enter_count_by_week >= records[1].enter_count_by_week,
                    "按本周进入次数降序排列");
    }
}

void test_get_by_modified_after(DBConnectionManager* manager) {
    TEST_START("获取指定日期之后修改的记录");

    CharacTowerScream records[10];
    int count = CharacTowerScream_GetByModifiedAfter(manager, "2025-11-13 00:00:00", records, 10);

    ASSERT_TRUE(count >= 1, "至少找到1条记录");
}

void test_batch_reset_today_enter_count(DBConnectionManager* manager) {
    TEST_START("批量重置今日进入次数");

    ASSERT_EQ(CharacTowerScream_BatchResetTodayEnterCount(manager), 0, "批量重置今日进入次数成功");

    CharacTowerScream retrieved;
    ASSERT_EQ(CharacTowerScream_Get(manager, 6000001, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.today_enter_count, 0, "今日进入次数已重置为0");
}

void test_batch_reset_week_enter_count(DBConnectionManager* manager) {
    TEST_START("批量重置本周进入次数");

    ASSERT_EQ(CharacTowerScream_BatchResetWeekEnterCount(manager), 0, "批量重置本周进入次数成功");

    CharacTowerScream retrieved;
    ASSERT_EQ(CharacTowerScream_Get(manager, 6000001, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.enter_count_by_week, 0, "本周进入次数已重置为0");
}

void test_delete(DBConnectionManager* manager) {
    TEST_START("删除记录");

    ASSERT_EQ(CharacTowerScream_Delete(manager, 6000002), 0, "删除记录成功");
    ASSERT_EQ(CharacTowerScream_Exists(manager, 6000002), 0, "记录已删除");
}

void test_print_info(DBConnectionManager* manager) {
    TEST_START("打印惊叫之塔记录信息");

    CharacTowerScream record;
    if (CharacTowerScream_Get(manager, 6000001, &record) == 0) {
        printf("\n");
        CharacTowerScream_PrintInfo(&record);
        assertion_count++;
        assertion_passed++;
        printf("  ✓ 打印信息成功\n");
    } else {
        assertion_count++;
        current_test_failed = 1;
        printf("  ✗ 获取记录失败\n");
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
        fprintf(stderr, "连接到 taiwan_cain 数据库失败\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("========================================\n");
    printf("角色惊叫之塔记录 ORM 测试\n");
    printf("========================================\n");

    /* 运行测试 */
    test_add_and_get(&manager);
    test_update(&manager);
    test_exists(&manager);
    test_add_multiple_records(&manager);
    test_update_today_enter_count(&manager);
    test_increment_today_enter_count(&manager);
    test_reset_today_enter_count(&manager);
    test_update_last_clear_layer(&manager);
    test_update_week_enter_count(&manager);
    test_increment_week_enter_count(&manager);
    test_reset_week_enter_count(&manager);
    test_update_last_clear_date(&manager);
    test_get_by_min_layer(&manager);
    test_get_top_by_today_enter_count(&manager);
    test_get_top_by_week_enter_count(&manager);
    test_get_by_modified_after(&manager);
    test_batch_reset_today_enter_count(&manager);
    test_batch_reset_week_enter_count(&manager);
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
