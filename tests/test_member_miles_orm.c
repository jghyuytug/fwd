#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "member_miles_orm.h"

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
    DBConnectionManager_ExecuteQuery(&manager, DB_TYPE_TAIWAN, "TRUNCATE TABLE member_miles", &result);
    DBQueryResult_Free(&result);
}

/* Test 1: Add - 添加会员里程记录 */
static void Test_Add() {
    TEST_START("MemberMiles_Add");

    MemberMiles record;
    record.m_id = 10001;
    record.miles = 1000;
    record.daily_miles = 50;

    int ret = MemberMiles_Add(&manager, &record);
    TEST_ASSERT(ret == 0, "Add member miles record");
}

/* Test 2: Get - 获取会员里程记录 */
static void Test_Get() {
    TEST_START("MemberMiles_Get");

    MemberMiles record;
    int ret = MemberMiles_Get(&manager, 10001, &record);

    TEST_ASSERT(ret == 0, "Get member miles record");
    TEST_ASSERT(record.m_id == 10001, "m_id matches");
    TEST_ASSERT(record.miles == 1000, "miles matches");
    TEST_ASSERT(record.daily_miles == 50, "daily_miles matches");
}

/* Test 3: Exists - 检查存在性 */
static void Test_Exists() {
    TEST_START("MemberMiles_Exists");

    int exists = MemberMiles_Exists(&manager, 10001);
    TEST_ASSERT(exists == 1, "Record exists");

    exists = MemberMiles_Exists(&manager, 99999);
    TEST_ASSERT(exists == 0, "Non-existent record");
}

/* Test 4: Update - 更新会员里程记录 */
static void Test_Update() {
    TEST_START("MemberMiles_Update");

    MemberMiles record;
    record.m_id = 10001;
    record.miles = 1500;
    record.daily_miles = 75;

    int ret = MemberMiles_Update(&manager, &record);
    TEST_ASSERT(ret == 0, "Update member miles record");

    /* 验证更新 */
    MemberMiles updated;
    MemberMiles_Get(&manager, 10001, &updated);
    TEST_ASSERT(updated.miles == 1500, "miles updated");
    TEST_ASSERT(updated.daily_miles == 75, "daily_miles updated");
}

/* Test 5: Add幂等性测试 */
static void Test_AddIdempotent() {
    TEST_START("MemberMiles_Add (Idempotent)");

    MemberMiles record;
    record.m_id = 10002;
    record.miles = 2000;
    record.daily_miles = 100;

    /* 第一次添加 */
    int ret1 = MemberMiles_Add(&manager, &record);
    TEST_ASSERT(ret1 == 0, "First add");

    /* 第二次添加（幂等） */
    record.miles = 2500;
    int ret2 = MemberMiles_Add(&manager, &record);
    TEST_ASSERT(ret2 == 0, "Second add (idempotent)");

    /* 验证miles被更新 */
    MemberMiles check;
    MemberMiles_Get(&manager, 10002, &check);
    TEST_ASSERT(check.miles == 2500, "miles updated by REPLACE");
}

/* Test 6: GetByMilesRange - 根据总里程数范围查询 */
static void Test_GetByMilesRange() {
    TEST_START("MemberMiles_GetByMilesRange");

    /* 添加更多测试数据 */
    MemberMiles r1 = {10003, 3000, 150};
    MemberMiles_Add(&manager, &r1);

    MemberMiles results[10];
    int count = MemberMiles_GetByMilesRange(&manager, 2000, 3500, results, 10);

    TEST_ASSERT(count >= 2, "Found at least 2 records in miles range");
}

/* Test 7: GetByDailyMilesRange - 根据每日里程数范围查询 */
static void Test_GetByDailyMilesRange() {
    TEST_START("MemberMiles_GetByDailyMilesRange");

    MemberMiles results[10];
    int count = MemberMiles_GetByDailyMilesRange(&manager, 50, 200, results, 10);

    TEST_ASSERT(count >= 2, "Found at least 2 records in daily_miles range");
}

/* Test 8: GetTopMiles - 获取总里程数排行榜 */
static void Test_GetTopMiles() {
    TEST_START("MemberMiles_GetTopMiles");

    MemberMiles results[10];
    int count = MemberMiles_GetTopMiles(&manager, results, 10);

    TEST_ASSERT(count >= 3, "Found at least 3 records");
    /* 验证排序（降序） */
    if (count >= 2) {
        TEST_ASSERT(results[0].miles >= results[1].miles, "Top miles sorted DESC");
    }
}

/* Test 9: GetAll - 获取所有记录（分页） */
static void Test_GetAll() {
    TEST_START("MemberMiles_GetAll");

    MemberMiles results[10];
    int count = MemberMiles_GetAll(&manager, results, 0, 10);

    TEST_ASSERT(count >= 3, "GetAll returned at least 3 records");
}

/* Test 10: GetCount - 获取总数 */
static void Test_GetCount() {
    TEST_START("MemberMiles_GetCount");

    int count = MemberMiles_GetCount(&manager);
    TEST_ASSERT(count >= 3, "Count is at least 3");
}

/* Test 11: IncrementMiles - 增加总里程数（原子操作） */
static void Test_IncrementMiles() {
    TEST_START("MemberMiles_IncrementMiles");

    /* 获取当前里程数 */
    MemberMiles before;
    MemberMiles_Get(&manager, 10001, &before);

    /* 增加500里程 */
    int ret = MemberMiles_IncrementMiles(&manager, 10001, 500);
    TEST_ASSERT(ret == 0, "IncrementMiles successful");

    /* 验证增加 */
    MemberMiles after;
    MemberMiles_Get(&manager, 10001, &after);
    TEST_ASSERT(after.miles == before.miles + 500, "Miles incremented by 500");
}

/* Test 12: ResetDailyMiles - 重置每日里程数 */
static void Test_ResetDailyMiles() {
    TEST_START("MemberMiles_ResetDailyMiles");

    int ret = MemberMiles_ResetDailyMiles(&manager, 10001);
    TEST_ASSERT(ret == 0, "ResetDailyMiles successful");

    /* 验证重置 */
    MemberMiles record;
    MemberMiles_Get(&manager, 10001, &record);
    TEST_ASSERT(record.daily_miles == 0, "daily_miles reset to 0");
}

/* Test 13: BulkAdd - 批量添加 */
static void Test_BulkAdd() {
    TEST_START("MemberMiles_BulkAdd");

    MemberMiles records[3];

    records[0].m_id = 20001;
    records[0].miles = 5000;
    records[0].daily_miles = 200;

    records[1].m_id = 20002;
    records[1].miles = 6000;
    records[1].daily_miles = 250;

    records[2].m_id = 20003;
    records[2].miles = 7000;
    records[2].daily_miles = 300;

    int count = MemberMiles_BulkAdd(&manager, records, 3);
    TEST_ASSERT(count == 3, "BulkAdd added 3 records");
}

/* Test 14: BulkDelete - 批量删除 */
static void Test_BulkDelete() {
    TEST_START("MemberMiles_BulkDelete");

    int m_ids[3] = {20001, 20002, 20003};
    int ret = MemberMiles_BulkDelete(&manager, m_ids, 3);
    TEST_ASSERT(ret == 0, "BulkDelete successful");

    /* 验证删除 */
    int exists = MemberMiles_Exists(&manager, 20001);
    TEST_ASSERT(exists == 0, "Record 20001 deleted");
}

/* Test 15: Delete幂等性测试 */
static void Test_DeleteIdempotent() {
    TEST_START("MemberMiles_Delete (Idempotent)");

    /* 第一次删除 */
    int ret1 = MemberMiles_Delete(&manager, 10002);
    TEST_ASSERT(ret1 == 0, "First delete");

    /* 第二次删除（幂等） */
    int ret2 = MemberMiles_Delete(&manager, 10002);
    TEST_ASSERT(ret2 == 0, "Second delete (idempotent)");

    /* 验证不存在 */
    int exists = MemberMiles_Exists(&manager, 10002);
    TEST_ASSERT(exists == 0, "Record does not exist");
}

/* Test 16: PrintInfo - 工具函数测试 */
static void Test_PrintInfo() {
    TEST_START("MemberMiles_PrintInfo");

    MemberMiles record;
    record.m_id = 99999;
    record.miles = 99999;
    record.daily_miles = 999;

    printf("  Testing PrintInfo output:\n");
    MemberMiles_PrintInfo(&record);

    TEST_ASSERT(1, "PrintInfo executed");
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    printf("===========================================\n");
    printf("  member_miles_orm Test Suite\n");
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
    Test_GetByMilesRange();
    Test_GetByDailyMilesRange();
    Test_GetTopMiles();
    Test_GetAll();
    Test_GetCount();
    Test_IncrementMiles();
    Test_ResetDailyMiles();
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
