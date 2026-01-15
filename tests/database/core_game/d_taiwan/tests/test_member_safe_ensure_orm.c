#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "member_safe_ensure_orm.h"

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
    DBConnectionManager_ExecuteQuery(&manager, DB_TYPE_TAIWAN, "TRUNCATE TABLE member_safe_ensure", &result);
    DBQueryResult_Free(&result);
}

/* Test 1: Add - 添加安全保障记录 */
static void Test_Add() {
    TEST_START("MemberSafeEnsure_Add");

    MemberSafeEnsure record;
    record.m_id = 10001;
    strcpy(record.occ_time, "2025-11-14 10:00:00");
    strcpy(record.mobile_no, "13800138000");
    record.service_flag = 1;
    record.type1_flag = 1;
    record.type2_flag = 0;
    strcpy(record.expire_time, "2026-11-14 10:00:00");
    strcpy(record.settle_id, "SETTLE001");

    int ret = MemberSafeEnsure_Add(&manager, &record);
    TEST_ASSERT(ret == 0, "Add safe ensure record");
}

/* Test 2: Get - 获取记录（复合主键） */
static void Test_Get() {
    TEST_START("MemberSafeEnsure_Get");

    MemberSafeEnsure record;
    int ret = MemberSafeEnsure_Get(&manager, 10001, "2025-11-14 10:00:00", &record);

    TEST_ASSERT(ret == 0, "Get safe ensure record");
    TEST_ASSERT(record.m_id == 10001, "m_id matches");
    TEST_ASSERT(strcmp(record.mobile_no, "13800138000") == 0, "mobile_no matches");
    TEST_ASSERT(strcmp(record.settle_id, "SETTLE001") == 0, "settle_id matches");
}

/* Test 3: Exists - 检查存在性 */
static void Test_Exists() {
    TEST_START("MemberSafeEnsure_Exists");

    int exists = MemberSafeEnsure_Exists(&manager, 10001, "2025-11-14 10:00:00");
    TEST_ASSERT(exists == 1, "Record exists");

    exists = MemberSafeEnsure_Exists(&manager, 99999, "2025-11-14 10:00:00");
    TEST_ASSERT(exists == 0, "Non-existent record");
}

/* Test 4: Update - 更新记录 */
static void Test_Update() {
    TEST_START("MemberSafeEnsure_Update");

    MemberSafeEnsure record;
    record.m_id = 10001;
    strcpy(record.occ_time, "2025-11-14 10:00:00");
    strcpy(record.mobile_no, "13900139000");  /* 更新手机号 */
    record.service_flag = 2;
    record.type1_flag = 1;
    record.type2_flag = 1;
    strcpy(record.expire_time, "2027-11-14 10:00:00");  /* 延长过期时间 */
    strcpy(record.settle_id, "SETTLE001");

    int ret = MemberSafeEnsure_Update(&manager, &record);
    TEST_ASSERT(ret == 0, "Update safe ensure record");

    /* 验证更新 */
    MemberSafeEnsure updated;
    MemberSafeEnsure_Get(&manager, 10001, "2025-11-14 10:00:00", &updated);
    TEST_ASSERT(strcmp(updated.mobile_no, "13900139000") == 0, "mobile_no updated");
    TEST_ASSERT(updated.service_flag == 2, "service_flag updated");
}

/* Test 5: Add幂等性测试 */
static void Test_AddIdempotent() {
    TEST_START("MemberSafeEnsure_Add (Idempotent)");

    MemberSafeEnsure record1;
    record1.m_id = 10002;
    strcpy(record1.occ_time, "2025-11-14 11:00:00");
    strcpy(record1.mobile_no, "13700137000");
    record1.service_flag = 1;
    record1.type1_flag = 0;
    record1.type2_flag = 0;
    strcpy(record1.expire_time, "2026-11-14 11:00:00");
    strcpy(record1.settle_id, "SETTLE002");

    /* 第一次添加 */
    int ret1 = MemberSafeEnsure_Add(&manager, &record1);
    TEST_ASSERT(ret1 == 0, "First add");

    /* 第二次添加（幂等） - 更新手机号 */
    MemberSafeEnsure record2;
    record2.m_id = 10002;
    strcpy(record2.occ_time, "2025-11-14 11:00:00");
    strcpy(record2.mobile_no, "13600136000");  /* 不同手机号 */
    record2.service_flag = 1;
    record2.type1_flag = 0;
    record2.type2_flag = 0;
    strcpy(record2.expire_time, "2026-11-14 11:00:00");
    strcpy(record2.settle_id, "SETTLE002");

    int ret2 = MemberSafeEnsure_Add(&manager, &record2);
    TEST_ASSERT(ret2 == 0, "Second add (idempotent)");

    /* 验证最新数据 */
    MemberSafeEnsure result;
    MemberSafeEnsure_Get(&manager, 10002, "2025-11-14 11:00:00", &result);
    TEST_ASSERT(strcmp(result.mobile_no, "13600136000") == 0, "mobile_no replaced");
}

/* Test 6: GetByMemberId - 按会员ID查询 */
static void Test_GetByMemberId() {
    TEST_START("MemberSafeEnsure_GetByMemberId");

    /* 添加更多测试数据 */
    MemberSafeEnsure r1 = {10003, "2025-11-14 08:00:00", "13500135000", 1, 0, 0, "2026-11-14 08:00:00", "SETTLE003"};
    MemberSafeEnsure r2 = {10003, "2025-11-14 09:00:00", "13500135000", 1, 1, 0, "2026-11-14 09:00:00", "SETTLE004"};
    MemberSafeEnsure_Add(&manager, &r1);
    MemberSafeEnsure_Add(&manager, &r2);

    MemberSafeEnsure results[10];
    int count = MemberSafeEnsure_GetByMemberId(&manager, 10003, results, 10);

    TEST_ASSERT(count >= 2, "Found at least 2 records for member 10003");
}

/* Test 7: GetByMobileNo - 按手机号查询 */
static void Test_GetByMobileNo() {
    TEST_START("MemberSafeEnsure_GetByMobileNo");

    MemberSafeEnsure results[10];
    int count = MemberSafeEnsure_GetByMobileNo(&manager, "13500135000", results, 10);

    TEST_ASSERT(count >= 2, "Found at least 2 records for mobile 13500135000");
}

/* Test 8: GetBySettleId - 按结算ID查询 */
static void Test_GetBySettleId() {
    TEST_START("MemberSafeEnsure_GetBySettleId");

    MemberSafeEnsure results[10];
    int count = MemberSafeEnsure_GetBySettleId(&manager, "SETTLE001", results, 10);

    TEST_ASSERT(count >= 1, "Found at least 1 record for SETTLE001");
}

/* Test 9: GetActiveServices - 获取当前有效服务 */
static void Test_GetActiveServices() {
    TEST_START("MemberSafeEnsure_GetActiveServices");

    /* 添加一个有效服务（从现在到明年） */
    MemberSafeEnsure active_record = {20001, "2025-11-13 00:00:00", "15000150000", 1, 1, 1, "2026-12-31 23:59:59", "SETTLE_ACTIVE"};
    MemberSafeEnsure_Add(&manager, &active_record);

    MemberSafeEnsure results[10];
    int count = MemberSafeEnsure_GetActiveServices(&manager, results, 10);

    TEST_ASSERT(count >= 1, "Found at least 1 active service");
}

/* Test 10: GetExpiredServices - 获取已过期服务 */
static void Test_GetExpiredServices() {
    TEST_START("MemberSafeEnsure_GetExpiredServices");

    /* 添加一个已过期服务 */
    MemberSafeEnsure expired_record = {30001, "2020-01-01 00:00:00", "16000160000", 1, 0, 0, "2020-12-31 23:59:59", "SETTLE_EXPIRED"};
    MemberSafeEnsure_Add(&manager, &expired_record);

    MemberSafeEnsure results[10];
    int count = MemberSafeEnsure_GetExpiredServices(&manager, results, 10);

    TEST_ASSERT(count >= 1, "Found at least 1 expired service");
}

/* Test 11: GetByServiceFlag - 按服务标志查询 */
static void Test_GetByServiceFlag() {
    TEST_START("MemberSafeEnsure_GetByServiceFlag");

    MemberSafeEnsure results[10];
    int count = MemberSafeEnsure_GetByServiceFlag(&manager, 1, results, 10);

    TEST_ASSERT(count >= 3, "Found at least 3 records with service_flag=1");
}

/* Test 12: GetByTimeRange - 按时间范围查询 */
static void Test_GetByTimeRange() {
    TEST_START("MemberSafeEnsure_GetByTimeRange");

    MemberSafeEnsure results[10];
    int count = MemberSafeEnsure_GetByTimeRange(&manager,
        "2025-11-14 08:00:00", "2025-11-14 12:00:00", results, 10);

    TEST_ASSERT(count >= 3, "Found records in time range");
}

/* Test 13: GetAll - 获取所有记录 */
static void Test_GetAll() {
    TEST_START("MemberSafeEnsure_GetAll");

    MemberSafeEnsure results[20];
    int count = MemberSafeEnsure_GetAll(&manager, results, 0, 20);

    TEST_ASSERT(count >= 5, "GetAll returned at least 5 records");
}

/* Test 14: GetCount - 获取总数 */
static void Test_GetCount() {
    TEST_START("MemberSafeEnsure_GetCount");

    int count = MemberSafeEnsure_GetCount(&manager);
    TEST_ASSERT(count >= 5, "Count is at least 5");
}

/* Test 15: BulkAdd - 批量添加 */
static void Test_BulkAdd() {
    TEST_START("MemberSafeEnsure_BulkAdd");

    MemberSafeEnsure records[3] = {
        {40001, "2025-11-14 12:00:00", "17000170000", 1, 0, 0, "2026-11-14 12:00:00", "SETTLE_BULK1"},
        {40002, "2025-11-14 12:10:00", "17000170001", 1, 0, 0, "2026-11-14 12:10:00", "SETTLE_BULK2"},
        {40003, "2025-11-14 12:20:00", "17000170002", 1, 0, 0, "2026-11-14 12:20:00", "SETTLE_BULK3"}
    };

    int count = MemberSafeEnsure_BulkAdd(&manager, records, 3);
    TEST_ASSERT(count == 3, "BulkAdd added 3 records");

    /* 验证添加 */
    int exists = MemberSafeEnsure_Exists(&manager, 40001, "2025-11-14 12:00:00");
    TEST_ASSERT(exists == 1, "40001 exists");
}

/* Test 16: BulkDeleteByMemberIds - 批量删除 */
static void Test_BulkDeleteByMemberIds() {
    TEST_START("MemberSafeEnsure_BulkDeleteByMemberIds");

    unsigned int m_ids[3] = {40001, 40002, 40003};

    int ret = MemberSafeEnsure_BulkDeleteByMemberIds(&manager, m_ids, 3);
    TEST_ASSERT(ret == 0, "BulkDeleteByMemberIds successful");

    /* 验证删除 */
    int exists = MemberSafeEnsure_Exists(&manager, 40001, "2025-11-14 12:00:00");
    TEST_ASSERT(exists == 0, "40001 deleted");
}

/* Test 17: Delete幂等性测试 */
static void Test_DeleteIdempotent() {
    TEST_START("MemberSafeEnsure_Delete (Idempotent)");

    /* 第一次删除 */
    int ret1 = MemberSafeEnsure_Delete(&manager, 10002, "2025-11-14 11:00:00");
    TEST_ASSERT(ret1 == 0, "First delete");

    /* 第二次删除（幂等） */
    int ret2 = MemberSafeEnsure_Delete(&manager, 10002, "2025-11-14 11:00:00");
    TEST_ASSERT(ret2 == 0, "Second delete (idempotent)");

    /* 验证不存在 */
    int exists = MemberSafeEnsure_Exists(&manager, 10002, "2025-11-14 11:00:00");
    TEST_ASSERT(exists == 0, "Record does not exist");
}

/* Test 18: PrintInfo - 工具函数测试 */
static void Test_PrintInfo() {
    TEST_START("MemberSafeEnsure_PrintInfo");

    MemberSafeEnsure record = {10001, "2025-11-14 10:00:00", "13900139000", 2, 1, 1, "2027-11-14 10:00:00", "SETTLE001"};

    printf("  Testing PrintInfo output:\n");
    MemberSafeEnsure_PrintInfo(&record);

    TEST_ASSERT(1, "PrintInfo executed");
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    printf("===========================================\n");
    printf("  member_safe_ensure_orm Test Suite\n");
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
    Test_GetActiveServices();
    Test_GetExpiredServices();
    Test_GetByServiceFlag();
    Test_GetByTimeRange();
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
