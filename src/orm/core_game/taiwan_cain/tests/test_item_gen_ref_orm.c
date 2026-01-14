#include "item_gen_ref_orm.h"
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
    TEST_START("添加并获取物品生成配置");

    ItemGenRef record, retrieved;
    memset(&record, 0, sizeof(ItemGenRef));
    memset(&retrieved, 0, sizeof(ItemGenRef));

    record.item_grade = 1;
    record.rate_type = 1;
    record.money_rate = 1000;
    record.item_rate = 2000;
    record.free_rate = 500;

    int ret = ItemGenRef_Add(manager, &record);
    ASSERT(ret == 0, "添加物品生成配置成功");

    ret = ItemGenRef_Get(manager, 1, 1, &retrieved);
    ASSERT(ret == 0, "获取物品生成配置成功");
    ASSERT_EQ(retrieved.item_grade, 1, "物品等级正确");
    ASSERT_EQ(retrieved.rate_type, 1, "掉落类型正确");
    ASSERT_EQ(retrieved.money_rate, 1000, "金钱概率正确");
    ASSERT_EQ(retrieved.item_rate, 2000, "物品概率正确");
    ASSERT_EQ(retrieved.free_rate, 500, "免费概率正确");

    ItemGenRef_Delete(manager, 1, 1);
    TEST_PASS();
}

void test_update(DBConnectionManager* manager)
{
    TEST_START("更新物品生成配置");

    ItemGenRef record, retrieved;
    memset(&record, 0, sizeof(ItemGenRef));

    record.item_grade = 2;
    record.rate_type = 1;
    record.money_rate = 1500;
    record.item_rate = 2500;
    record.free_rate = 600;
    ItemGenRef_Add(manager, &record);

    record.money_rate = 1800;
    record.item_rate = 2800;
    record.free_rate = 700;
    int ret = ItemGenRef_Update(manager, &record);
    ASSERT(ret == 0, "更新物品生成配置成功");

    ItemGenRef_Get(manager, 2, 1, &retrieved);
    ASSERT_EQ(retrieved.money_rate, 1800, "更新后金钱概率正确");
    ASSERT_EQ(retrieved.item_rate, 2800, "更新后物品概率正确");
    ASSERT_EQ(retrieved.free_rate, 700, "更新后免费概率正确");

    ItemGenRef_Delete(manager, 2, 1);
    TEST_PASS();
}

void test_delete(DBConnectionManager* manager)
{
    TEST_START("删除物品生成配置");

    ItemGenRef record;
    memset(&record, 0, sizeof(ItemGenRef));

    record.item_grade = 3;
    record.rate_type = 1;
    record.money_rate = 2000;
    record.item_rate = 3000;
    record.free_rate = 800;
    ItemGenRef_Add(manager, &record);

    int ret = ItemGenRef_Delete(manager, 3, 1);
    ASSERT(ret == 0, "删除物品生成配置成功");

    ret = ItemGenRef_Exists(manager, 3, 1);
    ASSERT(ret == 0, "删除后记录不存在");

    TEST_PASS();
}

void test_exists(DBConnectionManager* manager)
{
    TEST_START("检查物品生成配置是否存在");

    ItemGenRef record;
    memset(&record, 0, sizeof(ItemGenRef));

    int ret = ItemGenRef_Exists(manager, 4, 1);
    ASSERT(ret == 0, "不存在的记录返回0");

    record.item_grade = 4;
    record.rate_type = 1;
    record.money_rate = 2500;
    record.item_rate = 3500;
    record.free_rate = 900;
    ItemGenRef_Add(manager, &record);

    ret = ItemGenRef_Exists(manager, 4, 1);
    ASSERT(ret == 1, "存在的记录返回1");

    ItemGenRef_Delete(manager, 4, 1);
    TEST_PASS();
}

void test_count(DBConnectionManager* manager)
{
    TEST_START("统计记录数");

    ItemGenRef record;
    int i;

    int initial_count = ItemGenRef_Count(manager);
    ASSERT(initial_count >= 0, "获取初始记录数成功");

    memset(&record, 0, sizeof(ItemGenRef));
    for (i = 0; i < 3; i++) {
        record.item_grade = 5;
        record.rate_type = i + 1;
        record.money_rate = (i + 1) * 1000;
        record.item_rate = (i + 1) * 2000;
        record.free_rate = (i + 1) * 500;
        ItemGenRef_Add(manager, &record);
    }

    int new_count = ItemGenRef_Count(manager);
    ASSERT_EQ(new_count, initial_count + 3, "记录数增加3条");

    for (i = 0; i < 3; i++) {
        ItemGenRef_Delete(manager, 5, i + 1);
    }

    TEST_PASS();
}

void test_get_all(DBConnectionManager* manager)
{
    TEST_START("获取所有记录");

    ItemGenRef record;
    ItemGenRef records[10];
    int i;

    memset(&record, 0, sizeof(ItemGenRef));
    for (i = 0; i < 3; i++) {
        record.item_grade = 6;
        record.rate_type = i + 1;
        record.money_rate = (i + 1) * 1200;
        record.item_rate = (i + 1) * 2200;
        record.free_rate = (i + 1) * 550;
        ItemGenRef_Add(manager, &record);
    }

    int count = ItemGenRef_GetAll(manager, records, 10);
    ASSERT(count >= 3, "至少获取到3条记录");

    for (i = 0; i < 3; i++) {
        ItemGenRef_Delete(manager, 6, i + 1);
    }

    TEST_PASS();
}

void test_get_by_grade(DBConnectionManager* manager)
{
    TEST_START("按物品等级获取配置");

    ItemGenRef record;
    ItemGenRef records[10];
    int i;

    memset(&record, 0, sizeof(ItemGenRef));
    for (i = 0; i < 4; i++) {
        record.item_grade = 7;
        record.rate_type = i + 1;
        record.money_rate = (i + 1) * 1300;
        record.item_rate = (i + 1) * 2300;
        record.free_rate = (i + 1) * 600;
        ItemGenRef_Add(manager, &record);
    }

    int count = ItemGenRef_GetByGrade(manager, 7, records, 10);
    ASSERT_EQ(count, 4, "获取到4条等级7的配置");

    for (i = 0; i < count; i++) {
        ASSERT_EQ(records[i].item_grade, 7, "物品等级正确");
    }

    for (i = 0; i < 4; i++) {
        ItemGenRef_Delete(manager, 7, i + 1);
    }

    TEST_PASS();
}

void test_count_by_grade(DBConnectionManager* manager)
{
    TEST_START("统计指定物品等级的配置数量");

    ItemGenRef record;
    int i;

    int initial_count = ItemGenRef_CountByGrade(manager, 8);
    ASSERT(initial_count >= 0, "获取初始等级8配置数成功");

    memset(&record, 0, sizeof(ItemGenRef));
    for (i = 0; i < 5; i++) {
        record.item_grade = 8;
        record.rate_type = i + 1;
        record.money_rate = (i + 1) * 1400;
        record.item_rate = (i + 1) * 2400;
        record.free_rate = (i + 1) * 650;
        ItemGenRef_Add(manager, &record);
    }

    int new_count = ItemGenRef_CountByGrade(manager, 8);
    ASSERT_EQ(new_count, initial_count + 5, "等级8配置数增加5条");

    for (i = 0; i < 5; i++) {
        ItemGenRef_Delete(manager, 8, i + 1);
    }

    TEST_PASS();
}

void test_batch_delete(DBConnectionManager* manager)
{
    TEST_START("批量删除");

    ItemGenRef records[5];
    int count = 5;
    int i;

    memset(records, 0, sizeof(records));
    for (i = 0; i < count; i++) {
        records[i].item_grade = 9;
        records[i].rate_type = i + 1;
        records[i].money_rate = (i + 1) * 1500;
        records[i].item_rate = (i + 1) * 2500;
        records[i].free_rate = (i + 1) * 700;
        ItemGenRef_Add(manager, &records[i]);
    }

    int ret = ItemGenRef_BatchDelete(manager, records, count);
    ASSERT(ret == 0, "批量删除成功");

    for (i = 0; i < count; i++) {
        int exists = ItemGenRef_Exists(manager, records[i].item_grade, records[i].rate_type);
        ASSERT(exists == 0, "记录已删除");
    }

    TEST_PASS();
}

void test_replace_into_idempotent(DBConnectionManager* manager)
{
    TEST_START("REPLACE INTO 幂等性测试");

    ItemGenRef record, retrieved;
    memset(&record, 0, sizeof(ItemGenRef));

    record.item_grade = 10;
    record.rate_type = 1;
    record.money_rate = 1600;
    record.item_rate = 2600;
    record.free_rate = 750;
    int ret = ItemGenRef_Add(manager, &record);
    ASSERT(ret == 0, "第一次添加成功");

    record.money_rate = 1900;
    record.item_rate = 2900;
    record.free_rate = 850;
    ret = ItemGenRef_Add(manager, &record);
    ASSERT(ret == 0, "第二次添加成功（幂等操作）");

    ItemGenRef_Get(manager, 10, 1, &retrieved);
    ASSERT_EQ(retrieved.money_rate, 1900, "数据为最新数据");
    ASSERT_EQ(retrieved.item_rate, 2900, "物品概率为最新数据");
    ASSERT_EQ(retrieved.free_rate, 850, "免费概率为最新数据");

    int count_before = ItemGenRef_Count(manager);
    record.money_rate = 2000;
    ItemGenRef_Add(manager, &record);
    int count_after = ItemGenRef_Count(manager);
    ASSERT_EQ(count_before, count_after, "记录数不变（未重复插入）");

    ItemGenRef_Delete(manager, 10, 1);
    TEST_PASS();
}

void test_grade_range(DBConnectionManager* manager)
{
    TEST_START("等级范围测试");

    ItemGenRef record, retrieved;
    memset(&record, 0, sizeof(ItemGenRef));

    record.item_grade = 1;
    record.rate_type = 1;
    record.money_rate = 100;
    record.item_rate = 200;
    record.free_rate = 50;
    int ret = ItemGenRef_Add(manager, &record);
    ASSERT(ret == 0, "添加最低等级成功");

    record.item_grade = 127;
    record.rate_type = 127;
    record.money_rate = 30000;
    record.item_rate = 30000;
    record.free_rate = 30000;
    ret = ItemGenRef_Add(manager, &record);
    ASSERT(ret == 0, "添加最高等级成功");

    ItemGenRef_Get(manager, 1, 1, &retrieved);
    ASSERT_EQ(retrieved.money_rate, 100, "最低等级金钱概率正确");

    ItemGenRef_Get(manager, 127, 127, &retrieved);
    ASSERT_EQ(retrieved.money_rate, 30000, "最高等级金钱概率正确");

    ItemGenRef_Delete(manager, 1, 1);
    ItemGenRef_Delete(manager, 127, 127);
    TEST_PASS();
}

void test_print_info(DBConnectionManager* manager)
{
    TEST_START("打印配置信息");

    ItemGenRef record;
    memset(&record, 0, sizeof(ItemGenRef));

    record.item_grade = 11;
    record.rate_type = 2;
    record.money_rate = 1700;
    record.item_rate = 2700;
    record.free_rate = 800;
    ItemGenRef_Add(manager, &record);

    ItemGenRef_Get(manager, 11, 2, &record);
    ItemGenRef_PrintInfo(&record);
    ASSERT(1, "打印配置信息成功");

    ItemGenRef_Delete(manager, 11, 2);
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
    printf("item_gen_ref ORM 测试\n");
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
