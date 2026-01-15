#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "member_passwd_mod_orm.h"

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
    DBConnectionManager_ExecuteQuery(&manager, DB_TYPE_TAIWAN, "TRUNCATE TABLE member_passwd_mod", &result);
    DBQueryResult_Free(&result);
}

/* Test 1: Add - 添加密码修改记录 */
static void Test_Add() {
    TEST_START("MemberPasswdMod_Add");

    MemberPasswdMod record;
    record.m_id = 1001;
    strcpy(record.first_time, "2025-01-01 10:00:00");
    strcpy(record.last_time, "2025-01-01 10:00:00");
    record.cnt = 1;

    int ret = MemberPasswdMod_Add(&manager, &record);
    TEST_ASSERT(ret == 0, "Add password modification record");
}

/* Test 2: Get - 获取密码修改记录 */
static void Test_Get() {
    TEST_START("MemberPasswdMod_Get");

    MemberPasswdMod record;
    int ret = MemberPasswdMod_Get(&manager, 1001, &record);

    TEST_ASSERT(ret == 0, "Get password modification record");
    TEST_ASSERT(record.m_id == 1001, "m_id matches");
    TEST_ASSERT(record.cnt == 1, "cnt is 1");
}

/* Test 3: Exists - 检查存在性 */
static void Test_Exists() {
    TEST_START("MemberPasswdMod_Exists");

    int exists = MemberPasswdMod_Exists(&manager, 1001);
    TEST_ASSERT(exists == 1, "Record exists");

    exists = MemberPasswdMod_Exists(&manager, 9999);
    TEST_ASSERT(exists == 0, "Non-existent record");
}

/* Test 4: Update - 更新密码修改记录 */
static void Test_Update() {
    TEST_START("MemberPasswdMod_Update");

    MemberPasswdMod record;
    record.m_id = 1001;
    strcpy(record.first_time, "2025-01-01 10:00:00");
    strcpy(record.last_time, "2025-01-05 15:30:00");
    record.cnt = 5;

    int ret = MemberPasswdMod_Update(&manager, &record);
    TEST_ASSERT(ret == 0, "Update password modification record");

    /* 验证更新 */
    MemberPasswdMod updated;
    MemberPasswdMod_Get(&manager, 1001, &updated);
    TEST_ASSERT(updated.cnt == 5, "cnt updated to 5");
}

/* Test 5: Add幂等性测试 */
static void Test_AddIdempotent() {
    TEST_START("MemberPasswdMod_Add (Idempotent)");

    MemberPasswdMod record;
    record.m_id = 1002;
    strcpy(record.first_time, "2025-01-02 11:00:00");
    strcpy(record.last_time, "2025-01-02 11:00:00");
    record.cnt = 1;

    /* 第一次添加 */
    int ret1 = MemberPasswdMod_Add(&manager, &record);
    TEST_ASSERT(ret1 == 0, "First add");

    /* 第二次添加（幂等） */
    record.cnt = 2;
    strcpy(record.last_time, "2025-01-03 12:00:00");
    int ret2 = MemberPasswdMod_Add(&manager, &record);
    TEST_ASSERT(ret2 == 0, "Second add (idempotent)");

    /* 验证只有一条记录，且cnt被更新 */
    MemberPasswdMod check;
    MemberPasswdMod_Get(&manager, 1002, &check);
    TEST_ASSERT(check.cnt == 2, "cnt updated by REPLACE");
}

/* Test 6: IncrementCount - 增加修改次数 */
static void Test_IncrementCount() {
    TEST_START("MemberPasswdMod_IncrementCount");

    /* 重置cnt为1 */
    MemberPasswdMod record;
    record.m_id = 1001;
    strcpy(record.first_time, "2025-01-01 10:00:00");
    strcpy(record.last_time, "2025-01-01 10:00:00");
    record.cnt = 1;
    MemberPasswdMod_Update(&manager, &record);

    /* 增加2次 */
    int ret1 = MemberPasswdMod_IncrementCount(&manager, 1001, "2025-01-10 14:00:00");
    int ret2 = MemberPasswdMod_IncrementCount(&manager, 1001, "2025-01-15 16:00:00");

    TEST_ASSERT(ret1 == 0 && ret2 == 0, "Increment count 2 times");

    /* 验证cnt = 3 */
    MemberPasswdMod check;
    MemberPasswdMod_Get(&manager, 1001, &check);
    TEST_ASSERT(check.cnt == 3, "cnt is 3");
}

/* Test 7: UpdateLastTime - 更新最后修改时间 */
static void Test_UpdateLastTime() {
    TEST_START("MemberPasswdMod_UpdateLastTime");

    int ret = MemberPasswdMod_UpdateLastTime(&manager, 1001, "2025-01-20 18:00:00");
    TEST_ASSERT(ret == 0, "Update last_time");

    /* 验证last_time已更新 */
    MemberPasswdMod check;
    MemberPasswdMod_Get(&manager, 1001, &check);
    TEST_ASSERT(strcmp(check.last_time, "2025-01-20 18:00:00") == 0, "last_time updated");
}

/* Test 8: GetByCountRange - 修改次数范围查询 */
static void Test_GetByCountRange() {
    TEST_START("MemberPasswdMod_GetByCountRange");

    /* 添加测试数据 */
    MemberPasswdMod r1 = {1003, "2025-01-03 12:00:00", "2025-01-03 12:00:00", 1};
    MemberPasswdMod r2 = {1004, "2025-01-04 13:00:00", "2025-01-08 15:00:00", 5};
    MemberPasswdMod r3 = {1005, "2025-01-05 14:00:00", "2025-01-12 17:00:00", 10};
    MemberPasswdMod_Add(&manager, &r1);
    MemberPasswdMod_Add(&manager, &r2);
    MemberPasswdMod_Add(&manager, &r3);

    /* 查询cnt在1-5范围内的记录 */
    MemberPasswdMod results[10];
    int count = MemberPasswdMod_GetByCountRange(&manager, 1, 5, results, 10);

    TEST_ASSERT(count >= 2, "Found at least 2 records in cnt range 1-5");
}

/* Test 9: GetByFirstTimeRange - 首次修改时间范围查询 */
static void Test_GetByFirstTimeRange() {
    TEST_START("MemberPasswdMod_GetByFirstTimeRange");

    MemberPasswdMod results[10];
    int count = MemberPasswdMod_GetByFirstTimeRange(&manager, "2025-01-01 00:00:00",
                                                     "2025-01-03 23:59:59", results, 10);

    TEST_ASSERT(count >= 2, "Found at least 2 records in first_time range");
}

/* Test 10: GetByLastTimeRange - 最后修改时间范围查询 */
static void Test_GetByLastTimeRange() {
    TEST_START("MemberPasswdMod_GetByLastTimeRange");

    MemberPasswdMod results[10];
    int count = MemberPasswdMod_GetByLastTimeRange(&manager, "2025-01-10 00:00:00",
                                                    "2025-01-20 23:59:59", results, 10);

    TEST_ASSERT(count >= 1, "Found at least 1 record in last_time range");
}

/* Test 11: GetRecentModified - 获取最近修改的记录 */
static void Test_GetRecentModified() {
    TEST_START("MemberPasswdMod_GetRecentModified");

    MemberPasswdMod results[5];
    int count = MemberPasswdMod_GetRecentModified(&manager, results, 5);

    TEST_ASSERT(count >= 4, "GetRecentModified returned at least 4 records");
    /* 验证按时间降序排列 */
    if (count >= 2) {
        TEST_ASSERT(strcmp(results[0].last_time, results[1].last_time) >= 0,
                    "Results ordered by last_time DESC");
    }
}

/* Test 12: GetAll - 获取所有记录（分页） */
static void Test_GetAll() {
    TEST_START("MemberPasswdMod_GetAll");

    MemberPasswdMod results[10];
    int count = MemberPasswdMod_GetAll(&manager, results, 0, 10);

    TEST_ASSERT(count >= 4, "GetAll returned at least 4 records");
}

/* Test 13: GetCount - 获取总数 */
static void Test_GetCount() {
    TEST_START("MemberPasswdMod_GetCount");

    int count = MemberPasswdMod_GetCount(&manager);
    TEST_ASSERT(count >= 4, "Count is at least 4");
}

/* Test 14: BulkAdd - 批量添加 */
static void Test_BulkAdd() {
    TEST_START("MemberPasswdMod_BulkAdd");

    MemberPasswdMod records[3];
    records[0].m_id = 2001;
    strcpy(records[0].first_time, "2025-01-06 15:00:00");
    strcpy(records[0].last_time, "2025-01-06 15:00:00");
    records[0].cnt = 1;

    records[1].m_id = 2002;
    strcpy(records[1].first_time, "2025-01-07 16:00:00");
    strcpy(records[1].last_time, "2025-01-09 18:00:00");
    records[1].cnt = 3;

    records[2].m_id = 2003;
    strcpy(records[2].first_time, "2025-01-08 17:00:00");
    strcpy(records[2].last_time, "2025-01-11 19:00:00");
    records[2].cnt = 5;

    int count = MemberPasswdMod_BulkAdd(&manager, records, 3);
    TEST_ASSERT(count == 3, "BulkAdd added 3 records");
}

/* Test 15: BulkDelete - 批量删除 */
static void Test_BulkDelete() {
    TEST_START("MemberPasswdMod_BulkDelete");

    int m_ids[3] = {2001, 2002, 2003};
    int ret = MemberPasswdMod_BulkDelete(&manager, m_ids, 3);
    TEST_ASSERT(ret == 0, "BulkDelete successful");

    /* 验证删除 */
    int exists = MemberPasswdMod_Exists(&manager, 2001);
    TEST_ASSERT(exists == 0, "Record 2001 deleted");
}

/* Test 16: Delete幂等性测试 */
static void Test_DeleteIdempotent() {
    TEST_START("MemberPasswdMod_Delete (Idempotent)");

    /* 第一次删除 */
    int ret1 = MemberPasswdMod_Delete(&manager, 1002);
    TEST_ASSERT(ret1 == 0, "First delete");

    /* 第二次删除（幂等） */
    int ret2 = MemberPasswdMod_Delete(&manager, 1002);
    TEST_ASSERT(ret2 == 0, "Second delete (idempotent)");

    /* 验证不存在 */
    int exists = MemberPasswdMod_Exists(&manager, 1002);
    TEST_ASSERT(exists == 0, "Record does not exist");
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    printf("===========================================\n");
    printf("  member_passwd_mod_orm Test Suite\n");
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
    Test_UpdateLastTime();
    Test_GetByCountRange();
    Test_GetByFirstTimeRange();
    Test_GetByLastTimeRange();
    Test_GetRecentModified();
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
