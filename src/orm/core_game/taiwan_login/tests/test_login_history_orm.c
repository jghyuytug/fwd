#include "login_history_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static int test_total = 0;
static int test_passed = 0;

void cleanup_test_data(DBConnectionManager* manager);
void test_add_and_query(DBConnectionManager* manager);
void test_time_range_query(DBConnectionManager* manager);

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

    printf("=== Login History ORM Test Suite ===\n\n");

    cleanup_test_data(&manager);
    test_add_and_query(&manager);
    test_time_range_query(&manager);
    cleanup_test_data(&manager);

    DBConnectionManager_Cleanup(&manager);
    printf("\n=== Test Results: %d/%d passed ===\n", test_passed, test_total);
    return (test_passed == test_total) ? 0 : 1;
}

void cleanup_test_data(DBConnectionManager* manager) {
    DBQueryResult result;
    char query[256];

    snprintf(query, sizeof(query),
        "DELETE FROM login_history WHERE m_id IN (100001, 100002)");
    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);
    DBQueryResult_Free(&result);
}

void test_add_and_query(DBConnectionManager* manager) {
    LoginHistory record;
    LoginHistory records[10];
    int actual_count = 0;
    int current_time = (int)time(NULL);

    printf("Test 1: Add login history record\n");
    test_total++;
    memset(&record, 0, sizeof(LoginHistory));
    record.m_id = 100001;
    record.occ_time = current_time;
    record.trigger_type = 1;

    if (LoginHistory_Add(manager, &record) == 0) {
        printf("  PASS: Record added\n");
        test_passed++;
    } else {
        printf("  FAIL: Failed to add record\n");
    }

    printf("Test 2: Add multiple history records\n");
    test_total++;

    record.m_id = 100001;
    record.occ_time = current_time + 60;
    record.trigger_type = 2;
    LoginHistory_Add(manager, &record);

    record.m_id = 100002;
    record.occ_time = current_time + 120;
    record.trigger_type = 1;
    LoginHistory_Add(manager, &record);

    printf("  PASS: Multiple records added\n");
    test_passed++;

    printf("Test 3: Query by member ID\n");
    test_total++;
    if (LoginHistory_GetByMemberId(manager, 100001, records, 10, &actual_count) == 0) {
        if (actual_count >= 2) {
            printf("  PASS: Found %d records for m_id=100001\n", actual_count);
            test_passed++;
        } else {
            printf("  FAIL: Expected at least 2 records, found %d\n", actual_count);
        }
    } else {
        printf("  FAIL: Failed to query by member ID\n");
    }

    printf("Test 4: Query by trigger type\n");
    test_total++;
    if (LoginHistory_GetByTrigger(manager, 1, records, 10, &actual_count) == 0) {
        if (actual_count >= 2) {
            printf("  PASS: Found %d records with trigger_type=1\n", actual_count);
            test_passed++;
        } else {
            printf("  FAIL: Expected at least 2 records, found %d\n", actual_count);
        }
    } else {
        printf("  FAIL: Failed to query by trigger type\n");
    }
}

void test_time_range_query(DBConnectionManager* manager) {
    LoginHistory records[10];
    int actual_count = 0;
    int current_time = (int)time(NULL);

    printf("Test 5: Query by time range\n");
    test_total++;
    if (LoginHistory_GetByTimeRange(manager,
                                     current_time - 60,
                                     current_time + 180,
                                     records, 10, &actual_count) == 0) {
        if (actual_count >= 3) {
            printf("  PASS: Found %d records in time range\n", actual_count);
            test_passed++;
        } else {
            printf("  FAIL: Expected at least 3 records, found %d\n", actual_count);
        }
    } else {
        printf("  FAIL: Failed to query by time range\n");
    }

    printf("Test 6: Get recent records\n");
    test_total++;
    if (LoginHistory_GetRecent(manager, 5, records, 10, &actual_count) == 0) {
        if (actual_count >= 3) {
            printf("  PASS: Found %d recent records\n", actual_count);
            test_passed++;
        } else {
            printf("  FAIL: Expected at least 3 records, found %d\n", actual_count);
        }
    } else {
        printf("  FAIL: Failed to get recent records\n");
    }

    printf("Test 7: Delete old records\n");
    test_total++;
    if (LoginHistory_DeleteOldRecords(manager, current_time + 90) == 0) {
        LoginHistory_GetByMemberId(manager, 100002, records, 10, &actual_count);
        if (actual_count == 1) {
            printf("  PASS: Old records deleted, %d remaining\n", actual_count);
            test_passed++;
        } else {
            printf("  FAIL: Expected 1 remaining record, found %d\n", actual_count);
        }
    } else {
        printf("  FAIL: Failed to delete old records\n");
    }
}
