#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "geo_allow_orm.h"

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
    DBConnectionManager_ExecuteQuery(&manager, DB_TYPE_TAIWAN, "TRUNCATE TABLE geo_allow", &result);
    DBQueryResult_Free(&result);
}

/* Test 1: Add - 添加IP白名单 */
static void Test_Add() {
    TEST_START("GeoAllow_Add");

    GeoAllow allow;
    strcpy(allow.allow_ip, "192.168.1.100");
    strcpy(allow.allow_c_code, "CN");
    strcpy(allow.allow_date, "2025-11-14 10:00:00");

    int ret = GeoAllow_Add(&manager, &allow);
    TEST_ASSERT(ret == 0, "Add IP whitelist");
}

/* Test 2: Get - 获取IP白名单 */
static void Test_Get() {
    TEST_START("GeoAllow_Get");

    GeoAllow allow;
    int ret = GeoAllow_Get(&manager, "192.168.1.100", &allow);

    TEST_ASSERT(ret == 0, "Get IP whitelist");
    TEST_ASSERT(strcmp(allow.allow_ip, "192.168.1.100") == 0, "IP matches");
    TEST_ASSERT(strcmp(allow.allow_c_code, "CN") == 0, "Country code matches");
}

/* Test 3: Exists - 检查存��性 */
static void Test_Exists() {
    TEST_START("GeoAllow_Exists");

    int exists = GeoAllow_Exists(&manager, "192.168.1.100");
    TEST_ASSERT(exists == 1, "Record exists");

    exists = GeoAllow_Exists(&manager, "10.0.0.1");
    TEST_ASSERT(exists == 0, "Non-existent record");
}

/* Test 4: Update - 更新IP白名单 */
static void Test_Update() {
    TEST_START("GeoAllow_Update");

    GeoAllow allow;
    strcpy(allow.allow_ip, "192.168.1.100");
    strcpy(allow.allow_c_code, "US");
    strcpy(allow.allow_date, "2025-11-14 11:00:00");

    int ret = GeoAllow_Update(&manager, &allow);
    TEST_ASSERT(ret == 0, "Update IP whitelist");

    /* 验证更新 */
    GeoAllow updated;
    GeoAllow_Get(&manager, "192.168.1.100", &updated);
    TEST_ASSERT(strcmp(updated.allow_c_code, "US") == 0, "Country code updated");
}

/* Test 5: Add幂等性测试 */
static void Test_AddIdempotent() {
    TEST_START("GeoAllow_Add (Idempotent)");

    GeoAllow allow;
    strcpy(allow.allow_ip, "10.0.0.1");
    strcpy(allow.allow_c_code, "JP");
    strcpy(allow.allow_date, "2025-11-14 12:00:00");

    /* 第一次添加 */
    int ret1 = GeoAllow_Add(&manager, &allow);
    TEST_ASSERT(ret1 == 0, "First add");

    /* 第二次添加（幂等） */
    int ret2 = GeoAllow_Add(&manager, &allow);
    TEST_ASSERT(ret2 == 0, "Second add (idempotent)");

    /* 验证只有一条记录 */
    int count = GeoAllow_GetCount(&manager);
    TEST_ASSERT(count == 2, "Only 2 records exist");
}

/* Test 6: GetByCountryCode - 根据国家代码查询 */
static void Test_GetByCountryCode() {
    TEST_START("GeoAllow_GetByCountryCode");

    /* 添加测试数据 */
    GeoAllow allow1;
    strcpy(allow1.allow_ip, "192.168.2.1");
    strcpy(allow1.allow_c_code, "KR");
    strcpy(allow1.allow_date, "2025-11-14 13:00:00");
    GeoAllow_Add(&manager, &allow1);

    GeoAllow allow2;
    strcpy(allow2.allow_ip, "192.168.2.2");
    strcpy(allow2.allow_c_code, "KR");
    strcpy(allow2.allow_date, "2025-11-14 14:00:00");
    GeoAllow_Add(&manager, &allow2);

    /* 查询 */
    GeoAllow results[10];
    int count = GeoAllow_GetByCountryCode(&manager, "KR", results, 10);

    TEST_ASSERT(count == 2, "Found 2 records with country code KR");
}

/* Test 7: GetByIPPattern - IP模糊查询 */
static void Test_GetByIPPattern() {
    TEST_START("GeoAllow_GetByIPPattern");

    GeoAllow results[10];
    int count = GeoAllow_GetByIPPattern(&manager, "192.168.%", results, 10);

    TEST_ASSERT(count >= 3, "Found records matching IP pattern 192.168.%");
}

/* Test 8: GetByDateRange - 日期范围查询 */
static void Test_GetByDateRange() {
    TEST_START("GeoAllow_GetByDateRange");

    GeoAllow results[10];
    int count = GeoAllow_GetByDateRange(&manager, "2025-11-14", "2025-11-15", results, 10);

    TEST_ASSERT(count >= 4, "Found records in date range");
}

/* Test 9: GetAll - 获取所有记录（分页） */
static void Test_GetAll() {
    TEST_START("GeoAllow_GetAll");

    GeoAllow results[10];
    int count = GeoAllow_GetAll(&manager, results, 0, 10);

    TEST_ASSERT(count >= 4, "GetAll returned records");
}

/* Test 10: GetCount - 获取总数 */
static void Test_GetCount() {
    TEST_START("GeoAllow_GetCount");

    int count = GeoAllow_GetCount(&manager);
    TEST_ASSERT(count >= 4, "Count is correct");
}

/* Test 11: GetRecent - 获取最近记录 */
static void Test_GetRecent() {
    TEST_START("GeoAllow_GetRecent");

    GeoAllow results[5];
    int count = GeoAllow_GetRecent(&manager, results, 5);

    TEST_ASSERT(count >= 4 && count <= 5, "GetRecent returned records");
}

/* Test 12: GetOldRecords - 获取旧记录 */
static void Test_GetOldRecords() {
    TEST_START("GeoAllow_GetOldRecords");

    GeoAllow results[10];
    int count = GeoAllow_GetOldRecords(&manager, 1, results, 10);

    /* 所有测试数据都是今天的，不应该有旧记录 */
    TEST_ASSERT(count == 0, "No old records found");
}

/* Test 13: BulkAdd - 批量添加 */
static void Test_BulkAdd() {
    TEST_START("GeoAllow_BulkAdd");

    GeoAllow allows[3];

    strcpy(allows[0].allow_ip, "172.16.0.1");
    strcpy(allows[0].allow_c_code, "TW");
    strcpy(allows[0].allow_date, "2025-11-14 15:00:00");

    strcpy(allows[1].allow_ip, "172.16.0.2");
    strcpy(allows[1].allow_c_code, "HK");
    strcpy(allows[1].allow_date, "2025-11-14 16:00:00");

    strcpy(allows[2].allow_ip, "172.16.0.3");
    strcpy(allows[2].allow_c_code, "SG");
    strcpy(allows[2].allow_date, "2025-11-14 17:00:00");

    int count = GeoAllow_BulkAdd(&manager, allows, 3);
    TEST_ASSERT(count == 3, "BulkAdd added 3 records");
}

/* Test 14: BulkDelete - 批量删除 */
static void Test_BulkDelete() {
    TEST_START("GeoAllow_BulkDelete");

    const char* ips[] = {"172.16.0.1", "172.16.0.2", "172.16.0.3"};
    int ret = GeoAllow_BulkDelete(&manager, ips, 3);
    TEST_ASSERT(ret == 0, "BulkDelete successful");

    /* 验证删除 */
    int exists = GeoAllow_Exists(&manager, "172.16.0.1");
    TEST_ASSERT(exists == 0, "Record 172.16.0.1 deleted");
}

/* Test 15: Delete幂等性测试 */
static void Test_DeleteIdempotent() {
    TEST_START("GeoAllow_Delete (Idempotent)");

    /* 第一次删除 */
    int ret1 = GeoAllow_Delete(&manager, "10.0.0.1");
    TEST_ASSERT(ret1 == 0, "First delete");

    /* 第二次删除（幂等） */
    int ret2 = GeoAllow_Delete(&manager, "10.0.0.1");
    TEST_ASSERT(ret2 == 0, "Second delete (idempotent)");

    /* 验证不存在 */
    int exists = GeoAllow_Exists(&manager, "10.0.0.1");
    TEST_ASSERT(exists == 0, "Record does not exist");
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    printf("===========================================\n");
    printf("  geo_allow_orm Test Suite\n");
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
    Test_GetByIPPattern();
    Test_GetByDateRange();
    Test_GetAll();
    Test_GetCount();
    Test_GetRecent();
    Test_GetOldRecords();
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
