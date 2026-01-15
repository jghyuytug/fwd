#include "money_gen_ref_orm.h"
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
    TEST_START("添加并获取金钱生成配置");

    MoneyGenRef record, retrieved;
    memset(&record, 0, sizeof(MoneyGenRef));
    memset(&retrieved, 0, sizeof(MoneyGenRef));

    record.grade = 10;
    record.bottom_grade = 1;
    record.money = 1000;
    record.random_value = 100;

    int ret = MoneyGenRef_Add(manager, &record);
    ASSERT(ret == 0, "添加金钱生成配置成功");

    ret = MoneyGenRef_Get(manager, 10, 1, &retrieved);
    ASSERT(ret == 0, "获取金钱生成配置成功");
    ASSERT_EQ(retrieved.grade, 10, "等级正确");
    ASSERT_EQ(retrieved.bottom_grade, 1, "底层等级正确");
    ASSERT_EQ(retrieved.money, 1000, "金钱数量正确");
    ASSERT_EQ(retrieved.random_value, 100, "随机值正确");

    MoneyGenRef_Delete(manager, 10, 1);
    TEST_PASS();
}

void test_update(DBConnectionManager* manager)
{
    TEST_START("更新金钱生成配置");

    MoneyGenRef record, retrieved;
    memset(&record, 0, sizeof(MoneyGenRef));

    record.grade = 20;
    record.bottom_grade = 5;
    record.money = 2000;
    record.random_value = 200;
    MoneyGenRef_Add(manager, &record);

    record.money = 2500;
    record.random_value = 250;
    int ret = MoneyGenRef_Update(manager, &record);
    ASSERT(ret == 0, "更新金钱生成配置成功");

    MoneyGenRef_Get(manager, 20, 5, &retrieved);
    ASSERT_EQ(retrieved.money, 2500, "更新后金钱数量正确");
    ASSERT_EQ(retrieved.random_value, 250, "更新后随机值正确");

    MoneyGenRef_Delete(manager, 20, 5);
    TEST_PASS();
}

void test_delete(DBConnectionManager* manager)
{
    TEST_START("删除金钱生成配置");

    MoneyGenRef record;
    memset(&record, 0, sizeof(MoneyGenRef));

    record.grade = 30;
    record.bottom_grade = 10;
    record.money = 3000;
    record.random_value = 300;
    MoneyGenRef_Add(manager, &record);

    int ret = MoneyGenRef_Delete(manager, 30, 10);
    ASSERT(ret == 0, "删除金钱生成配置成功");

    ret = MoneyGenRef_Exists(manager, 30, 10);
    ASSERT(ret == 0, "删除后记录不存在");

    TEST_PASS();
}

void test_exists(DBConnectionManager* manager)
{
    TEST_START("检查金钱生成配置是否存在");

    MoneyGenRef record;
    memset(&record, 0, sizeof(MoneyGenRef));

    int ret = MoneyGenRef_Exists(manager, 40, 15);
    ASSERT(ret == 0, "不存在的记录返回0");

    record.grade = 40;
    record.bottom_grade = 15;
    record.money = 4000;
    record.random_value = 400;
    MoneyGenRef_Add(manager, &record);

    ret = MoneyGenRef_Exists(manager, 40, 15);
    ASSERT(ret == 1, "存在的记录返回1");

    MoneyGenRef_Delete(manager, 40, 15);
    TEST_PASS();
}

void test_count(DBConnectionManager* manager)
{
    TEST_START("统计记录数");

    MoneyGenRef record;
    int i;

    int initial_count = MoneyGenRef_Count(manager);
    ASSERT(initial_count >= 0, "获取初始记录数成功");

    memset(&record, 0, sizeof(MoneyGenRef));
    for (i = 0; i < 3; i++) {
        record.grade = 50 + i;
        record.bottom_grade = 20 + i;
        record.money = (50 + i) * 100;
        record.random_value = (50 + i) * 10;
        MoneyGenRef_Add(manager, &record);
    }

    int new_count = MoneyGenRef_Count(manager);
    ASSERT_EQ(new_count, initial_count + 3, "记录数增加3条");

    for (i = 0; i < 3; i++) {
        MoneyGenRef_Delete(manager, 50 + i, 20 + i);
    }

    TEST_PASS();
}

void test_get_all(DBConnectionManager* manager)
{
    TEST_START("获取所有记录");

    MoneyGenRef record;
    MoneyGenRef records[10];
    int i;

    memset(&record, 0, sizeof(MoneyGenRef));
    for (i = 0; i < 3; i++) {
        record.grade = 60 + i;
        record.bottom_grade = 25 + i;
        record.money = (60 + i) * 100;
        record.random_value = (60 + i) * 10;
        MoneyGenRef_Add(manager, &record);
    }

    int count = MoneyGenRef_GetAll(manager, records, 10);
    ASSERT(count >= 3, "至少获取到3条记录");

    for (i = 0; i < 3; i++) {
        MoneyGenRef_Delete(manager, 60 + i, 25 + i);
    }

    TEST_PASS();
}

void test_get_by_grade(DBConnectionManager* manager)
{
    TEST_START("按等级获取配置");

    MoneyGenRef record;
    MoneyGenRef records[10];
    int i;

    memset(&record, 0, sizeof(MoneyGenRef));
    for (i = 0; i < 3; i++) {
        record.grade = 70;
        record.bottom_grade = 30 + i;
        record.money = 7000 + i * 100;
        record.random_value = 700 + i * 10;
        MoneyGenRef_Add(manager, &record);
    }

    int count = MoneyGenRef_GetByGrade(manager, 70, records, 10);
    ASSERT_EQ(count, 3, "获取到3条等级70的配置");

    for (i = 0; i < count; i++) {
        ASSERT_EQ(records[i].grade, 70, "等级正确");
    }

    for (i = 0; i < 3; i++) {
        MoneyGenRef_Delete(manager, 70, 30 + i);
    }

    TEST_PASS();
}

void test_count_by_grade(DBConnectionManager* manager)
{
    TEST_START("统计指定等级的配置数量");

    MoneyGenRef record;
    int i;

    int initial_count = MoneyGenRef_CountByGrade(manager, 80);
    ASSERT(initial_count >= 0, "获取初始等级80配置数成功");

    memset(&record, 0, sizeof(MoneyGenRef));
    for (i = 0; i < 4; i++) {
        record.grade = 80;
        record.bottom_grade = 35 + i;
        record.money = 8000 + i * 100;
        record.random_value = 800 + i * 10;
        MoneyGenRef_Add(manager, &record);
    }

    int new_count = MoneyGenRef_CountByGrade(manager, 80);
    ASSERT_EQ(new_count, initial_count + 4, "等级80配置数增加4条");

    for (i = 0; i < 4; i++) {
        MoneyGenRef_Delete(manager, 80, 35 + i);
    }

    TEST_PASS();
}

void test_batch_delete(DBConnectionManager* manager)
{
    TEST_START("批量删除");

    MoneyGenRef records[5];
    int count = 5;
    int i;

    memset(records, 0, sizeof(records));
    for (i = 0; i < count; i++) {
        records[i].grade = 90 + i;
        records[i].bottom_grade = 40 + i;
        records[i].money = (90 + i) * 100;
        records[i].random_value = (90 + i) * 10;
        MoneyGenRef_Add(manager, &records[i]);
    }

    int ret = MoneyGenRef_BatchDelete(manager, records, count);
    ASSERT(ret == 0, "批量删除成功");

    for (i = 0; i < count; i++) {
        int exists = MoneyGenRef_Exists(manager, records[i].grade, records[i].bottom_grade);
        ASSERT(exists == 0, "记录已删除");
    }

    TEST_PASS();
}

void test_replace_into_idempotent(DBConnectionManager* manager)
{
    TEST_START("REPLACE INTO 幂等性测试");

    MoneyGenRef record, retrieved;
    memset(&record, 0, sizeof(MoneyGenRef));

    record.grade = 100;
    record.bottom_grade = 50;
    record.money = 10000;
    record.random_value = 1000;
    int ret = MoneyGenRef_Add(manager, &record);
    ASSERT(ret == 0, "第一次添加成功");

    record.money = 15000;
    record.random_value = 1500;
    ret = MoneyGenRef_Add(manager, &record);
    ASSERT(ret == 0, "第二次添加成功（幂等操作）");

    MoneyGenRef_Get(manager, 100, 50, &retrieved);
    ASSERT_EQ(retrieved.money, 15000, "数据为最新数据");
    ASSERT_EQ(retrieved.random_value, 1500, "随机值为最新数据");

    int count_before = MoneyGenRef_Count(manager);
    record.money = 20000;
    MoneyGenRef_Add(manager, &record);
    int count_after = MoneyGenRef_Count(manager);
    ASSERT_EQ(count_before, count_after, "记录数不变（未重复插入）");

    MoneyGenRef_Delete(manager, 100, 50);
    TEST_PASS();
}

void test_grade_range(DBConnectionManager* manager)
{
    TEST_START("等级范围测试");

    MoneyGenRef record, retrieved;
    memset(&record, 0, sizeof(MoneyGenRef));

    record.grade = 1;
    record.bottom_grade = 0;
    record.money = 10;
    record.random_value = 1;
    int ret = MoneyGenRef_Add(manager, &record);
    ASSERT(ret == 0, "添加最低等级成功");

    record.grade = 999;
    record.bottom_grade = 999;
    record.money = 999999;
    record.random_value = 9999;
    ret = MoneyGenRef_Add(manager, &record);
    ASSERT(ret == 0, "添加最高等级成功");

    MoneyGenRef_Get(manager, 1, 0, &retrieved);
    ASSERT_EQ(retrieved.money, 10, "最低等级金钱数量正确");

    MoneyGenRef_Get(manager, 999, 999, &retrieved);
    ASSERT_EQ(retrieved.money, 999999, "最高等级金钱数量正确");

    MoneyGenRef_Delete(manager, 1, 0);
    MoneyGenRef_Delete(manager, 999, 999);
    TEST_PASS();
}

void test_print_info(DBConnectionManager* manager)
{
    TEST_START("打印配置信息");

    MoneyGenRef record;
    memset(&record, 0, sizeof(MoneyGenRef));

    record.grade = 110;
    record.bottom_grade = 55;
    record.money = 11000;
    record.random_value = 1100;
    MoneyGenRef_Add(manager, &record);

    MoneyGenRef_Get(manager, 110, 55, &record);
    MoneyGenRef_PrintInfo(&record);
    ASSERT(1, "打印配置信息成功");

    MoneyGenRef_Delete(manager, 110, 55);
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
    printf("money_gen_ref ORM 测试\n");
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
    test_get_by_grade(&manager);
    test_count_by_grade(&manager);
    test_batch_delete(&manager);
    test_replace_into_idempotent(&manager);
    test_grade_range(&manager);
    test_print_info(&manager);

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
