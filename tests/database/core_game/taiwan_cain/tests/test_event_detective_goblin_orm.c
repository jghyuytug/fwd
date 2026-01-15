#include "event_detective_goblin_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// 测试框架
static int g_test_count = 0;
static int g_test_passed = 0;
static int g_assert_count = 0;
static int g_assert_passed = 0;

#define TEST_START(name) \
    do { \
        g_test_count++; \
        printf("\n=== Test %d: %s ===\n", g_test_count, name); \
    } while(0)

#define TEST_PASS() \
    do { \
        g_test_passed++; \
        printf("✓ PASSED\n"); \
    } while(0)

#define ASSERT(condition, message) \
    do { \
        g_assert_count++; \
        if (condition) { \
            g_assert_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            printf("  ✗ FAILED: %s\n", message); \
        } \
    } while(0)

#define ASSERT_EQ(a, b, message) \
    do { \
        g_assert_count++; \
        if ((a) == (b)) { \
            g_assert_passed++; \
            printf("  ✓ %s (%d == %d)\n", message, (int)(a), (int)(b)); \
        } else { \
            printf("  ✗ FAILED: %s (%d != %d)\n", message, (int)(a), (int)(b)); \
        } \
    } while(0)

// 测试用例
void test_add_and_get(DBConnectionManager* manager)
{
    TEST_START("添加并获取积分");

    EventDetectiveGoblin record, retrieved;
    memset(&record, 0, sizeof(EventDetectiveGoblin));
    memset(&retrieved, 0, sizeof(EventDetectiveGoblin));

    record.charac_no = 10;
    record.point = 100;

    int ret = EventDetectiveGoblin_Add(manager, &record);
    ASSERT(ret == 0, "添加积分成功");

    ret = EventDetectiveGoblin_Get(manager, 10, &retrieved);
    ASSERT(ret == 0, "获取积分成功");
    ASSERT_EQ(retrieved.charac_no, 10, "角色编号正确");
    ASSERT_EQ(retrieved.point, 100, "SP数量正确");

    EventDetectiveGoblin_Delete(manager, 10);
    TEST_PASS();
}

void test_update(DBConnectionManager* manager)
{
    TEST_START("更新积分");

    EventDetectiveGoblin record, retrieved;
    memset(&record, 0, sizeof(EventDetectiveGoblin));

    record.charac_no = 20;
    record.point = 200;
    EventDetectiveGoblin_Add(manager, &record);

    record.point = 250;
    int ret = EventDetectiveGoblin_Update(manager, &record);
    ASSERT(ret == 0, "更新积分成功");

    EventDetectiveGoblin_Get(manager, 20, &retrieved);
    ASSERT_EQ(retrieved.point, 250, "更新后SP数量正确");

    EventDetectiveGoblin_Delete(manager, 20);
    TEST_PASS();
}

void test_delete(DBConnectionManager* manager)
{
    TEST_START("删除积分");

    EventDetectiveGoblin record;
    memset(&record, 0, sizeof(EventDetectiveGoblin));

    record.charac_no = 30;
    record.point = 300;
    EventDetectiveGoblin_Add(manager, &record);

    int ret = EventDetectiveGoblin_Delete(manager, 30);
    ASSERT(ret == 0, "删除积分成功");

    ret = EventDetectiveGoblin_Exists(manager, 30);
    ASSERT(ret == 0, "删除后记录不存在");

    TEST_PASS();
}

void test_exists(DBConnectionManager* manager)
{
    TEST_START("检查积分是否存在");

    EventDetectiveGoblin record;
    memset(&record, 0, sizeof(EventDetectiveGoblin));

    int ret = EventDetectiveGoblin_Exists(manager, 40);
    ASSERT(ret == 0, "不存在的记录返回0");

    record.charac_no = 40;
    record.point = 400;
    EventDetectiveGoblin_Add(manager, &record);

    ret = EventDetectiveGoblin_Exists(manager, 40);
    ASSERT(ret == 1, "存在的记录返回1");

    EventDetectiveGoblin_Delete(manager, 40);
    TEST_PASS();
}

void test_count(DBConnectionManager* manager)
{
    TEST_START("统计记录数");

    EventDetectiveGoblin record;
    int charac_nos[] = {51, 52, 53};
    int i;

    int initial_count = EventDetectiveGoblin_Count(manager);
    ASSERT(initial_count >= 0, "获取初始记录数成功");

    memset(&record, 0, sizeof(EventDetectiveGoblin));
    for (i = 0; i < 3; i++) {
        record.charac_no = charac_nos[i];
        record.point = (charac_nos[i] - 50) * 100;
        EventDetectiveGoblin_Add(manager, &record);
    }

    int new_count = EventDetectiveGoblin_Count(manager);
    ASSERT_EQ(new_count, initial_count + 3, "记录数增加3条");

    for (i = 0; i < 3; i++) {
        EventDetectiveGoblin_Delete(manager, charac_nos[i]);
    }

    TEST_PASS();
}

void test_get_all(DBConnectionManager* manager)
{
    TEST_START("获取所有记录");

    EventDetectiveGoblin record;
    EventDetectiveGoblin records[10];
    int charac_nos[] = {61, 62, 63};
    int i;

    memset(&record, 0, sizeof(EventDetectiveGoblin));
    for (i = 0; i < 3; i++) {
        record.charac_no = charac_nos[i];
        record.point = (charac_nos[i] - 60) * 100;
        EventDetectiveGoblin_Add(manager, &record);
    }

    int count = EventDetectiveGoblin_GetAll(manager, records, 10);
    ASSERT(count >= 3, "至少获取到3条记录");

    for (i = 0; i < 3; i++) {
        EventDetectiveGoblin_Delete(manager, charac_nos[i]);
    }

    TEST_PASS();
}

void test_batch_delete(DBConnectionManager* manager)
{
    TEST_START("批量删除");

    EventDetectiveGoblin record;
    int charac_nos[] = {71, 72, 73, 74, 75};
    int count = 5;
    int i;

    memset(&record, 0, sizeof(EventDetectiveGoblin));
    for (i = 0; i < count; i++) {
        record.charac_no = charac_nos[i];
        record.point = (charac_nos[i] - 70) * 100;
        EventDetectiveGoblin_Add(manager, &record);
    }

    int ret = EventDetectiveGoblin_BatchDelete(manager, charac_nos, count);
    ASSERT(ret == 0, "批量删除成功");

    for (i = 0; i < count; i++) {
        int exists = EventDetectiveGoblin_Exists(manager, charac_nos[i]);
        ASSERT(exists == 0, "记录已删除");
    }

    TEST_PASS();
}

void test_replace_into_idempotent(DBConnectionManager* manager)
{
    TEST_START("REPLACE INTO 幂等性测试");

    EventDetectiveGoblin record, retrieved;
    memset(&record, 0, sizeof(EventDetectiveGoblin));

    record.charac_no = 80;
    record.point = 800;
    int ret = EventDetectiveGoblin_Add(manager, &record);
    ASSERT(ret == 0, "第一次添加成功");

    record.point = 850;
    ret = EventDetectiveGoblin_Add(manager, &record);
    ASSERT(ret == 0, "第二次添加成功（幂等操作）");

    EventDetectiveGoblin_Get(manager, 80, &retrieved);
    ASSERT_EQ(retrieved.point, 850, "数据为最新数据");

    int count_before = EventDetectiveGoblin_Count(manager);
    record.point = 900;
    EventDetectiveGoblin_Add(manager, &record);
    int count_after = EventDetectiveGoblin_Count(manager);
    ASSERT_EQ(count_before, count_after, "记录数不变（未重复插入）");

    EventDetectiveGoblin_Delete(manager, 80);
    TEST_PASS();
}

void test_charac_no_range(DBConnectionManager* manager)
{
    TEST_START("角色编号范围测试");

    EventDetectiveGoblin record, retrieved;
    memset(&record, 0, sizeof(EventDetectiveGoblin));

    record.charac_no = 1;
    record.point = 10;
    int ret = EventDetectiveGoblin_Add(manager, &record);
    ASSERT(ret == 0, "添加最小角色编号成功");

    record.charac_no = 999;
    record.point = 9990;
    ret = EventDetectiveGoblin_Add(manager, &record);
    ASSERT(ret == 0, "添加最大角色编号成功");

    EventDetectiveGoblin_Get(manager, 1, &retrieved);
    ASSERT_EQ(retrieved.point, 10, "最小角色编号SP正确");

    EventDetectiveGoblin_Get(manager, 999, &retrieved);
    ASSERT_EQ(retrieved.point, 9990, "最大角色编号SP正确");

    EventDetectiveGoblin_Delete(manager, 1);
    EventDetectiveGoblin_Delete(manager, 999);
    TEST_PASS();
}

// 主函数
int main(int argc, char** argv)
{
    DBConnectionManager manager;
    int ret;

    if (argc < 2) {
        printf("Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    printf("==========================================\n");
    printf("event_detective_goblin ORM 测试\n");
    printf("==========================================\n");

    memset(&manager, 0, sizeof(DBConnectionManager));
    ret = DBConnectionManager_Initialize(&manager, argv[1]);
    if (ret < 0) {
        printf("Failed to initialize database connection\n");
        return 1;
    }

    ret = DBConnectionManager_Connect(&manager, DB_TYPE_CAIN);
    if (ret < 0) {
        printf("Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    test_add_and_get(&manager);
    test_update(&manager);
    test_delete(&manager);
    test_exists(&manager);
    test_count(&manager);
    test_get_all(&manager);
    test_batch_delete(&manager);
    test_replace_into_idempotent(&manager);
    test_charac_no_range(&manager);

    DBConnectionManager_Cleanup(&manager);

    printf("\n==========================================\n");
    printf("测试结果汇总\n");
    printf("==========================================\n");
    printf("测试用例: %d/%d 通过\n", g_test_passed, g_test_count);
    printf("断言: %d/%d 通过\n", g_assert_passed, g_assert_count);
    printf("成功率: %.1f%%\n", (g_test_count > 0) ? (g_test_passed * 100.0 / g_test_count) : 0);
    printf("==========================================\n");

    if (g_test_passed == g_test_count && g_assert_passed == g_assert_count) {
        printf("✓ 所有测试通过！\n");
        return 0;
    } else {
        printf("✗ 部分测试失败\n");
        return 1;
    }
}
