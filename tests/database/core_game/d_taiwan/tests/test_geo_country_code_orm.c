#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "geo_country_code_orm.h"

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
    DBConnectionManager_ExecuteQuery(&manager, DB_TYPE_TAIWAN, "TRUNCATE TABLE geo_country_code", &result);
    DBQueryResult_Free(&result);
}

/* Test 1: Add - 添加国家代码 */
static void Test_Add() {
    TEST_START("GeoCountryCode_Add");

    GeoCountryCode code;
    code.code_no = 156;
    strcpy(code.country_code_a2, "CN");
    strcpy(code.country_code_a3, "CHN");
    strcpy(code.country, "China");

    int ret = GeoCountryCode_Add(&manager, &code);
    TEST_ASSERT(ret == 0, "Add country code");
}

/* Test 2: Get - 获取国家代码 */
static void Test_Get() {
    TEST_START("GeoCountryCode_Get");

    GeoCountryCode code;
    int ret = GeoCountryCode_Get(&manager, 156, &code);

    TEST_ASSERT(ret == 0, "Get country code");
    TEST_ASSERT(code.code_no == 156, "code_no matches");
    TEST_ASSERT(strcmp(code.country_code_a2, "CN") == 0, "A2 code matches");
    TEST_ASSERT(strcmp(code.country_code_a3, "CHN") == 0, "A3 code matches");
    TEST_ASSERT(strcmp(code.country, "China") == 0, "Country name matches");
}

/* Test 3: Exists - 检查存在性 */
static void Test_Exists() {
    TEST_START("GeoCountryCode_Exists");

    int exists = GeoCountryCode_Exists(&manager, 156);
    TEST_ASSERT(exists == 1, "Record exists");

    exists = GeoCountryCode_Exists(&manager, 999);
    TEST_ASSERT(exists == 0, "Non-existent record");
}

/* Test 4: Update - 更新国家代码 */
static void Test_Update() {
    TEST_START("GeoCountryCode_Update");

    GeoCountryCode code;
    code.code_no = 156;
    strcpy(code.country_code_a2, "CN");
    strcpy(code.country_code_a3, "CHN");
    strcpy(code.country, "Peoples Republic of China");

    int ret = GeoCountryCode_Update(&manager, &code);
    TEST_ASSERT(ret == 0, "Update country code");

    /* 验证更新 */
    GeoCountryCode updated;
    GeoCountryCode_Get(&manager, 156, &updated);
    TEST_ASSERT(strcmp(updated.country, "Peoples Republic of China") == 0, "Country name updated");
}

/* Test 5: Add幂等性测试 */
static void Test_AddIdempotent() {
    TEST_START("GeoCountryCode_Add (Idempotent)");

    GeoCountryCode code;
    code.code_no = 840;
    strcpy(code.country_code_a2, "US");
    strcpy(code.country_code_a3, "USA");
    strcpy(code.country, "United States");

    /* 第一次添加 */
    int ret1 = GeoCountryCode_Add(&manager, &code);
    TEST_ASSERT(ret1 == 0, "First add");

    /* 第二次添加（幂等） */
    int ret2 = GeoCountryCode_Add(&manager, &code);
    TEST_ASSERT(ret2 == 0, "Second add (idempotent)");

    /* 验证只有一条记录 */
    int count = GeoCountryCode_GetCount(&manager);
    TEST_ASSERT(count == 2, "Only 2 records exist");
}

/* Test 6: GetByA2 - 根据A2代码查询 */
static void Test_GetByA2() {
    TEST_START("GeoCountryCode_GetByA2");

    GeoCountryCode code;
    int ret = GeoCountryCode_GetByA2(&manager, "CN", &code);

    TEST_ASSERT(ret == 0, "Get by A2 code");
    TEST_ASSERT(code.code_no == 156, "code_no matches");
    TEST_ASSERT(strcmp(code.country_code_a3, "CHN") == 0, "A3 code matches");
}

/* Test 7: GetByA3 - 根据A3代码查询 */
static void Test_GetByA3() {
    TEST_START("GeoCountryCode_GetByA3");

    GeoCountryCode code;
    int ret = GeoCountryCode_GetByA3(&manager, "USA", &code);

    TEST_ASSERT(ret == 0, "Get by A3 code");
    TEST_ASSERT(code.code_no == 840, "code_no matches");
    TEST_ASSERT(strcmp(code.country_code_a2, "US") == 0, "A2 code matches");
}

/* Test 8: GetByCountryPattern - 国家名称模糊查询 */
static void Test_GetByCountryPattern() {
    TEST_START("GeoCountryCode_GetByCountryPattern");

    /* 添加更多测试数据 */
    GeoCountryCode code1;
    code1.code_no = 392;
    strcpy(code1.country_code_a2, "JP");
    strcpy(code1.country_code_a3, "JPN");
    strcpy(code1.country, "Japan");
    GeoCountryCode_Add(&manager, &code1);

    GeoCountryCode code2;
    code2.code_no = 410;
    strcpy(code2.country_code_a2, "KR");
    strcpy(code2.country_code_a3, "KOR");
    strcpy(code2.country, "Korea");
    GeoCountryCode_Add(&manager, &code2);

    /* 查询 */
    GeoCountryCode results[10];
    int count = GeoCountryCode_GetByCountryPattern(&manager, "a", results, 10);

    TEST_ASSERT(count >= 3, "Found records matching country pattern");
}

/* Test 9: GetByCodeRange - 编号范围查询 */
static void Test_GetByCodeRange() {
    TEST_START("GeoCountryCode_GetByCodeRange");

    GeoCountryCode results[10];
    int count = GeoCountryCode_GetByCodeRange(&manager, 100, 500, results, 10);

    TEST_ASSERT(count >= 3, "Found records in code range");
}

/* Test 10: GetAll - 获取所有记录（分页） */
static void Test_GetAll() {
    TEST_START("GeoCountryCode_GetAll");

    GeoCountryCode results[10];
    int count = GeoCountryCode_GetAll(&manager, results, 0, 10);

    TEST_ASSERT(count >= 4, "GetAll returned records");
}

/* Test 11: GetCount - 获取总数 */
static void Test_GetCount() {
    TEST_START("GeoCountryCode_GetCount");

    int count = GeoCountryCode_GetCount(&manager);
    TEST_ASSERT(count >= 4, "Count is correct");
}

/* Test 12: GetTop - 获取前N条记录 */
static void Test_GetTop() {
    TEST_START("GeoCountryCode_GetTop");

    GeoCountryCode results[3];
    int count = GeoCountryCode_GetTop(&manager, results, 3);

    TEST_ASSERT(count == 3, "GetTop returned 3 records");
    TEST_ASSERT(results[0].code_no == 156, "First record is correct");
}

/* Test 13: BulkAdd - 批量添加 */
static void Test_BulkAdd() {
    TEST_START("GeoCountryCode_BulkAdd");

    GeoCountryCode codes[3];

    codes[0].code_no = 826;
    strcpy(codes[0].country_code_a2, "GB");
    strcpy(codes[0].country_code_a3, "GBR");
    strcpy(codes[0].country, "United Kingdom");

    codes[1].code_no = 250;
    strcpy(codes[1].country_code_a2, "FR");
    strcpy(codes[1].country_code_a3, "FRA");
    strcpy(codes[1].country, "France");

    codes[2].code_no = 276;
    strcpy(codes[2].country_code_a2, "DE");
    strcpy(codes[2].country_code_a3, "DEU");
    strcpy(codes[2].country, "Germany");

    int count = GeoCountryCode_BulkAdd(&manager, codes, 3);
    TEST_ASSERT(count == 3, "BulkAdd added 3 records");
}

/* Test 14: BulkDelete - 批量删除 */
static void Test_BulkDelete() {
    TEST_START("GeoCountryCode_BulkDelete");

    int code_nos[] = {826, 250, 276};
    int ret = GeoCountryCode_BulkDelete(&manager, code_nos, 3);
    TEST_ASSERT(ret == 0, "BulkDelete successful");

    /* 验证删除 */
    int exists = GeoCountryCode_Exists(&manager, 826);
    TEST_ASSERT(exists == 0, "Record 826 deleted");
}

/* Test 15: Delete幂等性测试 */
static void Test_DeleteIdempotent() {
    TEST_START("GeoCountryCode_Delete (Idempotent)");

    /* 第一次删除 */
    int ret1 = GeoCountryCode_Delete(&manager, 840);
    TEST_ASSERT(ret1 == 0, "First delete");

    /* 第二次删除（幂等） */
    int ret2 = GeoCountryCode_Delete(&manager, 840);
    TEST_ASSERT(ret2 == 0, "Second delete (idempotent)");

    /* 验证不存在 */
    int exists = GeoCountryCode_Exists(&manager, 840);
    TEST_ASSERT(exists == 0, "Record does not exist");
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    printf("===========================================\n");
    printf("  geo_country_code_orm Test Suite\n");
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
    Test_GetByA2();
    Test_GetByA3();
    Test_GetByCountryPattern();
    Test_GetByCodeRange();
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
