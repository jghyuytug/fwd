#include "log_growth_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Test statistics */
static int test_total = 0;
static int test_passed = 0;

/* Test data constants */
#define TEST_MID_1      100001
#define TEST_MID_2      100002
#define TEST_SERVER_1   1
#define TEST_SERVER_2   2
#define TEST_CHARAC_1   200001
#define TEST_CHARAC_2   200002

/* Function prototypes */
void test_basic_crud(DBConnectionManager* manager);
void test_business_operations(DBConnectionManager* manager);
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

    printf("=== Log Growth ORM Test Suite ===\n\n");

    /* Clean up before test */
    cleanup_test_data(&manager);

    /* Run tests */
    test_basic_crud(&manager);
    test_business_operations(&manager);

    /* Clean up after test */
    cleanup_test_data(&manager);

    /* Release resources */
    DBConnectionManager_Cleanup(&manager);

    /* Print results */
    printf("\n=== Test Results ===\n");
    printf("Total: %d\n", test_total);
    printf("Passed: %d\n", test_passed);
    printf("Failed: %d\n", test_total - test_passed);
    printf("Success Rate: %.2f%%\n", test_total > 0 ? (test_passed * 100.0 / test_total) : 0);

    return (test_passed == test_total) ? 0 : 1;
}

void cleanup_test_data(DBConnectionManager* manager) {
    DBQueryResult result;
    char query[512];

    snprintf(query, sizeof(query),
        "DELETE FROM log_growth WHERE m_id IN (%d, %d)",
        TEST_MID_1, TEST_MID_2);

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    LogGrowth record1, record2, fetched;
    int exists;

    printf("TEST 1: Add first log_growth record\n");
    test_total++;
    memset(&record1, 0, sizeof(LogGrowth));
    record1.m_id = TEST_MID_1;
    record1.server_info = TEST_SERVER_1;
    record1.charac_no = TEST_CHARAC_1;
    strncpy(record1.charac_name, "TestChar1", sizeof(record1.charac_name) - 1);
    record1.job = 5;
    record1.grow_type = 1;
    strncpy(record1.occ_time, "2025-11-19 10:00:00", sizeof(record1.occ_time) - 1);

    if (LogGrowth_Add(manager, &record1) == 0) {
        printf("  PASS\n");
        test_passed++;
    } else {
        printf("  FAIL: Failed to add record\n");
    }

    printf("TEST 2: Add second log_growth record\n");
    test_total++;
    memset(&record2, 0, sizeof(LogGrowth));
    record2.m_id = TEST_MID_1;
    record2.server_info = TEST_SERVER_1;
    record2.charac_no = TEST_CHARAC_2;
    strncpy(record2.charac_name, "TestChar2", sizeof(record2.charac_name) - 1);
    record2.job = 10;
    record2.grow_type = 2;
    strncpy(record2.occ_time, "2025-11-19 11:00:00", sizeof(record2.occ_time) - 1);

    if (LogGrowth_Add(manager, &record2) == 0) {
        printf("  PASS\n");
        test_passed++;
    } else {
        printf("  FAIL: Failed to add record\n");
    }

    printf("TEST 3: Get log_growth record (3-field composite key)\n");
    test_total++;
    memset(&fetched, 0, sizeof(LogGrowth));
    if (LogGrowth_Get(manager, TEST_MID_1, TEST_SERVER_1, TEST_CHARAC_1, &fetched) == 0) {
        if (fetched.m_id == TEST_MID_1 &&
            fetched.server_info == TEST_SERVER_1 &&
            fetched.charac_no == TEST_CHARAC_1 &&
            strcmp(fetched.charac_name, "TestChar1") == 0 &&
            fetched.job == 5 &&
            fetched.grow_type == 1) {
            printf("  PASS\n");
            test_passed++;
        } else {
            printf("  FAIL: Data mismatch\n");
        }
    } else {
        printf("  FAIL: Failed to get record\n");
    }

    printf("TEST 4: Check record exists (3-field composite key)\n");
    test_total++;
    exists = LogGrowth_Exists(manager, TEST_MID_1, TEST_SERVER_1, TEST_CHARAC_1);
    if (exists == 1) {
        printf("  PASS\n");
        test_passed++;
    } else {
        printf("  FAIL: Record should exist (got %d)\n", exists);
    }

    printf("TEST 5: Update log_growth record\n");
    test_total++;
    strncpy(fetched.charac_name, "UpdatedChar1", sizeof(fetched.charac_name) - 1);
    fetched.job = 15;
    fetched.grow_type = 3;

    if (LogGrowth_Update(manager, &fetched) == 0) {
        memset(&fetched, 0, sizeof(LogGrowth));
        if (LogGrowth_Get(manager, TEST_MID_1, TEST_SERVER_1, TEST_CHARAC_1, &fetched) == 0) {
            if (strcmp(fetched.charac_name, "UpdatedChar1") == 0 &&
                fetched.job == 15 &&
                fetched.grow_type == 3) {
                printf("  PASS\n");
                test_passed++;
            } else {
                printf("  FAIL: Update not reflected\n");
            }
        } else {
            printf("  FAIL: Failed to verify update\n");
        }
    } else {
        printf("  FAIL: Failed to update record\n");
    }

    printf("TEST 6: Delete log_growth record (3-field composite key)\n");
    test_total++;
    if (LogGrowth_Delete(manager, TEST_MID_1, TEST_SERVER_1, TEST_CHARAC_1) == 0) {
        exists = LogGrowth_Exists(manager, TEST_MID_1, TEST_SERVER_1, TEST_CHARAC_1);
        if (exists == 0) {
            printf("  PASS\n");
            test_passed++;
        } else {
            printf("  FAIL: Record still exists after delete\n");
        }
    } else {
        printf("  FAIL: Failed to delete record\n");
    }

    printf("TEST 7: Check non-existent record\n");
    test_total++;
    exists = LogGrowth_Exists(manager, 999999, 99, 999999);
    if (exists == 0) {
        printf("  PASS\n");
        test_passed++;
    } else {
        printf("  FAIL: Non-existent record should return 0 (got %d)\n", exists);
    }

    /* Clean up record2 from test_basic_crud */
    LogGrowth_Delete(manager, TEST_MID_1, TEST_SERVER_1, TEST_CHARAC_2);
}

void test_business_operations(DBConnectionManager* manager) {
    LogGrowth records[10];
    int actual_count;

    /* Add test data */
    LogGrowth test_records[3];
    memset(test_records, 0, sizeof(test_records));

    test_records[0].m_id = TEST_MID_1;
    test_records[0].server_info = TEST_SERVER_1;
    test_records[0].charac_no = TEST_CHARAC_1;
    strncpy(test_records[0].charac_name, "TestChar1", sizeof(test_records[0].charac_name) - 1);
    test_records[0].job = 5;
    test_records[0].grow_type = 1;
    strncpy(test_records[0].occ_time, "2025-11-19 10:00:00", sizeof(test_records[0].occ_time) - 1);

    test_records[1].m_id = TEST_MID_1;
    test_records[1].server_info = TEST_SERVER_1;
    test_records[1].charac_no = TEST_CHARAC_2;
    strncpy(test_records[1].charac_name, "TestChar2", sizeof(test_records[1].charac_name) - 1);
    test_records[1].job = 5;
    test_records[1].grow_type = 2;
    strncpy(test_records[1].occ_time, "2025-11-19 11:00:00", sizeof(test_records[1].occ_time) - 1);

    test_records[2].m_id = TEST_MID_2;
    test_records[2].server_info = TEST_SERVER_2;
    test_records[2].charac_no = TEST_CHARAC_1;
    strncpy(test_records[2].charac_name, "TestChar3", sizeof(test_records[2].charac_name) - 1);
    test_records[2].job = 10;
    test_records[2].grow_type = 1;
    strncpy(test_records[2].occ_time, "2025-11-19 12:00:00", sizeof(test_records[2].occ_time) - 1);

    LogGrowth_Add(manager, &test_records[0]);
    LogGrowth_Add(manager, &test_records[1]);
    LogGrowth_Add(manager, &test_records[2]);

    printf("TEST 8: Get logs by member ID\n");
    test_total++;
    actual_count = 0;
    if (LogGrowth_GetByMemberId(manager, TEST_MID_1, records, 10, &actual_count) == 0) {
        if (actual_count == 2) {
            printf("  PASS (found %d records)\n", actual_count);
            test_passed++;
        } else {
            printf("  FAIL: Expected 2 records, got %d\n", actual_count);
        }
    } else {
        printf("  FAIL: Failed to get records\n");
    }

    printf("TEST 9: Get logs by character\n");
    test_total++;
    actual_count = 0;
    if (LogGrowth_GetByCharacter(manager, TEST_MID_1, TEST_SERVER_1, TEST_CHARAC_1, records, 10, &actual_count) == 0) {
        if (actual_count == 1) {
            printf("  PASS (found %d record)\n", actual_count);
            test_passed++;
        } else {
            printf("  FAIL: Expected 1 record, got %d\n", actual_count);
        }
    } else {
        printf("  FAIL: Failed to get records\n");
    }

    printf("TEST 10: Get logs by job\n");
    test_total++;
    actual_count = 0;
    if (LogGrowth_GetByJob(manager, 5, records, 10, &actual_count) == 0) {
        if (actual_count == 2) {
            printf("  PASS (found %d records for job 5)\n", actual_count);
            test_passed++;
        } else {
            printf("  FAIL: Expected 2 records, got %d\n", actual_count);
        }
    } else {
        printf("  FAIL: Failed to get records\n");
    }

    printf("TEST 11: Get logs by grow type\n");
    test_total++;
    actual_count = 0;
    if (LogGrowth_GetByGrowType(manager, 1, records, 10, &actual_count) == 0) {
        if (actual_count == 2) {
            printf("  PASS (found %d records for grow_type 1)\n", actual_count);
            test_passed++;
        } else {
            printf("  FAIL: Expected 2 records, got %d\n", actual_count);
        }
    } else {
        printf("  FAIL: Failed to get records\n");
    }

    printf("TEST 12: PrintInfo function\n");
    test_total++;
    if (actual_count > 0) {
        LogGrowth_PrintInfo(&records[0]);
        printf("  PASS\n");
        test_passed++;
    } else {
        printf("  FAIL: No record to print\n");
    }
}
