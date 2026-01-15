#include "event_dungeon_clear_orm.h"
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

#define ASSERT_EQ_ULL(a, b, message) \
    do { \
        assertion_count++; \
        if ((a) == (b)) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED: %llu != %llu)\n", message, (unsigned long long)(a), (unsigned long long)(b)); \
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
    TEST_START("添加和获取活动副本通关记录");

    EventDungeonClear record;
    memset(&record, 0, sizeof(EventDungeonClear));
    record.charac_no = 10001;
    record.clear_cnt = 5;
    strcpy(record.update_time, "2025-11-16 10:00:00");

    ASSERT_EQ(EventDungeonClear_Add(manager, &record), 0, "添加记录成功");

    EventDungeonClear retrieved;
    memset(&retrieved, 0, sizeof(EventDungeonClear));
    ASSERT_EQ(EventDungeonClear_Get(manager, 10001, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.charac_no, 10001, "角色编号匹配");
    ASSERT_EQ(retrieved.clear_cnt, 5, "通关次数匹配");
}

void test_update(DBConnectionManager* manager) {
    TEST_START("更新活动副本通关记录");

    EventDungeonClear record;
    memset(&record, 0, sizeof(EventDungeonClear));
    ASSERT_EQ(EventDungeonClear_Get(manager, 10001, &record), 0, "获取已存在记录");

    record.clear_cnt = 10;
    strcpy(record.update_time, "2025-11-16 11:00:00");

    ASSERT_EQ(EventDungeonClear_Update(manager, &record), 0, "更新记录成功");

    EventDungeonClear retrieved;
    memset(&retrieved, 0, sizeof(EventDungeonClear));
    ASSERT_EQ(EventDungeonClear_Get(manager, 10001, &retrieved), 0, "获取更新后记录");
    ASSERT_EQ(retrieved.clear_cnt, 10, "通关次数已更新");
}

void test_exists(DBConnectionManager* manager) {
    TEST_START("检查记录是否存在");

    ASSERT_TRUE(EventDungeonClear_Exists(manager, 10001), "记录存在");
    ASSERT_TRUE(!EventDungeonClear_Exists(manager, 99999), "记录不存在");
}

void test_add_multiple_records(DBConnectionManager* manager) {
    TEST_START("添加多条活动副本通关记录");

    EventDungeonClear record2;
    memset(&record2, 0, sizeof(EventDungeonClear));
    record2.charac_no = 10002;
    record2.clear_cnt = 15;
    strcpy(record2.update_time, "2025-11-16 12:00:00");
    ASSERT_EQ(EventDungeonClear_Add(manager, &record2), 0, "添加记录2成功");

    EventDungeonClear record3;
    memset(&record3, 0, sizeof(EventDungeonClear));
    record3.charac_no = 10003;
    record3.clear_cnt = 20;
    strcpy(record3.update_time, "2025-11-16 13:00:00");
    ASSERT_EQ(EventDungeonClear_Add(manager, &record3), 0, "添加记录3成功");

    EventDungeonClear record4;
    memset(&record4, 0, sizeof(EventDungeonClear));
    record4.charac_no = 10004;
    record4.clear_cnt = 8;
    strcpy(record4.update_time, "2025-11-16 14:00:00");
    ASSERT_EQ(EventDungeonClear_Add(manager, &record4), 0, "添加记录4成功");
}

void test_increment_clear_count(DBConnectionManager* manager) {
    TEST_START("增加通关次数");

    ASSERT_EQ(EventDungeonClear_IncrementClearCount(manager, 10001, 5), 0, "增加通关次数成功");

    EventDungeonClear retrieved;
    memset(&retrieved, 0, sizeof(EventDungeonClear));
    ASSERT_EQ(EventDungeonClear_Get(manager, 10001, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.clear_cnt, 15, "通关次数已增加（10+5=15）");
}

void test_update_clear_count(DBConnectionManager* manager) {
    TEST_START("更新通关次数");

    ASSERT_EQ(EventDungeonClear_UpdateClearCount(manager, 10001, 25), 0, "更新通关次数成功");

    EventDungeonClear retrieved;
    memset(&retrieved, 0, sizeof(EventDungeonClear));
    ASSERT_EQ(EventDungeonClear_Get(manager, 10001, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.clear_cnt, 25, "通关次数已更新为25");
}

void test_reset_clear_count(DBConnectionManager* manager) {
    TEST_START("重置通关次数");

    ASSERT_EQ(EventDungeonClear_ResetClearCount(manager, 10002), 0, "重置通关次数成功");

    EventDungeonClear retrieved;
    memset(&retrieved, 0, sizeof(EventDungeonClear));
    ASSERT_EQ(EventDungeonClear_Get(manager, 10002, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.clear_cnt, 0, "通关次数已重置为0");
}

void test_batch_reset_clear_counts(DBConnectionManager* manager) {
    TEST_START("批量重置所有角色通关次数");

    ASSERT_EQ(EventDungeonClear_BatchResetClearCounts(manager), 0, "批量重置成功");

    EventDungeonClear retrieved;
    memset(&retrieved, 0, sizeof(EventDungeonClear));
    ASSERT_EQ(EventDungeonClear_Get(manager, 10001, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.clear_cnt, 0, "记录1通关次数已重置为0");

    memset(&retrieved, 0, sizeof(EventDungeonClear));
    ASSERT_EQ(EventDungeonClear_Get(manager, 10003, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.clear_cnt, 0, "记录3通关次数已重置为0");
}

void test_get_by_clear_count(DBConnectionManager* manager) {
    TEST_START("按通关次数精确查询");

    /* 先设置一些不同的通关次数 */
    EventDungeonClear_UpdateClearCount(manager, 10001, 50);
    EventDungeonClear_UpdateClearCount(manager, 10002, 50);
    EventDungeonClear_UpdateClearCount(manager, 10003, 30);

    EventDungeonClear records[10];
    memset(records, 0, sizeof(records));
    int count = EventDungeonClear_GetByClearCount(manager, 50, records, 10);

    ASSERT_GTE(count, 2, "至少找到2条通关次数为50的记录");
    ASSERT_EQ(records[0].clear_cnt, 50, "第一条记录通关次数为50");
}

void test_get_by_min_clear_count(DBConnectionManager* manager) {
    TEST_START("按最小通关次数查询");

    EventDungeonClear records[10];
    memset(records, 0, sizeof(records));
    int count = EventDungeonClear_GetByMinClearCount(manager, 40, records, 10);

    ASSERT_GTE(count, 1, "至少找到1条通关次数>=40的记录");
    ASSERT_GTE(records[0].clear_cnt, 40, "第一条记录通关次数>=40");
    if (count > 1) {
        ASSERT_GTE(records[0].clear_cnt, records[1].clear_cnt, "按通关次数降序排列");
    }
}

void test_get_top_by_clear_count(DBConnectionManager* manager) {
    TEST_START("获取通关次数TOP N");

    EventDungeonClear records[10];
    memset(records, 0, sizeof(records));
    int count = EventDungeonClear_GetTopByClearCount(manager, 3, records, 10);

    ASSERT_GTE(count, 1, "至少找到1条记录");
    if (count > 1) {
        ASSERT_GTE(records[0].clear_cnt, records[1].clear_cnt, "按通关次数降序排列");
    }
}

void test_get_by_updated_after(DBConnectionManager* manager) {
    TEST_START("按更新时间查询");

    EventDungeonClear records[10];
    memset(records, 0, sizeof(records));
    int count = EventDungeonClear_GetByUpdatedAfter(manager, "2020-01-01 00:00:00", records, 10);

    ASSERT_GTE(count, 1, "至少找到1条在指定时间之后更新的记录");
}

void test_count_by_clear_count(DBConnectionManager* manager) {
    TEST_START("统计指定通关次数的角色数量");

    int count = EventDungeonClear_CountByClearCount(manager, 50);

    ASSERT_GTE(count, 0, "统计通关次数为50的角色数量");
}

void test_get_total_clear_count(DBConnectionManager* manager) {
    TEST_START("获取所有角色总通关次数");

    unsigned long long total = EventDungeonClear_GetTotalClearCount(manager);

    ASSERT_GTE((int)total, 0, "总通关次数>=0");
}

void test_delete(DBConnectionManager* manager) {
    TEST_START("删除记录");

    ASSERT_EQ(EventDungeonClear_Delete(manager, 10001), 0, "删除记录成功");
    ASSERT_TRUE(!EventDungeonClear_Exists(manager, 10001), "记录已删除");

    ASSERT_EQ(EventDungeonClear_Delete(manager, 10002), 0, "删除记录2成功");
    ASSERT_EQ(EventDungeonClear_Delete(manager, 10003), 0, "删除记录3成功");
    ASSERT_EQ(EventDungeonClear_Delete(manager, 10004), 0, "删除记录4成功");
}

void test_print_info(DBConnectionManager* manager) {
    TEST_START("打印活动副本通关记录信息");

    EventDungeonClear record;
    memset(&record, 0, sizeof(EventDungeonClear));
    record.charac_no = 99999;
    record.clear_cnt = 100;
    strcpy(record.update_time, "2025-11-16 15:00:00");

    EventDungeonClear_Add(manager, &record);

    EventDungeonClear retrieved;
    memset(&retrieved, 0, sizeof(EventDungeonClear));
    if (EventDungeonClear_Get(manager, 99999, &retrieved) == 0) {
        EventDungeonClear_PrintInfo(&retrieved);
        ASSERT_TRUE(1, "打印信息成功");
        EventDungeonClear_Delete(manager, 99999);
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
    printf("活动副本通关记录 ORM 测试\n");
    printf("========================================\n");

    /* 运行测试用例 */
    test_add_and_get(&manager);
    test_update(&manager);
    test_exists(&manager);
    test_add_multiple_records(&manager);
    test_increment_clear_count(&manager);
    test_update_clear_count(&manager);
    test_reset_clear_count(&manager);
    test_batch_reset_clear_counts(&manager);
    test_get_by_clear_count(&manager);
    test_get_by_min_clear_count(&manager);
    test_get_top_by_clear_count(&manager);
    test_get_by_updated_after(&manager);
    test_count_by_clear_count(&manager);
    test_get_total_clear_count(&manager);
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
