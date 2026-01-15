#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "member_join_info_orm.h"

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
    DBConnectionManager_ExecuteQuery(&manager, DB_TYPE_TAIWAN, "TRUNCATE TABLE member_join_info", &result);
    DBQueryResult_Free(&result);
}

/* Test 1: Add - 添加会员注册信息 */
static void Test_Add() {
    TEST_START("MemberJoinInfo_Add");

    MemberJoinInfo record;
    record.m_id = 1001;
    record.reg_date = (int)time(NULL);
    strcpy(record.ip, "192.168.1.100");
    record.contry_code = 86;  /* China */
    record.login_time = (int)time(NULL);
    record.error_type = 0;
    strcpy(record.login_ip, "192.168.1.100");
    record.game_use_history = 1;

    int ret = MemberJoinInfo_Add(&manager, &record);
    TEST_ASSERT(ret == 0, "Add member join info");
}

/* Test 2: Get - 获取会员注册信息 */
static void Test_Get() {
    TEST_START("MemberJoinInfo_Get");

    MemberJoinInfo record;
    int ret = MemberJoinInfo_Get(&manager, 1001, &record);

    TEST_ASSERT(ret == 0, "Get member join info");
    TEST_ASSERT(record.m_id == 1001, "m_id matches");
    TEST_ASSERT(strcmp(record.ip, "192.168.1.100") == 0, "ip matches");
    TEST_ASSERT(record.contry_code == 86, "contry_code matches");
}

/* Test 3: Exists - 检查存在性 */
static void Test_Exists() {
    TEST_START("MemberJoinInfo_Exists");

    int exists = MemberJoinInfo_Exists(&manager, 1001);
    TEST_ASSERT(exists == 1, "Record exists");

    exists = MemberJoinInfo_Exists(&manager, 9999);
    TEST_ASSERT(exists == 0, "Non-existent record");
}

/* Test 4: Update - 更新会员注册信息 */
static void Test_Update() {
    TEST_START("MemberJoinInfo_Update");

    MemberJoinInfo record;
    record.m_id = 1001;
    record.reg_date = (int)time(NULL);
    strcpy(record.ip, "192.168.1.100");
    record.contry_code = 86;
    record.login_time = (int)time(NULL) + 3600;
    record.error_type = 1;
    strcpy(record.login_ip, "192.168.1.101");
    record.game_use_history = 2;

    int ret = MemberJoinInfo_Update(&manager, &record);
    TEST_ASSERT(ret == 0, "Update member join info");

    /* 验证更新 */
    MemberJoinInfo updated;
    MemberJoinInfo_Get(&manager, 1001, &updated);
    TEST_ASSERT(updated.error_type == 1, "error_type updated to 1");
    TEST_ASSERT(updated.game_use_history == 2, "game_use_history updated to 2");
}

/* Test 5: Add幂等性测试 */
static void Test_AddIdempotent() {
    TEST_START("MemberJoinInfo_Add (Idempotent)");

    MemberJoinInfo record;
    record.m_id = 1002;
    record.reg_date = (int)time(NULL);
    strcpy(record.ip, "192.168.1.200");
    record.contry_code = 1;  /* USA */
    record.login_time = (int)time(NULL);
    record.error_type = 0;
    strcpy(record.login_ip, "192.168.1.200");
    record.game_use_history = 1;

    /* 第一次添加 */
    int ret1 = MemberJoinInfo_Add(&manager, &record);
    TEST_ASSERT(ret1 == 0, "First add");

    /* 第二次添加（幂等） */
    record.error_type = 2;
    int ret2 = MemberJoinInfo_Add(&manager, &record);
    TEST_ASSERT(ret2 == 0, "Second add (idempotent)");

    /* 验证error_type被更新 */
    MemberJoinInfo check;
    MemberJoinInfo_Get(&manager, 1002, &check);
    TEST_ASSERT(check.error_type == 2, "error_type updated by REPLACE");
}

/* Test 6: GetByRegIP - 根据注册IP查询 */
static void Test_GetByRegIP() {
    TEST_START("MemberJoinInfo_GetByRegIP");

    /* 添加更多测试数据 */
    MemberJoinInfo r1 = {1003, (int)time(NULL), "192.168.1.100", 86, (int)time(NULL), 0, "192.168.1.100", 1};
    MemberJoinInfo_Add(&manager, &r1);

    MemberJoinInfo results[10];
    int count = MemberJoinInfo_GetByRegIP(&manager, "192.168.1.100", results, 10);

    TEST_ASSERT(count >= 2, "Found at least 2 records with IP 192.168.1.100");
}

/* Test 7: GetByCountryCode - 根据国家代码查询 */
static void Test_GetByCountryCode() {
    TEST_START("MemberJoinInfo_GetByCountryCode");

    MemberJoinInfo results[10];
    int count = MemberJoinInfo_GetByCountryCode(&manager, 86, results, 10);

    TEST_ASSERT(count >= 2, "Found at least 2 records with country code 86");
}

/* Test 8: GetByRegDateRange - 根据注册日期范围查询 */
static void Test_GetByRegDateRange() {
    TEST_START("MemberJoinInfo_GetByRegDateRange");

    int now = (int)time(NULL);
    int start_date = now - 3600;
    int end_date = now + 3600;

    MemberJoinInfo results[10];
    int count = MemberJoinInfo_GetByRegDateRange(&manager, start_date, end_date, results, 10);

    TEST_ASSERT(count >= 3, "Found at least 3 records in date range");
}

/* Test 9: GetByErrorType - 根据错误类型查询 */
static void Test_GetByErrorType() {
    TEST_START("MemberJoinInfo_GetByErrorType");

    /* 添加有错误类型的记录 */
    MemberJoinInfo r2 = {1004, (int)time(NULL), "192.168.1.101", 1, (int)time(NULL), 1, "192.168.1.101", 1};
    MemberJoinInfo_Add(&manager, &r2);

    MemberJoinInfo results[10];
    int count = MemberJoinInfo_GetByErrorType(&manager, 1, results, 10);

    TEST_ASSERT(count >= 1, "Found at least 1 record with error_type 1");
}

/* Test 10: GetByGameUseHistory - 根据游戏使用历史查询 */
static void Test_GetByGameUseHistory() {
    TEST_START("MemberJoinInfo_GetByGameUseHistory");

    MemberJoinInfo results[10];
    int count = MemberJoinInfo_GetByGameUseHistory(&manager, 1, results, 10);

    TEST_ASSERT(count >= 3, "Found at least 3 records with game_use_history 1");
}

/* Test 11: GetRecentRegistered - 获取最近注册的会员 */
static void Test_GetRecentRegistered() {
    TEST_START("MemberJoinInfo_GetRecentRegistered");

    MemberJoinInfo results[5];
    int count = MemberJoinInfo_GetRecentRegistered(&manager, results, 5);

    TEST_ASSERT(count >= 4, "GetRecentRegistered returned at least 4 records");
    /* 验证按日期降序排列 */
    if (count >= 2) {
        TEST_ASSERT(results[0].reg_date >= results[1].reg_date,
                    "Results ordered by reg_date DESC");
    }
}

/* Test 12: GetAll - 获取所有记录（分页） */
static void Test_GetAll() {
    TEST_START("MemberJoinInfo_GetAll");

    MemberJoinInfo results[10];
    int count = MemberJoinInfo_GetAll(&manager, results, 0, 10);

    TEST_ASSERT(count >= 4, "GetAll returned at least 4 records");
}

/* Test 13: GetCount - 获取总数 */
static void Test_GetCount() {
    TEST_START("MemberJoinInfo_GetCount");

    int count = MemberJoinInfo_GetCount(&manager);
    TEST_ASSERT(count >= 4, "Count is at least 4");
}

/* Test 14: BulkAdd - 批量添加 */
static void Test_BulkAdd() {
    TEST_START("MemberJoinInfo_BulkAdd");

    MemberJoinInfo records[3];

    records[0].m_id = 2001;
    records[0].reg_date = (int)time(NULL);
    strcpy(records[0].ip, "10.0.0.1");
    records[0].contry_code = 81;  /* Japan */
    records[0].login_time = (int)time(NULL);
    records[0].error_type = 0;
    strcpy(records[0].login_ip, "10.0.0.1");
    records[0].game_use_history = 1;

    records[1].m_id = 2002;
    records[1].reg_date = (int)time(NULL);
    strcpy(records[1].ip, "10.0.0.2");
    records[1].contry_code = 82;  /* Korea */
    records[1].login_time = (int)time(NULL);
    records[1].error_type = 0;
    strcpy(records[1].login_ip, "10.0.0.2");
    records[1].game_use_history = 1;

    records[2].m_id = 2003;
    records[2].reg_date = (int)time(NULL);
    strcpy(records[2].ip, "10.0.0.3");
    records[2].contry_code = 44;  /* UK */
    records[2].login_time = (int)time(NULL);
    records[2].error_type = 0;
    strcpy(records[2].login_ip, "10.0.0.3");
    records[2].game_use_history = 1;

    int count = MemberJoinInfo_BulkAdd(&manager, records, 3);
    TEST_ASSERT(count == 3, "BulkAdd added 3 records");
}

/* Test 15: BulkDelete - 批量删除 */
static void Test_BulkDelete() {
    TEST_START("MemberJoinInfo_BulkDelete");

    unsigned int m_ids[3] = {2001, 2002, 2003};
    int ret = MemberJoinInfo_BulkDelete(&manager, m_ids, 3);
    TEST_ASSERT(ret == 0, "BulkDelete successful");

    /* 验证删除 */
    int exists = MemberJoinInfo_Exists(&manager, 2001);
    TEST_ASSERT(exists == 0, "Record 2001 deleted");
}

/* Test 16: Delete幂等性测试 */
static void Test_DeleteIdempotent() {
    TEST_START("MemberJoinInfo_Delete (Idempotent)");

    /* 第一次删除 */
    int ret1 = MemberJoinInfo_Delete(&manager, 1002);
    TEST_ASSERT(ret1 == 0, "First delete");

    /* 第二次删除（幂等） */
    int ret2 = MemberJoinInfo_Delete(&manager, 1002);
    TEST_ASSERT(ret2 == 0, "Second delete (idempotent)");

    /* 验证不存在 */
    int exists = MemberJoinInfo_Exists(&manager, 1002);
    TEST_ASSERT(exists == 0, "Record does not exist");
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    printf("===========================================\n");
    printf("  member_join_info_orm Test Suite\n");
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
    Test_GetByRegIP();
    Test_GetByCountryCode();
    Test_GetByRegDateRange();
    Test_GetByErrorType();
    Test_GetByGameUseHistory();
    Test_GetRecentRegistered();
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
