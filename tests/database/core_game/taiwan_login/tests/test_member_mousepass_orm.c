#include "member_mousepass_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Test counters */
static int test_total = 0;
static int test_passed = 0;

void cleanup_test_data(DBConnectionManager* manager);
void test_basic_crud(DBConnectionManager* manager);
void test_business_operations(DBConnectionManager* manager);

int main(int argc, char* argv[]) {
    DBConnectionManager manager;

    if (argc != 2) {
        printf("Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    memset(&manager, 0, sizeof(DBConnectionManager));
    if (DBConnectionManager_Initialize(&manager, argv[1]) < 0) {
        printf("Failed to initialize connection manager\n");
        return 1;
    }

    if (DBConnectionManager_Connect(&manager, DB_TYPE_LOGIN) < 0) {
        printf("Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("=== Member Mousepass ORM Test Suite ===\n\n");

    cleanup_test_data(&manager);
    test_basic_crud(&manager);
    test_business_operations(&manager);
    cleanup_test_data(&manager);

    DBConnectionManager_Cleanup(&manager);
    printf("\n=== Test Results: %d/%d passed ===\n", test_passed, test_total);
    return (test_passed == test_total) ? 0 : 1;
}

void cleanup_test_data(DBConnectionManager* manager) {
    DBQueryResult result;
    char query[256];

    snprintf(query, sizeof(query),
        "DELETE FROM member_mousepass WHERE m_id IN (100001, 100002, 100003)");
    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    MemberMousepass record;
    int exists;

    printf("Test 1: Add mouse password record\n");
    test_total++;
    memset(&record, 0, sizeof(MemberMousepass));
    record.m_id = 100001;
    strcpy(record.mousepass, "abc123def456");
    strcpy(record.occ_time, "2025-11-19 10:00:00");
    record.fail_cnt = 0;
    record.cancel_cnt = 0;
    record.version_info = '1';
    record.validity_time = 86400;
    record.reward_time = 0;
    record.enable_flag = 'Y';

    if (MemberMousepass_Add(manager, &record) == 0) {
        printf("  PASS: Record added successfully\n");
        test_passed++;
    } else {
        printf("  FAIL: Failed to add record\n");
    }

    printf("Test 2: Check if record exists\n");
    test_total++;
    exists = MemberMousepass_Exists(manager, 100001);
    if (exists) {
        printf("  PASS: Record exists\n");
        test_passed++;
    } else {
        printf("  FAIL: Record not found\n");
    }

    printf("Test 3: Get mouse password record\n");
    test_total++;
    memset(&record, 0, sizeof(MemberMousepass));
    if (MemberMousepass_Get(manager, 100001, &record) == 0) {
        printf("  PASS: Record retrieved successfully\n");
        printf("    m_id: %d, mousepass: %s, enable_flag: %c\n",
               record.m_id, record.mousepass, record.enable_flag);
        test_passed++;
    } else {
        printf("  FAIL: Failed to retrieve record\n");
    }

    printf("Test 4: Update fail count\n");
    test_total++;
    record.fail_cnt = 3;
    if (MemberMousepass_Update(manager, &record) == 0) {
        memset(&record, 0, sizeof(MemberMousepass));
        MemberMousepass_Get(manager, 100001, &record);
        if (record.fail_cnt == 3) {
            printf("  PASS: Fail count updated to %u\n", record.fail_cnt);
            test_passed++;
        } else {
            printf("  FAIL: Fail count not updated correctly\n");
        }
    } else {
        printf("  FAIL: Failed to update record\n");
    }

    printf("Test 5: Delete mouse password record\n");
    test_total++;
    if (MemberMousepass_Delete(manager, 100001) == 0) {
        exists = MemberMousepass_Exists(manager, 100001);
        if (!exists) {
            printf("  PASS: Record deleted successfully\n");
            test_passed++;
        } else {
            printf("  FAIL: Record still exists after deletion\n");
        }
    } else {
        printf("  FAIL: Failed to delete record\n");
    }
}

void test_business_operations(DBConnectionManager* manager) {
    MemberMousepass records[10];
    int actual_count = 0;
    MemberMousepass record;

    printf("Test 6: Add multiple records\n");
    test_total++;

    memset(&record, 0, sizeof(MemberMousepass));
    record.m_id = 100001;
    strcpy(record.mousepass, "pass1");
    strcpy(record.occ_time, "2025-11-19 10:00:00");
    record.enable_flag = 'Y';
    record.version_info = '1';
    MemberMousepass_Add(manager, &record);

    record.m_id = 100002;
    strcpy(record.mousepass, "pass2");
    record.enable_flag = 'Y';
    MemberMousepass_Add(manager, &record);

    record.m_id = 100003;
    strcpy(record.mousepass, "pass3");
    record.enable_flag = 'N';
    MemberMousepass_Add(manager, &record);

    printf("  PASS: Multiple records added\n");
    test_passed++;

    printf("Test 7: Get records by enable flag\n");
    test_total++;
    if (MemberMousepass_GetByEnableFlag(manager, 'Y', records, 10, &actual_count) == 0) {
        if (actual_count >= 2) {
            printf("  PASS: Found %d records with enable_flag='Y'\n", actual_count);
            test_passed++;
        } else {
            printf("  FAIL: Expected at least 2 records, found %d\n", actual_count);
        }
    } else {
        printf("  FAIL: Failed to query records\n");
    }

    printf("Test 8: Update fail count\n");
    test_total++;
    if (MemberMousepass_UpdateFailCount(manager, 100001, 5) == 0) {
        memset(&record, 0, sizeof(MemberMousepass));
        MemberMousepass_Get(manager, 100001, &record);
        if (record.fail_cnt == 5) {
            printf("  PASS: Fail count updated to %u\n", record.fail_cnt);
            test_passed++;
        } else {
            printf("  FAIL: Fail count not updated correctly\n");
        }
    } else {
        printf("  FAIL: Failed to update fail count\n");
    }

    printf("Test 9: Reset fail count\n");
    test_total++;
    if (MemberMousepass_ResetFailCount(manager, 100001) == 0) {
        memset(&record, 0, sizeof(MemberMousepass));
        MemberMousepass_Get(manager, 100001, &record);
        if (record.fail_cnt == 0) {
            printf("  PASS: Fail count reset to 0\n");
            test_passed++;
        } else {
            printf("  FAIL: Fail count not reset correctly\n");
        }
    } else {
        printf("  FAIL: Failed to reset fail count\n");
    }

    printf("Test 10: Increment cancel count\n");
    test_total++;
    if (MemberMousepass_IncrementCancelCount(manager, 100001) == 0) {
        memset(&record, 0, sizeof(MemberMousepass));
        MemberMousepass_Get(manager, 100001, &record);
        if (record.cancel_cnt > 0) {
            printf("  PASS: Cancel count incremented to %u\n", record.cancel_cnt);
            test_passed++;
        } else {
            printf("  FAIL: Cancel count not incremented\n");
        }
    } else {
        printf("  FAIL: Failed to increment cancel count\n");
    }
}
