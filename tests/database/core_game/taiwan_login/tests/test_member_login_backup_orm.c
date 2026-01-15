#include "member_login_backup_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Test counters */
static int test_total = 0;
static int test_passed = 0;

void test_basic_operations(DBConnectionManager* manager);
void test_query_operations(DBConnectionManager* manager);
void cleanup_test_data(DBConnectionManager* manager);

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

    printf("=== MemberLoginBackup ORM Test Suite ===\n\n");

    /* Clean up before test */
    cleanup_test_data(&manager);

    /* Run tests */
    test_basic_operations(&manager);
    test_query_operations(&manager);

    /* Clean up after test */
    cleanup_test_data(&manager);

    /* Release resources */
    DBConnectionManager_Cleanup(&manager);

    printf("\n=== Test Results ===\n");
    printf("Total: %d, Passed: %d, Failed: %d\n",
           test_total, test_passed, test_total - test_passed);
    printf("\n=== All Tests Completed ===\n");

    return (test_total == test_passed) ? 0 : 1;
}

void test_basic_operations(DBConnectionManager* manager) {
    MemberLoginBackup backup;
    MemberLoginBackup retrieved[10];
    int actual_count;
    unsigned int current_time = (unsigned int)time(NULL);

    printf("[TEST] Basic Operations\n");

    /* Test 1: Add backup record */
    test_total++;
    memset(&backup, 0, sizeof(MemberLoginBackup));
    backup.m_id = 100001;
    backup.login_time = current_time;
    backup.expire_time = current_time + 3600;
    backup.last_play_time = current_time;
    backup.total_account_fail = 0;
    backup.account_fail = 0;
    backup.report_cnt = 0;
    backup.reliable_flag = 1;
    backup.trade_gold_daily = 0;
    backup.last_gift_time = 0;
    backup.gift_cnt = 0;
    strncpy(backup.login_ip, "192.168.1.100", sizeof(backup.login_ip) - 1);
    backup.security_flag = 0;
    backup.power_side = 1;
    backup.dungeon_gain_gold = 0;
    backup.school_id = 0;
    backup.rating = 0.0f;
    backup.cleanpad_point = 0;
    backup.tutorial_skipable = '0';

    if (MemberLoginBackup_Add(manager, &backup) == 0) {
        printf("  [PASS] Add backup record\n");
        test_passed++;
    } else {
        printf("  [FAIL] Add backup record\n");
    }

    /* Test 2: Get by member ID */
    test_total++;
    memset(retrieved, 0, sizeof(retrieved));

    if (MemberLoginBackup_GetByMemberId(manager, 100001, retrieved, 10, &actual_count) == 0) {
        if (actual_count == 1 && retrieved[0].m_id == 100001) {
            printf("  [PASS] Get by member ID (count=%d)\n", actual_count);
            test_passed++;
        } else {
            printf("  [FAIL] Get by member ID - unexpected count %d\n", actual_count);
        }
    } else {
        printf("  [FAIL] Get by member ID\n");
    }

    /* Test 3: Count by member ID */
    test_total++;
    int count = MemberLoginBackup_CountByMemberId(manager, 100001);
    if (count == 1) {
        printf("  [PASS] Count by member ID (count=%d)\n", count);
        test_passed++;
    } else {
        printf("  [FAIL] Count by member ID - expected 1, got %d\n", count);
    }

    /* Test 4: Add multiple records */
    test_total++;
    backup.login_time = current_time + 100;
    backup.login_ip[0] = '\0';
    strncpy(backup.login_ip, "192.168.1.101", sizeof(backup.login_ip) - 1);
    MemberLoginBackup_Add(manager, &backup);

    backup.login_time = current_time + 200;
    backup.login_ip[0] = '\0';
    strncpy(backup.login_ip, "192.168.1.102", sizeof(backup.login_ip) - 1);

    if (MemberLoginBackup_Add(manager, &backup) == 0) {
        count = MemberLoginBackup_CountByMemberId(manager, 100001);
        if (count == 3) {
            printf("  [PASS] Add multiple records (total=%d)\n", count);
            test_passed++;
        } else {
            printf("  [FAIL] Add multiple records - expected 3, got %d\n", count);
        }
    } else {
        printf("  [FAIL] Add multiple records\n");
    }

    /* Test 5: Delete by member ID */
    test_total++;
    if (MemberLoginBackup_DeleteByMemberId(manager, 100001) == 0) {
        count = MemberLoginBackup_CountByMemberId(manager, 100001);
        if (count == 0) {
            printf("  [PASS] Delete by member ID\n");
            test_passed++;
        } else {
            printf("  [FAIL] Delete by member ID - %d records remain\n", count);
        }
    } else {
        printf("  [FAIL] Delete by member ID\n");
    }

    printf("\n");
}

void test_query_operations(DBConnectionManager* manager) {
    MemberLoginBackup backup;
    MemberLoginBackup retrieved[10];
    int actual_count;
    unsigned int base_time = (unsigned int)time(NULL);

    printf("[TEST] Query Operations\n");

    /* Prepare test data */
    memset(&backup, 0, sizeof(MemberLoginBackup));
    backup.m_id = 200001;
    backup.login_time = base_time - 1000;
    backup.expire_time = base_time + 3600;
    backup.last_play_time = base_time - 1000;
    strncpy(backup.login_ip, "10.0.0.1", sizeof(backup.login_ip) - 1);
    backup.reliable_flag = 1;
    backup.power_side = 1;
    backup.tutorial_skipable = '0';
    MemberLoginBackup_Add(manager, &backup);

    backup.m_id = 200002;
    backup.login_time = base_time - 500;
    backup.login_ip[0] = '\0';
    strncpy(backup.login_ip, "10.0.0.1", sizeof(backup.login_ip) - 1);
    MemberLoginBackup_Add(manager, &backup);

    backup.m_id = 200003;
    backup.login_time = base_time - 100;
    backup.login_ip[0] = '\0';
    strncpy(backup.login_ip, "10.0.0.2", sizeof(backup.login_ip) - 1);
    MemberLoginBackup_Add(manager, &backup);

    /* Test 6: Get by IP address */
    test_total++;
    memset(retrieved, 0, sizeof(retrieved));

    if (MemberLoginBackup_GetByIp(manager, "10.0.0.1", retrieved, 10, &actual_count) == 0) {
        if (actual_count == 2) {
            printf("  [PASS] Get by IP address (count=%d)\n", actual_count);
            test_passed++;
        } else {
            printf("  [FAIL] Get by IP - expected 2, got %d\n", actual_count);
        }
    } else {
        printf("  [FAIL] Get by IP address\n");
    }

    /* Test 7: Get by date range */
    test_total++;
    memset(retrieved, 0, sizeof(retrieved));

    if (MemberLoginBackup_GetByDateRange(manager, base_time - 600, base_time,
                                          retrieved, 10, &actual_count) == 0) {
        if (actual_count == 2) {
            printf("  [PASS] Get by date range (count=%d)\n", actual_count);
            test_passed++;
        } else {
            printf("  [FAIL] Get by date range - expected 2, got %d\n", actual_count);
        }
    } else {
        printf("  [FAIL] Get by date range\n");
    }

    printf("\n");
}

void cleanup_test_data(DBConnectionManager* manager) {
    MemberLoginBackup_DeleteByMemberId(manager, 100001);
    MemberLoginBackup_DeleteByMemberId(manager, 200001);
    MemberLoginBackup_DeleteByMemberId(manager, 200002);
    MemberLoginBackup_DeleteByMemberId(manager, 200003);
}
