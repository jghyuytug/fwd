#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "member_info_detail_orm.h"

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
    DBConnectionManager_ExecuteQuery(&manager, DB_TYPE_TAIWAN, "TRUNCATE TABLE member_info_detail", &result);
    DBQueryResult_Free(&result);
}

/* Test 1: Add - 添加会员详细信息 */
static void Test_Add() {
    TEST_START("MemberInfoDetail_Add");

    MemberInfoDetail detail;
    detail.m_id = 1001;
    strcpy(detail.zipcode, "100001");
    strcpy(detail.address, "台北市信义区");
    strcpy(detail.address_detail, "信义路五段7号");
    strcpy(detail.occ_date, "2025-11-14 10:00:00");

    int ret = MemberInfoDetail_Add(&manager, &detail);
    TEST_ASSERT(ret == 0, "Add member info detail");
}

/* Test 2: Get - 获取会员详细信息 */
static void Test_Get() {
    TEST_START("MemberInfoDetail_Get");

    MemberInfoDetail detail;
    int ret = MemberInfoDetail_Get(&manager, 1001, &detail);

    TEST_ASSERT(ret == 0, "Get member info detail");
    TEST_ASSERT(detail.m_id == 1001, "m_id matches");
    TEST_ASSERT(strcmp(detail.zipcode, "100001") == 0, "zipcode matches");
    TEST_ASSERT(strcmp(detail.address, "台北市信义区") == 0, "address matches");
}

/* Test 3: Exists - 检查存在性 */
static void Test_Exists() {
    TEST_START("MemberInfoDetail_Exists");

    int exists = MemberInfoDetail_Exists(&manager, 1001);
    TEST_ASSERT(exists == 1, "Record exists");

    exists = MemberInfoDetail_Exists(&manager, 9999);
    TEST_ASSERT(exists == 0, "Non-existent record");
}

/* Test 4: Update - 更新会员详细信息 */
static void Test_Update() {
    TEST_START("MemberInfoDetail_Update");

    MemberInfoDetail detail;
    detail.m_id = 1001;
    strcpy(detail.zipcode, "100002");
    strcpy(detail.address, "台北市中山区");
    strcpy(detail.address_detail, "南京东路一段");
    strcpy(detail.occ_date, "2025-11-14 11:00:00");

    int ret = MemberInfoDetail_Update(&manager, &detail);
    TEST_ASSERT(ret == 0, "Update member info detail");

    /* 验证更新 */
    MemberInfoDetail updated;
    MemberInfoDetail_Get(&manager, 1001, &updated);
    TEST_ASSERT(strcmp(updated.zipcode, "100002") == 0, "zipcode updated");
    TEST_ASSERT(strcmp(updated.address, "台北市中山区") == 0, "address updated");
}

/* Test 5: Add幂等性测试 */
static void Test_AddIdempotent() {
    TEST_START("MemberInfoDetail_Add (Idempotent)");

    MemberInfoDetail detail;
    detail.m_id = 1002;
    strcpy(detail.zipcode, "200001");
    strcpy(detail.address, "高雄市前金区");
    strcpy(detail.address_detail, "中正四路");
    strcpy(detail.occ_date, "2025-11-14 12:00:00");

    /* 第一次添加 */
    int ret1 = MemberInfoDetail_Add(&manager, &detail);
    TEST_ASSERT(ret1 == 0, "First add");

    /* 第二次添加（幂等） */
    int ret2 = MemberInfoDetail_Add(&manager, &detail);
    TEST_ASSERT(ret2 == 0, "Second add (idempotent)");

    /* 验证只有一条记录 */
    int count = MemberInfoDetail_GetCount(&manager);
    TEST_ASSERT(count == 2, "Only 2 records exist");
}

/* Test 6: GetByZipcode - 根据邮编查询 */
static void Test_GetByZipcode() {
    TEST_START("MemberInfoDetail_GetByZipcode");

    /* 添加测试数据 */
    MemberInfoDetail detail1;
    detail1.m_id = 1003;
    strcpy(detail1.zipcode, "300001");
    strcpy(detail1.address, "台中市西区");
    strcpy(detail1.address_detail, "公益路");
    strcpy(detail1.occ_date, "2025-11-14 13:00:00");
    MemberInfoDetail_Add(&manager, &detail1);

    MemberInfoDetail detail2;
    detail2.m_id = 1004;
    strcpy(detail2.zipcode, "300001");
    strcpy(detail2.address, "台中市西区");
    strcpy(detail2.address_detail, "台湾大道");
    strcpy(detail2.occ_date, "2025-11-14 14:00:00");
    MemberInfoDetail_Add(&manager, &detail2);

    /* 查询 */
    MemberInfoDetail results[10];
    int count = MemberInfoDetail_GetByZipcode(&manager, "300001", results, 10);

    TEST_ASSERT(count == 2, "Found 2 records with zipcode 300001");
}

/* Test 7: GetByAddressPattern - 地址模糊查询 */
static void Test_GetByAddressPattern() {
    TEST_START("MemberInfoDetail_GetByAddressPattern");

    MemberInfoDetail results[10];
    int count = MemberInfoDetail_GetByAddressPattern(&manager, "台中市", results, 10);

    TEST_ASSERT(count >= 2, "Found records matching address pattern");
}

/* Test 8: GetByDateRange - 日期范围查询 */
static void Test_GetByDateRange() {
    TEST_START("MemberInfoDetail_GetByDateRange");

    MemberInfoDetail results[10];
    int count = MemberInfoDetail_GetByDateRange(&manager, "2025-11-14", "2025-11-15", results, 10);

    TEST_ASSERT(count >= 4, "Found records in date range");
}

/* Test 9: GetAll - 获取所有记录（分页） */
static void Test_GetAll() {
    TEST_START("MemberInfoDetail_GetAll");

    MemberInfoDetail results[10];
    int count = MemberInfoDetail_GetAll(&manager, results, 0, 10);

    TEST_ASSERT(count >= 4, "GetAll returned records");
}

/* Test 10: GetCount - 获取总数 */
static void Test_GetCount() {
    TEST_START("MemberInfoDetail_GetCount");

    int count = MemberInfoDetail_GetCount(&manager);
    TEST_ASSERT(count >= 4, "Count is correct");
}

/* Test 11: GetRecent - 获取最近记录 */
static void Test_GetRecent() {
    TEST_START("MemberInfoDetail_GetRecent");

    MemberInfoDetail results[5];
    int count = MemberInfoDetail_GetRecent(&manager, results, 5);

    TEST_ASSERT(count >= 4 && count <= 5, "GetRecent returned records");
}

/* Test 12: GetOldRecords - 获取旧记录 */
static void Test_GetOldRecords() {
    TEST_START("MemberInfoDetail_GetOldRecords");

    MemberInfoDetail results[10];
    int count = MemberInfoDetail_GetOldRecords(&manager, 1, results, 10);

    /* 所有测试数据都是今天的，不应该有旧记录 */
    TEST_ASSERT(count == 0, "No old records found");
}

/* Test 13: BulkAdd - 批量添加 */
static void Test_BulkAdd() {
    TEST_START("MemberInfoDetail_BulkAdd");

    MemberInfoDetail details[3];

    details[0].m_id = 1005;
    strcpy(details[0].zipcode, "400001");
    strcpy(details[0].address, "台南市中西区");
    strcpy(details[0].address_detail, "民权路");
    strcpy(details[0].occ_date, "2025-11-14 15:00:00");

    details[1].m_id = 1006;
    strcpy(details[1].zipcode, "500001");
    strcpy(details[1].address, "新竹市东区");
    strcpy(details[1].address_detail, "光复路");
    strcpy(details[1].occ_date, "2025-11-14 16:00:00");

    details[2].m_id = 1007;
    strcpy(details[2].zipcode, "600001");
    strcpy(details[2].address, "嘉义市西区");
    strcpy(details[2].address_detail, "中山路");
    strcpy(details[2].occ_date, "2025-11-14 17:00:00");

    int count = MemberInfoDetail_BulkAdd(&manager, details, 3);
    TEST_ASSERT(count == 3, "BulkAdd added 3 records");
}

/* Test 14: BulkDelete - 批量删除 */
static void Test_BulkDelete() {
    TEST_START("MemberInfoDetail_BulkDelete");

    int m_ids[] = {1005, 1006, 1007};
    int ret = MemberInfoDetail_BulkDelete(&manager, m_ids, 3);
    TEST_ASSERT(ret == 0, "BulkDelete successful");

    /* 验证删除 */
    int exists = MemberInfoDetail_Exists(&manager, 1005);
    TEST_ASSERT(exists == 0, "Record 1005 deleted");
}

/* Test 15: Delete幂等性测试 */
static void Test_DeleteIdempotent() {
    TEST_START("MemberInfoDetail_Delete (Idempotent)");

    /* 第一次删除 */
    int ret1 = MemberInfoDetail_Delete(&manager, 1002);
    TEST_ASSERT(ret1 == 0, "First delete");

    /* 第二次删除（幂等） */
    int ret2 = MemberInfoDetail_Delete(&manager, 1002);
    TEST_ASSERT(ret2 == 0, "Second delete (idempotent)");

    /* 验证不存在 */
    int exists = MemberInfoDetail_Exists(&manager, 1002);
    TEST_ASSERT(exists == 0, "Record does not exist");
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    printf("===========================================\n");
    printf("  member_info_detail_orm Test Suite\n");
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

    /* No need to get connection pointer - pass manager directly */

    /* 创建测试数据 */
    CreateTestData();

    /* 运行测试 */
    Test_Add();
    Test_Get();
    Test_Exists();
    Test_Update();
    Test_AddIdempotent();
    Test_GetByZipcode();
    Test_GetByAddressPattern();
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
