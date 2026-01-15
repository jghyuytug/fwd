#include "dnf_event_log_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static int test_count = 0;
static int test_passed = 0;
static int assertion_count = 0;
static int assertion_passed = 0;
static int current_test_failed = 0;

#define TEST_START(name) \
    do { \
        if (test_count > 0 && !current_test_failed) test_passed++; \
        current_test_failed = 0; \
        printf("\n========================================\n[TEST %d] %s\n========================================\n", ++test_count, name); \
    } while(0)

#define ASSERT_EQ(a, b, msg) \
    do { \
        assertion_count++; \
        if ((a) == (b)) { \
            assertion_passed++; \
            printf("  ✓ %s\n", msg); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (expected %d, got %d)\n", msg, (int)(b), (int)(a)); \
        } \
    } while(0)

#define ASSERT_TRUE(expr, msg) \
    do { \
        assertion_count++; \
        if (expr) { \
            assertion_passed++; \
            printf("  ✓ %s\n", msg); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED)\n", msg); \
        } \
    } while(0)

#define ASSERT_STR_EQ(a, b, msg) \
    do { \
        assertion_count++; \
        if (strcmp((a), (b)) == 0) { \
            assertion_passed++; \
            printf("  ✓ %s\n", msg); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (expected '%s', got '%s')\n", msg, (b), (a)); \
        } \
    } while(0)

void cleanup_test_data(DBConnectionManager* manager, unsigned int log_id) {
    DnfEventLog_Delete(manager, log_id);
}

int main(int argc, char* argv[]) {
    DBConnectionManager manager;
    DnfEventLog record, retrieved;
    unsigned int inserted_id = 0;
    int current_time = (int)time(NULL);
    int result;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    printf("\n================================================\n");
    printf("  DNF Event Log ORM 测试套件\n");
    printf("================================================\n");

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

    /* 测试1: 添加日志记录并获取自增ID */
    TEST_START("添加日志记录并获取自增ID");
    memset(&record, 0, sizeof(DnfEventLog));
    record.occ_time = current_time;
    record.event_type = 1;
    record.parameter1 = 100;
    record.parameter2 = 200;
    record.server_id = 1;
    record.event_flag = 0;
    record.start_time = current_time - 3600;
    record.end_time = current_time + 3600;
    record.m_id = 123456;
    strcpy(record.expl, "Test event log");
    strcpy(record.etc, "Additional info");

    result = DnfEventLog_Add(&manager, &record, &inserted_id);
    ASSERT_EQ(result, 0, "添加日志记录成功");
    ASSERT_TRUE(inserted_id > 0, "获取到自增ID");

    /* 测试2: 获取日志记录 */
    TEST_START("获取日志记录");
    memset(&retrieved, 0, sizeof(DnfEventLog));
    result = DnfEventLog_Get(&manager, inserted_id, &retrieved);
    ASSERT_EQ(result, 0, "获取日志记录成功");
    ASSERT_EQ(retrieved.log_id, inserted_id, "log_id匹配");
    ASSERT_EQ(retrieved.event_type, 1, "event_type正确");
    ASSERT_EQ(retrieved.parameter1, 100, "parameter1正确");
    ASSERT_EQ(retrieved.m_id, 123456, "m_id正确");
    ASSERT_STR_EQ(retrieved.expl, "Test event log", "说明字段正确");

    /* 测试3: 更新日志记录 */
    TEST_START("更新日志记录");
    retrieved.event_type = 2;
    retrieved.parameter1 = 300;
    strcpy(retrieved.expl, "Updated log");
    result = DnfEventLog_Update(&manager, &retrieved);
    ASSERT_EQ(result, 0, "更新日志记录成功");

    memset(&retrieved, 0, sizeof(DnfEventLog));
    DnfEventLog_Get(&manager, inserted_id, &retrieved);
    ASSERT_EQ(retrieved.event_type, 2, "event_type已更新");
    ASSERT_EQ(retrieved.parameter1, 300, "parameter1已更新");
    ASSERT_STR_EQ(retrieved.expl, "Updated log", "说明已更新");

    /* 测试4: 按时间范围统计 */
    TEST_START("按时间范围统计");
    result = DnfEventLog_GetCountByTimeRange(&manager, current_time - 7200, current_time + 7200);
    ASSERT_TRUE(result >= 1, "时间范围内至少有1条记录");

    /* 测试5: 按活动类型统计 */
    TEST_START("按活动类型统计");
    result = DnfEventLog_GetCountByEventType(&manager, 2);
    ASSERT_TRUE(result >= 1, "活动类型2至少有1条记录");

    /* 测试6: 按服务器统计 */
    TEST_START("按服务器统计");
    result = DnfEventLog_GetCountByServer(&manager, 1);
    ASSERT_TRUE(result >= 1, "服务器1至少有1条记录");

    /* 测试7: 按账户统计 */
    TEST_START("按账户统计");
    result = DnfEventLog_GetCountByAccount(&manager, 123456);
    ASSERT_TRUE(result >= 1, "账户123456至少有1条记录");

    /* 测试8: 获取最新日志ID */
    TEST_START("获取最新日志ID");
    unsigned int log_ids[10];
    result = DnfEventLog_GetLatestLogIds(&manager, 5, log_ids);
    ASSERT_TRUE(result > 0, "获取到最新日志ID");
    ASSERT_TRUE(log_ids[0] >= inserted_id, "最新ID应该>=插入的ID");

    /* 测试9: 字符串转义测试 */
    TEST_START("字符串转义测试");
    memset(&record, 0, sizeof(DnfEventLog));
    record.occ_time = current_time;
    record.event_type = 3;
    strcpy(record.expl, "Test's \"quote\" and \\backslash");
    strcpy(record.etc, "Special chars: ' \" \\");

    unsigned int id2 = 0;
    result = DnfEventLog_Add(&manager, &record, &id2);
    ASSERT_EQ(result, 0, "添加含特殊字符的记录成功");

    DnfEventLog_Get(&manager, id2, &retrieved);
    ASSERT_STR_EQ(retrieved.expl, "Test's \"quote\" and \\backslash", "特殊字符正确保存");

    cleanup_test_data(&manager, id2);

    /* 测试10: PrintInfo测试 */
    TEST_START("PrintInfo函数测试");
    printf("\n调用 PrintInfo:\n");
    DnfEventLog_PrintInfo(&retrieved);
    ASSERT_TRUE(1, "PrintInfo执行成功");

    /* 测试11: 删除日志记录 */
    TEST_START("删除日志记录");
    result = DnfEventLog_Delete(&manager, inserted_id);
    ASSERT_EQ(result, 0, "删除日志记录成功");

    result = DnfEventLog_Exists(&manager, inserted_id);
    ASSERT_EQ(result, 0, "记录已删除");

    DBConnectionManager_Cleanup(&manager);

    if (!current_test_failed) test_passed++;

    printf("\n================================================\n");
    printf("              测试结果汇总\n");
    printf("================================================\n");
    printf("测试用例: %d/%d 通过 (%.1f%%)\n", test_passed, test_count,
           (test_count > 0) ? (100.0 * test_passed / test_count) : 0.0);
    printf("断言:     %d/%d 通过 (%.1f%%)\n", assertion_passed, assertion_count,
           (assertion_count > 0) ? (100.0 * assertion_passed / assertion_count) : 0.0);
    printf("================================================\n");

    if (test_passed == test_count && assertion_passed == assertion_count) {
        printf("✓ 所有测试通过！\n================================================\n");
        return 0;
    } else {
        printf("✗ 部分测试失败\n================================================\n");
        return 1;
    }
}
