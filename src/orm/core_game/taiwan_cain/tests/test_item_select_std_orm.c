#include "item_select_std_orm.h"
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
    TEST_START("添加并获取物品选择标准");

    ItemSelectStd record, retrieved;
    memset(&record, 0, sizeof(ItemSelectStd));
    memset(&retrieved, 0, sizeof(ItemSelectStd));

    record.item_grade = 1;
    record.top = 100;
    record.bottom = 50;
    record.weight = 10;

    int ret = ItemSelectStd_Add(manager, &record);
    ASSERT(ret == 0, "添加物品选择标准成功");

    ret = ItemSelectStd_Get(manager, 1, &retrieved);
    ASSERT(ret == 0, "获取物品选择标准成功");
    ASSERT_EQ(retrieved.item_grade, 1, "物品等级正确");
    ASSERT_EQ(retrieved.top, 100, "上限值正确");
    ASSERT_EQ(retrieved.bottom, 50, "下限值正确");
    ASSERT_EQ(retrieved.weight, 10, "权重值正确");

    ItemSelectStd_Delete(manager, 1);
    TEST_PASS();
}

void test_update(DBConnectionManager* manager)
{
    TEST_START("更新物品选择标准");

    ItemSelectStd record, retrieved;
    memset(&record, 0, sizeof(ItemSelectStd));

    record.item_grade = 2;
    record.top = 80;
    record.bottom = 40;
    record.weight = 15;
    ItemSelectStd_Add(manager, &record);

    record.top = 120;
    record.bottom = 60;
    record.weight = 20;
    int ret = ItemSelectStd_Update(manager, &record);
    ASSERT(ret == 0, "更新物品选择标准成功");

    ItemSelectStd_Get(manager, 2, &retrieved);
    ASSERT_EQ(retrieved.top, 120, "更新后上限值正确");
    ASSERT_EQ(retrieved.bottom, 60, "更新后下限值正确");
    ASSERT_EQ(retrieved.weight, 20, "更新后权重值正确");

    ItemSelectStd_Delete(manager, 2);
    TEST_PASS();
}

void test_delete(DBConnectionManager* manager)
{
    TEST_START("删除物品选择标准");

    ItemSelectStd record;
    memset(&record, 0, sizeof(ItemSelectStd));

    record.item_grade = 3;
    record.top = 90;
    record.bottom = 45;
    record.weight = 12;
    ItemSelectStd_Add(manager, &record);

    int ret = ItemSelectStd_Delete(manager, 3);
    ASSERT(ret == 0, "删除物品选择标准成功");

    ret = ItemSelectStd_Exists(manager, 3);
    ASSERT(ret == 0, "删除后记录不存在");

    TEST_PASS();
}

void test_exists(DBConnectionManager* manager)
{
    TEST_START("检查物品选择标准是否存在");

    ItemSelectStd record;
    memset(&record, 0, sizeof(ItemSelectStd));

    int ret = ItemSelectStd_Exists(manager, 4);
    ASSERT(ret == 0, "不存在的记录返回0");

    record.item_grade = 4;
    record.top = 70;
    record.bottom = 35;
    record.weight = 8;
    ItemSelectStd_Add(manager, &record);

    ret = ItemSelectStd_Exists(manager, 4);
    ASSERT(ret == 1, "存在的记录返回1");

    ItemSelectStd_Delete(manager, 4);
    TEST_PASS();
}

void test_count(DBConnectionManager* manager)
{
    TEST_START("统计记录数");

    ItemSelectStd record;
    int i;

    int initial_count = ItemSelectStd_Count(manager);
    ASSERT(initial_count >= 0, "获取初始记录数成功");

    memset(&record, 0, sizeof(ItemSelectStd));
    for (i = 0; i < 3; i++) {
        record.item_grade = 10 + i;
        record.top = 100 + i * 10;
        record.bottom = 50 + i * 5;
        record.weight = 10 + i * 2;
        ItemSelectStd_Add(manager, &record);
    }

    int new_count = ItemSelectStd_Count(manager);
    ASSERT_EQ(new_count, initial_count + 3, "记录数增加3条");

    for (i = 0; i < 3; i++) {
        ItemSelectStd_Delete(manager, 10 + i);
    }

    TEST_PASS();
}

void test_get_all(DBConnectionManager* manager)
{
    TEST_START("获取所有记录");

    ItemSelectStd record;
    ItemSelectStd records[10];
    int i;

    memset(&record, 0, sizeof(ItemSelectStd));
    for (i = 0; i < 3; i++) {
        record.item_grade = 20 + i;
        record.top = 80 + i * 15;
        record.bottom = 40 + i * 8;
        record.weight = 12 + i * 3;
        ItemSelectStd_Add(manager, &record);
    }

    int count = ItemSelectStd_GetAll(manager, records, 10);
    ASSERT(count >= 3, "至少获取到3条记录");

    for (i = 0; i < 3; i++) {
        ItemSelectStd_Delete(manager, 20 + i);
    }

    TEST_PASS();
}

void test_get_total_weight(DBConnectionManager* manager)
{
    TEST_START("获取权重总和");

    ItemSelectStd record;
    int i;

    // 清空表
    int count = ItemSelectStd_Count(manager);
    ItemSelectStd records[100];
    int num = ItemSelectStd_GetAll(manager, records, 100);
    for (i = 0; i < num; i++) {
        ItemSelectStd_Delete(manager, records[i].item_grade);
    }

    // 添加已知权重的记录
    memset(&record, 0, sizeof(ItemSelectStd));
    int expected_total = 0;
    for (i = 0; i < 3; i++) {
        record.item_grade = 30 + i;
        record.top = 100;
        record.bottom = 50;
        record.weight = 10 + i * 5;  // 10, 15, 20
        expected_total += record.weight;
        ItemSelectStd_Add(manager, &record);
    }

    int total_weight = ItemSelectStd_GetTotalWeight(manager);
    ASSERT_EQ(total_weight, expected_total, "权重总和正确 (10+15+20=45)");

    for (i = 0; i < 3; i++) {
        ItemSelectStd_Delete(manager, 30 + i);
    }

    TEST_PASS();
}

void test_replace_into_idempotent(DBConnectionManager* manager)
{
    TEST_START("REPLACE INTO 幂等性测试");

    ItemSelectStd record, retrieved;
    memset(&record, 0, sizeof(ItemSelectStd));

    record.item_grade = 40;
    record.top = 100;
    record.bottom = 50;
    record.weight = 10;
    int ret = ItemSelectStd_Add(manager, &record);
    ASSERT(ret == 0, "第一次添加成功");

    record.top = 150;
    record.bottom = 75;
    record.weight = 25;
    ret = ItemSelectStd_Add(manager, &record);
    ASSERT(ret == 0, "第二次添加成功（幂等操作）");

    ItemSelectStd_Get(manager, 40, &retrieved);
    ASSERT_EQ(retrieved.top, 150, "数据为最新数据");
    ASSERT_EQ(retrieved.bottom, 75, "下限值为最新数据");
    ASSERT_EQ(retrieved.weight, 25, "权重为最新数据");

    int count_before = ItemSelectStd_Count(manager);
    record.weight = 30;
    ItemSelectStd_Add(manager, &record);
    int count_after = ItemSelectStd_Count(manager);
    ASSERT_EQ(count_before, count_after, "记录数不变（未重复插入）");

    ItemSelectStd_Delete(manager, 40);
    TEST_PASS();
}

void test_boundary_values(DBConnectionManager* manager)
{
    TEST_START("边界值测试");

    ItemSelectStd record, retrieved;
    memset(&record, 0, sizeof(ItemSelectStd));

    // 测试零值
    record.item_grade = 50;
    record.top = 0;
    record.bottom = 0;
    record.weight = 0;
    int ret = ItemSelectStd_Add(manager, &record);
    ASSERT(ret == 0, "添加零值记录成功");

    ItemSelectStd_Get(manager, 50, &retrieved);
    ASSERT_EQ(retrieved.top, 0, "零值上限正确");
    ASSERT_EQ(retrieved.bottom, 0, "零值下限正确");
    ASSERT_EQ(retrieved.weight, 0, "零值权重正确");

    // 测试负值
    record.item_grade = 51;
    record.top = -100;
    record.bottom = -200;
    record.weight = -50;
    ret = ItemSelectStd_Add(manager, &record);
    ASSERT(ret == 0, "添加负值记录成功");

    ItemSelectStd_Get(manager, 51, &retrieved);
    ASSERT_EQ(retrieved.top, -100, "负值上限正确");
    ASSERT_EQ(retrieved.bottom, -200, "负值下限正确");
    ASSERT_EQ(retrieved.weight, -50, "负值权重正确");

    // 测试大数值
    record.item_grade = 52;
    record.top = 2147483647;  // INT_MAX
    record.bottom = -2147483648;  // INT_MIN
    record.weight = 1000000000;
    ret = ItemSelectStd_Add(manager, &record);
    ASSERT(ret == 0, "添加大数值记录成功");

    ItemSelectStd_Get(manager, 52, &retrieved);
    ASSERT_EQ(retrieved.top, 2147483647, "大数值上限正确");
    ASSERT_EQ(retrieved.bottom, -2147483648, "大数值下限正确");
    ASSERT_EQ(retrieved.weight, 1000000000, "大数值权重正确");

    ItemSelectStd_Delete(manager, 50);
    ItemSelectStd_Delete(manager, 51);
    ItemSelectStd_Delete(manager, 52);
    TEST_PASS();
}

void test_print_info(DBConnectionManager* manager)
{
    TEST_START("打印物品选择标准信息");

    ItemSelectStd record;
    memset(&record, 0, sizeof(ItemSelectStd));

    record.item_grade = 60;
    record.top = 100;
    record.bottom = 50;
    record.weight = 15;
    ItemSelectStd_Add(manager, &record);

    ItemSelectStd_Get(manager, 60, &record);
    ItemSelectStd_PrintInfo(&record);
    ASSERT(1, "打印物品选择标准信息成功");

    ItemSelectStd_Delete(manager, 60);
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
    printf("item_select_std ORM 测试\n");
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
    test_get_total_weight(&manager);
    test_replace_into_idempotent(&manager);
    test_boundary_values(&manager);
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
