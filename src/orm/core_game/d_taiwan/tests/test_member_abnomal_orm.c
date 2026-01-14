#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "member_abnomal_orm.h"

/* 测试计数器 */
static int tests_passed = 0;
static int tests_failed = 0;

/* 测试辅助宏 */
#define TEST_START(name) printf("\n Test %d: %s\n", (tests_passed + tests_failed + 1), name)
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
    DBConnectionManager_ExecuteQuery(&manager, DB_TYPE_TAIWAN, "TRUNCATE TABLE member_abnomal", &result);
    DBQueryResult_Free(&result);
}

/* Test 1: Add - 添加异常会员记录 */
static void Test_Add() {
    TEST_START("MemberAbnomal_Add");

    MemberAbnomal record;
    strcpy(record.user_id, "user001");
    record.overlab_count = 5;

    int ret = MemberAbnomal_Add(&manager, &record);
    TEST_ASSERT(ret == 0, "Add member abnomal record");
}

/* Test 2: Get - 获取异常会员记录 */
static void Test_Get() {
    TEST_START("MemberAbnomal_Get");

    MemberAbnomal record;
    int ret = MemberAbnomal_Get(&manager, "user001", &record);

    TEST_ASSERT(ret == 0, "Get member abnomal record");
    TEST_ASSERT(strcmp(record.user_id, "user001") == 0, "user_id matches");
    TEST_ASSERT(record.overlab_count == 5, "overlab_count matches");
}

/* Test 3: Exists - 检查存在性 */
static void Test_Exists() {
    TEST_START("MemberAbnomal_Exists");

    int exists = MemberAbnomal_Exists(&manager, "user001");
    TEST_ASSERT(exists == 1, "Record exists");

    exists = MemberAbnomal_Exists(&manager, "nonexistent");
    TEST_ASSERT(exists == 0, "Non-existent record");
}

/* Test 4: Update - 更新异常会员记录 */
static void Test_Update() {
    TEST_START("MemberAbnomal_Update");

    MemberAbnomal record;
    strcpy(record.user_id, "user001");
    record.overlab_count = 10;

    int ret = MemberAbnomal_Update(&manager, &record);
    TEST_ASSERT(ret == 0, "Update member abnomal record");

    /* 验证更新 */
    MemberAbnomal updated;
    MemberAbnomal_Get(&manager, "user001", &updated);
    TEST_ASSERT(updated.overlab_count == 10, "overlab_count updated to 10");
}

/* Test 5: Add幂等性测试 */
static void Test_AddIdempotent() {
    TEST_START("MemberAbnomal_Add (Idempotent)");

    MemberAbnomal record;
    strcpy(record.user_id, "user002");
    record.overlab_count = 3;

    /* 第一次添加 */
    int ret1 = MemberAbnomal_Add(&manager, &record);
    TEST_ASSERT(ret1 == 0, "First add");

    /* 第二次添加（幂等） */
    record.overlab_count = 8;
    int ret2 = MemberAbnomal_Add(&manager, &record);
    TEST_ASSERT(ret2 == 0, "Second add (idempotent)");

    /* 验证overlab_count被更新 */
    MemberAbnomal check;
    MemberAbnomal_Get(&manager, "user002", &check);
    TEST_ASSERT(check.overlab_count == 8, "overlab_count updated by REPLACE");
}

/* Test 6: IncrementCount - 递增重叠次数 */
static void Test_IncrementCount() {
    TEST_START("MemberAbnomal_IncrementCount");

    /* 添加测试记录 */
    MemberAbnomal record;
    strcpy(record.user_id, "user003");
    record.overlab_count = 5;
    MemberAbnomal_Add(&manager, &record);

    /* 递增 */
    int ret = MemberAbnomal_IncrementCount(&manager, "user003");
    TEST_ASSERT(ret == 0, "Increment count");

    /* 验证 */
    MemberAbnomal check;
    MemberAbnomal_Get(&manager, "user003", &check);
    TEST_ASSERT(check.overlab_count == 6, "Count incremented to 6");
}

/* Test 7: ResetCount - 重置重叠次数 */
static void Test_ResetCount() {
    TEST_START("MemberAbnomal_ResetCount");

    int ret = MemberAbnomal_ResetCount(&manager, "user003");
    TEST_ASSERT(ret == 0, "Reset count");

    /* 验证 */
    MemberAbnomal check;
    MemberAbnomal_Get(&manager, "user003", &check);
    TEST_ASSERT(check.overlab_count == 0, "Count reset to 0");
}

/* Test 8: GetByCountRange - 根据重叠次数范围查询 */
static void Test_GetByCountRange() {
    TEST_START("MemberAbnomal_GetByCountRange");

    /* 添加更多测试数据 */
    MemberAbnomal r1;
    strcpy(r1.user_id, "user004");
    r1.overlab_count = 15;
    MemberAbnomal_Add(&manager, &r1);

    MemberAbnomal r2;
    strcpy(r2.user_id, "user005");
    r2.overlab_count = 20;
    MemberAbnomal_Add(&manager, &r2);

    MemberAbnomal results[10];
    int count = MemberAbnomal_GetByCountRange(&manager, 8, 15, results, 10);

    TEST_ASSERT(count >= 2, "Found at least 2 records in range [8, 15]");
}

/* Test 9: GetHighCount - 获取高重叠次数记录 */
static void Test_GetHighCount() {
    TEST_START("MemberAbnomal_GetHighCount");

    MemberAbnomal results[10];
    int count = MemberAbnomal_GetHighCount(&manager, 10, results, 10);

    TEST_ASSERT(count >= 2, "Found at least 2 records with count >= 10");

    /* 验证降序排列 */
    if (count >= 2) {
        TEST_ASSERT(results[0].overlab_count >= results[1].overlab_count,
                    "Results ordered by count DESC");
    }
}

/* Test 10: GetByUserIdPattern - 按用户ID模式查询 */
static void Test_GetByUserIdPattern() {
    TEST_START("MemberAbnomal_GetByUserIdPattern");

    MemberAbnomal results[10];
    int count = MemberAbnomal_GetByUserIdPattern(&manager, "user00%", results, 10);

    TEST_ASSERT(count >= 5, "Found at least 5 records matching 'user00%'");
}

/* Test 11: GetAll - 获取所有记录（分页） */
static void Test_GetAll() {
    TEST_START("MemberAbnomal_GetAll");

    MemberAbnomal results[10];
    int count = MemberAbnomal_GetAll(&manager, results, 0, 10);

    TEST_ASSERT(count >= 5, "GetAll returned at least 5 records");
}

/* Test 12: GetCount - 获取总数 */
static void Test_GetCount() {
    TEST_START("MemberAbnomal_GetCount");

    int count = MemberAbnomal_GetCount(&manager);
    TEST_ASSERT(count >= 5, "Count is at least 5");
}

/* Test 13: BulkAdd - 批量添加 */
static void Test_BulkAdd() {
    TEST_START("MemberAbnomal_BulkAdd");

    MemberAbnomal records[3];

    strcpy(records[0].user_id, "bulk001");
    records[0].overlab_count = 1;

    strcpy(records[1].user_id, "bulk002");
    records[1].overlab_count = 2;

    strcpy(records[2].user_id, "bulk003");
    records[2].overlab_count = 3;

    int count = MemberAbnomal_BulkAdd(&manager, records, 3);
    TEST_ASSERT(count == 3, "BulkAdd added 3 records");
}

/* Test 14: BulkDelete - 批量删除 */
static void Test_BulkDelete() {
    TEST_START("MemberAbnomal_BulkDelete");

    const char* user_ids[3] = {"bulk001", "bulk002", "bulk003"};
    int ret = MemberAbnomal_BulkDelete(&manager, user_ids, 3);
    TEST_ASSERT(ret == 0, "BulkDelete successful");

    /* 验证删除 */
    int exists = MemberAbnomal_Exists(&manager, "bulk001");
    TEST_ASSERT(exists == 0, "Record bulk001 deleted");
}

/* Test 15: Delete幂等性测试 */
static void Test_DeleteIdempotent() {
    TEST_START("MemberAbnomal_Delete (Idempotent)");

    /* 第一次删除 */
    int ret1 = MemberAbnomal_Delete(&manager, "user002");
    TEST_ASSERT(ret1 == 0, "First delete");

    /* 第二次删除（幂等） */
    int ret2 = MemberAbnomal_Delete(&manager, "user002");
    TEST_ASSERT(ret2 == 0, "Second delete (idempotent)");

    /* 验证不存在 */
    int exists = MemberAbnomal_Exists(&manager, "user002");
    TEST_ASSERT(exists == 0, "Record does not exist");
}

/* Test 16: PrintInfo - 工具函数测试 */
static void Test_PrintInfo() {
    TEST_START("MemberAbnomal_PrintInfo");

    MemberAbnomal record;
    strcpy(record.user_id, "testuser");
    record.overlab_count = 99;

    printf("  Testing PrintInfo output:\n");
    MemberAbnomal_PrintInfo(&record);

    TEST_ASSERT(1, "PrintInfo executed");
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    printf("===========================================\n");
    printf("  member_abnomal_orm Test Suite\n");
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
    Test_IncrementCount();
    Test_ResetCount();
    Test_GetByCountRange();
    Test_GetHighCount();
    Test_GetByUserIdPattern();
    Test_GetAll();
    Test_GetCount();
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
