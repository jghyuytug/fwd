#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "member_premium_history_orm.h"

/* 测试计数器 */
static int tests_passed = 0;
static int tests_failed = 0;

/* 测试辅助宏 */
#define TEST_START(name) printf("\nTest %d: %s\n", (tests_passed + tests_failed + 1), name)
#define TEST_ASSERT(cond, msg) \
    do { \
        if (cond) { \
            printf("  [PASS] %s\n", msg); \
            tests_passed++; \
        } else { \
            printf("  [FAIL] %s\n", msg); \
            tests_failed++; \
        } \
    } while(0)

/* 全局数据库连接 */
static DBConnectionManager manager;

/* 测试数据 */
static void CreateTestData() {
    /* 清空测试表 */
    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    DBConnectionManager_ExecuteQuery(&manager, DB_TYPE_TAIWAN, "TRUNCATE TABLE member_premium_history", &result);
    DBQueryResult_Free(&result);
}

/* Test 1: Add - 添加高级会员服务历史记录 */
static void Test_Add() {
    TEST_START("MemberPremiumHistory_Add");

    MemberPremiumHistory record;
    record.event_id = 1001;
    record.pre_type = 1;  /* VIP */
    record.m_id = 10001;
    strcpy(record.service_start, "2025-11-14 10:00:00");
    strcpy(record.service_end, "2025-12-14 10:00:00");

    int ret = MemberPremiumHistory_Add(&manager, &record);
    TEST_ASSERT(ret == 0, "Add premium history record");
}

/* Test 2: Get - 获取记录（4字段复合主键） */
static void Test_Get() {
    TEST_START("MemberPremiumHistory_Get");

    MemberPremiumHistory record;
    int ret = MemberPremiumHistory_Get(&manager, 1001, 1, 10001, "2025-11-14 10:00:00", &record);

    TEST_ASSERT(ret == 0, "Get premium history record");
    TEST_ASSERT(record.event_id == 1001, "event_id matches");
    TEST_ASSERT(record.pre_type == 1, "pre_type matches");
    TEST_ASSERT(record.m_id == 10001, "m_id matches");
}

/* Test 3: Exists - 检查存在性 */
static void Test_Exists() {
    TEST_START("MemberPremiumHistory_Exists");

    int exists = MemberPremiumHistory_Exists(&manager, 1001, 1, 10001, "2025-11-14 10:00:00");
    TEST_ASSERT(exists == 1, "Record exists");

    exists = MemberPremiumHistory_Exists(&manager, 9999, 1, 10001, "2025-11-14 10:00:00");
    TEST_ASSERT(exists == 0, "Non-existent record");
}

/* Test 4: Update - 更新记录 */
static void Test_Update() {
    TEST_START("MemberPremiumHistory_Update");

    MemberPremiumHistory record;
    record.event_id = 1001;
    record.pre_type = 1;
    record.m_id = 10001;
    strcpy(record.service_start, "2025-11-14 10:00:00");
    strcpy(record.service_end, "2026-01-14 10:00:00");  /* 延长服务期 */

    int ret = MemberPremiumHistory_Update(&manager, &record);
    TEST_ASSERT(ret == 0, "Update premium history record");

    /* 验证更新 */
    MemberPremiumHistory updated;
    MemberPremiumHistory_Get(&manager, 1001, 1, 10001, "2025-11-14 10:00:00", &updated);
    TEST_ASSERT(strcmp(updated.service_end, "2026-01-14 10:00:00") == 0, "service_end updated");
}

/* Test 5: Add幂等性测试 */
static void Test_AddIdempotent() {
    TEST_START("MemberPremiumHistory_Add (Idempotent)");

    MemberPremiumHistory record1;
    record1.event_id = 1002;
    record1.pre_type = 2;
    record1.m_id = 10002;
    strcpy(record1.service_start, "2025-11-14 11:00:00");
    strcpy(record1.service_end, "2025-12-14 11:00:00");

    /* 第一次添加 */
    int ret1 = MemberPremiumHistory_Add(&manager, &record1);
    TEST_ASSERT(ret1 == 0, "First add");

    /* 第二次添加（幂等） - 延长service_end */
    MemberPremiumHistory record2;
    record2.event_id = 1002;
    record2.pre_type = 2;
    record2.m_id = 10002;
    strcpy(record2.service_start, "2025-11-14 11:00:00");
    strcpy(record2.service_end, "2026-01-14 11:00:00");

    int ret2 = MemberPremiumHistory_Add(&manager, &record2);
    TEST_ASSERT(ret2 == 0, "Second add (idempotent)");

    /* 验证最新数据 */
    MemberPremiumHistory result;
    MemberPremiumHistory_Get(&manager, 1002, 2, 10002, "2025-11-14 11:00:00", &result);
    TEST_ASSERT(strcmp(result.service_end, "2026-01-14 11:00:00") == 0, "service_end replaced");
}

/* Test 6: GetAll - 获取所有记录 */
static void Test_GetAll() {
    TEST_START("MemberPremiumHistory_GetAll");

    /* 添加更多测试数据 */
    MemberPremiumHistory r1 = {1003, 1, 10003, "2025-11-14 08:00:00", "2025-12-14 08:00:00"};
    MemberPremiumHistory r2 = {1004, 2, 10004, "2025-11-14 09:00:00", "2025-12-14 09:00:00"};
    MemberPremiumHistory_Add(&manager, &r1);
    MemberPremiumHistory_Add(&manager, &r2);

    MemberPremiumHistory results[10];
    int count = MemberPremiumHistory_GetAll(&manager, results, 0, 10);

    TEST_ASSERT(count >= 4, "GetAll returned at least 4 records");
}

/* Test 7: GetCount - 获取总数 */
static void Test_GetCount() {
    TEST_START("MemberPremiumHistory_GetCount");

    int count = MemberPremiumHistory_GetCount(&manager);
    TEST_ASSERT(count >= 4, "Count is at least 4");
}

/* Test 8: GetByEventId - 按事件ID查询 */
static void Test_GetByEventId() {
    TEST_START("MemberPremiumHistory_GetByEventId");

    MemberPremiumHistory results[10];
    int count = MemberPremiumHistory_GetByEventId(&manager, 1001, results, 10);

    TEST_ASSERT(count >= 1, "Found at least 1 record for event 1001");
}

/* Test 9: GetByMemberId - 按会员ID查询 */
static void Test_GetByMemberId() {
    TEST_START("MemberPremiumHistory_GetByMemberId");

    MemberPremiumHistory results[10];
    int count = MemberPremiumHistory_GetByMemberId(&manager, 10001, results, 10);

    TEST_ASSERT(count >= 1, "Found at least 1 record for member 10001");
}

/* Test 10: GetByPreType - 按高级会员类型查询 */
static void Test_GetByPreType() {
    TEST_START("MemberPremiumHistory_GetByPreType");

    MemberPremiumHistory results[10];
    int count = MemberPremiumHistory_GetByPreType(&manager, 1, results, 10);

    TEST_ASSERT(count >= 2, "Found at least 2 records for pre_type 1 (VIP)");
}

/* Test 11: GetActiveServices - 获取当前活动的服务 */
static void Test_GetActiveServices() {
    TEST_START("MemberPremiumHistory_GetActiveServices");

    /* 添加一个当前活动的记录（从昨天到明年） */
    MemberPremiumHistory active_record = {2001, 3, 20001,
        "2025-11-13 00:00:00", "2026-11-15 23:59:59"};
    MemberPremiumHistory_Add(&manager, &active_record);

    MemberPremiumHistory results[10];
    int count = MemberPremiumHistory_GetActiveServices(&manager, results, 10);

    TEST_ASSERT(count >= 1, "Found at least 1 active service");
}

/* Test 12: GetByTimeRange - 按时间范围查询 */
static void Test_GetByTimeRange() {
    TEST_START("MemberPremiumHistory_GetByTimeRange");

    MemberPremiumHistory results[10];
    int count = MemberPremiumHistory_GetByTimeRange(&manager,
        "2025-11-14 08:00:00", "2025-11-14 12:00:00", results, 10);

    TEST_ASSERT(count >= 2, "Found records in time range");
}

/* Test 13: GetExpiringSoon - 获取即将过期的服务 */
static void Test_GetExpiringSoon() {
    TEST_START("MemberPremiumHistory_GetExpiringSoon");

    /* 添加一个即将过期的记录（5天后过期） */
    MemberPremiumHistory expiring_record = {3001, 4, 30001,
        "2025-10-14 00:00:00", "2025-11-19 23:59:59"};
    MemberPremiumHistory_Add(&manager, &expiring_record);

    MemberPremiumHistory results[10];
    int count = MemberPremiumHistory_GetExpiringSoon(&manager, 30, results, 10);

    TEST_ASSERT(count >= 1, "Found at least 1 expiring service (within 30 days)");
}

/* Test 14: BulkAdd - 批量添加 */
static void Test_BulkAdd() {
    TEST_START("MemberPremiumHistory_BulkAdd");

    MemberPremiumHistory records[3] = {
        {4001, 1, 40001, "2025-11-14 12:00:00", "2025-12-14 12:00:00"},
        {4002, 2, 40002, "2025-11-14 12:10:00", "2025-12-14 12:10:00"},
        {4003, 3, 40003, "2025-11-14 12:20:00", "2025-12-14 12:20:00"}
    };

    int count = MemberPremiumHistory_BulkAdd(&manager, records, 3);
    TEST_ASSERT(count == 3, "BulkAdd added 3 records");

    /* 验证添加 */
    int exists = MemberPremiumHistory_Exists(&manager, 4001, 1, 40001, "2025-11-14 12:00:00");
    TEST_ASSERT(exists == 1, "4001 exists");
}

/* Test 15: BulkDeleteByEventIds - 批量删除 */
static void Test_BulkDeleteByEventIds() {
    TEST_START("MemberPremiumHistory_BulkDeleteByEventIds");

    int event_ids[3] = {4001, 4002, 4003};

    int ret = MemberPremiumHistory_BulkDeleteByEventIds(&manager, event_ids, 3);
    TEST_ASSERT(ret == 0, "BulkDeleteByEventIds successful");

    /* 验证删除 */
    int exists = MemberPremiumHistory_Exists(&manager, 4001, 1, 40001, "2025-11-14 12:00:00");
    TEST_ASSERT(exists == 0, "4001 deleted");
}

/* Test 16: Delete幂等性测试 */
static void Test_DeleteIdempotent() {
    TEST_START("MemberPremiumHistory_Delete (Idempotent)");

    /* 第一次删除 */
    int ret1 = MemberPremiumHistory_Delete(&manager, 1002, 2, 10002, "2025-11-14 11:00:00");
    TEST_ASSERT(ret1 == 0, "First delete");

    /* 第二次删除（幂等） */
    int ret2 = MemberPremiumHistory_Delete(&manager, 1002, 2, 10002, "2025-11-14 11:00:00");
    TEST_ASSERT(ret2 == 0, "Second delete (idempotent)");

    /* 验证不存在 */
    int exists = MemberPremiumHistory_Exists(&manager, 1002, 2, 10002, "2025-11-14 11:00:00");
    TEST_ASSERT(exists == 0, "Record does not exist");
}

/* Test 17: PrintInfo - 工具函数测试 */
static void Test_PrintInfo() {
    TEST_START("MemberPremiumHistory_PrintInfo");

    MemberPremiumHistory record = {1001, 1, 10001, "2025-11-14 10:00:00", "2026-01-14 10:00:00"};

    printf("  Testing PrintInfo output:\n");
    MemberPremiumHistory_PrintInfo(&record);

    TEST_ASSERT(1, "PrintInfo executed");
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    printf("===========================================\n");
    printf("  member_premium_history_orm Test Suite\n");
    printf("===========================================\n");

    /* 初始化数据库连接 */
    memset(&manager, 0, sizeof(DBConnectionManager));
    if (DBConnectionManager_Initialize(&manager, argv[1]) != 0) {
        fprintf(stderr, "Failed to initialize connection manager\n");
        return 1;
    }

    if (DBConnectionManager_Connect(&manager, DB_TYPE_TAIWAN) != 0) {
        fprintf(stderr, "Failed to connect to database\n");
        return 1;
    }

    /* 创建测试数据 */
    CreateTestData();

    /* 运行测试 */
    Test_Add();
    Test_Get();
    Test_Exists();
    Test_Update();
    Test_AddIdempotent();
    Test_GetAll();
    Test_GetCount();
    Test_GetByEventId();
    Test_GetByMemberId();
    Test_GetByPreType();
    Test_GetActiveServices();
    Test_GetByTimeRange();
    Test_GetExpiringSoon();
    Test_BulkAdd();
    Test_BulkDeleteByEventIds();
    Test_DeleteIdempotent();
    Test_PrintInfo();

    /* 测试结果统计 */
    printf("\n===========================================\n");
    printf("  Test Results\n");
    printf("===========================================\n");
    printf("  Tests Passed:  %d\n", tests_passed);
    printf("  Tests Failed:  %d\n", tests_failed);
    printf("  Total Tests:   %d\n", tests_passed + tests_failed);
    printf("  Success Rate:  %.1f%%\n",
           (tests_passed * 100.0) / (tests_passed + tests_failed));
    printf("===========================================\n");

    return (tests_failed == 0) ? 0 : 1;
}
