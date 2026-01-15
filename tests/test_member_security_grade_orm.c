#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "member_security_grade_orm.h"

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
    DBConnectionManager_ExecuteQuery(&manager, DB_TYPE_TAIWAN, "TRUNCATE TABLE member_security_grade", &result);
    DBQueryResult_Free(&result);
}

/* Test 1: Add - 添加安全等级记录 */
static void Test_Add() {
    TEST_START("MemberSecurityGrade_Add");

    MemberSecurityGrade record;
    memset(&record, 0, sizeof(MemberSecurityGrade));

    record.m_id = 10001;
    strcpy(record.last_visit_time, "2025-11-14 10:00:00");
    record.pass_fail_cnt = 0;
    strcpy(record.last_vaccine_date, "2025-11-14 09:00:00");
    strcpy(record.last_window_date, "2025-11-14 09:00:00");
    strcpy(record.goblin_pass_mod, "2025-11-01 00:00:00");
    record.goblin_fail_cnt = 0;
    strcpy(record.security_card_reg, "2025-11-01 00:00:00");
    record.security_card_fail_cnt = 0;
    strcpy(record.m_opt_reg, "2025-11-01 00:00:00");
    strcpy(record.pc_opt_reg, "2025-11-01 00:00:00");
    strcpy(record.black_ip_try_time, "0000-00-00 00:00:00");
    record.linear_pass_fail_cnt = 0;
    record.last_pass_fail_time = 0;
    strcpy(record.last_check_time, "2025-11-14 09:00:00");
    strcpy(record.pass_modify_check, "2025-11-01 00:00:00");
    strcpy(record.member_pc_reg, "2025-11-01 00:00:00");
    strcpy(record.gatekeeper_otp_reg, "2025-11-01 00:00:00");
    record.goblin_validity_time = 86400;
    record.security_card_validity_time = 86400;
    strcpy(record.validity_ip, "192.168.1.100");
    record.cargopad_status = 1;
    strcpy(record.cargopad_mod, "2025-11-01 00:00:00");
    record.cargopad_validity_time = 86400;

    int ret = MemberSecurityGrade_Add(&manager, &record);
    TEST_ASSERT(ret == 0, "Add security grade record");
}

/* Test 2: Get - 获取记录 */
static void Test_Get() {
    TEST_START("MemberSecurityGrade_Get");

    MemberSecurityGrade record;
    int ret = MemberSecurityGrade_Get(&manager, 10001, &record);

    TEST_ASSERT(ret == 0, "Get security grade record");
    TEST_ASSERT(record.m_id == 10001, "m_id matches");
    TEST_ASSERT(record.pass_fail_cnt == 0, "pass_fail_cnt matches");
    TEST_ASSERT(strcmp(record.validity_ip, "192.168.1.100") == 0, "validity_ip matches");
}

/* Test 3: Exists - 检查存在性 */
static void Test_Exists() {
    TEST_START("MemberSecurityGrade_Exists");

    int exists = MemberSecurityGrade_Exists(&manager, 10001);
    TEST_ASSERT(exists == 1, "Record exists");

    exists = MemberSecurityGrade_Exists(&manager, 99999);
    TEST_ASSERT(exists == 0, "Non-existent record");
}

/* Test 4: Update - 更新记录 */
static void Test_Update() {
    TEST_START("MemberSecurityGrade_Update");

    MemberSecurityGrade record;
    MemberSecurityGrade_Get(&manager, 10001, &record);

    record.pass_fail_cnt = 2;
    record.linear_pass_fail_cnt = 1;
    strcpy(record.validity_ip, "192.168.1.101");

    int ret = MemberSecurityGrade_Update(&manager, &record);
    TEST_ASSERT(ret == 0, "Update security grade record");

    /* 验证更新 */
    MemberSecurityGrade updated;
    MemberSecurityGrade_Get(&manager, 10001, &updated);
    TEST_ASSERT(updated.pass_fail_cnt == 2, "pass_fail_cnt updated");
    TEST_ASSERT(strcmp(updated.validity_ip, "192.168.1.101") == 0, "validity_ip updated");
}

/* Test 5: Add幂等性测试 */
static void Test_AddIdempotent() {
    TEST_START("MemberSecurityGrade_Add (Idempotent)");

    MemberSecurityGrade record1;
    memset(&record1, 0, sizeof(MemberSecurityGrade));
    record1.m_id = 10002;
    strcpy(record1.last_visit_time, "2025-11-14 11:00:00");
    record1.pass_fail_cnt = 5;
    strcpy(record1.last_vaccine_date, "2025-11-14 10:00:00");
    strcpy(record1.last_window_date, "2025-11-14 10:00:00");
    strcpy(record1.goblin_pass_mod, "2025-11-01 00:00:00");
    record1.goblin_fail_cnt = 0;
    strcpy(record1.security_card_reg, "2025-11-01 00:00:00");
    record1.security_card_fail_cnt = 0;
    strcpy(record1.m_opt_reg, "2025-11-01 00:00:00");
    strcpy(record1.pc_opt_reg, "2025-11-01 00:00:00");
    strcpy(record1.black_ip_try_time, "0000-00-00 00:00:00");
    record1.linear_pass_fail_cnt = 3;
    record1.last_pass_fail_time = 1700000000;
    strcpy(record1.last_check_time, "2025-11-14 10:00:00");
    strcpy(record1.pass_modify_check, "2025-11-01 00:00:00");
    strcpy(record1.member_pc_reg, "2025-11-01 00:00:00");
    strcpy(record1.gatekeeper_otp_reg, "2025-11-01 00:00:00");
    record1.goblin_validity_time = 86400;
    record1.security_card_validity_time = 86400;
    strcpy(record1.validity_ip, "192.168.2.100");
    record1.cargopad_status = 1;
    strcpy(record1.cargopad_mod, "2025-11-01 00:00:00");
    record1.cargopad_validity_time = 86400;

    /* 第一次添加 */
    int ret1 = MemberSecurityGrade_Add(&manager, &record1);
    TEST_ASSERT(ret1 == 0, "First add");

    /* 第二次添加（幂等，替换） */
    MemberSecurityGrade record2 = record1;
    record2.pass_fail_cnt = 10;
    record2.linear_pass_fail_cnt = 5;

    int ret2 = MemberSecurityGrade_Add(&manager, &record2);
    TEST_ASSERT(ret2 == 0, "Second add (idempotent)");

    /* 验证最新数据 */
    MemberSecurityGrade result;
    MemberSecurityGrade_Get(&manager, 10002, &result);
    TEST_ASSERT(result.pass_fail_cnt == 10, "pass_fail_cnt replaced");
}

/* Test 6: IncrementPassFailCnt - 原子递增密码失败次数 */
static void Test_IncrementPassFailCnt() {
    TEST_START("MemberSecurityGrade_IncrementPassFailCnt");

    int ret = MemberSecurityGrade_IncrementPassFailCnt(&manager, 10001);
    TEST_ASSERT(ret == 0, "Increment pass fail count");

    /* 验证递增 */
    MemberSecurityGrade record;
    MemberSecurityGrade_Get(&manager, 10001, &record);
    TEST_ASSERT(record.pass_fail_cnt == 3, "pass_fail_cnt incremented to 3");
    TEST_ASSERT(record.last_pass_fail_time > 0, "last_pass_fail_time updated");
}

/* Test 7: ResetPassFailCnt - 重置密码失败次数 */
static void Test_ResetPassFailCnt() {
    TEST_START("MemberSecurityGrade_ResetPassFailCnt");

    int ret = MemberSecurityGrade_ResetPassFailCnt(&manager, 10002);
    TEST_ASSERT(ret == 0, "Reset pass fail count");

    /* 验证重置 */
    MemberSecurityGrade record;
    MemberSecurityGrade_Get(&manager, 10002, &record);
    TEST_ASSERT(record.pass_fail_cnt == 0, "pass_fail_cnt reset to 0");
    TEST_ASSERT(record.linear_pass_fail_cnt == 0, "linear_pass_fail_cnt reset to 0");
}

/* Test 8: IncrementLinearPassFailCnt - 原子递增连续密码失败次数 */
static void Test_IncrementLinearPassFailCnt() {
    TEST_START("MemberSecurityGrade_IncrementLinearPassFailCnt");

    int ret = MemberSecurityGrade_IncrementLinearPassFailCnt(&manager, 10001);
    TEST_ASSERT(ret == 0, "Increment linear pass fail count");

    /* 验证递增 */
    MemberSecurityGrade record;
    MemberSecurityGrade_Get(&manager, 10001, &record);
    TEST_ASSERT(record.linear_pass_fail_cnt == 2, "linear_pass_fail_cnt incremented to 2");
}

/* Test 9: GetByPassFailCntRange - 按密码失败次数范围查询 */
static void Test_GetByPassFailCntRange() {
    TEST_START("MemberSecurityGrade_GetByPassFailCntRange");

    /* 添加更多测试数据 */
    MemberSecurityGrade r1;
    memset(&r1, 0, sizeof(MemberSecurityGrade));
    r1.m_id = 10003;
    strcpy(r1.last_visit_time, "2025-11-14 12:00:00");
    r1.pass_fail_cnt = 15;
    strcpy(r1.last_vaccine_date, "2025-11-14 11:00:00");
    strcpy(r1.last_window_date, "2025-11-14 11:00:00");
    strcpy(r1.goblin_pass_mod, "2025-11-01 00:00:00");
    r1.goblin_fail_cnt = 0;
    strcpy(r1.security_card_reg, "2025-11-01 00:00:00");
    r1.security_card_fail_cnt = 0;
    strcpy(r1.m_opt_reg, "2025-11-01 00:00:00");
    strcpy(r1.pc_opt_reg, "2025-11-01 00:00:00");
    strcpy(r1.black_ip_try_time, "0000-00-00 00:00:00");
    r1.linear_pass_fail_cnt = 10;
    r1.last_pass_fail_time = 1700000000;
    strcpy(r1.last_check_time, "2025-11-14 11:00:00");
    strcpy(r1.pass_modify_check, "2025-11-01 00:00:00");
    strcpy(r1.member_pc_reg, "2025-11-01 00:00:00");
    strcpy(r1.gatekeeper_otp_reg, "2025-11-01 00:00:00");
    r1.goblin_validity_time = 86400;
    r1.security_card_validity_time = 86400;
    strcpy(r1.validity_ip, "192.168.3.100");
    r1.cargopad_status = 1;
    strcpy(r1.cargopad_mod, "2025-11-01 00:00:00");
    r1.cargopad_validity_time = 86400;

    MemberSecurityGrade_Add(&manager, &r1);

    MemberSecurityGrade results[10];
    int count = MemberSecurityGrade_GetByPassFailCntRange(&manager, 3, 20, results, 10);

    TEST_ASSERT(count >= 2, "Found at least 2 records in pass_fail_cnt range [3,20]");
}

/* Test 10: GetByLinearPassFailCnt - 按连续失败次数查询 */
static void Test_GetByLinearPassFailCnt() {
    TEST_START("MemberSecurityGrade_GetByLinearPassFailCnt");

    MemberSecurityGrade results[10];
    int count = MemberSecurityGrade_GetByLinearPassFailCnt(&manager, 2, results, 10);

    TEST_ASSERT(count >= 2, "Found at least 2 records with linear_pass_fail_cnt >= 2");
}

/* Test 11: GetByValidityIP - 按有效IP查询 */
static void Test_GetByValidityIP() {
    TEST_START("MemberSecurityGrade_GetByValidityIP");

    MemberSecurityGrade results[10];
    int count = MemberSecurityGrade_GetByValidityIP(&manager, "192.168.%", results, 10);

    TEST_ASSERT(count >= 3, "Found at least 3 records for IP pattern 192.168.%");
}

/* Test 12: GetByLastVisitTimeRange - 按最后访问时间范围查询 */
static void Test_GetByLastVisitTimeRange() {
    TEST_START("MemberSecurityGrade_GetByLastVisitTimeRange");

    MemberSecurityGrade results[10];
    int count = MemberSecurityGrade_GetByLastVisitTimeRange(&manager,
        "2025-11-14 10:00:00", "2025-11-14 13:00:00", results, 10);

    TEST_ASSERT(count >= 3, "Found records in last visit time range");
}

/* Test 13: GetHighRiskUsers - 获取高风险用户 */
static void Test_GetHighRiskUsers() {
    TEST_START("MemberSecurityGrade_GetHighRiskUsers");

    MemberSecurityGrade results[10];
    int count = MemberSecurityGrade_GetHighRiskUsers(&manager, 3, 2, results, 10);

    TEST_ASSERT(count >= 2, "Found at least 2 high risk users");
    /* 验证排序（按失败次数降序） */
    if (count >= 2) {
        TEST_ASSERT(results[0].pass_fail_cnt >= results[1].pass_fail_cnt,
                   "Results sorted by pass_fail_cnt DESC");
    }
}

/* Test 14: UpdateLastVisitTime - 更新最后访问时间 */
static void Test_UpdateLastVisitTime() {
    TEST_START("MemberSecurityGrade_UpdateLastVisitTime");

    /* 获取当前时间 */
    MemberSecurityGrade before;
    MemberSecurityGrade_Get(&manager, 10001, &before);

    int ret = MemberSecurityGrade_UpdateLastVisitTime(&manager, 10001);
    TEST_ASSERT(ret == 0, "Update last visit time");

    /* 验证时间已更新 */
    MemberSecurityGrade after;
    MemberSecurityGrade_Get(&manager, 10001, &after);
    TEST_ASSERT(strcmp(after.last_visit_time, before.last_visit_time) != 0,
               "last_visit_time updated");
}

/* Test 15: BulkAdd - 批量添加 */
static void Test_BulkAdd() {
    TEST_START("MemberSecurityGrade_BulkAdd");

    MemberSecurityGrade records[3];
    int i;
    for (i = 0; i < 3; i++) {
        memset(&records[i], 0, sizeof(MemberSecurityGrade));
        records[i].m_id = 20001 + i;
        sprintf(records[i].last_visit_time, "2025-11-14 13:%02d:00", i);
        records[i].pass_fail_cnt = i;
        strcpy(records[i].last_vaccine_date, "2025-11-14 12:00:00");
        strcpy(records[i].last_window_date, "2025-11-14 12:00:00");
        strcpy(records[i].goblin_pass_mod, "2025-11-01 00:00:00");
        records[i].goblin_fail_cnt = 0;
        strcpy(records[i].security_card_reg, "2025-11-01 00:00:00");
        records[i].security_card_fail_cnt = 0;
        strcpy(records[i].m_opt_reg, "2025-11-01 00:00:00");
        strcpy(records[i].pc_opt_reg, "2025-11-01 00:00:00");
        strcpy(records[i].black_ip_try_time, "0000-00-00 00:00:00");
        records[i].linear_pass_fail_cnt = 0;
        records[i].last_pass_fail_time = 0;
        strcpy(records[i].last_check_time, "2025-11-14 12:00:00");
        strcpy(records[i].pass_modify_check, "2025-11-01 00:00:00");
        strcpy(records[i].member_pc_reg, "2025-11-01 00:00:00");
        strcpy(records[i].gatekeeper_otp_reg, "2025-11-01 00:00:00");
        records[i].goblin_validity_time = 86400;
        records[i].security_card_validity_time = 86400;
        sprintf(records[i].validity_ip, "192.168.4.%d", 100 + i);
        records[i].cargopad_status = 1;
        strcpy(records[i].cargopad_mod, "2025-11-01 00:00:00");
        records[i].cargopad_validity_time = 86400;
    }

    int count = MemberSecurityGrade_BulkAdd(&manager, records, 3);
    TEST_ASSERT(count == 3, "BulkAdd added 3 records");

    /* 验证添加 */
    int exists = MemberSecurityGrade_Exists(&manager, 20001);
    TEST_ASSERT(exists == 1, "20001 exists");
}

/* Test 16: BulkDelete - 批量删除 */
static void Test_BulkDelete() {
    TEST_START("MemberSecurityGrade_BulkDelete");

    int m_ids[3] = {20001, 20002, 20003};

    int ret = MemberSecurityGrade_BulkDelete(&manager, m_ids, 3);
    TEST_ASSERT(ret == 0, "BulkDelete successful");

    /* 验证删除 */
    int exists = MemberSecurityGrade_Exists(&manager, 20001);
    TEST_ASSERT(exists == 0, "20001 deleted");
}

/* Test 17: Delete幂等性测试 */
static void Test_DeleteIdempotent() {
    TEST_START("MemberSecurityGrade_Delete (Idempotent)");

    /* 第一次删除 */
    int ret1 = MemberSecurityGrade_Delete(&manager, 10002);
    TEST_ASSERT(ret1 == 0, "First delete");

    /* 第二次删除（幂等） */
    int ret2 = MemberSecurityGrade_Delete(&manager, 10002);
    TEST_ASSERT(ret2 == 0, "Second delete (idempotent)");

    /* 验证不存在 */
    int exists = MemberSecurityGrade_Exists(&manager, 10002);
    TEST_ASSERT(exists == 0, "Record does not exist");
}

/* Test 18: PrintInfo - 工具函数测试 */
static void Test_PrintInfo() {
    TEST_START("MemberSecurityGrade_PrintInfo");

    MemberSecurityGrade record;
    MemberSecurityGrade_Get(&manager, 10001, &record);

    printf("  Testing PrintInfo output:\n");
    MemberSecurityGrade_PrintInfo(&record);

    TEST_ASSERT(1, "PrintInfo executed");
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    printf("===========================================\n");
    printf("  member_security_grade_orm Test Suite\n");
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
    Test_IncrementPassFailCnt();
    Test_ResetPassFailCnt();
    Test_IncrementLinearPassFailCnt();
    Test_GetByPassFailCntRange();
    Test_GetByLinearPassFailCnt();
    Test_GetByValidityIP();
    Test_GetByLastVisitTimeRange();
    Test_GetHighRiskUsers();
    Test_UpdateLastVisitTime();
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
