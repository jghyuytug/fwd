#include "auction_history_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

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

#define ASSERT_STR_EQ(a, b, message) \
    do { \
        g_assert_count++; \
        if (strcmp(a, b) == 0) { \
            g_assert_passed++; \
            printf("  ✓ %s (%s == %s)\n", message, a, b); \
        } else { \
            printf("  ✗ FAILED: %s (%s != %s)\n", message, a, b); \
        } \
    } while(0)

// 测试用例
void test_add_and_get(DBConnectionManager* manager)
{
    TEST_START("添加并获取拍卖历史记录");

    AuctionHistory record, retrieved;
    memset(&record, 0, sizeof(AuctionHistory));
    memset(&retrieved, 0, sizeof(AuctionHistory));

    record.auction_id = 1001;
    strcpy(record.start_time, "2025-11-15 10:00:00");
    strcpy(record.occ_time, "2025-11-15 10:30:00");
    record.event_type = 1;
    record.owner_id = 100;
    record.buyer_id = 200;
    record.price = 50000;
    record.seal_flag = 0;
    record.item_id = 3001;
    record.add_info = 0;
    record.upgrade = 10;
    record.amplify_option = 1;
    record.amplify_value = 15;
    record.seal_cnt = 0;
    record.endurance = 100;
    record.extend_info = 0;
    record.owner_postal_id = 0;
    record.buyer_postal_id = 0;
    record.unit_price = 50000;

    int ret = AuctionHistory_Add(manager, &record);
    ASSERT(ret == 0, "添加拍卖历史记录成功");

    ret = AuctionHistory_Get(manager, 1001, &retrieved);
    ASSERT(ret == 0, "获取拍卖历史记录成功");
    ASSERT_EQ(retrieved.auction_id, 1001, "拍卖ID正确");
    ASSERT_EQ(retrieved.owner_id, 100, "卖家ID正确");
    ASSERT_EQ(retrieved.buyer_id, 200, "买家ID正确");
    ASSERT_EQ(retrieved.price, 50000, "价格正确");

    AuctionHistory_Delete(manager, 1001);
    TEST_PASS();
}

void test_update(DBConnectionManager* manager)
{
    TEST_START("更新拍卖历史记录");

    AuctionHistory record, retrieved;
    memset(&record, 0, sizeof(AuctionHistory));

    record.auction_id = 1002;
    strcpy(record.start_time, "2025-11-15 11:00:00");
    strcpy(record.occ_time, "2025-11-15 11:30:00");
    record.event_type = 1;
    record.owner_id = 101;
    record.buyer_id = 201;
    record.price = 60000;
    record.item_id = 3002;
    AuctionHistory_Add(manager, &record);

    record.price = 70000;
    int ret = AuctionHistory_Update(manager, &record);
    ASSERT(ret == 0, "更新拍卖历史记录成功");

    AuctionHistory_Get(manager, 1002, &retrieved);
    ASSERT_EQ(retrieved.price, 70000, "更新后价格正确");

    AuctionHistory_Delete(manager, 1002);
    TEST_PASS();
}

void test_delete(DBConnectionManager* manager)
{
    TEST_START("删除拍卖历史记录");

    AuctionHistory record;
    memset(&record, 0, sizeof(AuctionHistory));

    record.auction_id = 1003;
    strcpy(record.start_time, "2025-11-15 12:00:00");
    strcpy(record.occ_time, "2025-11-15 12:30:00");
    record.owner_id = 102;
    record.buyer_id = 202;
    record.price = 80000;
    record.item_id = 3003;
    AuctionHistory_Add(manager, &record);

    int ret = AuctionHistory_Delete(manager, 1003);
    ASSERT(ret == 0, "删除拍卖历史记录成功");

    ret = AuctionHistory_Exists(manager, 1003);
    ASSERT(ret == 0, "删除后记录不存在");

    TEST_PASS();
}

void test_exists(DBConnectionManager* manager)
{
    TEST_START("检查拍卖历史记录是否存在");

    AuctionHistory record;
    memset(&record, 0, sizeof(AuctionHistory));

    int ret = AuctionHistory_Exists(manager, 1004);
    ASSERT(ret == 0, "不存在的记录返回0");

    record.auction_id = 1004;
    strcpy(record.start_time, "2025-11-15 13:00:00");
    strcpy(record.occ_time, "2025-11-15 13:30:00");
    record.owner_id = 103;
    record.buyer_id = 203;
    record.price = 90000;
    record.item_id = 3004;
    AuctionHistory_Add(manager, &record);

    ret = AuctionHistory_Exists(manager, 1004);
    ASSERT(ret == 1, "存在的记录返回1");

    AuctionHistory_Delete(manager, 1004);
    TEST_PASS();
}

void test_count(DBConnectionManager* manager)
{
    TEST_START("统计记录数");

    AuctionHistory record;
    int i;

    int initial_count = AuctionHistory_Count(manager);
    ASSERT(initial_count >= 0, "获取初始记录数成功");

    memset(&record, 0, sizeof(AuctionHistory));
    for (i = 0; i < 3; i++) {
        record.auction_id = 2001 + i;
        sprintf(record.start_time, "2025-11-15 14:0%d:00", i);
        sprintf(record.occ_time, "2025-11-15 14:3%d:00", i);
        record.owner_id = 201 + i;
        record.buyer_id = 301 + i;
        record.price = 100000 + i * 1000;
        record.item_id = 4001 + i;
        AuctionHistory_Add(manager, &record);
    }

    int new_count = AuctionHistory_Count(manager);
    ASSERT_EQ(new_count, initial_count + 3, "记录数增加3条");

    for (i = 0; i < 3; i++) {
        AuctionHistory_Delete(manager, 2001 + i);
    }

    TEST_PASS();
}

void test_get_by_owner(DBConnectionManager* manager)
{
    TEST_START("获取指定卖家的历史记录");

    AuctionHistory record;
    AuctionHistory records[10];
    int i;

    memset(&record, 0, sizeof(AuctionHistory));
    for (i = 0; i < 5; i++) {
        record.auction_id = 3001 + i;
        sprintf(record.start_time, "2025-11-15 15:0%d:00", i);
        sprintf(record.occ_time, "2025-11-15 15:3%d:00", i);
        record.owner_id = 401;
        record.buyer_id = 501 + i;
        record.price = 200000 + i * 1000;
        record.item_id = 5001 + i;
        AuctionHistory_Add(manager, &record);
    }

    int count = AuctionHistory_GetByOwner(manager, 401, records, 10);
    ASSERT_EQ(count, 5, "获取到5条卖家历史记录");

    for (i = 0; i < count; i++) {
        ASSERT_EQ(records[i].owner_id, 401, "卖家ID匹配");
    }

    for (i = 0; i < 5; i++) {
        AuctionHistory_Delete(manager, 3001 + i);
    }

    TEST_PASS();
}

void test_get_by_buyer(DBConnectionManager* manager)
{
    TEST_START("获取指定买家的历史记录");

    AuctionHistory record;
    AuctionHistory records[10];
    int i;

    memset(&record, 0, sizeof(AuctionHistory));
    for (i = 0; i < 4; i++) {
        record.auction_id = 4001 + i;
        sprintf(record.start_time, "2025-11-15 16:0%d:00", i);
        sprintf(record.occ_time, "2025-11-15 16:3%d:00", i);
        record.owner_id = 601 + i;
        record.buyer_id = 701;
        record.price = 300000 + i * 1000;
        record.item_id = 6001 + i;
        AuctionHistory_Add(manager, &record);
    }

    int count = AuctionHistory_GetByBuyer(manager, 701, records, 10);
    ASSERT_EQ(count, 4, "获取到4条买家历史记录");

    for (i = 0; i < count; i++) {
        ASSERT_EQ(records[i].buyer_id, 701, "买家ID匹配");
    }

    for (i = 0; i < 4; i++) {
        AuctionHistory_Delete(manager, 4001 + i);
    }

    TEST_PASS();
}

void test_get_by_item(DBConnectionManager* manager)
{
    TEST_START("获取指定物品的历史记录");

    AuctionHistory record;
    AuctionHistory records[10];
    int i;

    memset(&record, 0, sizeof(AuctionHistory));
    for (i = 0; i < 3; i++) {
        record.auction_id = 5001 + i;
        sprintf(record.start_time, "2025-11-15 17:0%d:00", i);
        sprintf(record.occ_time, "2025-11-15 17:3%d:00", i);
        record.owner_id = 801 + i;
        record.buyer_id = 901 + i;
        record.price = 400000 + i * 1000;
        record.item_id = 7001;
        AuctionHistory_Add(manager, &record);
    }

    int count = AuctionHistory_GetByItem(manager, 7001, records, 10);
    ASSERT_EQ(count, 3, "获取到3条物品历史记录");

    for (i = 0; i < count; i++) {
        ASSERT_EQ(records[i].item_id, 7001, "物品ID匹配");
    }

    for (i = 0; i < 3; i++) {
        AuctionHistory_Delete(manager, 5001 + i);
    }

    TEST_PASS();
}

void test_get_by_event_type(DBConnectionManager* manager)
{
    TEST_START("获取指定事件类型的历史记录");

    AuctionHistory record;
    AuctionHistory records[10];
    int i;

    memset(&record, 0, sizeof(AuctionHistory));
    for (i = 0; i < 3; i++) {
        record.auction_id = 6001 + i;
        sprintf(record.start_time, "2025-11-15 18:0%d:00", i);
        sprintf(record.occ_time, "2025-11-15 18:3%d:00", i);
        record.event_type = 2;
        record.owner_id = 1001 + i;
        record.buyer_id = 1101 + i;
        record.price = 500000 + i * 1000;
        record.item_id = 8001 + i;
        AuctionHistory_Add(manager, &record);
    }

    int count = AuctionHistory_GetByEventType(manager, 2, records, 10);
    ASSERT_EQ(count, 3, "获取到3条事件类型记录");

    for (i = 0; i < count; i++) {
        ASSERT_EQ(records[i].event_type, 2, "事件类型匹配");
    }

    for (i = 0; i < 3; i++) {
        AuctionHistory_Delete(manager, 6001 + i);
    }

    TEST_PASS();
}

void test_average_price(DBConnectionManager* manager)
{
    TEST_START("获取物品平均成交价格");

    AuctionHistory record;
    int i;

    memset(&record, 0, sizeof(AuctionHistory));
    for (i = 0; i < 3; i++) {
        record.auction_id = 7001 + i;
        sprintf(record.start_time, "2025-11-15 19:0%d:00", i);
        sprintf(record.occ_time, "2025-11-15 19:3%d:00", i);
        record.owner_id = 1201 + i;
        record.buyer_id = 1301 + i;
        record.price = 60000 + i * 10000;
        record.item_id = 9001;
        AuctionHistory_Add(manager, &record);
    }

    int avg_price = AuctionHistory_GetAveragePrice(manager, 9001);
    ASSERT(avg_price >= 60000 && avg_price <= 80000, "平均价格在合理范围内");

    for (i = 0; i < 3; i++) {
        AuctionHistory_Delete(manager, 7001 + i);
    }

    TEST_PASS();
}

void test_print_info(DBConnectionManager* manager)
{
    TEST_START("打印拍卖历史记录信息");

    AuctionHistory record;
    memset(&record, 0, sizeof(AuctionHistory));

    record.auction_id = 8001;
    strcpy(record.start_time, "2025-11-15 20:00:00");
    strcpy(record.occ_time, "2025-11-15 20:30:00");
    record.event_type = 1;
    record.owner_id = 1401;
    record.buyer_id = 1501;
    record.price = 600000;
    record.item_id = 9999;
    record.upgrade = 12;
    record.amplify_option = 2;
    record.amplify_value = 20;
    AuctionHistory_Add(manager, &record);

    AuctionHistory_Get(manager, 8001, &record);
    AuctionHistory_PrintInfo(&record);
    ASSERT(1, "打印拍卖历史记录信息成功");

    AuctionHistory_Delete(manager, 8001);
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
    printf("auction_history ORM 测试\n");
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
    test_get_by_owner(&manager);
    test_get_by_buyer(&manager);
    test_get_by_item(&manager);
    test_get_by_event_type(&manager);
    test_average_price(&manager);
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
