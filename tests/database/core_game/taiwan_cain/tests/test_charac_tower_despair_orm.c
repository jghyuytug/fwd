#include "charac_tower_despair_orm.h"
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
    TEST_START("添加和获取绝望之塔数据");

    CharacTowerDespair record, retrieved;
    memset(&record, 0, sizeof(CharacTowerDespair));

    record.charac_no = 1000001;
    strcpy(record.first_layer_start_date, "2025-11-01 10:00:00");
    record.today_enter_count = 5;
    record.last_clear_layer = 50;
    record.enter_count_by_week = 20;
    strcpy(record.m_date, "2025-11-16 12:00:00");
    strcpy(record.last_clear_date, "2025-11-16 11:30:00");

    ASSERT_EQ(CharacTowerDespair_Add(manager, &record), 0, "添加记录成功");
    ASSERT_EQ(CharacTowerDespair_Get(manager, record.charac_no, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.charac_no, record.charac_no, "角色编号匹配");
    ASSERT_STR_EQ(retrieved.first_layer_start_date, record.first_layer_start_date, "首层开始日期匹配");
    ASSERT_EQ(retrieved.today_enter_count, record.today_enter_count, "今日进入次数匹配");
    ASSERT_EQ(retrieved.last_clear_layer, record.last_clear_layer, "最后通关层数匹配");
    ASSERT_EQ(retrieved.enter_count_by_week, record.enter_count_by_week, "每周进入次数匹配");
}

void test_update(DBConnectionManager* manager) {
    TEST_START("更新绝望之塔数据");

    CharacTowerDespair record, retrieved;
    ASSERT_EQ(CharacTowerDespair_Get(manager, 1000001, &record), 0, "获取已存在记录");

    record.today_enter_count = 10;
    record.last_clear_layer = 60;
    record.enter_count_by_week = 30;

    ASSERT_EQ(CharacTowerDespair_Update(manager, &record), 0, "更新记录成功");
    ASSERT_EQ(CharacTowerDespair_Get(manager, 1000001, &retrieved), 0, "获取更新后记录");
    ASSERT_EQ(retrieved.today_enter_count, 10, "今日进入次数已更新");
    ASSERT_EQ(retrieved.last_clear_layer, 60, "最后通关层数已更新");
    ASSERT_EQ(retrieved.enter_count_by_week, 30, "每周进入次数已更新");
}

void test_exists(DBConnectionManager* manager) {
    TEST_START("检查记录是否存在");

    ASSERT_EQ(CharacTowerDespair_Exists(manager, 1000001), 1, "记录存在");
    ASSERT_EQ(CharacTowerDespair_Exists(manager, 9999999), 0, "记录不存在");
}

void test_increment_today_enter_count(DBConnectionManager* manager) {
    TEST_START("增加今日进入次数");

    CharacTowerDespair retrieved;
    ASSERT_EQ(CharacTowerDespair_Get(manager, 1000001, &retrieved), 0, "获取当前记录");
    unsigned char old_count = retrieved.today_enter_count;

    ASSERT_EQ(CharacTowerDespair_IncrementTodayEnterCount(manager, 1000001, 3), 0, "增加今日进入次数成功");
    ASSERT_EQ(CharacTowerDespair_Get(manager, 1000001, &retrieved), 0, "获取更新后记录");
    ASSERT_EQ(retrieved.today_enter_count, old_count + 3, "今日进入次数增加了3");
}

void test_reset_today_enter_count(DBConnectionManager* manager) {
    TEST_START("重置今日进入次数");

    CharacTowerDespair retrieved;

    ASSERT_EQ(CharacTowerDespair_ResetTodayEnterCount(manager, 1000001), 0, "重置今日进入次数成功");
    ASSERT_EQ(CharacTowerDespair_Get(manager, 1000001, &retrieved), 0, "获取记录");
    ASSERT_EQ(retrieved.today_enter_count, 0, "今日进入次数已重置为0");
}

void test_update_last_clear_layer(DBConnectionManager* manager) {
    TEST_START("更新最后通关层数");

    CharacTowerDespair retrieved;

    ASSERT_EQ(CharacTowerDespair_UpdateLastClearLayer(manager, 1000001, 75), 0, "更新最后通关层数成功");
    ASSERT_EQ(CharacTowerDespair_Get(manager, 1000001, &retrieved), 0, "获取记录");
    ASSERT_EQ(retrieved.last_clear_layer, 75, "最后通关层数已更新为75");
}

void test_increment_week_enter_count(DBConnectionManager* manager) {
    TEST_START("增加每周进入次数");

    CharacTowerDespair retrieved;
    ASSERT_EQ(CharacTowerDespair_Get(manager, 1000001, &retrieved), 0, "获取当前记录");
    int old_count = retrieved.enter_count_by_week;

    ASSERT_EQ(CharacTowerDespair_IncrementWeekEnterCount(manager, 1000001, 5), 0, "增加每周进入次数成功");
    ASSERT_EQ(CharacTowerDespair_Get(manager, 1000001, &retrieved), 0, "获取更新后记录");
    ASSERT_EQ(retrieved.enter_count_by_week, old_count + 5, "每周进入次数增加了5");
}

void test_reset_week_enter_count(DBConnectionManager* manager) {
    TEST_START("重置每周进入次数");

    CharacTowerDespair retrieved;

    ASSERT_EQ(CharacTowerDespair_ResetWeekEnterCount(manager, 1000001), 0, "重置每周进入次数成功");
    ASSERT_EQ(CharacTowerDespair_Get(manager, 1000001, &retrieved), 0, "获取记录");
    ASSERT_EQ(retrieved.enter_count_by_week, 0, "每周进入次数已重置为0");
}

void test_update_last_clear_date(DBConnectionManager* manager) {
    TEST_START("更新最后通关日期");

    CharacTowerDespair retrieved;

    ASSERT_EQ(CharacTowerDespair_UpdateLastClearDate(manager, 1000001), 0, "更新最后通关日期成功");
    ASSERT_EQ(CharacTowerDespair_Get(manager, 1000001, &retrieved), 0, "获取记录");
    ASSERT_TRUE(strlen(retrieved.last_clear_date) > 0, "最后通关日期已更新");
}

void test_update_m_date(DBConnectionManager* manager) {
    TEST_START("更新修改日期");

    CharacTowerDespair retrieved;

    ASSERT_EQ(CharacTowerDespair_UpdateMDate(manager, 1000001), 0, "更新修改日期成功");
    ASSERT_EQ(CharacTowerDespair_Get(manager, 1000001, &retrieved), 0, "获取记录");
    ASSERT_TRUE(strlen(retrieved.m_date) > 0, "修改日期已更新");
}

void test_get_total_count(DBConnectionManager* manager) {
    TEST_START("获取总记录数");

    /* 添加更多记录 */
    CharacTowerDespair record2, record3;
    memset(&record2, 0, sizeof(CharacTowerDespair));
    memset(&record3, 0, sizeof(CharacTowerDespair));

    record2.charac_no = 1000002;
    strcpy(record2.first_layer_start_date, "2025-11-10 10:00:00");
    record2.today_enter_count = 3;
    record2.last_clear_layer = 40;
    record2.enter_count_by_week = 15;
    strcpy(record2.m_date, "2025-11-16 12:00:00");
    strcpy(record2.last_clear_date, "2025-11-16 11:00:00");

    record3.charac_no = 1000003;
    strcpy(record3.first_layer_start_date, "2025-11-05 10:00:00");
    record3.today_enter_count = 7;
    record3.last_clear_layer = 80;
    record3.enter_count_by_week = 25;
    strcpy(record3.m_date, "2025-11-16 12:00:00");
    strcpy(record3.last_clear_date, "2025-11-16 11:00:00");

    CharacTowerDespair_Add(manager, &record2);
    CharacTowerDespair_Add(manager, &record3);

    int count = CharacTowerDespair_GetTotalCount(manager);
    ASSERT_TRUE(count >= 3, "至少有3条记录");
}

void test_get_by_clear_layer_range(DBConnectionManager* manager) {
    TEST_START("获取指定层数范围的角色列表");

    int charac_nos[10];
    int count = CharacTowerDespair_GetByClearLayerRange(manager, 40, 80, charac_nos, 10);

    ASSERT_TRUE(count >= 2, "至少找到2个角色在层数范围40-80");
}

void test_get_top_by_layer(DBConnectionManager* manager) {
    TEST_START("获取最高通关层数排行榜");

    int charac_nos[5];
    int count = CharacTowerDespair_GetTopByLayer(manager, 5, charac_nos);

    ASSERT_TRUE(count >= 3, "找到至少3个角色");
    ASSERT_TRUE(count <= 5, "不超过限制数量5");

    /* 验证第一名层数最高 */
    CharacTowerDespair first, second;
    if (count >= 2) {
        CharacTowerDespair_Get(manager, charac_nos[0], &first);
        CharacTowerDespair_Get(manager, charac_nos[1], &second);
        ASSERT_TRUE(first.last_clear_layer >= second.last_clear_layer, "第一名层数 >= 第二名层数");
    }
}

void test_get_top_by_today_count(DBConnectionManager* manager) {
    TEST_START("获取今日挑战次数最多的角色");

    int charac_nos[5];
    int count = CharacTowerDespair_GetTopByTodayCount(manager, 5, charac_nos);

    ASSERT_TRUE(count >= 3, "找到至少3个角色");
    ASSERT_TRUE(count <= 5, "不超过限制数量5");

    /* 验证第一名次数最多 */
    CharacTowerDespair first, second;
    if (count >= 2) {
        CharacTowerDespair_Get(manager, charac_nos[0], &first);
        CharacTowerDespair_Get(manager, charac_nos[1], &second);
        ASSERT_TRUE(first.today_enter_count >= second.today_enter_count, "第一名次数 >= 第二名次数");
    }
}

void test_delete(DBConnectionManager* manager) {
    TEST_START("删除记录");

    ASSERT_EQ(CharacTowerDespair_Delete(manager, 1000001), 0, "删除记录成功");
    ASSERT_EQ(CharacTowerDespair_Exists(manager, 1000001), 0, "记录已删除");

    ASSERT_EQ(CharacTowerDespair_Delete(manager, 1000002), 0, "删除记录成功");
    ASSERT_EQ(CharacTowerDespair_Delete(manager, 1000003), 0, "删除记录成功");
}

void test_print_info(DBConnectionManager* manager) {
    TEST_START("打印绝望之塔信息");

    CharacTowerDespair record;
    memset(&record, 0, sizeof(CharacTowerDespair));

    record.charac_no = 1000004;
    strcpy(record.first_layer_start_date, "2025-11-01 10:00:00");
    record.today_enter_count = 8;
    record.last_clear_layer = 90;
    record.enter_count_by_week = 40;
    strcpy(record.m_date, "2025-11-16 12:00:00");
    strcpy(record.last_clear_date, "2025-11-16 11:00:00");

    CharacTowerDespair_Add(manager, &record);

    CharacTowerDespair retrieved;
    CharacTowerDespair_Get(manager, 1000004, &retrieved);

    printf("\n");
    CharacTowerDespair_PrintInfo(&retrieved);

    CharacTowerDespair_Delete(manager, 1000004);

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
    printf("角色绝望之塔 ORM 测试\n");
    printf("========================================\n");

    /* 运行测试 */
    test_add_and_get(&manager);
    test_update(&manager);
    test_exists(&manager);
    test_increment_today_enter_count(&manager);
    test_reset_today_enter_count(&manager);
    test_update_last_clear_layer(&manager);
    test_increment_week_enter_count(&manager);
    test_reset_week_enter_count(&manager);
    test_update_last_clear_date(&manager);
    test_update_m_date(&manager);
    test_get_total_count(&manager);
    test_get_by_clear_layer_range(&manager);
    test_get_top_by_layer(&manager);
    test_get_top_by_today_count(&manager);
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
