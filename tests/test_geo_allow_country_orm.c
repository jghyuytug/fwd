#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "geo_allow_country_orm.h"

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
    DBConnectionManager_ExecuteQuery(&manager, DB_TYPE_TAIWAN, "TRUNCATE TABLE geo_allow_country", &result);
    DBQueryResult_Free(&result);
}

/* Test 1: Add - 添加白名单记录 */
static void Test_Add() {
    TEST_START("GeoAllowCountry_Add");

    GeoAllowCountry record;
    record.server_group = 1;
    strcpy(record.country_code, "CN");
    strcpy(record.reg_date, "2025-01-01 10:00:00");

    int ret = GeoAllowCountry_Add(&manager, &record);
    TEST_ASSERT(ret == 0, "Add country whitelist");
}

/* Test 2: Get - 获取白名单记录 */
static void Test_Get() {
    TEST_START("GeoAllowCountry_Get");

    GeoAllowCountry record;
    int ret = GeoAllowCountry_Get(&manager, 1, "CN", &record);

    TEST_ASSERT(ret == 0, "Get country whitelist");
    TEST_ASSERT(record.server_group == 1, "server_group matches");
    TEST_ASSERT(strcmp(record.country_code, "CN") == 0, "country_code matches");
    TEST_ASSERT(strcmp(record.reg_date, "2025-01-01 10:00:00") == 0, "reg_date matches");
}

/* Test 3: Exists - 检查存在性 */
static void Test_Exists() {
    TEST_START("GeoAllowCountry_Exists");

    int exists = GeoAllowCountry_Exists(&manager, 1, "CN");
    TEST_ASSERT(exists == 1, "Record exists");

    exists = GeoAllowCountry_Exists(&manager, 99, "XX");
    TEST_ASSERT(exists == 0, "Non-existent record");
}

/* Test 4: Update - 更新白名单记录 */
static void Test_Update() {
    TEST_START("GeoAllowCountry_Update");

    GeoAllowCountry record;
    record.server_group = 1;
    strcpy(record.country_code, "CN");
    strcpy(record.reg_date, "2025-01-15 15:30:00");

    int ret = GeoAllowCountry_Update(&manager, &record);
    TEST_ASSERT(ret == 0, "Update country whitelist");

    /* 验证更新 */
    GeoAllowCountry updated;
    GeoAllowCountry_Get(&manager, 1, "CN", &updated);
    TEST_ASSERT(strcmp(updated.reg_date, "2025-01-15 15:30:00") == 0, "reg_date updated");
}

/* Test 5: Add幂等性测试 */
static void Test_AddIdempotent() {
    TEST_START("GeoAllowCountry_Add (Idempotent)");

    GeoAllowCountry record;
    record.server_group = 2;
    strcpy(record.country_code, "US");
    strcpy(record.reg_date, "2025-01-02 11:00:00");

    /* 第一次添加 */
    int ret1 = GeoAllowCountry_Add(&manager, &record);
    TEST_ASSERT(ret1 == 0, "First add");

    /* 第二次添加（幂等） */
    int ret2 = GeoAllowCountry_Add(&manager, &record);
    TEST_ASSERT(ret2 == 0, "Second add (idempotent)");

    /* 验证只有一条记录 */
    int count = GeoAllowCountry_GetCount(&manager);
    TEST_ASSERT(count == 2, "Only 2 records exist");
}

/* Test 6: GetByServerGroup - 根据服务器组查询 */
static void Test_GetByServerGroup() {
    TEST_START("GeoAllowCountry_GetByServerGroup");

    /* 添加更多测试数据 */
    GeoAllowCountry record1;
    record1.server_group = 1;
    strcpy(record1.country_code, "JP");
    strcpy(record1.reg_date, "2025-01-03 12:00:00");
    GeoAllowCountry_Add(&manager, &record1);

    GeoAllowCountry record2;
    record2.server_group = 1;
    strcpy(record2.country_code, "KR");
    strcpy(record2.reg_date, "2025-01-04 13:00:00");
    GeoAllowCountry_Add(&manager, &record2);

    /* 查询 */
    GeoAllowCountry results[10];
    int count = GeoAllowCountry_GetByServerGroup(&manager, 1, results, 10);

    TEST_ASSERT(count == 3, "Found 3 records for server_group 1");
}

/* Test 7: GetByCountryCode - 根据国家代码查询 */
static void Test_GetByCountryCode() {
    TEST_START("GeoAllowCountry_GetByCountryCode");

    /* 添加更多服务器组 */
    GeoAllowCountry record;
    record.server_group = 3;
    strcpy(record.country_code, "CN");
    strcpy(record.reg_date, "2025-01-05 14:00:00");
    GeoAllowCountry_Add(&manager, &record);

    /* 查询 */
    GeoAllowCountry results[10];
    int count = GeoAllowCountry_GetByCountryCode(&manager, "CN", results, 10);

    TEST_ASSERT(count == 2, "Found 2 server groups for CN");
}

/* Test 8: GetByDateRange - 日期范围查询 */
static void Test_GetByDateRange() {
    TEST_START("GeoAllowCountry_GetByDateRange");

    GeoAllowCountry results[10];
    int count = GeoAllowCountry_GetByDateRange(&manager,
        "2025-01-01 00:00:00", "2025-01-03 23:59:59", results, 10);

    TEST_ASSERT(count >= 2, "Found records in date range");
}

/* Test 9: GetRecentAdded - 获取最近添加的记录 */
static void Test_GetRecentAdded() {
    TEST_START("GeoAllowCountry_GetRecentAdded");

    GeoAllowCountry results[5];
    int count = GeoAllowCountry_GetRecentAdded(&manager, results, 5);

    TEST_ASSERT(count == 5, "GetRecentAdded returned 5 records");
    /* 最近的应该是 server_group=1, country_code=CN (已更新到2025-01-15) */
    TEST_ASSERT(results[0].server_group == 1 && strcmp(results[0].country_code, "CN") == 0,
                "Most recent is server_group 1, CN");
}

/* Test 10: GetAll - 获取所有记录（分页） */
static void Test_GetAll() {
    TEST_START("GeoAllowCountry_GetAll");

    GeoAllowCountry results[10];
    int count = GeoAllowCountry_GetAll(&manager, results, 0, 10);

    TEST_ASSERT(count >= 5, "GetAll returned records");
}

/* Test 11: GetCount - 获取总数 */
static void Test_GetCount() {
    TEST_START("GeoAllowCountry_GetCount");

    int count = GeoAllowCountry_GetCount(&manager);
    TEST_ASSERT(count == 5, "Count is correct");
}

/* Test 12: GetTop - 获取前N条记录 */
static void Test_GetTop() {
    TEST_START("GeoAllowCountry_GetTop");

    GeoAllowCountry results[3];
    int count = GeoAllowCountry_GetTop(&manager, results, 3);

    TEST_ASSERT(count == 3, "GetTop returned 3 records");
    TEST_ASSERT(results[0].server_group == 1, "First record server_group is 1");
}

/* Test 13: BulkAdd - 批量添加 */
static void Test_BulkAdd() {
    TEST_START("GeoAllowCountry_BulkAdd");

    GeoAllowCountry records[3];

    records[0].server_group = 4;
    strcpy(records[0].country_code, "GB");
    strcpy(records[0].reg_date, "2025-01-06 15:00:00");

    records[1].server_group = 4;
    strcpy(records[1].country_code, "FR");
    strcpy(records[1].reg_date, "2025-01-07 16:00:00");

    records[2].server_group = 4;
    strcpy(records[2].country_code, "DE");
    strcpy(records[2].reg_date, "2025-01-08 17:00:00");

    int count = GeoAllowCountry_BulkAdd(&manager, records, 3);
    TEST_ASSERT(count == 3, "BulkAdd added 3 records");
}

/* Test 14: BulkDelete - 批量删除 */
static void Test_BulkDelete() {
    TEST_START("GeoAllowCountry_BulkDelete");

    GeoAllowCountry records[3];
    records[0].server_group = 4;
    strcpy(records[0].country_code, "GB");

    records[1].server_group = 4;
    strcpy(records[1].country_code, "FR");

    records[2].server_group = 4;
    strcpy(records[2].country_code, "DE");

    int ret = GeoAllowCountry_BulkDelete(&manager, records, 3);
    TEST_ASSERT(ret == 0, "BulkDelete successful");

    /* 验证删除 */
    int exists = GeoAllowCountry_Exists(&manager, 4, "GB");
    TEST_ASSERT(exists == 0, "Record deleted");
}

/* Test 15: Delete幂等性测试 */
static void Test_DeleteIdempotent() {
    TEST_START("GeoAllowCountry_Delete (Idempotent)");

    /* 第一次删除 */
    int ret1 = GeoAllowCountry_Delete(&manager, 2, "US");
    TEST_ASSERT(ret1 == 0, "First delete");

    /* 第二次删除（幂等） */
    int ret2 = GeoAllowCountry_Delete(&manager, 2, "US");
    TEST_ASSERT(ret2 == 0, "Second delete (idempotent)");

    /* 验证不存在 */
    int exists = GeoAllowCountry_Exists(&manager, 2, "US");
    TEST_ASSERT(exists == 0, "Record does not exist");
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    printf("===========================================\n");
    printf("  geo_allow_country_orm Test Suite\n");
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
    Test_GetByServerGroup();
    Test_GetByCountryCode();
    Test_GetByDateRange();
    Test_GetRecentAdded();
    Test_GetAll();
    Test_GetCount();
    Test_GetTop();
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
