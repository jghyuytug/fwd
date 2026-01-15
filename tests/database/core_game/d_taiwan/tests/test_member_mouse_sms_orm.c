#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "member_mouse_sms_orm.h"

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
    DBConnectionManager_ExecuteQuery(&manager, DB_TYPE_TAIWAN, "TRUNCATE TABLE member_mouse_sms", &result);
    DBQueryResult_Free(&result);
}

/* Test 1: Add - 添加短信验证记录 */
static void Test_Add() {
    TEST_START("MemberMouseSms_Add");

    MemberMouseSms record;
    record.m_id = 1001;
    strcpy(record.occ_time, "2025-11-14 10:30:00");
    record.cnt = 1;

    int ret = MemberMouseSms_Add(&manager, &record);
    TEST_ASSERT(ret == 0, "Add SMS record");
}

/* Test 2: Get - 获取记录 */
static void Test_Get() {
    TEST_START("MemberMouseSms_Get");

    MemberMouseSms record;
    int ret = MemberMouseSms_Get(&manager, 1001, &record);

    TEST_ASSERT(ret == 0, "Get SMS record");
    TEST_ASSERT(record.m_id == 1001, "m_id matches");
    TEST_ASSERT(record.cnt == 1, "cnt matches");
}

/* Test 3: Exists - 检查存在性 */
static void Test_Exists() {
    TEST_START("MemberMouseSms_Exists");

    int exists = MemberMouseSms_Exists(&manager, 1001);
    TEST_ASSERT(exists == 1, "Record exists");

    exists = MemberMouseSms_Exists(&manager, 9999);
    TEST_ASSERT(exists == 0, "Non-existent record");
}

/* Test 4: Update - 更新记录 */
static void Test_Update() {
    TEST_START("MemberMouseSms_Update");

    MemberMouseSms record;
    record.m_id = 1001;
    strcpy(record.occ_time, "2025-11-14 11:00:00");
    record.cnt = 3;

    int ret = MemberMouseSms_Update(&manager, &record);
    TEST_ASSERT(ret == 0, "Update SMS record");

    /* 验证更新 */
    MemberMouseSms updated;
    MemberMouseSms_Get(&manager, 1001, &updated);
    TEST_ASSERT(updated.cnt == 3, "cnt updated");
}

/* Test 5: Add幂等性测试 */
static void Test_AddIdempotent() {
    TEST_START("MemberMouseSms_Add (Idempotent)");

    MemberMouseSms record1;
    record1.m_id = 1002;
    strcpy(record1.occ_time, "2025-11-14 09:00:00");
    record1.cnt = 1;

    /* 第一次添加 */
    int ret1 = MemberMouseSms_Add(&manager, &record1);
    TEST_ASSERT(ret1 == 0, "First add");

    /* 第二次添加（幂等） - cnt=5 */
    MemberMouseSms record2;
    record2.m_id = 1002;
    strcpy(record2.occ_time, "2025-11-14 10:00:00");
    record2.cnt = 5;

    int ret2 = MemberMouseSms_Add(&manager, &record2);
    TEST_ASSERT(ret2 == 0, "Second add (idempotent)");

    /* 验证最新数据 */
    MemberMouseSms result;
    MemberMouseSms_Get(&manager, 1002, &result);
    TEST_ASSERT(result.cnt == 5, "cnt replaced to 5");
}

/* Test 6: GetAll - 获取所有记录 */
static void Test_GetAll() {
    TEST_START("MemberMouseSms_GetAll");

    /* 添加更多测试数据 */
    MemberMouseSms r1 = {1003, "2025-11-14 08:00:00", 2};
    MemberMouseSms r2 = {1004, "2025-11-14 09:00:00", 4};
    MemberMouseSms_Add(&manager, &r1);
    MemberMouseSms_Add(&manager, &r2);

    MemberMouseSms results[10];
    int count = MemberMouseSms_GetAll(&manager, results, 0, 10);

    TEST_ASSERT(count >= 4, "GetAll returned at least 4 records");
}

/* Test 7: GetCount - 获取总数 */
static void Test_GetCount() {
    TEST_START("MemberMouseSms_GetCount");

    int count = MemberMouseSms_GetCount(&manager);
    TEST_ASSERT(count >= 4, "Count is at least 4");
}

/* Test 8: GetByOccTimeRange - 按时间范围查询 */
static void Test_GetByOccTimeRange() {
    TEST_START("MemberMouseSms_GetByOccTimeRange");

    MemberMouseSms results[10];
    int count = MemberMouseSms_GetByOccTimeRange(&manager,
        "2025-11-14 08:00:00", "2025-11-14 10:00:00", results, 10);

    TEST_ASSERT(count >= 2, "Found records in time range");
}

/* Test 9: GetByCntRange - 按验证次数范围查询 */
static void Test_GetByCntRange() {
    TEST_START("MemberMouseSms_GetByCntRange");

    MemberMouseSms results[10];
    int count = MemberMouseSms_GetByCntRange(&manager, 2, 5, results, 10);

    TEST_ASSERT(count >= 2, "Found records with cnt in range [2,5]");
}

/* Test 10: GetRecent - 获取最近记录 */
static void Test_GetRecent() {
    TEST_START("MemberMouseSms_GetRecent");

    MemberMouseSms results[10];
    int count = MemberMouseSms_GetRecent(&manager, results, 3);

    TEST_ASSERT(count >= 1, "Got recent records");
    /* 验证排序（应该是最新的在前） */
    if (count >= 2) {
        TEST_ASSERT(strcmp(results[0].occ_time, results[1].occ_time) >= 0,
                   "Records sorted DESC by occ_time");
    }
}

/* Test 11: GetOldRecords - 获取旧记录 */
static void Test_GetOldRecords() {
    TEST_START("MemberMouseSms_GetOldRecords");

    /* 添加一条旧记录 */
    MemberMouseSms old_record = {1005, "2020-01-01 00:00:00", 1};
    MemberMouseSms_Add(&manager, &old_record);

    MemberMouseSms results[10];
    int count = MemberMouseSms_GetOldRecords(&manager, 365, results, 10);

    TEST_ASSERT(count >= 1, "Found old records (>365 days)");
}

/* Test 12: IncrementCnt - 原子递增 */
static void Test_IncrementCnt() {
    TEST_START("MemberMouseSms_IncrementCnt");

    /* 获取当前cnt */
    MemberMouseSms before;
    MemberMouseSms_Get(&manager, 1001, &before);

    /* 递增 */
    int ret = MemberMouseSms_IncrementCnt(&manager, 1001);
    TEST_ASSERT(ret == 0, "IncrementCnt successful");

    /* 验证递增 */
    MemberMouseSms after;
    MemberMouseSms_Get(&manager, 1001, &after);
    TEST_ASSERT(after.cnt == before.cnt + 1, "cnt incremented by 1");
}

/* Test 13: BulkAdd - 批量添加 */
static void Test_BulkAdd() {
    TEST_START("MemberMouseSms_BulkAdd");

    MemberMouseSms records[3] = {
        {2001, "2025-11-14 12:00:00", 1},
        {2002, "2025-11-14 12:10:00", 2},
        {2003, "2025-11-14 12:20:00", 3}
    };

    int count = MemberMouseSms_BulkAdd(&manager, records, 3);
    TEST_ASSERT(count == 3, "BulkAdd added 3 records");

    /* 验证添加 */
    int exists = MemberMouseSms_Exists(&manager, 2001);
    TEST_ASSERT(exists == 1, "2001 exists");
}

/* Test 14: BulkDelete - 批量删除 */
static void Test_BulkDelete() {
    TEST_START("MemberMouseSms_BulkDelete");

    int m_ids[3] = {2001, 2002, 2003};

    int ret = MemberMouseSms_BulkDelete(&manager, m_ids, 3);
    TEST_ASSERT(ret == 0, "BulkDelete successful");

    /* 验证删除 */
    int exists = MemberMouseSms_Exists(&manager, 2001);
    TEST_ASSERT(exists == 0, "2001 deleted");
}

/* Test 15: Delete幂等性测试 */
static void Test_DeleteIdempotent() {
    TEST_START("MemberMouseSms_Delete (Idempotent)");

    /* 第一次删除 */
    int ret1 = MemberMouseSms_Delete(&manager, 1002);
    TEST_ASSERT(ret1 == 0, "First delete");

    /* 第二次删除（幂等） */
    int ret2 = MemberMouseSms_Delete(&manager, 1002);
    TEST_ASSERT(ret2 == 0, "Second delete (idempotent)");

    /* 验证不存在 */
    int exists = MemberMouseSms_Exists(&manager, 1002);
    TEST_ASSERT(exists == 0, "Record does not exist");
}

/* Test 16: PrintInfo - 工具函数测试 */
static void Test_PrintInfo() {
    TEST_START("MemberMouseSms_PrintInfo");

    MemberMouseSms record = {1001, "2025-11-14 10:00:00", 5};

    printf("  Testing PrintInfo output:\n");
    MemberMouseSms_PrintInfo(&record);

    TEST_ASSERT(1, "PrintInfo executed");
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    printf("===========================================\n");
    printf("  member_mouse_sms_orm Test Suite\n");
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
    Test_GetByOccTimeRange();
    Test_GetByCntRange();
    Test_GetRecent();
    Test_GetOldRecords();
    Test_IncrementCnt();
    Test_BulkAdd();
    Test_BulkDelete();
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
