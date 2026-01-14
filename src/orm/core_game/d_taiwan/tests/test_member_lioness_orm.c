#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "member_lioness_orm.h"

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
    DBConnectionManager_ExecuteQuery(&manager, DB_TYPE_TAIWAN, "TRUNCATE TABLE member_lioness", &result);
    DBQueryResult_Free(&result);
}

/* Test 1: Add - 添加会员标记 */
static void Test_Add() {
    TEST_START("MemberLioness_Add");

    int ret = MemberLioness_Add(&manager, "lioness_user001");
    TEST_ASSERT(ret == 0, "Add member lioness record");
}

/* Test 2: Exists - 检查存在性 */
static void Test_Exists() {
    TEST_START("MemberLioness_Exists");

    int exists = MemberLioness_Exists(&manager, "lioness_user001");
    TEST_ASSERT(exists == 1, "Record exists");

    exists = MemberLioness_Exists(&manager, "nonexistent_user");
    TEST_ASSERT(exists == 0, "Non-existent record");
}

/* Test 3: Add幂等性测试 */
static void Test_AddIdempotent() {
    TEST_START("MemberLioness_Add (Idempotent)");

    /* 第一次添加 */
    int ret1 = MemberLioness_Add(&manager, "lioness_user002");
    TEST_ASSERT(ret1 == 0, "First add");

    /* 第二次添加（幂等） */
    int ret2 = MemberLioness_Add(&manager, "lioness_user002");
    TEST_ASSERT(ret2 == 0, "Second add (idempotent)");

    /* 验证仍存在 */
    int exists = MemberLioness_Exists(&manager, "lioness_user002");
    TEST_ASSERT(exists == 1, "Record still exists");
}

/* Test 4: GetAll - 获取所有记录（分页） */
static void Test_GetAll() {
    TEST_START("MemberLioness_GetAll");

    /* 添加更多测试数据 */
    MemberLioness_Add(&manager, "lioness_user003");
    MemberLioness_Add(&manager, "lioness_user004");

    MemberLioness results[10];
    int count = MemberLioness_GetAll(&manager, results, 0, 10);

    TEST_ASSERT(count >= 4, "GetAll returned at least 4 records");
}

/* Test 5: GetCount - 获取总数 */
static void Test_GetCount() {
    TEST_START("MemberLioness_GetCount");

    int count = MemberLioness_GetCount(&manager);
    TEST_ASSERT(count >= 4, "Count is at least 4");
}

/* Test 6: GetByPattern - 按模式查询 */
static void Test_GetByPattern() {
    TEST_START("MemberLioness_GetByPattern");

    MemberLioness results[10];
    int count = MemberLioness_GetByPattern(&manager, "lioness_user%", results, 10);

    TEST_ASSERT(count >= 4, "Found at least 4 records with pattern");
}

/* Test 7: GetByPrefix - 按前缀查询 */
static void Test_GetByPrefix() {
    TEST_START("MemberLioness_GetByPrefix");

    MemberLioness results[10];
    int count = MemberLioness_GetByPrefix(&manager, "lioness", results, 10);

    TEST_ASSERT(count >= 4, "Found at least 4 records with prefix");
}

/* Test 8: GetBySuffix - 按后缀查询 */
static void Test_GetBySuffix() {
    TEST_START("MemberLioness_GetBySuffix");

    /* 添加特定后缀的数据 */
    MemberLioness_Add(&manager, "user_special");
    MemberLioness_Add(&manager, "test_special");

    MemberLioness results[10];
    int count = MemberLioness_GetBySuffix(&manager, "special", results, 10);

    TEST_ASSERT(count >= 2, "Found at least 2 records with suffix");
}

/* Test 9: GetContaining - 包含子字符串查询 */
static void Test_GetContaining() {
    TEST_START("MemberLioness_GetContaining");

    MemberLioness results[10];
    int count = MemberLioness_GetContaining(&manager, "user", results, 10);

    TEST_ASSERT(count >= 4, "Found at least 4 records containing 'user'");
}

/* Test 10: GetByLengthRange - 按长度范围查询 */
static void Test_GetByLengthRange() {
    TEST_START("MemberLioness_GetByLengthRange");

    /* lioness_user001 = 16 characters */
    MemberLioness results[10];
    int count = MemberLioness_GetByLengthRange(&manager, 10, 20, results, 10);

    TEST_ASSERT(count >= 4, "Found at least 4 records in length range");
}

/* Test 11: GetRandom - 获取随机记录 */
static void Test_GetRandom() {
    TEST_START("MemberLioness_GetRandom");

    MemberLioness results[5];
    int count = MemberLioness_GetRandom(&manager, results, 5);

    TEST_ASSERT(count >= 1, "Got at least 1 random record");
}

/* Test 12: GetSorted - 获取排序记录 */
static void Test_GetSorted() {
    TEST_START("MemberLioness_GetSorted");

    MemberLioness results[10];
    int count = MemberLioness_GetSorted(&manager, results, 10);

    TEST_ASSERT(count >= 4, "Got at least 4 sorted records");
    /* 验证排序（升序） */
    if (count >= 2) {
        TEST_ASSERT(strcmp(results[0].user_id, results[1].user_id) <= 0, "Records sorted ASC");
    }
}

/* Test 13: BulkAdd - 批量添加 */
static void Test_BulkAdd() {
    TEST_START("MemberLioness_BulkAdd");

    const char* user_ids[3] = {
        "bulk_user001",
        "bulk_user002",
        "bulk_user003"
    };

    int count = MemberLioness_BulkAdd(&manager, user_ids, 3);
    TEST_ASSERT(count == 3, "BulkAdd added 3 records");

    /* 验证添加 */
    int exists = MemberLioness_Exists(&manager, "bulk_user001");
    TEST_ASSERT(exists == 1, "bulk_user001 exists");
}

/* Test 14: BulkDelete - 批量删除 */
static void Test_BulkDelete() {
    TEST_START("MemberLioness_BulkDelete");

    const char* user_ids[3] = {
        "bulk_user001",
        "bulk_user002",
        "bulk_user003"
    };

    int ret = MemberLioness_BulkDelete(&manager, user_ids, 3);
    TEST_ASSERT(ret == 0, "BulkDelete successful");

    /* 验证删除 */
    int exists = MemberLioness_Exists(&manager, "bulk_user001");
    TEST_ASSERT(exists == 0, "bulk_user001 deleted");
}

/* Test 15: Delete幂等性测试 */
static void Test_DeleteIdempotent() {
    TEST_START("MemberLioness_Delete (Idempotent)");

    /* 第一次删除 */
    int ret1 = MemberLioness_Delete(&manager, "lioness_user002");
    TEST_ASSERT(ret1 == 0, "First delete");

    /* 第二次删除（幂等） */
    int ret2 = MemberLioness_Delete(&manager, "lioness_user002");
    TEST_ASSERT(ret2 == 0, "Second delete (idempotent)");

    /* 验证不存在 */
    int exists = MemberLioness_Exists(&manager, "lioness_user002");
    TEST_ASSERT(exists == 0, "Record does not exist");
}

/* Test 16: PrintInfo - 工具函数测试 */
static void Test_PrintInfo() {
    TEST_START("MemberLioness_PrintInfo");

    MemberLioness record;
    strcpy(record.user_id, "test_print_user");

    printf("  Testing PrintInfo output:\n");
    MemberLioness_PrintInfo(&record);

    TEST_ASSERT(1, "PrintInfo executed");
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    printf("===========================================\n");
    printf("  member_lioness_orm Test Suite\n");
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
    Test_Exists();
    Test_AddIdempotent();
    Test_GetAll();
    Test_GetCount();
    Test_GetByPattern();
    Test_GetByPrefix();
    Test_GetBySuffix();
    Test_GetContaining();
    Test_GetByLengthRange();
    Test_GetRandom();
    Test_GetSorted();
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
