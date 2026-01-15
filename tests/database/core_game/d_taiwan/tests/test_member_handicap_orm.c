#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "member_handicap_orm.h"

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
    DBConnectionManager_ExecuteQuery(&manager, DB_TYPE_TAIWAN, "TRUNCATE TABLE member_handicap", &result);
    DBQueryResult_Free(&result);
}

/* Test 1: Add - 添加能力值调整记录 */
static void Test_Add() {
    TEST_START("MemberHandicap_Add");

    MemberHandicap record;
    record.event_id = 1001;
    record.cap_type = 1;  /* 经验加倍 */
    record.server_id = 1;
    record.m_id = 10001;
    strcpy(record.start_time, "2025-11-14 10:00:00");
    strcpy(record.end_time, "2025-11-14 20:00:00");
    record.handicap_value = 200;  /* 200% */

    int ret = MemberHandicap_Add(&manager, &record);
    TEST_ASSERT(ret == 0, "Add handicap record");
}

/* Test 2: Get - 获取记录（5字段复合主键） */
static void Test_Get() {
    TEST_START("MemberHandicap_Get");

    MemberHandicap record;
    int ret = MemberHandicap_Get(&manager, 1001, 1, 1, 10001, "2025-11-14 10:00:00", &record);

    TEST_ASSERT(ret == 0, "Get handicap record");
    TEST_ASSERT(record.event_id == 1001, "event_id matches");
    TEST_ASSERT(record.cap_type == 1, "cap_type matches");
    TEST_ASSERT(record.handicap_value == 200, "handicap_value matches");
}

/* Test 3: Exists - 检查存在性 */
static void Test_Exists() {
    TEST_START("MemberHandicap_Exists");

    int exists = MemberHandicap_Exists(&manager, 1001, 1, 1, 10001, "2025-11-14 10:00:00");
    TEST_ASSERT(exists == 1, "Record exists");

    exists = MemberHandicap_Exists(&manager, 9999, 1, 1, 10001, "2025-11-14 10:00:00");
    TEST_ASSERT(exists == 0, "Non-existent record");
}

/* Test 4: Update - 更新记录 */
static void Test_Update() {
    TEST_START("MemberHandicap_Update");

    MemberHandicap record;
    record.event_id = 1001;
    record.cap_type = 1;
    record.server_id = 1;
    record.m_id = 10001;
    strcpy(record.start_time, "2025-11-14 10:00:00");
    strcpy(record.end_time, "2025-11-14 22:00:00");
    record.handicap_value = 300;

    int ret = MemberHandicap_Update(&manager, &record);
    TEST_ASSERT(ret == 0, "Update handicap record");

    /* 验证更新 */
    MemberHandicap updated;
    MemberHandicap_Get(&manager, 1001, 1, 1, 10001, "2025-11-14 10:00:00", &updated);
    TEST_ASSERT(updated.handicap_value == 300, "handicap_value updated");
}

/* Test 5: Add幂等性测试 */
static void Test_AddIdempotent() {
    TEST_START("MemberHandicap_Add (Idempotent)");

    MemberHandicap record1;
    record1.event_id = 1002;
    record1.cap_type = 2;
    record1.server_id = 1;
    record1.m_id = 10002;
    strcpy(record1.start_time, "2025-11-14 11:00:00");
    strcpy(record1.end_time, "2025-11-14 21:00:00");
    record1.handicap_value = 150;

    /* 第一次添加 */
    int ret1 = MemberHandicap_Add(&manager, &record1);
    TEST_ASSERT(ret1 == 0, "First add");

    /* 第二次添加（幂等） - handicap_value=250 */
    MemberHandicap record2;
    record2.event_id = 1002;
    record2.cap_type = 2;
    record2.server_id = 1;
    record2.m_id = 10002;
    strcpy(record2.start_time, "2025-11-14 11:00:00");
    strcpy(record2.end_time, "2025-11-14 23:00:00");
    record2.handicap_value = 250;

    int ret2 = MemberHandicap_Add(&manager, &record2);
    TEST_ASSERT(ret2 == 0, "Second add (idempotent)");

    /* 验证最新数据 */
    MemberHandicap result;
    MemberHandicap_Get(&manager, 1002, 2, 1, 10002, "2025-11-14 11:00:00", &result);
    TEST_ASSERT(result.handicap_value == 250, "handicap_value replaced to 250");
}

/* Test 6: GetAll - 获取所有记录 */
static void Test_GetAll() {
    TEST_START("MemberHandicap_GetAll");

    /* 添加更多测试数据 */
    MemberHandicap r1 = {1003, 1, 1, 10003, "2025-11-14 08:00:00", "2025-11-14 18:00:00", 180};
    MemberHandicap r2 = {1004, 2, 2, 10004, "2025-11-14 09:00:00", "2025-11-14 19:00:00", 220};
    MemberHandicap_Add(&manager, &r1);
    MemberHandicap_Add(&manager, &r2);

    MemberHandicap results[10];
    int count = MemberHandicap_GetAll(&manager, results, 0, 10);

    TEST_ASSERT(count >= 4, "GetAll returned at least 4 records");
}

/* Test 7: GetCount - 获取总数 */
static void Test_GetCount() {
    TEST_START("MemberHandicap_GetCount");

    int count = MemberHandicap_GetCount(&manager);
    TEST_ASSERT(count >= 4, "Count is at least 4");
}

/* Test 8: GetByEventId - 按事件ID查询 */
static void Test_GetByEventId() {
    TEST_START("MemberHandicap_GetByEventId");

    MemberHandicap results[10];
    int count = MemberHandicap_GetByEventId(&manager, 1001, results, 10);

    TEST_ASSERT(count >= 1, "Found at least 1 record for event 1001");
}

/* Test 9: GetByMemberId - 按会员ID查询 */
static void Test_GetByMemberId() {
    TEST_START("MemberHandicap_GetByMemberId");

    MemberHandicap results[10];
    int count = MemberHandicap_GetByMemberId(&manager, 10001, results, 10);

    TEST_ASSERT(count >= 1, "Found at least 1 record for member 10001");
}

/* Test 10: GetByServerId - 按服务器ID查询 */
static void Test_GetByServerId() {
    TEST_START("MemberHandicap_GetByServerId");

    MemberHandicap results[10];
    int count = MemberHandicap_GetByServerId(&manager, 1, results, 10);

    TEST_ASSERT(count >= 3, "Found at least 3 records for server 1");
}

/* Test 11: GetByCapType - 按能力类型查询 */
static void Test_GetByCapType() {
    TEST_START("MemberHandicap_GetByCapType");

    MemberHandicap results[10];
    int count = MemberHandicap_GetByCapType(&manager, 1, results, 10);

    TEST_ASSERT(count >= 2, "Found at least 2 records for cap_type 1");
}

/* Test 12: GetActiveHandicaps - 获取当前活动的能力值 */
static void Test_GetActiveHandicaps() {
    TEST_START("MemberHandicap_GetActiveHandicaps");

    /* 添加一个当前活动的记录（从昨天到明天） */
    MemberHandicap active_record = {2001, 3, 1, 20001,
        "2025-11-13 00:00:00", "2025-11-15 23:59:59", 500};
    MemberHandicap_Add(&manager, &active_record);

    MemberHandicap results[10];
    int count = MemberHandicap_GetActiveHandicaps(&manager, results, 10);

    TEST_ASSERT(count >= 1, "Found at least 1 active handicap");
}

/* Test 13: GetByTimeRange - 按时间范围查询 */
static void Test_GetByTimeRange() {
    TEST_START("MemberHandicap_GetByTimeRange");

    MemberHandicap results[10];
    int count = MemberHandicap_GetByTimeRange(&manager,
        "2025-11-14 08:00:00", "2025-11-14 12:00:00", results, 10);

    TEST_ASSERT(count >= 2, "Found records in time range");
}

/* Test 14: BulkAdd - 批量添加 */
static void Test_BulkAdd() {
    TEST_START("MemberHandicap_BulkAdd");

    MemberHandicap records[3] = {
        {3001, 1, 1, 30001, "2025-11-14 12:00:00", "2025-11-14 22:00:00", 150},
        {3002, 2, 1, 30002, "2025-11-14 12:10:00", "2025-11-14 22:10:00", 180},
        {3003, 3, 2, 30003, "2025-11-14 12:20:00", "2025-11-14 22:20:00", 200}
    };

    int count = MemberHandicap_BulkAdd(&manager, records, 3);
    TEST_ASSERT(count == 3, "BulkAdd added 3 records");

    /* 验证添加 */
    int exists = MemberHandicap_Exists(&manager, 3001, 1, 1, 30001, "2025-11-14 12:00:00");
    TEST_ASSERT(exists == 1, "3001 exists");
}

/* Test 15: BulkDeleteByEventIds - 批量删除 */
static void Test_BulkDeleteByEventIds() {
    TEST_START("MemberHandicap_BulkDeleteByEventIds");

    int event_ids[3] = {3001, 3002, 3003};

    int ret = MemberHandicap_BulkDeleteByEventIds(&manager, event_ids, 3);
    TEST_ASSERT(ret == 0, "BulkDeleteByEventIds successful");

    /* 验证删除 */
    int exists = MemberHandicap_Exists(&manager, 3001, 1, 1, 30001, "2025-11-14 12:00:00");
    TEST_ASSERT(exists == 0, "3001 deleted");
}

/* Test 16: Delete幂等性测试 */
static void Test_DeleteIdempotent() {
    TEST_START("MemberHandicap_Delete (Idempotent)");

    /* 第一次删除 */
    int ret1 = MemberHandicap_Delete(&manager, 1002, 2, 1, 10002, "2025-11-14 11:00:00");
    TEST_ASSERT(ret1 == 0, "First delete");

    /* 第二次删除（幂等） */
    int ret2 = MemberHandicap_Delete(&manager, 1002, 2, 1, 10002, "2025-11-14 11:00:00");
    TEST_ASSERT(ret2 == 0, "Second delete (idempotent)");

    /* 验证不存在 */
    int exists = MemberHandicap_Exists(&manager, 1002, 2, 1, 10002, "2025-11-14 11:00:00");
    TEST_ASSERT(exists == 0, "Record does not exist");
}

/* Test 17: PrintInfo - 工具函数测试 */
static void Test_PrintInfo() {
    TEST_START("MemberHandicap_PrintInfo");

    MemberHandicap record = {1001, 1, 1, 10001, "2025-11-14 10:00:00", "2025-11-14 20:00:00", 300};

    printf("  Testing PrintInfo output:\n");
    MemberHandicap_PrintInfo(&record);

    TEST_ASSERT(1, "PrintInfo executed");
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    printf("===========================================\n");
    printf("  member_handicap_orm Test Suite\n");
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
    Test_GetByServerId();
    Test_GetByCapType();
    Test_GetActiveHandicaps();
    Test_GetByTimeRange();
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
