#include "charac_housing_water_history_orm.h"
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
        if (strcmp((a), (b)) == 0) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED: '%s' != '%s')\n", message, (a), (b)); \
        } \
    } while(0)

/* 测试用例 */
void test_add_and_get(DBConnectionManager* manager) {
    TEST_START("添加和获取浇水历史记录");

    CharacHousingWaterHistory record, retrieved;
    memset(&record, 0, sizeof(CharacHousingWaterHistory));

    record.charac_no = 500001;
    strncpy(record.give_time, "2025-11-15 10:00:00", sizeof(record.give_time) - 1);
    strncpy(record.give_charac_name, "TestPlayer1", sizeof(record.give_charac_name) - 1);

    ASSERT_EQ(CharacHousingWaterHistory_Add(manager, &record), 0, "添加记录成功");
    ASSERT_EQ(CharacHousingWaterHistory_Get(manager, record.charac_no, record.give_time, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.charac_no, record.charac_no, "角色编号匹配");
    ASSERT_STR_EQ(retrieved.give_time, record.give_time, "浇水时间匹配");
    ASSERT_STR_EQ(retrieved.give_charac_name, record.give_charac_name, "浇水者姓名匹配");
}

void test_update(DBConnectionManager* manager) {
    TEST_START("更新浇水历史记录");

    CharacHousingWaterHistory record, retrieved;

    ASSERT_EQ(CharacHousingWaterHistory_Get(manager, 500001, "2025-11-15 10:00:00", &record), 0, "获取已存在记录");

    strncpy(record.give_charac_name, "UpdatedName", sizeof(record.give_charac_name) - 1);

    ASSERT_EQ(CharacHousingWaterHistory_Update(manager, &record), 0, "更新记录成功");
    ASSERT_EQ(CharacHousingWaterHistory_Get(manager, record.charac_no, record.give_time, &retrieved), 0, "获取更新后记录");
    ASSERT_STR_EQ(retrieved.give_charac_name, "UpdatedName", "浇水者姓名已更新");
}

void test_exists(DBConnectionManager* manager) {
    TEST_START("检查浇水历史记录是否存在");

    ASSERT_EQ(CharacHousingWaterHistory_Exists(manager, 500001, "2025-11-15 10:00:00"), 1, "记录存在");
    ASSERT_EQ(CharacHousingWaterHistory_Exists(manager, 500001, "2025-12-31 00:00:00"), 0, "记录不存在");
}

void test_count_by_charac(DBConnectionManager* manager) {
    TEST_START("统计角色浇水历史记录数量");

    /* 添加更多记录 */
    CharacHousingWaterHistory record;
    memset(&record, 0, sizeof(CharacHousingWaterHistory));
    record.charac_no = 500001;
    strncpy(record.give_time, "2025-11-15 11:00:00", sizeof(record.give_time) - 1);
    strncpy(record.give_charac_name, "TestPlayer2", sizeof(record.give_charac_name) - 1);
    CharacHousingWaterHistory_Add(manager, &record);

    record.charac_no = 500001;
    strncpy(record.give_time, "2025-11-15 12:00:00", sizeof(record.give_time) - 1);
    strncpy(record.give_charac_name, "TestPlayer3", sizeof(record.give_charac_name) - 1);
    CharacHousingWaterHistory_Add(manager, &record);

    int count = CharacHousingWaterHistory_CountByCharac(manager, 500001);
    ASSERT_EQ(count >= 3, 1, "至少有3条记录");
    printf("  角色500001的浇水记录数: %d\n", count);
}

void test_get_by_time_range(DBConnectionManager* manager) {
    TEST_START("按时间范围查询浇水记录");

    CharacHousingWaterHistory records[10];
    int count = CharacHousingWaterHistory_GetByTimeRange(manager, 500001,
                                                          "2025-11-15 09:00:00",
                                                          "2025-11-15 13:00:00",
                                                          records, 10);

    ASSERT_EQ(count >= 3, 1, "找到至少3条记录");
    printf("  时间范围内的记录数: %d\n", count);
    for (int i = 0; i < count && i < 3; i++) {
        printf("  - %s: %s\n", records[i].give_time, records[i].give_charac_name);
    }
}

void test_get_by_giver(DBConnectionManager* manager) {
    TEST_START("按浇水者查询记录");

    /* 添加更多来自同一浇水者的记录 */
    CharacHousingWaterHistory record;
    memset(&record, 0, sizeof(CharacHousingWaterHistory));
    record.charac_no = 500002;
    strncpy(record.give_time, "2025-11-15 10:00:00", sizeof(record.give_time) - 1);
    strncpy(record.give_charac_name, "TestPlayer2", sizeof(record.give_charac_name) - 1);
    CharacHousingWaterHistory_Add(manager, &record);

    CharacHousingWaterHistory records[10];
    int count = CharacHousingWaterHistory_GetByGiver(manager, "TestPlayer2", records, 10);

    ASSERT_EQ(count >= 2, 1, "找到至少2条记录");
    printf("  TestPlayer2的浇水记录数: %d\n", count);
}

void test_get_recent_days(DBConnectionManager* manager) {
    TEST_START("获取最近N天的浇水记录");

    /* 添加一条当天的记录 */
    CharacHousingWaterHistory record;
    memset(&record, 0, sizeof(CharacHousingWaterHistory));
    record.charac_no = 500001;
    strncpy(record.give_time, "2025-11-15 14:00:00", sizeof(record.give_time) - 1);
    strncpy(record.give_charac_name, "TodayPlayer", sizeof(record.give_charac_name) - 1);
    CharacHousingWaterHistory_Add(manager, &record);

    CharacHousingWaterHistory records[20];
    int count = CharacHousingWaterHistory_GetRecentDays(manager, 500001, 7, records, 20);

    ASSERT_EQ(count >= 0, 1, "查询成功");
    printf("  最近7天的浇水记录数: %d\n", count);
}

void test_delete_before(DBConnectionManager* manager) {
    TEST_START("删除指定时间之前的旧记录");

    /* 添加一些旧记录 */
    CharacHousingWaterHistory record;
    memset(&record, 0, sizeof(CharacHousingWaterHistory));
    record.charac_no = 500003;
    strncpy(record.give_time, "2020-01-01 00:00:00", sizeof(record.give_time) - 1);
    strncpy(record.give_charac_name, "OldPlayer", sizeof(record.give_charac_name) - 1);
    CharacHousingWaterHistory_Add(manager, &record);

    ASSERT_EQ(CharacHousingWaterHistory_DeleteBefore(manager, "2021-01-01 00:00:00"), 0, "删除旧记录成功");
    ASSERT_EQ(CharacHousingWaterHistory_Exists(manager, 500003, "2020-01-01 00:00:00"), 0, "旧记录已删除");
}

void test_delete_all_by_charac(DBConnectionManager* manager) {
    TEST_START("删除角色的所有浇水记录");

    ASSERT_EQ(CharacHousingWaterHistory_DeleteAllByCharac(manager, 500001), 0, "删除所有记录成功");

    int count = CharacHousingWaterHistory_CountByCharac(manager, 500001);
    ASSERT_EQ(count, 0, "所有记录已删除");
}

void test_delete(DBConnectionManager* manager) {
    TEST_START("删除指定浇水记录");

    ASSERT_EQ(CharacHousingWaterHistory_Delete(manager, 500002, "2025-11-15 10:00:00"), 0, "删除记录成功");
    ASSERT_EQ(CharacHousingWaterHistory_Exists(manager, 500002, "2025-11-15 10:00:00"), 0, "记录已删除");
}

void test_print_info(DBConnectionManager* manager) {
    TEST_START("打印浇水历史记录信息");

    /* 添加一条新记录用于打印 */
    CharacHousingWaterHistory record;
    memset(&record, 0, sizeof(CharacHousingWaterHistory));
    record.charac_no = 500004;
    strncpy(record.give_time, "2025-11-15 15:00:00", sizeof(record.give_time) - 1);
    strncpy(record.give_charac_name, "PrintTest", sizeof(record.give_charac_name) - 1);
    CharacHousingWaterHistory_Add(manager, &record);

    CharacHousingWaterHistory retrieved;
    ASSERT_EQ(CharacHousingWaterHistory_Get(manager, 500004, "2025-11-15 15:00:00", &retrieved), 0, "获取记录");

    printf("\n");
    CharacHousingWaterHistory_PrintInfo(&retrieved);

    /* 清理测试数据 */
    CharacHousingWaterHistory_Delete(manager, 500004, "2025-11-15 15:00:00");

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
    printf("角色住房浇水历史 ORM 测试\n");
    printf("========================================\n");

    /* 运行测试 */
    test_add_and_get(&manager);
    test_update(&manager);
    test_exists(&manager);
    test_count_by_charac(&manager);
    test_get_by_time_range(&manager);
    test_get_by_giver(&manager);
    test_get_recent_days(&manager);
    test_delete_before(&manager);
    test_delete_all_by_charac(&manager);
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
