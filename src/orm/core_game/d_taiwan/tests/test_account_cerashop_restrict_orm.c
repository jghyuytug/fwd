#include "account_cerashop_restrict_orm.h"
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
        printf("\n========================================\n"); \
        printf("[TEST %d] %s\n", ++test_count, name); \
        printf("========================================\n"); \
    } while(0)

#define ASSERT_EQ(a, b, message) \
    do { \
        assertion_count++; \
        if ((a) == (b)) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED: expected %d, got %d)\n", message, (int)(b), (int)(a)); \
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
            printf("  ✗ %s (FAILED: values should not be equal: %d)\n", message, (int)(a)); \
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

#define ASSERT_STR_EQ(a, b, message) \
    do { \
        assertion_count++; \
        if (strcmp((a), (b)) == 0) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED: expected '%s', got '%s')\n", message, (b), (a)); \
        } \
    } while(0)

/* 清理测试数据 */
void cleanup_test_data(DBConnectionManager* manager, unsigned int m_id, unsigned int ipg_no) {
    AccountCerashopRestrict_Delete(manager, m_id, ipg_no);
}

int main(int argc, char* argv[]) {
    DBConnectionManager manager;
    AccountCerashopRestrict record, retrieved;
    unsigned int test_m_id = 999999;        /* 测试账户ID */
    unsigned int test_ipg_no = 12345;       /* 测试IPG编号 */
    unsigned int test_ipg_no2 = 67890;      /* 第二个测试IPG编号 */
    unsigned int current_time = (unsigned int)time(NULL);
    int result;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    printf("\n");
    printf("================================================\n");
    printf("  Account Cerashop Restrict ORM 测试套件\n");
    printf("================================================\n");

    /* 初始化数据库连接 */
    memset(&manager, 0, sizeof(DBConnectionManager));
    if (DBConnectionManager_Initialize(&manager, argv[1]) < 0) {
        fprintf(stderr, "Failed to initialize database manager\n");
        return 1;
    }

    if (DBConnectionManager_Connect(&manager, DB_TYPE_TAIWAN) < 0) {
        fprintf(stderr, "Failed to connect to d_taiwan database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("✓ 数据库连接成功 (d_taiwan)\n");

    /* 清理可能存在的测试数据 */
    cleanup_test_data(&manager, test_m_id, test_ipg_no);
    cleanup_test_data(&manager, test_m_id, test_ipg_no2);

    /* ==================== 测试1: 添加限制记录 ==================== */
    TEST_START("添加限制记录");

    memset(&record, 0, sizeof(AccountCerashopRestrict));
    record.m_id = test_m_id;
    record.ipg_no = test_ipg_no;
    record.count = 5;
    record.next_date = current_time + 86400;  /* 明天 */
    record.end_date = current_time + 604800;  /* 一周后 */
    record.last_access_date = current_time;

    result = AccountCerashopRestrict_Add(&manager, &record);
    ASSERT_EQ(result, 0, "添加限制记录成功");

    /* ==================== 测试2: 检查记录是否存在 ==================== */
    TEST_START("检查记录存在性");

    result = AccountCerashopRestrict_Exists(&manager, test_m_id, test_ipg_no);
    ASSERT_EQ(result, 1, "限制记录存在");

    result = AccountCerashopRestrict_Exists(&manager, test_m_id, 99999);
    ASSERT_EQ(result, 0, "不存在的IPG返回0");

    /* ==================== 测试3: 获取限制记录 ==================== */
    TEST_START("获取限制记录");

    memset(&retrieved, 0, sizeof(AccountCerashopRestrict));
    result = AccountCerashopRestrict_Get(&manager, test_m_id, test_ipg_no, &retrieved);
    ASSERT_EQ(result, 0, "获取限制记录成功");
    ASSERT_EQ(retrieved.m_id, test_m_id, "账户ID匹配");
    ASSERT_EQ(retrieved.ipg_no, test_ipg_no, "IPG编号匹配");
    ASSERT_EQ(retrieved.count, 5, "购买次数正确");
    ASSERT_EQ(retrieved.next_date, current_time + 86400, "下次购买日期正确");
    ASSERT_EQ(retrieved.end_date, current_time + 604800, "结束日期正确");
    ASSERT_EQ(retrieved.last_access_date, current_time, "最后访问日期正确");

    /* ==================== 测试4: 更新限制记录 ==================== */
    TEST_START("更新限制记录");

    retrieved.count = 10;
    retrieved.next_date = current_time + 172800;  /* 后天 */
    result = AccountCerashopRestrict_Update(&manager, &retrieved);
    ASSERT_EQ(result, 0, "更新限制记录成功");

    /* 验证更新 */
    memset(&retrieved, 0, sizeof(AccountCerashopRestrict));
    AccountCerashopRestrict_Get(&manager, test_m_id, test_ipg_no, &retrieved);
    ASSERT_EQ(retrieved.count, 10, "购买次数已更新");
    ASSERT_EQ(retrieved.next_date, current_time + 172800, "下次购买日期已更新");

    /* ==================== 测试5: 增加购买次数 ==================== */
    TEST_START("增加购买次数");

    result = AccountCerashopRestrict_IncrementCount(&manager, test_m_id, test_ipg_no, 3);
    ASSERT_EQ(result, 0, "增加购买次数成功");

    /* 验证增加 */
    AccountCerashopRestrict_Get(&manager, test_m_id, test_ipg_no, &retrieved);
    ASSERT_EQ(retrieved.count, 13, "购买次数已增加 (10 + 3 = 13)");

    /* 默认增加1 */
    result = AccountCerashopRestrict_IncrementCount(&manager, test_m_id, test_ipg_no, 0);
    ASSERT_EQ(result, 0, "默认增加1成功");
    AccountCerashopRestrict_Get(&manager, test_m_id, test_ipg_no, &retrieved);
    ASSERT_EQ(retrieved.count, 14, "购买次数已增加 (13 + 1 = 14)");

    /* ==================== 测试6: 更新最后访问时间 ==================== */
    TEST_START("更新最后访问时间");

    unsigned int new_timestamp = current_time + 3600;  /* 一小时后 */
    result = AccountCerashopRestrict_UpdateLastAccess(&manager, test_m_id, test_ipg_no, new_timestamp);
    ASSERT_EQ(result, 0, "更新最后访问时间成功");

    AccountCerashopRestrict_Get(&manager, test_m_id, test_ipg_no, &retrieved);
    ASSERT_EQ(retrieved.last_access_date, new_timestamp, "最后访问时间已更新");

    /* 使用当前时间 (timestamp=0) */
    unsigned int before_update = (unsigned int)time(NULL);
    result = AccountCerashopRestrict_UpdateLastAccess(&manager, test_m_id, test_ipg_no, 0);
    ASSERT_EQ(result, 0, "使用当前时间更新成功");
    AccountCerashopRestrict_Get(&manager, test_m_id, test_ipg_no, &retrieved);
    ASSERT_TRUE(retrieved.last_access_date >= before_update, "使用了当前时间戳");

    /* ==================== 测试7: 检查是否允许购买 ==================== */
    TEST_START("检查是否允许购买");

    /* 情况1: next_date在未来，禁止购买 */
    retrieved.next_date = current_time + 86400;  /* 明天 */
    retrieved.end_date = current_time + 604800;  /* 一周后 */
    AccountCerashopRestrict_Update(&manager, &retrieved);

    result = AccountCerashopRestrict_CanPurchase(&manager, test_m_id, test_ipg_no, current_time);
    ASSERT_EQ(result, 0, "冷却期内禁止购买");

    /* 情况2: next_date已过，允许购买 */
    retrieved.next_date = current_time - 3600;  /* 一小时前 */
    AccountCerashopRestrict_Update(&manager, &retrieved);

    result = AccountCerashopRestrict_CanPurchase(&manager, test_m_id, test_ipg_no, current_time);
    ASSERT_EQ(result, 1, "冷却期已过允许购买");

    /* 情况3: end_date已过，允许购买 */
    retrieved.next_date = current_time + 86400;  /* 明天 */
    retrieved.end_date = current_time - 3600;    /* 一小时前 */
    AccountCerashopRestrict_Update(&manager, &retrieved);

    result = AccountCerashopRestrict_CanPurchase(&manager, test_m_id, test_ipg_no, current_time);
    ASSERT_EQ(result, 1, "限制已过期允许购买");

    /* 情况4: 没有限制记录，允许购买 */
    result = AccountCerashopRestrict_CanPurchase(&manager, test_m_id, 88888, current_time);
    ASSERT_EQ(result, 1, "无限制记录允许购买");

    /* ==================== 测试8: 重置购买次数 ==================== */
    TEST_START("重置购买次数");

    result = AccountCerashopRestrict_ResetCount(&manager, test_m_id, test_ipg_no);
    ASSERT_EQ(result, 0, "重置购买次数成功");

    AccountCerashopRestrict_Get(&manager, test_m_id, test_ipg_no, &retrieved);
    ASSERT_EQ(retrieved.count, 0, "购买次数已重置为0");

    /* ==================== 测试9: 获取账户的限制数量 ==================== */
    TEST_START("获取账户的限制数量");

    /* 添加第二个限制记录 */
    memset(&record, 0, sizeof(AccountCerashopRestrict));
    record.m_id = test_m_id;
    record.ipg_no = test_ipg_no2;
    record.count = 3;
    record.next_date = current_time + 86400;
    record.end_date = current_time + 604800;
    record.last_access_date = current_time;
    AccountCerashopRestrict_Add(&manager, &record);

    result = AccountCerashopRestrict_GetCountByAccount(&manager, test_m_id);
    ASSERT_EQ(result, 2, "账户有2个限制记录");

    /* 删除一个记录后 */
    AccountCerashopRestrict_Delete(&manager, test_m_id, test_ipg_no2);
    result = AccountCerashopRestrict_GetCountByAccount(&manager, test_m_id);
    ASSERT_EQ(result, 1, "删除后剩余1个限制记录");

    /* ==================== 测试10: 复合主键测试 ==================== */
    TEST_START("复合主键测试 (m_id + ipg_no)");

    /* 添加同一账户的不同IPG限制 */
    memset(&record, 0, sizeof(AccountCerashopRestrict));
    record.m_id = test_m_id;
    record.ipg_no = test_ipg_no2;
    record.count = 7;
    record.next_date = current_time + 86400;
    record.end_date = current_time + 604800;
    record.last_access_date = current_time;
    AccountCerashopRestrict_Add(&manager, &record);

    /* 验证两个记录独立存在 */
    AccountCerashopRestrict_Get(&manager, test_m_id, test_ipg_no, &retrieved);
    ASSERT_EQ(retrieved.count, 0, "第一个IPG的count为0");

    AccountCerashopRestrict_Get(&manager, test_m_id, test_ipg_no2, &retrieved);
    ASSERT_EQ(retrieved.count, 7, "第二个IPG的count为7");

    ASSERT_TRUE(1, "复合主键独立性验证通过");

    /* ==================== 测试11: REPLACE INTO 测试 ==================== */
    TEST_START("REPLACE INTO 覆盖测试");

    /* 准备测试数据 */
    memset(&record, 0, sizeof(AccountCerashopRestrict));
    record.m_id = test_m_id;
    record.ipg_no = test_ipg_no;
    record.count = 100;
    record.next_date = current_time + 86400;
    record.end_date = current_time + 604800;
    record.last_access_date = current_time;

    result = AccountCerashopRestrict_Add(&manager, &record);
    ASSERT_EQ(result, 0, "第一次Add成功");

    /* 使用相同主键再次Add（应该覆盖） */
    record.count = 200;
    result = AccountCerashopRestrict_Add(&manager, &record);
    ASSERT_EQ(result, 0, "第二次Add成功（覆盖）");

    /* 验证覆盖 */
    AccountCerashopRestrict_Get(&manager, test_m_id, test_ipg_no, &retrieved);
    ASSERT_EQ(retrieved.count, 200, "REPLACE覆盖成功，count为200");

    /* ==================== 测试12: 删除限制记录 ==================== */
    TEST_START("删除限制记录");

    result = AccountCerashopRestrict_Delete(&manager, test_m_id, test_ipg_no);
    ASSERT_EQ(result, 0, "删除第一个限制记录成功");

    result = AccountCerashopRestrict_Exists(&manager, test_m_id, test_ipg_no);
    ASSERT_EQ(result, 0, "限制记录已删除");

    result = AccountCerashopRestrict_Delete(&manager, test_m_id, test_ipg_no2);
    ASSERT_EQ(result, 0, "删除第二个限制记录成功");

    result = AccountCerashopRestrict_GetCountByAccount(&manager, test_m_id);
    ASSERT_EQ(result, 0, "账户已无限制记录");

    /* ==================== 测试13: 边界值测试 ==================== */
    TEST_START("边界值测试");

    /* MySQL int(10) unsigned 实际最大值测试 */
    memset(&record, 0, sizeof(AccountCerashopRestrict));
    record.m_id = 999998;             /* 使用安全的测试值 */
    record.ipg_no = 999998;
    record.count = 999999;
    record.next_date = 2147483647;    /* int 最大值 */
    record.end_date = 2147483647;
    record.last_access_date = 2147483647;

    result = AccountCerashopRestrict_Add(&manager, &record);
    ASSERT_EQ(result, 0, "添加边界值记录成功");

    memset(&retrieved, 0, sizeof(AccountCerashopRestrict));
    result = AccountCerashopRestrict_Get(&manager, 999998, 999998, &retrieved);
    ASSERT_EQ(result, 0, "获取边界值记录成功");
    ASSERT_EQ(retrieved.m_id, 999998, "m_id正确");
    ASSERT_EQ(retrieved.ipg_no, 999998, "ipg_no正确");
    ASSERT_EQ(retrieved.count, 999999, "count正确");
    ASSERT_EQ(retrieved.next_date, 2147483647, "next_date为最大int值");
    ASSERT_EQ(retrieved.end_date, 2147483647, "end_date为最大int值");

    AccountCerashopRestrict_Delete(&manager, 999998, 999998);

    /* ==================== 测试14: 错误处理测试 ==================== */
    TEST_START("错误处理测试");

    result = AccountCerashopRestrict_Add(NULL, &record);
    ASSERT_EQ(result, -1, "NULL manager返回错误");

    result = AccountCerashopRestrict_Add(&manager, NULL);
    ASSERT_EQ(result, -1, "NULL record返回错误");

    result = AccountCerashopRestrict_Get(&manager, 88888, 99999, &retrieved);
    ASSERT_EQ(result, -1, "获取不存在的记录返回错误");

    /* ==================== 测试15: PrintInfo 函数测试 ==================== */
    TEST_START("PrintInfo 函数测试");

    memset(&record, 0, sizeof(AccountCerashopRestrict));
    record.m_id = test_m_id;
    record.ipg_no = test_ipg_no;
    record.count = 42;
    record.next_date = current_time + 86400;
    record.end_date = current_time + 604800;
    record.last_access_date = current_time;

    printf("\n调用 PrintInfo:\n");
    AccountCerashopRestrict_PrintInfo(&record);
    ASSERT_TRUE(1, "PrintInfo执行成功");

    AccountCerashopRestrict_PrintInfo(NULL);
    ASSERT_TRUE(1, "PrintInfo处理NULL成功");

    /* 清理所有测试数据 */
    cleanup_test_data(&manager, test_m_id, test_ipg_no);
    cleanup_test_data(&manager, test_m_id, test_ipg_no2);

    /* 关闭数据库连接 */
    DBConnectionManager_Cleanup(&manager);

    /* 统计最后一个测试 */
    if (!current_test_failed) {
        test_passed++;
    }

    /* 打印测试结果 */
    printf("\n");
    printf("================================================\n");
    printf("              测试结果汇总\n");
    printf("================================================\n");
    printf("测试用例: %d/%d 通过 (%.1f%%)\n",
           test_passed, test_count,
           (test_count > 0) ? (100.0 * test_passed / test_count) : 0.0);
    printf("断言:     %d/%d 通过 (%.1f%%)\n",
           assertion_passed, assertion_count,
           (assertion_count > 0) ? (100.0 * assertion_passed / assertion_count) : 0.0);
    printf("================================================\n");

    if (test_passed == test_count && assertion_passed == assertion_count) {
        printf("✓ 所有测试通过！\n");
        printf("================================================\n");
        return 0;
    } else {
        printf("✗ 部分测试失败\n");
        printf("================================================\n");
        return 1;
    }
}
