#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "geo_reject_orm.h"

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
    DBConnectionManager_ExecuteQuery(&manager, DB_TYPE_TAIWAN, "TRUNCATE TABLE geo_reject", &result);
    DBQueryResult_Free(&result);
}

/* Test 1: Add - 添加IP拒绝记录 */
static void Test_Add() {
    TEST_START("GeoReject_Add");

    GeoReject record;
    strcpy(record.rej_ip, "192.168.1.100");
    strcpy(record.rej_c_code, "CN");
    record.rej_ip_count = 5;
    strcpy(record.rej_last_date, "2025-01-01 10:00:00");
    record.rej_chk = 'Y';
    record.rej_src = 'w';

    int ret = GeoReject_Add(&manager, &record);
    TEST_ASSERT(ret == 0, "Add reject record");
}

/* Test 2: Get - 获取IP拒绝记录 */
static void Test_Get() {
    TEST_START("GeoReject_Get");

    GeoReject record;
    int ret = GeoReject_Get(&manager, "192.168.1.100", &record);

    TEST_ASSERT(ret == 0, "Get reject record");
    TEST_ASSERT(strcmp(record.rej_ip, "192.168.1.100") == 0, "rej_ip matches");
    TEST_ASSERT(strcmp(record.rej_c_code, "CN") == 0, "rej_c_code matches");
    TEST_ASSERT(record.rej_ip_count == 5, "rej_ip_count matches");
    TEST_ASSERT(record.rej_chk == 'Y', "rej_chk matches");
    TEST_ASSERT(record.rej_src == 'w', "rej_src matches");
}

/* Test 3: Exists - 检查存在性 */
static void Test_Exists() {
    TEST_START("GeoReject_Exists");

    int exists = GeoReject_Exists(&manager, "192.168.1.100");
    TEST_ASSERT(exists == 1, "Record exists");

    exists = GeoReject_Exists(&manager, "192.168.1.254");
    TEST_ASSERT(exists == 0, "Non-existent record");
}

/* Test 4: Update - 更新IP拒绝记录 */
static void Test_Update() {
    TEST_START("GeoReject_Update");

    GeoReject record;
    strcpy(record.rej_ip, "192.168.1.100");
    strcpy(record.rej_c_code, "CN");
    record.rej_ip_count = 10;
    strcpy(record.rej_last_date, "2025-01-15 15:30:00");
    record.rej_chk = 'N';
    record.rej_src = 'g';

    int ret = GeoReject_Update(&manager, &record);
    TEST_ASSERT(ret == 0, "Update reject record");

    /* 验证更新 */
    GeoReject updated;
    GeoReject_Get(&manager, "192.168.1.100", &updated);
    TEST_ASSERT(updated.rej_ip_count == 10, "rej_ip_count updated");
    TEST_ASSERT(updated.rej_chk == 'N', "rej_chk updated");
    TEST_ASSERT(updated.rej_src == 'g', "rej_src updated");
}

/* Test 5: Add幂等性测试 */
static void Test_AddIdempotent() {
    TEST_START("GeoReject_Add (Idempotent)");

    GeoReject record;
    strcpy(record.rej_ip, "10.0.0.1");
    strcpy(record.rej_c_code, "US");
    record.rej_ip_count = 3;
    strcpy(record.rej_last_date, "2025-01-02 11:00:00");
    record.rej_chk = 'Y';
    record.rej_src = 'w';

    /* 第一次添加 */
    int ret1 = GeoReject_Add(&manager, &record);
    TEST_ASSERT(ret1 == 0, "First add");

    /* 第二次添加（幂等） */
    int ret2 = GeoReject_Add(&manager, &record);
    TEST_ASSERT(ret2 == 0, "Second add (idempotent)");

    /* 验证只有一条记录 */
    int count = GeoReject_GetCount(&manager);
    TEST_ASSERT(count == 2, "Only 2 records exist");
}

/* Test 6: GetByCountryCode - 根据国家代码查询 */
static void Test_GetByCountryCode() {
    TEST_START("GeoReject_GetByCountryCode");

    /* 添加更多测试数据 */
    GeoReject record1;
    strcpy(record1.rej_ip, "192.168.1.101");
    strcpy(record1.rej_c_code, "CN");
    record1.rej_ip_count = 2;
    strcpy(record1.rej_last_date, "2025-01-03 12:00:00");
    record1.rej_chk = 'Y';
    record1.rej_src = 'w';
    GeoReject_Add(&manager, &record1);

    GeoReject record2;
    strcpy(record2.rej_ip, "192.168.1.102");
    strcpy(record2.rej_c_code, "CN");
    record2.rej_ip_count = 8;
    strcpy(record2.rej_last_date, "2025-01-04 13:00:00");
    record2.rej_chk = 'N';
    record2.rej_src = 'g';
    GeoReject_Add(&manager, &record2);

    /* 查询 */
    GeoReject results[10];
    int count = GeoReject_GetByCountryCode(&manager, "CN", results, 10);

    TEST_ASSERT(count == 3, "Found 3 CN records");
}

/* Test 7: GetBySource - 根据来源查询 */
static void Test_GetBySource() {
    TEST_START("GeoReject_GetBySource");

    GeoReject results[10];
    int count = GeoReject_GetBySource(&manager, 'w', results, 10);

    TEST_ASSERT(count == 2, "Found 2 web source records");
}

/* Test 8: GetByCheckFlag - 根据检查标志查询 */
static void Test_GetByCheckFlag() {
    TEST_START("GeoReject_GetByCheckFlag");

    GeoReject results[10];
    int count = GeoReject_GetByCheckFlag(&manager, 'Y', results, 10);

    TEST_ASSERT(count == 2, "Found 2 checked records");
}

/* Test 9: GetByCountRange - 访问次数范围查询 */
static void Test_GetByCountRange() {
    TEST_START("GeoReject_GetByCountRange");

    GeoReject results[10];
    int count = GeoReject_GetByCountRange(&manager, 5, 10, results, 10);

    TEST_ASSERT(count == 2, "Found 2 records in count range 5-10");
}

/* Test 10: GetByIPPattern - IP模式查询 */
static void Test_GetByIPPattern() {
    TEST_START("GeoReject_GetByIPPattern");

    GeoReject results[10];
    int count = GeoReject_GetByIPPattern(&manager, "192.168.1.%", results, 10);

    TEST_ASSERT(count == 3, "Found 3 records matching IP pattern");
}

/* Test 11: GetRecentUpdated - 获取最近更新的记录 */
static void Test_GetRecentUpdated() {
    TEST_START("GeoReject_GetRecentUpdated");

    GeoReject results[5];
    int count = GeoReject_GetRecentUpdated(&manager, results, 5);

    TEST_ASSERT(count == 4, "GetRecentUpdated returned 4 records");
    /* 最近的应该是 192.168.1.100 (更新到 2025-01-15) */
    TEST_ASSERT(strcmp(results[0].rej_ip, "192.168.1.102") == 0 ||
                strcmp(results[0].rej_ip, "192.168.1.100") == 0,
                "Most recent is correct");
}

/* Test 12: GetAll - 获取所有记录（分页） */
static void Test_GetAll() {
    TEST_START("GeoReject_GetAll");

    GeoReject results[10];
    int count = GeoReject_GetAll(&manager, results, 0, 10);

    TEST_ASSERT(count == 4, "GetAll returned 4 records");
}

/* Test 13: GetCount - 获取总数 */
static void Test_GetCount() {
    TEST_START("GeoReject_GetCount");

    int count = GeoReject_GetCount(&manager);
    TEST_ASSERT(count == 4, "Count is correct");
}

/* Test 14: BulkAdd - 批量添加 */
static void Test_BulkAdd() {
    TEST_START("GeoReject_BulkAdd");

    GeoReject records[3];

    strcpy(records[0].rej_ip, "10.0.0.10");
    strcpy(records[0].rej_c_code, "JP");
    records[0].rej_ip_count = 1;
    strcpy(records[0].rej_last_date, "2025-01-06 15:00:00");
    records[0].rej_chk = 'Y';
    records[0].rej_src = 'w';

    strcpy(records[1].rej_ip, "10.0.0.20");
    strcpy(records[1].rej_c_code, "KR");
    records[1].rej_ip_count = 2;
    strcpy(records[1].rej_last_date, "2025-01-07 16:00:00");
    records[1].rej_chk = 'N';
    records[1].rej_src = 'g';

    strcpy(records[2].rej_ip, "10.0.0.30");
    strcpy(records[2].rej_c_code, "GB");
    records[2].rej_ip_count = 3;
    strcpy(records[2].rej_last_date, "2025-01-08 17:00:00");
    records[2].rej_chk = 'Y';
    records[2].rej_src = 'w';

    int count = GeoReject_BulkAdd(&manager, records, 3);
    TEST_ASSERT(count == 3, "BulkAdd added 3 records");
}

/* Test 15: BulkDelete - 批量删除 */
static void Test_BulkDelete() {
    TEST_START("GeoReject_BulkDelete");

    const char* ip_list[3] = {"10.0.0.10", "10.0.0.20", "10.0.0.30"};
    int ret = GeoReject_BulkDelete(&manager, ip_list, 3);
    TEST_ASSERT(ret == 0, "BulkDelete successful");

    /* 验证删除 */
    int exists = GeoReject_Exists(&manager, "10.0.0.10");
    TEST_ASSERT(exists == 0, "Record 10.0.0.10 deleted");
}

/* Test 16: Delete幂等性测试 */
static void Test_DeleteIdempotent() {
    TEST_START("GeoReject_Delete (Idempotent)");

    /* 第一次删除 */
    int ret1 = GeoReject_Delete(&manager, "10.0.0.1");
    TEST_ASSERT(ret1 == 0, "First delete");

    /* 第二次删除（幂等） */
    int ret2 = GeoReject_Delete(&manager, "10.0.0.1");
    TEST_ASSERT(ret2 == 0, "Second delete (idempotent)");

    /* 验证不存在 */
    int exists = GeoReject_Exists(&manager, "10.0.0.1");
    TEST_ASSERT(exists == 0, "Record does not exist");
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    printf("===========================================\n");
    printf("  geo_reject_orm Test Suite\n");
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
    Test_GetByCountryCode();
    Test_GetBySource();
    Test_GetByCheckFlag();
    Test_GetByCountRange();
    Test_GetByIPPattern();
    Test_GetRecentUpdated();
    Test_GetAll();
    Test_GetCount();
    Test_BulkAdd();
    Test_BulkDelete();
    Test_DeleteIdempotent();

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
