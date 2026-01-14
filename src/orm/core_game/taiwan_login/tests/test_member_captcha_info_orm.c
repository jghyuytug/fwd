#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "member_captcha_info_orm.h"

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
    DBConnectionManager_ExecuteQuery(&manager, DB_TYPE_TAIWAN, "TRUNCATE TABLE member_captcha_info", &result);
    DBQueryResult_Free(&result);
}

/* Test 1: Add - 添加验证码记录 */
static void Test_Add() {
    TEST_START("MemberCaptchaInfo_Add");

    MemberCaptchaInfo record;
    record.m_id = 1001;
    record.cert_time = (unsigned int)time(NULL);
    record.fail_count = 0;

    int ret = MemberCaptchaInfo_Add(&manager, &record);
    TEST_ASSERT(ret == 0, "Add captcha record");
}

/* Test 2: Get - 获取验证码记录 */
static void Test_Get() {
    TEST_START("MemberCaptchaInfo_Get");

    MemberCaptchaInfo record;
    int ret = MemberCaptchaInfo_Get(&manager, 1001, &record);

    TEST_ASSERT(ret == 0, "Get captcha record");
    TEST_ASSERT(record.m_id == 1001, "m_id matches");
    TEST_ASSERT(record.fail_count == 0, "fail_count is 0");
}

/* Test 3: Exists - 检查存在性 */
static void Test_Exists() {
    TEST_START("MemberCaptchaInfo_Exists");

    int exists = MemberCaptchaInfo_Exists(&manager, 1001);
    TEST_ASSERT(exists == 1, "Record exists");

    exists = MemberCaptchaInfo_Exists(&manager, 9999);
    TEST_ASSERT(exists == 0, "Non-existent record");
}

/* Test 4: Update - 更新验证码记录 */
static void Test_Update() {
    TEST_START("MemberCaptchaInfo_Update");

    MemberCaptchaInfo record;
    record.m_id = 1001;
    record.cert_time = (unsigned int)time(NULL) + 3600;
    record.fail_count = 3;

    int ret = MemberCaptchaInfo_Update(&manager, &record);
    TEST_ASSERT(ret == 0, "Update captcha record");

    /* 验证更新 */
    MemberCaptchaInfo updated;
    MemberCaptchaInfo_Get(&manager, 1001, &updated);
    TEST_ASSERT(updated.fail_count == 3, "fail_count updated to 3");
}

/* Test 5: Add幂等性测试 */
static void Test_AddIdempotent() {
    TEST_START("MemberCaptchaInfo_Add (Idempotent)");

    MemberCaptchaInfo record;
    record.m_id = 1002;
    record.cert_time = (unsigned int)time(NULL);
    record.fail_count = 1;

    /* 第一次添加 */
    int ret1 = MemberCaptchaInfo_Add(&manager, &record);
    TEST_ASSERT(ret1 == 0, "First add");

    /* 第二次添加（幂等） */
    record.fail_count = 2;
    int ret2 = MemberCaptchaInfo_Add(&manager, &record);
    TEST_ASSERT(ret2 == 0, "Second add (idempotent)");

    /* 验证只有一条记录，且fail_count被更新 */
    MemberCaptchaInfo check;
    MemberCaptchaInfo_Get(&manager, 1002, &check);
    TEST_ASSERT(check.fail_count == 2, "fail_count updated by REPLACE");
}

/* Test 6: IncrementFailCount - 增加失败次数 */
static void Test_IncrementFailCount() {
    TEST_START("MemberCaptchaInfo_IncrementFailCount");

    /* 重置fail_count为0 */
    MemberCaptchaInfo record;
    record.m_id = 1001;
    record.cert_time = (unsigned int)time(NULL);
    record.fail_count = 0;
    MemberCaptchaInfo_Update(&manager, &record);

    /* 增加3次 */
    int ret1 = MemberCaptchaInfo_IncrementFailCount(&manager, 1001);
    int ret2 = MemberCaptchaInfo_IncrementFailCount(&manager, 1001);
    int ret3 = MemberCaptchaInfo_IncrementFailCount(&manager, 1001);

    TEST_ASSERT(ret1 == 0 && ret2 == 0 && ret3 == 0, "Increment 3 times");

    /* 验证fail_count = 3 */
    MemberCaptchaInfo check;
    MemberCaptchaInfo_Get(&manager, 1001, &check);
    TEST_ASSERT(check.fail_count == 3, "fail_count is 3");
}

/* Test 7: ResetFailCount - 重置失败次数 */
static void Test_ResetFailCount() {
    TEST_START("MemberCaptchaInfo_ResetFailCount");

    int ret = MemberCaptchaInfo_ResetFailCount(&manager, 1001);
    TEST_ASSERT(ret == 0, "Reset fail_count");

    /* 验证fail_count = 0 */
    MemberCaptchaInfo check;
    MemberCaptchaInfo_Get(&manager, 1001, &check);
    TEST_ASSERT(check.fail_count == 0, "fail_count is 0");
}

/* Test 8: GetByFailCountRange - 失败次数范围查询 */
static void Test_GetByFailCountRange() {
    TEST_START("MemberCaptchaInfo_GetByFailCountRange");

    /* 添加测试数据 */
    MemberCaptchaInfo r1 = {1003, (unsigned int)time(NULL), 1};
    MemberCaptchaInfo r2 = {1004, (unsigned int)time(NULL), 5};
    MemberCaptchaInfo r3 = {1005, (unsigned int)time(NULL), 10};
    MemberCaptchaInfo_Add(&manager, &r1);
    MemberCaptchaInfo_Add(&manager, &r2);
    MemberCaptchaInfo_Add(&manager, &r3);

    /* 查询fail_count在1-5范围内的记录 */
    MemberCaptchaInfo results[10];
    int count = MemberCaptchaInfo_GetByFailCountRange(&manager, 1, 5, results, 10);

    TEST_ASSERT(count >= 2, "Found at least 2 records in range 1-5");
}

/* Test 9: GetByTimeRange - 时间范围查询 */
static void Test_GetByTimeRange() {
    TEST_START("MemberCaptchaInfo_GetByTimeRange");

    unsigned int now = (unsigned int)time(NULL);
    unsigned int start_time = now - 3600;
    unsigned int end_time = now + 3600;

    MemberCaptchaInfo results[10];
    int count = MemberCaptchaInfo_GetByTimeRange(&manager, start_time, end_time, results, 10);

    TEST_ASSERT(count >= 4, "Found at least 4 records in time range");
}

/* Test 10: GetRecentVerified - 获取最近验证的记录 */
static void Test_GetRecentVerified() {
    TEST_START("MemberCaptchaInfo_GetRecentVerified");

    MemberCaptchaInfo results[5];
    int count = MemberCaptchaInfo_GetRecentVerified(&manager, results, 5);

    TEST_ASSERT(count >= 4, "GetRecentVerified returned at least 4 records");
    /* 验证按时间降序排列 */
    if (count >= 2) {
        TEST_ASSERT(results[0].cert_time >= results[1].cert_time,
                    "Results ordered by cert_time DESC");
    }
}

/* Test 11: GetAll - 获取所有记录（分页） */
static void Test_GetAll() {
    TEST_START("MemberCaptchaInfo_GetAll");

    MemberCaptchaInfo results[10];
    int count = MemberCaptchaInfo_GetAll(&manager, results, 0, 10);

    TEST_ASSERT(count >= 4, "GetAll returned at least 4 records");
}

/* Test 12: GetCount - 获取总数 */
static void Test_GetCount() {
    TEST_START("MemberCaptchaInfo_GetCount");

    int count = MemberCaptchaInfo_GetCount(&manager);
    TEST_ASSERT(count >= 4, "Count is at least 4");
}

/* Test 13: BulkAdd - 批量添加 */
static void Test_BulkAdd() {
    TEST_START("MemberCaptchaInfo_BulkAdd");

    MemberCaptchaInfo records[3];
    records[0].m_id = 2001;
    records[0].cert_time = (unsigned int)time(NULL);
    records[0].fail_count = 0;

    records[1].m_id = 2002;
    records[1].cert_time = (unsigned int)time(NULL);
    records[1].fail_count = 1;

    records[2].m_id = 2003;
    records[2].cert_time = (unsigned int)time(NULL);
    records[2].fail_count = 2;

    int count = MemberCaptchaInfo_BulkAdd(&manager, records, 3);
    TEST_ASSERT(count == 3, "BulkAdd added 3 records");
}

/* Test 14: BulkDelete - 批量删除 */
static void Test_BulkDelete() {
    TEST_START("MemberCaptchaInfo_BulkDelete");

    unsigned int m_ids[3] = {2001, 2002, 2003};
    int ret = MemberCaptchaInfo_BulkDelete(&manager, m_ids, 3);
    TEST_ASSERT(ret == 0, "BulkDelete successful");

    /* 验证删除 */
    int exists = MemberCaptchaInfo_Exists(&manager, 2001);
    TEST_ASSERT(exists == 0, "Record 2001 deleted");
}

/* Test 15: Delete幂等性测试 */
static void Test_DeleteIdempotent() {
    TEST_START("MemberCaptchaInfo_Delete (Idempotent)");

    /* 第一次删除 */
    int ret1 = MemberCaptchaInfo_Delete(&manager, 1002);
    TEST_ASSERT(ret1 == 0, "First delete");

    /* 第二次删除（幂等） */
    int ret2 = MemberCaptchaInfo_Delete(&manager, 1002);
    TEST_ASSERT(ret2 == 0, "Second delete (idempotent)");

    /* 验证不存在 */
    int exists = MemberCaptchaInfo_Exists(&manager, 1002);
    TEST_ASSERT(exists == 0, "Record does not exist");
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    printf("===========================================\n");
    printf("  member_captcha_info_orm Test Suite\n");
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
    Test_IncrementFailCount();
    Test_ResetFailCount();
    Test_GetByFailCountRange();
    Test_GetByTimeRange();
    Test_GetRecentVerified();
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
