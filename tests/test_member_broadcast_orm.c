#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "member_broadcast_orm.h"

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
    DBConnectionManager_ExecuteQuery(&manager, DB_TYPE_TAIWAN, "TRUNCATE TABLE member_broadcast", &result);
    DBQueryResult_Free(&result);
}

/* Test 1: Add - 添加会员广播记录 */
static void Test_Add() {
    TEST_START("MemberBroadcast_Add");

    MemberBroadcast record;
    record.event_id = 1001;
    record.m_id = 10001;
    record.server_id = 1;
    record.charac_no = 100001;
    strcpy(record.charac_name, "TestCharacter1");
    strcpy(record.start_time, "2025-01-01 10:00:00");
    strcpy(record.end_time, "2025-01-01 12:00:00");

    int ret = MemberBroadcast_Add(&manager, &record);
    TEST_ASSERT(ret == 0, "Add member broadcast record");
}

/* Test 2: Get - 获取会员广播记录 */
static void Test_Get() {
    TEST_START("MemberBroadcast_Get");

    MemberBroadcast record;
    int ret = MemberBroadcast_Get(&manager, 1001, 10001, 1, 100001, "2025-01-01 10:00:00", &record);

    TEST_ASSERT(ret == 0, "Get member broadcast record");
    TEST_ASSERT(record.event_id == 1001, "event_id matches");
    TEST_ASSERT(record.m_id == 10001, "m_id matches");
    TEST_ASSERT(strcmp(record.charac_name, "TestCharacter1") == 0, "charac_name matches");
}

/* Test 3: Exists - 检查存在性 */
static void Test_Exists() {
    TEST_START("MemberBroadcast_Exists");

    int exists = MemberBroadcast_Exists(&manager, 1001, 10001, 1, 100001, "2025-01-01 10:00:00");
    TEST_ASSERT(exists == 1, "Record exists");

    exists = MemberBroadcast_Exists(&manager, 9999, 99999, 99, 999999, "2099-01-01 00:00:00");
    TEST_ASSERT(exists == 0, "Non-existent record");
}

/* Test 4: Update - 更新会员广播记录 */
static void Test_Update() {
    TEST_START("MemberBroadcast_Update");

    MemberBroadcast record;
    record.event_id = 1001;
    record.m_id = 10001;
    record.server_id = 1;
    record.charac_no = 100001;
    strcpy(record.charac_name, "UpdatedCharacter");
    strcpy(record.start_time, "2025-01-01 10:00:00");
    strcpy(record.end_time, "2025-01-01 14:00:00");

    int ret = MemberBroadcast_Update(&manager, &record);
    TEST_ASSERT(ret == 0, "Update member broadcast record");

    /* 验证更新 */
    MemberBroadcast updated;
    MemberBroadcast_Get(&manager, 1001, 10001, 1, 100001, "2025-01-01 10:00:00", &updated);
    TEST_ASSERT(strcmp(updated.charac_name, "UpdatedCharacter") == 0, "charac_name updated");
    TEST_ASSERT(strcmp(updated.end_time, "2025-01-01 14:00:00") == 0, "end_time updated");
}

/* Test 5: Add幂等性测试 */
static void Test_AddIdempotent() {
    TEST_START("MemberBroadcast_Add (Idempotent)");

    MemberBroadcast record;
    record.event_id = 1002;
    record.m_id = 10002;
    record.server_id = 2;
    record.charac_no = 100002;
    strcpy(record.charac_name, "TestCharacter2");
    strcpy(record.start_time, "2025-01-02 10:00:00");
    strcpy(record.end_time, "2025-01-02 12:00:00");

    /* 第一次添加 */
    int ret1 = MemberBroadcast_Add(&manager, &record);
    TEST_ASSERT(ret1 == 0, "First add");

    /* 第二次添加（幂等） */
    strcpy(record.end_time, "2025-01-02 15:00:00");
    int ret2 = MemberBroadcast_Add(&manager, &record);
    TEST_ASSERT(ret2 == 0, "Second add (idempotent)");

    /* 验证end_time被更新 */
    MemberBroadcast check;
    MemberBroadcast_Get(&manager, 1002, 10002, 2, 100002, "2025-01-02 10:00:00", &check);
    TEST_ASSERT(strcmp(check.end_time, "2025-01-02 15:00:00") == 0, "end_time updated by REPLACE");
}

/* Test 6: GetByEventId - 根据事件ID查询 */
static void Test_GetByEventId() {
    TEST_START("MemberBroadcast_GetByEventId");

    /* 添加更多测试数据 */
    MemberBroadcast r1;
    r1.event_id = 1001;
    r1.m_id = 10003;
    r1.server_id = 1;
    r1.charac_no = 100003;
    strcpy(r1.charac_name, "EventTest1");
    strcpy(r1.start_time, "2025-01-03 10:00:00");
    strcpy(r1.end_time, "2025-01-03 12:00:00");
    MemberBroadcast_Add(&manager, &r1);

    MemberBroadcast results[10];
    int count = MemberBroadcast_GetByEventId(&manager, 1001, results, 10);

    TEST_ASSERT(count >= 2, "Found at least 2 records with event_id 1001");
}

/* Test 7: GetByMemberId - 根据会员ID查询 */
static void Test_GetByMemberId() {
    TEST_START("MemberBroadcast_GetByMemberId");

    MemberBroadcast results[10];
    int count = MemberBroadcast_GetByMemberId(&manager, 10001, results, 10);

    TEST_ASSERT(count >= 1, "Found at least 1 record with m_id 10001");
}

/* Test 8: GetByServerId - 根据服务器ID查询 */
static void Test_GetByServerId() {
    TEST_START("MemberBroadcast_GetByServerId");

    MemberBroadcast results[10];
    int count = MemberBroadcast_GetByServerId(&manager, 1, results, 10);

    TEST_ASSERT(count >= 2, "Found at least 2 records with server_id 1");
}

/* Test 9: GetByCharacName - 根据角色名称查询 */
static void Test_GetByCharacName() {
    TEST_START("MemberBroadcast_GetByCharacName");

    MemberBroadcast record;
    int ret = MemberBroadcast_GetByCharacName(&manager, "TestCharacter2", &record);

    TEST_ASSERT(ret == 0, "Get by charac_name");
    TEST_ASSERT(record.event_id == 1002, "event_id matches");
}

/* Test 10: GetByTimeRange - 根据时间范围查询 */
static void Test_GetByTimeRange() {
    TEST_START("MemberBroadcast_GetByTimeRange");

    MemberBroadcast results[10];
    int count = MemberBroadcast_GetByTimeRange(&manager, "2025-01-01 00:00:00",
                                                "2025-01-31 23:59:59", results, 10);

    TEST_ASSERT(count >= 3, "Found at least 3 records in time range");
}

/* Test 11: GetActive - 获取活动中的广播 */
static void Test_GetActive() {
    TEST_START("MemberBroadcast_GetActive");

    /* 添加一个当前活动的记录 */
    MemberBroadcast active;
    active.event_id = 1003;
    active.m_id = 10004;
    active.server_id = 1;
    active.charac_no = 100004;
    strcpy(active.charac_name, "ActiveBroadcast");
    strcpy(active.start_time, "2020-01-01 00:00:00");
    strcpy(active.end_time, "2099-12-31 23:59:59");
    MemberBroadcast_Add(&manager, &active);

    MemberBroadcast results[10];
    int count = MemberBroadcast_GetActive(&manager, "2025-06-01 12:00:00", results, 10);

    TEST_ASSERT(count >= 1, "Found at least 1 active broadcast");
}

/* Test 12: GetAll - 获取所有记录（分页） */
static void Test_GetAll() {
    TEST_START("MemberBroadcast_GetAll");

    MemberBroadcast results[10];
    int count = MemberBroadcast_GetAll(&manager, results, 0, 10);

    TEST_ASSERT(count >= 4, "GetAll returned at least 4 records");
}

/* Test 13: GetCount - 获取总数 */
static void Test_GetCount() {
    TEST_START("MemberBroadcast_GetCount");

    int count = MemberBroadcast_GetCount(&manager);
    TEST_ASSERT(count >= 4, "Count is at least 4");
}

/* Test 14: BulkAdd - 批量添加 */
static void Test_BulkAdd() {
    TEST_START("MemberBroadcast_BulkAdd");

    MemberBroadcast records[3];

    records[0].event_id = 2001;
    records[0].m_id = 20001;
    records[0].server_id = 3;
    records[0].charac_no = 200001;
    strcpy(records[0].charac_name, "BulkCharacter1");
    strcpy(records[0].start_time, "2025-02-01 10:00:00");
    strcpy(records[0].end_time, "2025-02-01 12:00:00");

    records[1].event_id = 2002;
    records[1].m_id = 20002;
    records[1].server_id = 3;
    records[1].charac_no = 200002;
    strcpy(records[1].charac_name, "BulkCharacter2");
    strcpy(records[1].start_time, "2025-02-02 10:00:00");
    strcpy(records[1].end_time, "2025-02-02 12:00:00");

    records[2].event_id = 2003;
    records[2].m_id = 20003;
    records[2].server_id = 3;
    records[2].charac_no = 200003;
    strcpy(records[2].charac_name, "BulkCharacter3");
    strcpy(records[2].start_time, "2025-02-03 10:00:00");
    strcpy(records[2].end_time, "2025-02-03 12:00:00");

    int count = MemberBroadcast_BulkAdd(&manager, records, 3);
    TEST_ASSERT(count == 3, "BulkAdd added 3 records");
}

/* Test 15: BulkDeleteByEvent - 批量删除 */
static void Test_BulkDeleteByEvent() {
    TEST_START("MemberBroadcast_BulkDeleteByEvent");

    int event_ids[3] = {2001, 2002, 2003};
    int ret = MemberBroadcast_BulkDeleteByEvent(&manager, event_ids, 3);
    TEST_ASSERT(ret == 0, "BulkDeleteByEvent successful");

    /* 验证删除 */
    int exists = MemberBroadcast_Exists(&manager, 2001, 20001, 3, 200001, "2025-02-01 10:00:00");
    TEST_ASSERT(exists == 0, "Record with event_id 2001 deleted");
}

/* Test 16: Delete幂等性测试 */
static void Test_DeleteIdempotent() {
    TEST_START("MemberBroadcast_Delete (Idempotent)");

    /* 第一次删除 */
    int ret1 = MemberBroadcast_Delete(&manager, 1002, 10002, 2, 100002, "2025-01-02 10:00:00");
    TEST_ASSERT(ret1 == 0, "First delete");

    /* 第二次删除（幂等） */
    int ret2 = MemberBroadcast_Delete(&manager, 1002, 10002, 2, 100002, "2025-01-02 10:00:00");
    TEST_ASSERT(ret2 == 0, "Second delete (idempotent)");

    /* 验证不存在 */
    int exists = MemberBroadcast_Exists(&manager, 1002, 10002, 2, 100002, "2025-01-02 10:00:00");
    TEST_ASSERT(exists == 0, "Record does not exist");
}

/* Test 17: PrintInfo - 工具函数测试 */
static void Test_PrintInfo() {
    TEST_START("MemberBroadcast_PrintInfo");

    MemberBroadcast record;
    record.event_id = 9999;
    record.m_id = 99999;
    record.server_id = 99;
    record.charac_no = 999999;
    strcpy(record.charac_name, "TestPrint");
    strcpy(record.start_time, "2025-12-31 23:59:59");
    strcpy(record.end_time, "2026-01-01 00:00:00");

    printf("  Testing PrintInfo output:\n");
    MemberBroadcast_PrintInfo(&record);

    TEST_ASSERT(1, "PrintInfo executed");
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    printf("===========================================\n");
    printf("  member_broadcast_orm Test Suite\n");
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
    Test_GetByEventId();
    Test_GetByMemberId();
    Test_GetByServerId();
    Test_GetByCharacName();
    Test_GetByTimeRange();
    Test_GetActive();
    Test_GetAll();
    Test_GetCount();
    Test_BulkAdd();
    Test_BulkDeleteByEvent();
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
