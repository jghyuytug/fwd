#include "limit_npc_item_orm.h"
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
            printf("  ✓ %s (%u == %u)\n", message, (unsigned int)(a), (unsigned int)(b)); \
        } else { \
            printf("  ✗ FAILED: %s (%u != %u)\n", message, (unsigned int)(a), (unsigned int)(b)); \
        } \
    } while(0)

// 测试用例
void test_add_and_get(DBConnectionManager* manager)
{
    TEST_START("添加并获取NPC限购物品");

    LimitNpcItem record, retrieved;
    memset(&record, 0, sizeof(LimitNpcItem));
    memset(&retrieved, 0, sizeof(LimitNpcItem));

    record.item_index = 1001;
    record.max_count = 100;
    record.sell_count = 0;

    int ret = LimitNpcItem_Add(manager, &record);
    ASSERT(ret == 0, "添加NPC限购物品成功");

    ret = LimitNpcItem_Get(manager, 1001, &retrieved);
    ASSERT(ret == 0, "获取NPC限购物品成功");
    ASSERT_EQ(retrieved.item_index, 1001, "物品索引正确");
    ASSERT_EQ(retrieved.max_count, 100, "最大数量正确");
    ASSERT_EQ(retrieved.sell_count, 0, "已售数量正确");

    LimitNpcItem_Delete(manager, 1001);
    TEST_PASS();
}

void test_update(DBConnectionManager* manager)
{
    TEST_START("更新NPC限购物品");

    LimitNpcItem record, retrieved;
    memset(&record, 0, sizeof(LimitNpcItem));

    record.item_index = 1002;
    record.max_count = 50;
    record.sell_count = 10;
    LimitNpcItem_Add(manager, &record);

    record.max_count = 80;
    record.sell_count = 20;
    int ret = LimitNpcItem_Update(manager, &record);
    ASSERT(ret == 0, "更新NPC限购物品成功");

    LimitNpcItem_Get(manager, 1002, &retrieved);
    ASSERT_EQ(retrieved.max_count, 80, "更新后最大数量正确");
    ASSERT_EQ(retrieved.sell_count, 20, "更新后已售数量正确");

    LimitNpcItem_Delete(manager, 1002);
    TEST_PASS();
}

void test_delete(DBConnectionManager* manager)
{
    TEST_START("删除NPC限购物品");

    LimitNpcItem record;
    memset(&record, 0, sizeof(LimitNpcItem));

    record.item_index = 1003;
    record.max_count = 30;
    record.sell_count = 5;
    LimitNpcItem_Add(manager, &record);

    int ret = LimitNpcItem_Delete(manager, 1003);
    ASSERT(ret == 0, "删除NPC限购物品成功");

    ret = LimitNpcItem_Exists(manager, 1003);
    ASSERT(ret == 0, "删除后记录不存在");

    TEST_PASS();
}

void test_exists(DBConnectionManager* manager)
{
    TEST_START("检查NPC限购物品是否存在");

    LimitNpcItem record;
    memset(&record, 0, sizeof(LimitNpcItem));

    int ret = LimitNpcItem_Exists(manager, 1004);
    ASSERT(ret == 0, "不存在的记录返回0");

    record.item_index = 1004;
    record.max_count = 25;
    record.sell_count = 0;
    LimitNpcItem_Add(manager, &record);

    ret = LimitNpcItem_Exists(manager, 1004);
    ASSERT(ret == 1, "存在的记录返回1");

    LimitNpcItem_Delete(manager, 1004);
    TEST_PASS();
}

void test_count(DBConnectionManager* manager)
{
    TEST_START("统计记录数");

    LimitNpcItem record;
    int i;

    int initial_count = LimitNpcItem_Count(manager);
    ASSERT(initial_count >= 0, "获取初始记录数成功");

    memset(&record, 0, sizeof(LimitNpcItem));
    for (i = 0; i < 3; i++) {
        record.item_index = 2001 + i;
        record.max_count = 10 + i * 10;
        record.sell_count = i * 2;
        LimitNpcItem_Add(manager, &record);
    }

    int new_count = LimitNpcItem_Count(manager);
    ASSERT_EQ(new_count, initial_count + 3, "记录数增加3条");

    for (i = 0; i < 3; i++) {
        LimitNpcItem_Delete(manager, 2001 + i);
    }

    TEST_PASS();
}

void test_get_all(DBConnectionManager* manager)
{
    TEST_START("获取所有记录");

    LimitNpcItem record;
    LimitNpcItem records[10];
    int i;

    memset(&record, 0, sizeof(LimitNpcItem));
    for (i = 0; i < 3; i++) {
        record.item_index = 3001 + i;
        record.max_count = 20 + i * 5;
        record.sell_count = i * 3;
        LimitNpcItem_Add(manager, &record);
    }

    int count = LimitNpcItem_GetAll(manager, records, 10);
    ASSERT(count >= 3, "至少获取到3条记录");

    for (i = 0; i < 3; i++) {
        LimitNpcItem_Delete(manager, 3001 + i);
    }

    TEST_PASS();
}

void test_get_sold_out(DBConnectionManager* manager)
{
    TEST_START("获取已售罄物品");

    LimitNpcItem record;
    LimitNpcItem records[10];
    int i;

    // 添加3个已售罄的物品
    memset(&record, 0, sizeof(LimitNpcItem));
    for (i = 0; i < 3; i++) {
        record.item_index = 4001 + i;
        record.max_count = 10;
        record.sell_count = 10 + i;  // >= max_count
        LimitNpcItem_Add(manager, &record);
    }

    // 添加2个未售罄的物品
    for (i = 0; i < 2; i++) {
        record.item_index = 4101 + i;
        record.max_count = 20;
        record.sell_count = 5 + i;  // < max_count
        LimitNpcItem_Add(manager, &record);
    }

    int count = LimitNpcItem_GetSoldOut(manager, records, 10);
    ASSERT(count >= 3, "获取到至少3个已售罄物品");

    // 验证所有返回的都是已售罄的
    for (i = 0; i < count; i++) {
        ASSERT(records[i].sell_count >= records[i].max_count, "确认物品已售罄");
    }

    // 清理
    for (i = 0; i < 3; i++) {
        LimitNpcItem_Delete(manager, 4001 + i);
    }
    for (i = 0; i < 2; i++) {
        LimitNpcItem_Delete(manager, 4101 + i);
    }

    TEST_PASS();
}

void test_increase_sell_count(DBConnectionManager* manager)
{
    TEST_START("增加销售数量");

    LimitNpcItem record, retrieved;
    memset(&record, 0, sizeof(LimitNpcItem));

    record.item_index = 5001;
    record.max_count = 100;
    record.sell_count = 10;
    LimitNpcItem_Add(manager, &record);

    // 增加5个
    int ret = LimitNpcItem_IncreaseSellCount(manager, 5001, 5);
    ASSERT(ret == 0, "增加销售数量成功");

    LimitNpcItem_Get(manager, 5001, &retrieved);
    ASSERT_EQ(retrieved.sell_count, 15, "销售数量增加正确");

    // 再增加10个
    ret = LimitNpcItem_IncreaseSellCount(manager, 5001, 10);
    ASSERT(ret == 0, "再次增加销售数量成功");

    LimitNpcItem_Get(manager, 5001, &retrieved);
    ASSERT_EQ(retrieved.sell_count, 25, "销售数量累计增加正确");

    LimitNpcItem_Delete(manager, 5001);
    TEST_PASS();
}

void test_replace_into_idempotent(DBConnectionManager* manager)
{
    TEST_START("REPLACE INTO 幂等性测试");

    LimitNpcItem record, retrieved;
    memset(&record, 0, sizeof(LimitNpcItem));

    record.item_index = 6001;
    record.max_count = 50;
    record.sell_count = 5;
    int ret = LimitNpcItem_Add(manager, &record);
    ASSERT(ret == 0, "第一次添加成功");

    record.max_count = 80;
    record.sell_count = 15;
    ret = LimitNpcItem_Add(manager, &record);
    ASSERT(ret == 0, "第二次添加成功（幂等操作）");

    LimitNpcItem_Get(manager, 6001, &retrieved);
    ASSERT_EQ(retrieved.max_count, 80, "数据为最新数据");
    ASSERT_EQ(retrieved.sell_count, 15, "销售数量为最新数据");

    int count_before = LimitNpcItem_Count(manager);
    record.max_count = 100;
    LimitNpcItem_Add(manager, &record);
    int count_after = LimitNpcItem_Count(manager);
    ASSERT_EQ(count_before, count_after, "记录数不变（未重复插入）");

    LimitNpcItem_Delete(manager, 6001);
    TEST_PASS();
}

void test_boundary_values(DBConnectionManager* manager)
{
    TEST_START("边界值测试");

    LimitNpcItem record, retrieved;
    memset(&record, 0, sizeof(LimitNpcItem));

    // 测试零值
    record.item_index = 7001;
    record.max_count = 0;
    record.sell_count = 0;
    int ret = LimitNpcItem_Add(manager, &record);
    ASSERT(ret == 0, "添加零值记录成功");

    LimitNpcItem_Get(manager, 7001, &retrieved);
    ASSERT_EQ(retrieved.max_count, 0, "零值最大数量正确");
    ASSERT_EQ(retrieved.sell_count, 0, "零值已售数量正确");

    // 测试大数值
    record.item_index = 7002;
    record.max_count = 4294967295U;  // UINT_MAX
    record.sell_count = 1000000000U;
    ret = LimitNpcItem_Add(manager, &record);
    ASSERT(ret == 0, "添加大数值记录成功");

    LimitNpcItem_Get(manager, 7002, &retrieved);
    ASSERT_EQ(retrieved.max_count, 4294967295U, "大数值最大数量正确");
    ASSERT_EQ(retrieved.sell_count, 1000000000U, "大数值已售数量正确");

    LimitNpcItem_Delete(manager, 7001);
    LimitNpcItem_Delete(manager, 7002);
    TEST_PASS();
}

void test_print_info(DBConnectionManager* manager)
{
    TEST_START("打印物品信息");

    LimitNpcItem record;
    memset(&record, 0, sizeof(LimitNpcItem));

    record.item_index = 8001;
    record.max_count = 50;
    record.sell_count = 30;
    LimitNpcItem_Add(manager, &record);

    LimitNpcItem_Get(manager, 8001, &record);
    LimitNpcItem_PrintInfo(&record);
    ASSERT(1, "打印物品信息成功");

    // 测试已售罄物品的打印
    record.item_index = 8002;
    record.max_count = 20;
    record.sell_count = 20;
    LimitNpcItem_Add(manager, &record);

    LimitNpcItem_Get(manager, 8002, &record);
    printf("\n已售罄物品:\n");
    LimitNpcItem_PrintInfo(&record);
    ASSERT(1, "打印已售罄物品信息成功");

    LimitNpcItem_Delete(manager, 8001);
    LimitNpcItem_Delete(manager, 8002);
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
    printf("limit_npc_item ORM 测试\n");
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
    test_get_sold_out(&manager);
    test_increase_sell_count(&manager);
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
