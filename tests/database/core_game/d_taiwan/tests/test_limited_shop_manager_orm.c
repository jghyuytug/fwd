#include "limited_shop_manager_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

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

#define ASSERT_NEQ(a, b, message) \
    do { \
        assertion_count++; \
        if ((a) != (b)) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED: %d == %d)\n", message, (int)(a), (int)(b)); \
        } \
    } while(0)

#define ASSERT_GT(a, b, message) \
    do { \
        assertion_count++; \
        if ((a) > (b)) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED: %d <= %d)\n", message, (int)(a), (int)(b)); \
        } \
    } while(0)

#define ASSERT_GE(a, b, message) \
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

#define ASSERT_STREQ(a, b, message) \
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

/* 测试数据常量 */
#define TEST_SERVER_ID_1 1
#define TEST_SERVER_ID_2 2
#define TEST_IPG_NO_1 1001
#define TEST_IPG_NO_2 1002
#define TEST_IPG_NO_3 1003

/* 辅助函数 */
static void cleanup_test_data(DBConnectionManager* manager) {
    char query[256];
    DBQueryResult result;

    /* 删除测试数据（按 server_id 和 ipg_no 批量删除） */
    snprintf(query, sizeof(query),
        "DELETE FROM limited_shop_manager WHERE server_id IN (%d, %d) AND ipg_no IN (%d, %d, %d)",
        TEST_SERVER_ID_1, TEST_SERVER_ID_2, TEST_IPG_NO_1, TEST_IPG_NO_2, TEST_IPG_NO_3);

    memset(&result, 0, sizeof(DBQueryResult));
    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);
}

static void create_test_shop(LimitedShopManager* shop, unsigned char server_id, unsigned int ipg_no) {
    memset(shop, 0, sizeof(LimitedShopManager));

    shop->occ_time = (unsigned int)time(NULL);
    shop->server_id = server_id;
    shop->ipg_no = ipg_no;
    shop->item_no = 10001;
    shop->item_cnt = 1;
    shop->cera_price = 1000;
    shop->gold_price = 500000;
    shop->avatar_period_type = -1;
    shop->total_cnt = 100;
    shop->sell_cnt = 0;
    shop->restrict_no = 1;
    shop->start_time = (unsigned int)time(NULL);
    shop->end_time = (unsigned int)(time(NULL) + 86400 * 7);  /* 7天后 */
    shop->real_end_time = 0;
    shop->npc_idx = 10;
    shop->cond_charac_job = 0;  /* 无职业限制 */
    shop->cond_lev_begin = 1;
    shop->cond_lev_end = 100;
    shop->cond_acc_create_time_begin = 0;
    shop->cond_acc_create_time_end = 0;
    shop->cond_cha_create_time_begin = 0;
    shop->cond_cha_create_time_end = 0;
    shop->status_flag = SHOP_STATUS_ACTIVE;
    snprintf(shop->title, sizeof(shop->title), "测试限购商品%u", ipg_no);
    shop->range_section = 0;
    snprintf(shop->reason_etc, sizeof(shop->reason_etc), "测试原因");
    snprintf(shop->reason_stop, sizeof(shop->reason_stop), "");
    shop->pos_flag = '0';
}

/* ==================== 测试用例 ==================== */

void test_add_and_get(DBConnectionManager* manager) {
    TEST_START("测试添加和查询记录");

    LimitedShopManager shop;
    create_test_shop(&shop, TEST_SERVER_ID_1, TEST_IPG_NO_1);

    int new_no = LimitedShopManager_Add(manager, &shop);
    ASSERT_GT(new_no, 0, "添加记录成功，返回自增ID");

    LimitedShopManager retrieved;
    memset(&retrieved, 0, sizeof(LimitedShopManager));
    int ret = LimitedShopManager_Get(manager, new_no, TEST_SERVER_ID_1, TEST_IPG_NO_1, &retrieved);
    ASSERT_EQ(ret, 0, "查询记录成功");
    ASSERT_EQ(retrieved.no, new_no, "no匹配");
    ASSERT_EQ(retrieved.server_id, TEST_SERVER_ID_1, "server_id匹配");
    ASSERT_EQ(retrieved.ipg_no, TEST_IPG_NO_1, "ipg_no匹配");
    ASSERT_EQ(retrieved.item_no, 10001, "item_no匹配");
    ASSERT_EQ(retrieved.cera_price, 1000, "cera_price匹配");
    ASSERT_EQ(retrieved.total_cnt, 100, "total_cnt匹配");
    ASSERT_STREQ(retrieved.title, shop.title, "title匹配");

    LimitedShopManager_PrintInfo(&retrieved);
}

void test_update(DBConnectionManager* manager) {
    TEST_START("测试更新记录");

    LimitedShopManager shop;
    create_test_shop(&shop, TEST_SERVER_ID_1, TEST_IPG_NO_2);

    int new_no = LimitedShopManager_Add(manager, &shop);
    ASSERT_GT(new_no, 0, "添加记录成功");

    /* 查询并修改 */
    LimitedShopManager retrieved;
    memset(&retrieved, 0, sizeof(LimitedShopManager));
    LimitedShopManager_Get(manager, new_no, TEST_SERVER_ID_1, TEST_IPG_NO_2, &retrieved);

    retrieved.cera_price = 2000;
    retrieved.gold_price = 1000000;
    retrieved.sell_cnt = 10;
    snprintf(retrieved.title, sizeof(retrieved.title), "更新后的标题");

    int ret = LimitedShopManager_Update(manager, &retrieved);
    ASSERT_EQ(ret, 0, "更新记录成功");

    /* 验证更新 */
    LimitedShopManager updated;
    memset(&updated, 0, sizeof(LimitedShopManager));
    LimitedShopManager_Get(manager, new_no, TEST_SERVER_ID_1, TEST_IPG_NO_2, &updated);
    ASSERT_EQ(updated.cera_price, 2000, "cera_price已更新");
    ASSERT_EQ(updated.gold_price, 1000000, "gold_price已更新");
    ASSERT_EQ(updated.sell_cnt, 10, "sell_cnt已更新");
    ASSERT_STREQ(updated.title, "更新后的标题", "title已更新");
}

void test_exists(DBConnectionManager* manager) {
    TEST_START("测试记录存在性检查");

    LimitedShopManager shop;
    create_test_shop(&shop, TEST_SERVER_ID_1, TEST_IPG_NO_3);

    int new_no = LimitedShopManager_Add(manager, &shop);
    ASSERT_GT(new_no, 0, "添加记录成功");

    int exists = LimitedShopManager_Exists(manager, new_no, TEST_SERVER_ID_1, TEST_IPG_NO_3);
    ASSERT_EQ(exists, 1, "已存在的记录返回1");

    exists = LimitedShopManager_Exists(manager, 999999, TEST_SERVER_ID_1, TEST_IPG_NO_3);
    ASSERT_EQ(exists, 0, "不存在的记录返回0");
}

void test_delete(DBConnectionManager* manager) {
    TEST_START("测试删除记录");

    LimitedShopManager shop;
    create_test_shop(&shop, TEST_SERVER_ID_2, TEST_IPG_NO_1);

    int new_no = LimitedShopManager_Add(manager, &shop);
    ASSERT_GT(new_no, 0, "添加记录成功");

    int ret = LimitedShopManager_Delete(manager, new_no, TEST_SERVER_ID_2, TEST_IPG_NO_1);
    ASSERT_EQ(ret, 0, "删除记录成功");

    int exists = LimitedShopManager_Exists(manager, new_no, TEST_SERVER_ID_2, TEST_IPG_NO_1);
    ASSERT_EQ(exists, 0, "记录已不存在");
}

void test_increment_sell_count(DBConnectionManager* manager) {
    TEST_START("测试增加销售数量");

    LimitedShopManager shop;
    create_test_shop(&shop, TEST_SERVER_ID_1, TEST_IPG_NO_1);

    int new_no = LimitedShopManager_Add(manager, &shop);
    ASSERT_GT(new_no, 0, "添加记录成功");

    /* 查询初始值 */
    LimitedShopManager retrieved;
    memset(&retrieved, 0, sizeof(LimitedShopManager));
    LimitedShopManager_Get(manager, new_no, TEST_SERVER_ID_1, TEST_IPG_NO_1, &retrieved);
    unsigned int initial_sell_cnt = retrieved.sell_cnt;

    /* 增加销售数量 */
    int ret = LimitedShopManager_IncrementSellCount(manager, new_no, TEST_SERVER_ID_1, TEST_IPG_NO_1, 5);
    ASSERT_EQ(ret, 0, "增加销售数量成功");

    /* 验证 */
    memset(&retrieved, 0, sizeof(LimitedShopManager));
    LimitedShopManager_Get(manager, new_no, TEST_SERVER_ID_1, TEST_IPG_NO_1, &retrieved);
    ASSERT_EQ(retrieved.sell_cnt, initial_sell_cnt + 5, "sell_cnt已增加5");
}

void test_update_status(DBConnectionManager* manager) {
    TEST_START("测试更新商店状态");

    LimitedShopManager shop;
    create_test_shop(&shop, TEST_SERVER_ID_1, TEST_IPG_NO_2);

    int new_no = LimitedShopManager_Add(manager, &shop);
    ASSERT_GT(new_no, 0, "添加记录成功");

    /* 更新状态为暂停 */
    int ret = LimitedShopManager_UpdateStatus(manager, new_no, TEST_SERVER_ID_1, TEST_IPG_NO_2, SHOP_STATUS_PAUSED);
    ASSERT_EQ(ret, 0, "更新状态成功");

    /* 验证 */
    LimitedShopManager retrieved;
    memset(&retrieved, 0, sizeof(LimitedShopManager));
    LimitedShopManager_Get(manager, new_no, TEST_SERVER_ID_1, TEST_IPG_NO_2, &retrieved);
    ASSERT_EQ(retrieved.status_flag, SHOP_STATUS_PAUSED, "status_flag已更新为暂停");
}

void test_set_stop_reason(DBConnectionManager* manager) {
    TEST_START("测试设置停止原因");

    LimitedShopManager shop;
    create_test_shop(&shop, TEST_SERVER_ID_1, TEST_IPG_NO_3);

    int new_no = LimitedShopManager_Add(manager, &shop);
    ASSERT_GT(new_no, 0, "添加记录成功");

    /* 设置停止原因 */
    const char* reason = "库存不足，提前结束";
    int ret = LimitedShopManager_SetStopReason(manager, new_no, TEST_SERVER_ID_1, TEST_IPG_NO_3, reason);
    ASSERT_EQ(ret, 0, "设置停止原因成功");

    /* 验证 */
    LimitedShopManager retrieved;
    memset(&retrieved, 0, sizeof(LimitedShopManager));
    LimitedShopManager_Get(manager, new_no, TEST_SERVER_ID_1, TEST_IPG_NO_3, &retrieved);
    ASSERT_STREQ(retrieved.reason_stop, reason, "reason_stop已更新");
}

void test_check_stock(DBConnectionManager* manager) {
    TEST_START("测试库存检查");

    LimitedShopManager shop;
    create_test_shop(&shop, TEST_SERVER_ID_2, TEST_IPG_NO_2);
    shop.total_cnt = 50;
    shop.sell_cnt = 30;  /* 剩余20 */

    int new_no = LimitedShopManager_Add(manager, &shop);
    ASSERT_GT(new_no, 0, "添加记录成功");

    /* 检查库存是否充足 */
    int sufficient = LimitedShopManager_CheckStock(manager, new_no, TEST_SERVER_ID_2, TEST_IPG_NO_2, 15);
    ASSERT_EQ(sufficient, 1, "库存充足（需要15，剩余20）");

    sufficient = LimitedShopManager_CheckStock(manager, new_no, TEST_SERVER_ID_2, TEST_IPG_NO_2, 25);
    ASSERT_EQ(sufficient, 0, "库存不足（需要25，剩余20）");
}

void test_get_remaining_stock(DBConnectionManager* manager) {
    TEST_START("测试获取剩余库存");

    LimitedShopManager shop;
    create_test_shop(&shop, TEST_SERVER_ID_2, TEST_IPG_NO_3);
    shop.total_cnt = 100;
    shop.sell_cnt = 35;

    int new_no = LimitedShopManager_Add(manager, &shop);
    ASSERT_GT(new_no, 0, "添加记录成功");

    int remaining = 0;
    int ret = LimitedShopManager_GetRemainingStock(manager, new_no, TEST_SERVER_ID_2, TEST_IPG_NO_3, &remaining);
    ASSERT_EQ(ret, 0, "获取剩余库存成功");
    ASSERT_EQ(remaining, 65, "剩余库存为65（100-35）");
}

void test_count_by_server(DBConnectionManager* manager) {
    TEST_START("测试按服务器ID统计");

    /* 创建多个测试记录 */
    LimitedShopManager shop1, shop2, shop3;
    create_test_shop(&shop1, TEST_SERVER_ID_1, TEST_IPG_NO_1);
    create_test_shop(&shop2, TEST_SERVER_ID_1, TEST_IPG_NO_2);
    create_test_shop(&shop3, TEST_SERVER_ID_2, TEST_IPG_NO_1);

    LimitedShopManager_Add(manager, &shop1);
    LimitedShopManager_Add(manager, &shop2);
    LimitedShopManager_Add(manager, &shop3);

    int count1 = LimitedShopManager_CountByServer(manager, TEST_SERVER_ID_1);
    ASSERT_GE(count1, 2, "服务器1至少有2条记录");

    int count2 = LimitedShopManager_CountByServer(manager, TEST_SERVER_ID_2);
    ASSERT_GE(count2, 1, "服务器2至少有1条记录");
}

void test_count_by_status(DBConnectionManager* manager) {
    TEST_START("测试按状态统计");

    int count_active = LimitedShopManager_CountByStatus(manager, SHOP_STATUS_ACTIVE);
    ASSERT_GE(count_active, 1, "激活状态至少有1条记录");

    printf("  当前激活状态记录数: %d\n", count_active);
}

void test_count_by_server_and_status(DBConnectionManager* manager) {
    TEST_START("测试按服务器和状态统计");

    int count = LimitedShopManager_CountByServerAndStatus(manager, TEST_SERVER_ID_1, SHOP_STATUS_ACTIVE);
    ASSERT_GE(count, 1, "服务器1激活状态至少有1条记录");

    printf("  服务器1激活状态记录数: %d\n", count);
}

void test_count_total(DBConnectionManager* manager) {
    TEST_START("测试统计总记录数");

    int count = LimitedShopManager_CountTotal(manager);
    ASSERT_GE(count, 1, "总记录数>=1");

    printf("  当前总记录数: %d\n", count);
}

void test_count_expired(DBConnectionManager* manager) {
    TEST_START("测试统计过期记录数");

    /* 创建一个已过期的商店 */
    LimitedShopManager shop;
    create_test_shop(&shop, TEST_SERVER_ID_1, TEST_IPG_NO_1);
    shop.end_time = (unsigned int)(time(NULL) - 86400);  /* 1天前过期 */

    LimitedShopManager_Add(manager, &shop);

    int count = LimitedShopManager_CountExpired(manager);
    ASSERT_GE(count, 1, "过期记录数>=1");

    printf("  当前过期记录数: %d\n", count);
}

void test_update_expired_status(DBConnectionManager* manager) {
    TEST_START("测试批量更新过期商店状态");

    int affected = LimitedShopManager_UpdateExpiredStatus(manager);
    ASSERT_GE(affected, 0, "批量更新成功（受影响行数>=0）");

    printf("  更新了 %d 条过期记录的状态\n", affected);
}

void test_null_pointer_checks(DBConnectionManager* manager) {
    TEST_START("测试NULL指针检查");

    int ret = LimitedShopManager_Add(NULL, NULL);
    ASSERT_EQ(ret, -1, "manager=NULL时返回-1");

    LimitedShopManager shop;
    ret = LimitedShopManager_Get(manager, 1, 1, 1, NULL);
    ASSERT_EQ(ret, -1, "shop=NULL时返回-1");

    ret = LimitedShopManager_SetStopReason(manager, 1, 1, 1, NULL);
    ASSERT_EQ(ret, -1, "reason_stop=NULL时返回-1");

    int remaining;
    ret = LimitedShopManager_GetRemainingStock(manager, 1, 1, 1, NULL);
    ASSERT_EQ(ret, -1, "remaining=NULL时返回-1");
}

/* ==================== 主函数 ==================== */

int main(int argc, char* argv[]) {
    DBConnectionManager manager;

    if (argc < 2) {
        printf("用法: %s <database_config_file>\n", argv[0]);
        return 1;
    }

    printf("========================================\n");
    printf("limited_shop_manager_orm 测试套件\n");
    printf("========================================\n");

    /* 初始化数据库连接 */
    memset(&manager, 0, sizeof(DBConnectionManager));
    if (DBConnectionManager_Initialize(&manager, argv[1]) < 0) {
        printf("初始化数据库连接管理器失败\n");
        return 1;
    }

    if (DBConnectionManager_Connect(&manager, DB_TYPE_TAIWAN) < 0) {
        printf("连接数据库失败\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    /* 清理测试数据 */
    cleanup_test_data(&manager);

    /* 运行测试 */
    test_add_and_get(&manager);
    test_update(&manager);
    test_exists(&manager);
    test_delete(&manager);
    test_increment_sell_count(&manager);
    test_update_status(&manager);
    test_set_stop_reason(&manager);
    test_check_stock(&manager);
    test_get_remaining_stock(&manager);
    test_count_by_server(&manager);
    test_count_by_status(&manager);
    test_count_by_server_and_status(&manager);
    test_count_total(&manager);
    test_count_expired(&manager);
    test_update_expired_status(&manager);
    test_null_pointer_checks(&manager);

    /* 最后一个测试完成时增加test_passed */
    if (!current_test_failed) {
        test_passed++;
    }

    /* 清理测试数据 */
    cleanup_test_data(&manager);

    /* 清理数据库连接 */
    DBConnectionManager_Cleanup(&manager);

    /* 打印测试结果 */
    printf("\n========================================\n");
    printf("测试用例: %d/%d 通过 (%.1f%%)\n",
        test_passed, test_count, (test_passed * 100.0) / test_count);
    printf("断言: %d/%d 通过 (%.1f%%)\n",
        assertion_passed, assertion_count, (assertion_passed * 100.0) / assertion_count);
    printf("========================================\n");

    if (test_passed == test_count && assertion_passed == assertion_count) {
        printf("✅ 所有测试通过！\n");
        return 0;
    } else {
        printf("❌ 部分测试失败！\n");
        return 1;
    }
}
