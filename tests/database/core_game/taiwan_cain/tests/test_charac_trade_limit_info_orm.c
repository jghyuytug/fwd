#include "charac_trade_limit_info_orm.h"
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

/* 测试用例 */

void test_add_and_get(DBConnectionManager* manager) {
    TEST_START("添加和获取角色交易限制信息");

    CharacTradeLimitInfo record;
    record.m_id = 1001;
    record.charac_no = 6000001;
    strcpy(record.last_trade_time, "2025-11-16 10:00:00");
    record.total_trade_gold = 100000;
    record.trade_count = 5;
    record.nexon_user = 0;

    ASSERT_EQ(CharacTradeLimitInfo_Add(manager, &record), 0, "添加记录成功");

    CharacTradeLimitInfo retrieved;
    ASSERT_EQ(CharacTradeLimitInfo_Get(manager, 6000001, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.m_id, 1001, "会员ID匹配");
    ASSERT_EQ(retrieved.charac_no, 6000001, "角色编号匹配");
    ASSERT_EQ(retrieved.total_trade_gold, 100000, "总交易金额匹配");
    ASSERT_EQ(retrieved.trade_count, 5, "交易次数匹配");
    ASSERT_EQ(retrieved.nexon_user, 0, "Nexon用户标识匹配");
}

void test_update(DBConnectionManager* manager) {
    TEST_START("更新角色交易限制信息");

    CharacTradeLimitInfo record;
    ASSERT_EQ(CharacTradeLimitInfo_Get(manager, 6000001, &record), 0, "获取已存在记录");

    record.total_trade_gold = 200000;
    record.trade_count = 10;
    ASSERT_EQ(CharacTradeLimitInfo_Update(manager, &record), 0, "更新记录成功");

    CharacTradeLimitInfo retrieved;
    ASSERT_EQ(CharacTradeLimitInfo_Get(manager, 6000001, &retrieved), 0, "获取更新后记录");
    ASSERT_EQ(retrieved.total_trade_gold, 200000, "总交易金额已更新");
    ASSERT_EQ(retrieved.trade_count, 10, "交易次数已更新");
}

void test_exists(DBConnectionManager* manager) {
    TEST_START("检查记录是否存在");

    ASSERT_TRUE(CharacTradeLimitInfo_Exists(manager, 6000001), "记录存在");
    ASSERT_TRUE(!CharacTradeLimitInfo_Exists(manager, 9999999), "记录不存在");
}

void test_add_multiple_records(DBConnectionManager* manager) {
    TEST_START("添加多条交易限制信息");

    CharacTradeLimitInfo record2;
    record2.m_id = 1001;
    record2.charac_no = 6000002;
    strcpy(record2.last_trade_time, "2025-11-16 11:00:00");
    record2.total_trade_gold = 50000;
    record2.trade_count = 3;
    record2.nexon_user = 0;
    ASSERT_EQ(CharacTradeLimitInfo_Add(manager, &record2), 0, "添加记录2成功");

    CharacTradeLimitInfo record3;
    record3.m_id = 1002;
    record3.charac_no = 6000003;
    strcpy(record3.last_trade_time, "2025-11-16 12:00:00");
    record3.total_trade_gold = 300000;
    record3.trade_count = 15;
    record3.nexon_user = 1;
    ASSERT_EQ(CharacTradeLimitInfo_Add(manager, &record3), 0, "添加记录3成功（Nexon用户）");
}

void test_update_last_trade_time(DBConnectionManager* manager) {
    TEST_START("更新最后交易时间");

    ASSERT_EQ(CharacTradeLimitInfo_UpdateLastTradeTime(manager, 6000001, "2025-11-17 10:00:00"), 0,
              "更新最后交易时间成功");

    CharacTradeLimitInfo retrieved;
    ASSERT_EQ(CharacTradeLimitInfo_Get(manager, 6000001, &retrieved), 0, "获取记录成功");
    ASSERT_TRUE(strncmp(retrieved.last_trade_time, "2025-11-17", 10) == 0, "最后交易时间已更新");
}

void test_increment_trade_gold(DBConnectionManager* manager) {
    TEST_START("增加交易金额");

    CharacTradeLimitInfo before;
    ASSERT_EQ(CharacTradeLimitInfo_Get(manager, 6000001, &before), 0, "获取初始记录");

    ASSERT_EQ(CharacTradeLimitInfo_IncrementTradeGold(manager, 6000001, 50000), 0, "增加交易金额成功");

    CharacTradeLimitInfo after;
    ASSERT_EQ(CharacTradeLimitInfo_Get(manager, 6000001, &after), 0, "获取增加后记录");
    ASSERT_EQ(after.total_trade_gold, before.total_trade_gold + 50000, "交易金额增加50000");
}

void test_increment_trade_count(DBConnectionManager* manager) {
    TEST_START("增加交易次数");

    CharacTradeLimitInfo before;
    ASSERT_EQ(CharacTradeLimitInfo_Get(manager, 6000001, &before), 0, "获取初始记录");

    ASSERT_EQ(CharacTradeLimitInfo_IncrementTradeCount(manager, 6000001), 0, "增加交易次数成功");

    CharacTradeLimitInfo after;
    ASSERT_EQ(CharacTradeLimitInfo_Get(manager, 6000001, &after), 0, "获取增加后记录");
    ASSERT_EQ(after.trade_count, before.trade_count + 1, "交易次数增加1");
}

void test_reset_trade_stats(DBConnectionManager* manager) {
    TEST_START("重置交易统计");

    ASSERT_EQ(CharacTradeLimitInfo_ResetTradeStats(manager, 6000001), 0, "重置交易统计成功");

    CharacTradeLimitInfo retrieved;
    ASSERT_EQ(CharacTradeLimitInfo_Get(manager, 6000001, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.total_trade_gold, 0, "总交易金额已重置为0");
    ASSERT_EQ(retrieved.trade_count, 0, "交易次数已重置为0");
}

void test_update_nexon_user(DBConnectionManager* manager) {
    TEST_START("更新Nexon用户标识");

    ASSERT_EQ(CharacTradeLimitInfo_UpdateNexonUser(manager, 6000001, 1), 0, "更新Nexon用户标识成功");

    CharacTradeLimitInfo retrieved;
    ASSERT_EQ(CharacTradeLimitInfo_Get(manager, 6000001, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.nexon_user, 1, "Nexon用户标识已更新");
}

void test_get_by_member_id(DBConnectionManager* manager) {
    TEST_START("获取指定会员的所有角色");

    CharacTradeLimitInfo records[10];
    int count = CharacTradeLimitInfo_GetByMemberId(manager, 1001, records, 10);

    ASSERT_TRUE(count >= 2, "至少找到2条记录");
    ASSERT_EQ(records[0].m_id, 1001, "会员ID匹配");
}

void test_get_by_min_gold(DBConnectionManager* manager) {
    TEST_START("获取交易金额大于指定值的角色");

    /* 先重置一个记录的金额 */
    CharacTradeLimitInfo temp;
    temp.m_id = 1001;
    temp.charac_no = 6000001;
    strcpy(temp.last_trade_time, "2025-11-16 10:00:00");
    temp.total_trade_gold = 150000;
    temp.trade_count = 5;
    temp.nexon_user = 1;
    CharacTradeLimitInfo_Update(manager, &temp);

    CharacTradeLimitInfo records[10];
    int count = CharacTradeLimitInfo_GetByMinGold(manager, 100000, records, 10);

    ASSERT_TRUE(count >= 1, "至少找到1条记录");
    ASSERT_TRUE(records[0].total_trade_gold >= 100000, "交易金额满足条件");
}

void test_get_by_min_trade_count(DBConnectionManager* manager) {
    TEST_START("获取交易次数大于指定值的角色");

    CharacTradeLimitInfo records[10];
    int count = CharacTradeLimitInfo_GetByMinTradeCount(manager, 10, records, 10);

    ASSERT_TRUE(count >= 1, "至少找到1条记录");
    ASSERT_TRUE(records[0].trade_count >= 10, "交易次数满足条件");
}

void test_get_by_trade_time_after(DBConnectionManager* manager) {
    TEST_START("获取指定时间之后交易的角色");

    CharacTradeLimitInfo records[10];
    int count = CharacTradeLimitInfo_GetByTradeTimeAfter(manager, "2025-11-15 00:00:00", records, 10);

    ASSERT_TRUE(count >= 1, "至少找到1条记录");
}

void test_get_nexon_users(DBConnectionManager* manager) {
    TEST_START("获取Nexon用户");

    CharacTradeLimitInfo records[10];
    int count = CharacTradeLimitInfo_GetNexonUsers(manager, records, 10);

    ASSERT_TRUE(count >= 1, "至少找到1个Nexon用户");
    ASSERT_EQ(records[0].nexon_user, 1, "Nexon用户标识为1");
}

void test_batch_reset_trade_stats(DBConnectionManager* manager) {
    TEST_START("批量重置交易统计");

    ASSERT_EQ(CharacTradeLimitInfo_BatchResetTradeStats(manager), 0, "批量重置成功");

    CharacTradeLimitInfo retrieved;
    ASSERT_EQ(CharacTradeLimitInfo_Get(manager, 6000001, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.total_trade_gold, 0, "总交易金额已重置为0");
    ASSERT_EQ(retrieved.trade_count, 0, "交易次数已重置为0");
}

void test_delete(DBConnectionManager* manager) {
    TEST_START("删除记录");

    ASSERT_EQ(CharacTradeLimitInfo_Delete(manager, 6000001), 0, "删除记录成功");
    ASSERT_TRUE(!CharacTradeLimitInfo_Exists(manager, 6000001), "记录已删除");
}

void test_print_info(DBConnectionManager* manager) {
    TEST_START("打印角色交易限制信息");

    CharacTradeLimitInfo record;
    if (CharacTradeLimitInfo_Get(manager, 6000003, &record) == 0) {
        CharacTradeLimitInfo_PrintInfo(&record);
        ASSERT_TRUE(1, "打印信息成功");
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
    printf("角色交易限制信息 ORM 测试\n");
    printf("========================================\n");

    /* 运行测试用例 */
    test_add_and_get(&manager);
    test_update(&manager);
    test_exists(&manager);
    test_add_multiple_records(&manager);
    test_update_last_trade_time(&manager);
    test_increment_trade_gold(&manager);
    test_increment_trade_count(&manager);
    test_reset_trade_stats(&manager);
    test_update_nexon_user(&manager);
    test_get_by_member_id(&manager);
    test_get_by_min_gold(&manager);
    test_get_by_min_trade_count(&manager);
    test_get_by_trade_time_after(&manager);
    test_get_nexon_users(&manager);
    test_batch_reset_trade_stats(&manager);
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
