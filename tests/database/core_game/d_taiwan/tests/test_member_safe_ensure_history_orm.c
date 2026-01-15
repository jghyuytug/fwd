#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "member_safe_ensure_history_orm.h"

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
    DBConnectionManager_ExecuteQuery(&manager, DB_TYPE_TAIWAN, "TRUNCATE TABLE member_safe_ensure_history", &result);
    DBQueryResult_Free(&result);
}

/* Test 1: Add - 添加安全保障历史记录 */
static void Test_Add() {
    TEST_START("MemberSafeEnsureHistory_Add");

    MemberSafeEnsureHistory record;
    record.mod_flag = 0;  /* 0=插入 */
    strcpy(record.mod_time, "2025-11-14 10:00:00");
    record.m_id = 10001;
    strcpy(record.occ_time, "2025-11-14 09:00:00");
    strcpy(record.mobile_no, "13800138000");
    record.service_flag = 1;
    record.type1_flag = 1;
    record.type2_flag = 0;
    strcpy(record.expire_time, "2026-11-14 09:00:00");
    strcpy(record.settle_id, "SETTLE001");

    int ret = MemberSafeEnsureHistory_Add(&manager, &record);
    TEST_ASSERT(ret == 0, "Add history record");
}

/* Test 2: Get - 获取记录（3字段复合主键） */
static void Test_Get() {
    TEST_START("MemberSafeEnsureHistory_Get");

    MemberSafeEnsureHistory record;
    int ret = MemberSafeEnsureHistory_Get(&manager, 10001, "2025-11-14 09:00:00", "2025-11-14 10:00:00", &record);

    TEST_ASSERT(ret == 0, "Get history record");
    TEST_ASSERT(record.m_id == 10001, "m_id matches");
    TEST_ASSERT(record.mod_flag == 0, "mod_flag matches");
    TEST_ASSERT(strcmp(record.mobile_no, "13800138000") == 0, "mobile_no matches");
}

/* Test 3: Exists - 检查存在性 */
static void Test_Exists() {
    TEST_START("MemberSafeEnsureHistory_Exists");

    int exists = MemberSafeEnsureHistory_Exists(&manager, 10001, "2025-11-14 09:00:00", "2025-11-14 10:00:00");
    TEST_ASSERT(exists == 1, "Record exists");

    exists = MemberSafeEnsureHistory_Exists(&manager, 99999, "2025-11-14 09:00:00", "2025-11-14 10:00:00");
    TEST_ASSERT(exists == 0, "Non-existent record");
}

/* Test 4: Update - 更新记录 */
static void Test_Update() {
    TEST_START("MemberSafeEnsureHistory_Update");

    MemberSafeEnsureHistory record;
    record.mod_flag = 1;  /* 1=更新 */
    strcpy(record.mod_time, "2025-11-14 10:00:00");
    record.m_id = 10001;
    strcpy(record.occ_time, "2025-11-14 09:00:00");
    strcpy(record.mobile_no, "13900139000");  /* 更新手机号 */
    record.service_flag = 2;
    record.type1_flag = 1;
    record.type2_flag = 1;
    strcpy(record.expire_time, "2027-11-14 09:00:00");
    strcpy(record.settle_id, "SETTLE001");

    int ret = MemberSafeEnsureHistory_Update(&manager, &record);
    TEST_ASSERT(ret == 0, "Update history record");

    /* 验证更新 */
    MemberSafeEnsureHistory updated;
    MemberSafeEnsureHistory_Get(&manager, 10001, "2025-11-14 09:00:00", "2025-11-14 10:00:00", &updated);
    TEST_ASSERT(strcmp(updated.mobile_no, "13900139000") == 0, "mobile_no updated");
    TEST_ASSERT(updated.mod_flag == 1, "mod_flag updated");
}

/* Test 5: Add幂等性测试 */
static void Test_AddIdempotent() {
    TEST_START("MemberSafeEnsureHistory_Add (Idempotent)");

    MemberSafeEnsureHistory record1;
    record1.mod_flag = 0;
    strcpy(record1.mod_time, "2025-11-14 11:00:00");
    record1.m_id = 10002;
    strcpy(record1.occ_time, "2025-11-14 10:00:00");
    strcpy(record1.mobile_no, "13700137000");
    record1.service_flag = 1;
    record1.type1_flag = 0;
    record1.type2_flag = 0;
    strcpy(record1.expire_time, "2026-11-14 10:00:00");
    strcpy(record1.settle_id, "SETTLE002");

    /* 第一次添加 */
    int ret1 = MemberSafeEnsureHistory_Add(&manager, &record1);
    TEST_ASSERT(ret1 == 0, "First add");

    /* 第二次添加（幂等） */
    MemberSafeEnsureHistory record2;
    record2.mod_flag = 1;
    strcpy(record2.mod_time, "2025-11-14 11:00:00");
    record2.m_id = 10002;
    strcpy(record2.occ_time, "2025-11-14 10:00:00");
    strcpy(record2.mobile_no, "13600136000");
    record2.service_flag = 1;
    record2.type1_flag = 0;
    record2.type2_flag = 0;
    strcpy(record2.expire_time, "2026-11-14 10:00:00");
    strcpy(record2.settle_id, "SETTLE002");

    int ret2 = MemberSafeEnsureHistory_Add(&manager, &record2);
    TEST_ASSERT(ret2 == 0, "Second add (idempotent)");

    /* 验证最新数据 */
    MemberSafeEnsureHistory result;
    MemberSafeEnsureHistory_Get(&manager, 10002, "2025-11-14 10:00:00", "2025-11-14 11:00:00", &result);
    TEST_ASSERT(strcmp(result.mobile_no, "13600136000") == 0, "mobile_no replaced");
}

/* Test 6: GetByMemberId - 按会员ID查询 */
static void Test_GetByMemberId() {
    TEST_START("MemberSafeEnsureHistory_GetByMemberId");

    /* 添加更多测试数据 */
    MemberSafeEnsureHistory r1 = {0, "2025-11-14 08:00:00", 10003, "2025-11-14 07:00:00", "13500135000", 1, 0, 0, "2026-11-14 07:00:00", "SETTLE003"};
    MemberSafeEnsureHistory r2 = {1, "2025-11-14 09:00:00", 10003, "2025-11-14 07:00:00", "13500135001", 1, 1, 0, "2026-11-14 07:00:00", "SETTLE004"};
    MemberSafeEnsureHistory_Add(&manager, &r1);
    MemberSafeEnsureHistory_Add(&manager, &r2);

    MemberSafeEnsureHistory results[10];
    int count = MemberSafeEnsureHistory_GetByMemberId(&manager, 10003, results, 10);

    TEST_ASSERT(count >= 2, "Found at least 2 records for member 10003");
}

/* Test 7: GetByMobileNo - 按手机号查询 */
static void Test_GetByMobileNo() {
    TEST_START("MemberSafeEnsureHistory_GetByMobileNo");

    MemberSafeEnsureHistory results[10];
    int count = MemberSafeEnsureHistory_GetByMobileNo(&manager, "13600136000", results, 10);

    TEST_ASSERT(count >= 1, "Found at least 1 record for mobile 13600136000");
}

/* Test 8: GetBySettleId - 按结算ID查询 */
static void Test_GetBySettleId() {
    TEST_START("MemberSafeEnsureHistory_GetBySettleId");

    MemberSafeEnsureHistory results[10];
    int count = MemberSafeEnsureHistory_GetBySettleId(&manager, "SETTLE001", results, 10);

    TEST_ASSERT(count >= 1, "Found at least 1 record for SETTLE001");
}

/* Test 9: GetByModFlag - 按修改标志查询 */
static void Test_GetByModFlag() {
    TEST_START("MemberSafeEnsureHistory_GetByModFlag");

    MemberSafeEnsureHistory results[10];
    int count = MemberSafeEnsureHistory_GetByModFlag(&manager, 0, results, 10);  /* 0=插入 */

    TEST_ASSERT(count >= 1, "Found at least 1 insert record (mod_flag=0)");
}

/* Test 10: GetByModTimeRange - 按修改时间范围查询 */
static void Test_GetByModTimeRange() {
    TEST_START("MemberSafeEnsureHistory_GetByModTimeRange");

    MemberSafeEnsureHistory results[10];
    int count = MemberSafeEnsureHistory_GetByModTimeRange(&manager,
        "2025-11-14 08:00:00", "2025-11-14 12:00:00", results, 10);

    TEST_ASSERT(count >= 3, "Found records in modification time range");
}

/* Test 11: GetByOccTimeRange - 按发生时间范围查询 */
static void Test_GetByOccTimeRange() {
    TEST_START("MemberSafeEnsureHistory_GetByOccTimeRange");

    MemberSafeEnsureHistory results[10];
    int count = MemberSafeEnsureHistory_GetByOccTimeRange(&manager,
        "2025-11-14 07:00:00", "2025-11-14 10:00:00", results, 10);

    TEST_ASSERT(count >= 3, "Found records in occurrence time range");
}

/* Test 12: GetRecentModifications - 获取最近修改 */
static void Test_GetRecentModifications() {
    TEST_START("MemberSafeEnsureHistory_GetRecentModifications");

    MemberSafeEnsureHistory results[10];
    int count = MemberSafeEnsureHistory_GetRecentModifications(&manager, results, 5);

    TEST_ASSERT(count >= 3, "Found at least 3 recent modifications");
    /* 验证排序（最近的在前） */
    if (count >= 2) {
        TEST_ASSERT(strcmp(results[0].mod_time, results[1].mod_time) >= 0, "Sorted by mod_time DESC");
    }
}

/* Test 13: GetAll - 获取所有记录 */
static void Test_GetAll() {
    TEST_START("MemberSafeEnsureHistory_GetAll");

    MemberSafeEnsureHistory results[20];
    int count = MemberSafeEnsureHistory_GetAll(&manager, results, 0, 20);

    TEST_ASSERT(count >= 4, "GetAll returned at least 4 records");
}

/* Test 14: GetCount - 获取总数 */
static void Test_GetCount() {
    TEST_START("MemberSafeEnsureHistory_GetCount");

    int count = MemberSafeEnsureHistory_GetCount(&manager);
    TEST_ASSERT(count >= 4, "Count is at least 4");
}

/* Test 15: BulkAdd - 批量添加 */
static void Test_BulkAdd() {
    TEST_START("MemberSafeEnsureHistory_BulkAdd");

    MemberSafeEnsureHistory records[3] = {
        {2, "2025-11-14 12:00:00", 40001, "2025-11-14 11:00:00", "17000170000", 1, 0, 0, "2026-11-14 11:00:00", "SETTLE_BULK1"},
        {2, "2025-11-14 12:10:00", 40002, "2025-11-14 11:10:00", "17000170001", 1, 0, 0, "2026-11-14 11:10:00", "SETTLE_BULK2"},
        {2, "2025-11-14 12:20:00", 40003, "2025-11-14 11:20:00", "17000170002", 1, 0, 0, "2026-11-14 11:20:00", "SETTLE_BULK3"}
    };

    int count = MemberSafeEnsureHistory_BulkAdd(&manager, records, 3);
    TEST_ASSERT(count == 3, "BulkAdd added 3 records");

    /* 验证添加 */
    int exists = MemberSafeEnsureHistory_Exists(&manager, 40001, "2025-11-14 11:00:00", "2025-11-14 12:00:00");
    TEST_ASSERT(exists == 1, "40001 exists");
}

/* Test 16: BulkDeleteByMemberIds - 批量删除 */
static void Test_BulkDeleteByMemberIds() {
    TEST_START("MemberSafeEnsureHistory_BulkDeleteByMemberIds");

    unsigned int m_ids[3] = {40001, 40002, 40003};

    int ret = MemberSafeEnsureHistory_BulkDeleteByMemberIds(&manager, m_ids, 3);
    TEST_ASSERT(ret == 0, "BulkDeleteByMemberIds successful");

    /* 验证删除 */
    int exists = MemberSafeEnsureHistory_Exists(&manager, 40001, "2025-11-14 11:00:00", "2025-11-14 12:00:00");
    TEST_ASSERT(exists == 0, "40001 deleted");
}

/* Test 17: Delete幂等性测试 */
static void Test_DeleteIdempotent() {
    TEST_START("MemberSafeEnsureHistory_Delete (Idempotent)");

    /* 第一次删除 */
    int ret1 = MemberSafeEnsureHistory_Delete(&manager, 10002, "2025-11-14 10:00:00", "2025-11-14 11:00:00");
    TEST_ASSERT(ret1 == 0, "First delete");

    /* 第二次删除（幂等） */
    int ret2 = MemberSafeEnsureHistory_Delete(&manager, 10002, "2025-11-14 10:00:00", "2025-11-14 11:00:00");
    TEST_ASSERT(ret2 == 0, "Second delete (idempotent)");

    /* 验证不存在 */
    int exists = MemberSafeEnsureHistory_Exists(&manager, 10002, "2025-11-14 10:00:00", "2025-11-14 11:00:00");
    TEST_ASSERT(exists == 0, "Record does not exist");
}

/* Test 18: PrintInfo - 工具函数测试 */
static void Test_PrintInfo() {
    TEST_START("MemberSafeEnsureHistory_PrintInfo");

    MemberSafeEnsureHistory record = {1, "2025-11-14 10:00:00", 10001, "2025-11-14 09:00:00", "13900139000",
                                       2, 1, 1, "2027-11-14 09:00:00", "SETTLE001"};

    printf("  Testing PrintInfo output:\n");
    MemberSafeEnsureHistory_PrintInfo(&record);

    TEST_ASSERT(1, "PrintInfo executed");
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    printf("===========================================\n");
    printf("  member_safe_ensure_history_orm Test Suite\n");
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
    Test_GetByMemberId();
    Test_GetByMobileNo();
    Test_GetBySettleId();
    Test_GetByModFlag();
    Test_GetByModTimeRange();
    Test_GetByOccTimeRange();
    Test_GetRecentModifications();
    Test_GetAll();
    Test_GetCount();
    Test_BulkAdd();
    Test_BulkDeleteByMemberIds();
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
