#include "member_play_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Test counters */
static int test_total = 0;
static int test_passed = 0;

/* Helper function to check test result */
void check_test(const char* test_name, int condition) {
    test_total++;
    if (condition) {
        test_passed++;
        printf("[PASS] %s\n", test_name);
    } else {
        printf("[FAIL] %s\n", test_name);
    }
}

/* Clean up test data */
void cleanup_test_data(DBConnectionManager* manager) {
    DBQueryResult result;
    char query[512];

    snprintf(query, sizeof(query),
        "DELETE FROM member_play_info WHERE m_id IN (100001, 100002, 100003)");
    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);
    DBQueryResult_Free(&result);
}

/* Test basic CRUD operations */
void test_basic_crud(DBConnectionManager* manager) {
    MemberPlayInfo info;
    int ret;

    printf("\n=== Test Basic CRUD ===\n");

    /* Test 1: Add new record */
    memset(&info, 0, sizeof(MemberPlayInfo));
    strcpy(info.occ_date, "2025-01-15");
    info.m_id = 100001;
    info.play_time = 3600;
    info.play_count = 5;
    info.trade_cnt = 2;
    info.exp = 1000;
    info.used_fatigue = 50;
    strcpy(info.ip, "192.168.1.100");
    info.last_play_time = 1234567890;
    info.pcbang_flag = 0;
    strcpy(info.end_ip, "100");
    info.ting_count = 3;
    strcpy(info.mac_addr, "AA:BB:CC:DD:EE:FF");
    info.server_id = 1;

    ret = MemberPlayInfo_Add(manager, &info);
    check_test("Add new record", ret == 0);

    /* Test 2: Check record exists */
    ret = MemberPlayInfo_Exists(manager, "2025-01-15", 100001);
    check_test("Record exists", ret == 1);

    /* Test 3: Get record */
    MemberPlayInfo retrieved;
    ret = MemberPlayInfo_Get(manager, "2025-01-15", 100001, &retrieved);
    check_test("Get record", ret == 0 && retrieved.m_id == 100001);
    check_test("Verify play_time", retrieved.play_time == 3600);
    check_test("Verify exp", retrieved.exp == 1000);

    /* Test 4: Update record */
    retrieved.play_time = 7200;
    retrieved.play_count = 10;
    retrieved.exp = 2000;
    ret = MemberPlayInfo_Update(manager, &retrieved);
    check_test("Update record", ret == 0);

    /* Test 5: Verify update */
    MemberPlayInfo updated;
    ret = MemberPlayInfo_Get(manager, "2025-01-15", 100001, &updated);
    check_test("Verify update - play_time", updated.play_time == 7200);
    check_test("Verify update - exp", updated.exp == 2000);

    /* Test 6: Delete record */
    ret = MemberPlayInfo_Delete(manager, "2025-01-15", 100001);
    check_test("Delete record", ret == 0);

    /* Test 7: Verify deletion */
    ret = MemberPlayInfo_Exists(manager, "2025-01-15", 100001);
    check_test("Verify deletion", ret == 0);
}

/* Test business operations */
void test_business_operations(DBConnectionManager* manager) {
    MemberPlayInfo info;
    MemberPlayInfo results[10];
    int actual_count;
    int ret;

    printf("\n=== Test Business Operations ===\n");

    /* Add test data - member 100002 with multiple dates */
    memset(&info, 0, sizeof(MemberPlayInfo));
    strcpy(info.occ_date, "2025-01-15");
    info.m_id = 100002;
    info.play_time = 3600;
    info.play_count = 5;
    strcpy(info.ip, "192.168.1.101");
    info.server_id = 1;
    info.pcbang_flag = 0;
    MemberPlayInfo_Add(manager, &info);

    strcpy(info.occ_date, "2025-01-16");
    info.play_time = 5400;
    info.play_count = 8;
    MemberPlayInfo_Add(manager, &info);

    strcpy(info.occ_date, "2025-01-17");
    info.play_time = 7200;
    info.play_count = 10;
    MemberPlayInfo_Add(manager, &info);

    /* Add PC bang user */
    memset(&info, 0, sizeof(MemberPlayInfo));
    strcpy(info.occ_date, "2025-01-15");
    info.m_id = 100003;
    info.play_time = 1800;
    info.play_count = 3;
    strcpy(info.ip, "10.0.0.50");
    info.server_id = 1;
    info.pcbang_flag = 1;
    MemberPlayInfo_Add(manager, &info);

    /* Test 1: Get by member ID */
    ret = MemberPlayInfo_GetByMemberId(manager, 100002, results, 10, &actual_count);
    check_test("Get by member ID", ret == 0 && actual_count == 3);

    /* Test 2: Get by date range */
    ret = MemberPlayInfo_GetByDateRange(manager, "2025-01-15", "2025-01-16",
                                        results, 10, &actual_count);
    check_test("Get by date range", ret == 0 && actual_count >= 3);

    /* Test 3: Get by server */
    ret = MemberPlayInfo_GetByServer(manager, 1, results, 10, &actual_count);
    check_test("Get by server", ret == 0 && actual_count >= 4);

    /* Test 4: Get PC bang users */
    ret = MemberPlayInfo_GetPCBangUsers(manager, "2025-01-15", results, 10, &actual_count);
    check_test("Get PC bang users", ret == 0 && actual_count >= 1);
    if (actual_count > 0) {
        check_test("Verify PC bang flag", results[0].pcbang_flag == 1);
    }
}

int main(int argc, char* argv[]) {
    DBConnectionManager manager;

    if (argc != 2) {
        printf("Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    /* Initialize connection manager */
    memset(&manager, 0, sizeof(DBConnectionManager));
    if (DBConnectionManager_Initialize(&manager, argv[1]) < 0) {
        printf("Failed to initialize connection manager\n");
        return 1;
    }

    /* Connect to database */
    if (DBConnectionManager_Connect(&manager, DB_TYPE_LOGIN) < 0) {
        printf("Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("=== Member Play Info ORM Test Suite ===\n");

    /* Clean up before tests */
    cleanup_test_data(&manager);

    /* Run tests */
    test_basic_crud(&manager);
    test_business_operations(&manager);

    /* Clean up after tests */
    cleanup_test_data(&manager);

    /* Release resources */
    DBConnectionManager_Cleanup(&manager);

    /* Print summary */
    printf("\n=== Test Results ===\n");
    printf("Total: %d, Passed: %d, Failed: %d\n",
           test_total, test_passed, test_total - test_passed);
    printf("Success Rate: %.1f%%\n",
           test_total > 0 ? (100.0 * test_passed / test_total) : 0.0);

    return (test_total == test_passed) ? 0 : 1;
}
