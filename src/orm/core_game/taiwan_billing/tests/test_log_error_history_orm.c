#include "log_error_history_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Test counters */
static int test_total = 0;
static int test_passed = 0;

/* Test helper macros */
#define TEST_START(name) do { \
    printf("\n[TEST] %s\n", name); \
    test_total++; \
} while(0)

#define TEST_PASS() do { \
    printf("  PASS\n"); \
    test_passed++; \
} while(0)

#define TEST_FAIL(msg) do { \
    printf("  FAIL: %s\n", msg); \
} while(0)

/* Forward declarations */
void cleanup_test_data(DBConnectionManager* manager);
void test_crud_operations(DBConnectionManager* manager);
void test_business_operations(DBConnectionManager* manager);

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
    if (DBConnectionManager_Connect(&manager, DB_TYPE_BILLING) < 0) {
        printf("Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("=== Log Error History ORM Test Suite ===\n");

    /* Clean up before tests */
    cleanup_test_data(&manager);

    /* Run test suites */
    test_crud_operations(&manager);
    test_business_operations(&manager);

    /* Clean up after tests */
    cleanup_test_data(&manager);

    /* Release resources */
    DBConnectionManager_Cleanup(&manager);

    /* Print summary */
    printf("\n=== Test Summary ===\n");
    printf("Total: %d, Passed: %d, Failed: %d\n",
           test_total, test_passed, test_total - test_passed);
    printf("Success Rate: %.1f%%\n",
           test_total > 0 ? (test_passed * 100.0 / test_total) : 0.0);

    return (test_passed == test_total) ? 0 : 1;
}

void cleanup_test_data(DBConnectionManager* manager) {
    DBQueryResult result;
    char query[512];

    snprintf(query, sizeof(query),
        "DELETE FROM log_error_history WHERE error_id IN (9001, 9002, 9003, 9004)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result);
    DBQueryResult_Free(&result);
}

void test_crud_operations(DBConnectionManager* manager) {
    LogErrorHistory record;
    int exists;

    printf("\n--- CRUD Operations Tests ---\n");

    /* Test 1: Add new error log */
    TEST_START("Add new error log");
    memset(&record, 0, sizeof(LogErrorHistory));
    record.error_id = 9001;
    strncpy(record.error_msg, "Test error message", sizeof(record.error_msg) - 1);
    strncpy(record.error_query, "SELECT * FROM test_table", sizeof(record.error_query) - 1);
    strncpy(record.proc_name, "test_procedure", sizeof(record.proc_name) - 1);
    record.proc_line = 42;
    strncpy(record.query_user, "test_user", sizeof(record.query_user) - 1);
    /* Let database set occ_date with NOW() */

    if (LogErrorHistory_Add(manager, &record) == 0) {
        TEST_PASS();
    } else {
        TEST_FAIL("Failed to add error log");
    }

    /* Test 2: Get recent record to get auto-generated no */
    TEST_START("Get recent record");
    {
        LogErrorHistory recent_records[1];
        int count = 0;

        if (LogErrorHistory_GetRecent(manager, recent_records, 1, &count) == 0 && count > 0) {
            if (recent_records[0].error_id == 9001) {
                TEST_PASS();
                printf("  Retrieved record no: %u\n", recent_records[0].no);
                /* Save no for later tests */
                record.no = recent_records[0].no;
            } else {
                TEST_FAIL("Data mismatch");
            }
        } else {
            TEST_FAIL("Failed to get recent record");
        }
    }

    /* Test 3: Check existence */
    TEST_START("Check record existence");
    exists = LogErrorHistory_Exists(manager, record.no);
    if (exists == 1) {
        TEST_PASS();
    } else {
        TEST_FAIL("Record should exist");
    }

    /* Test 4: Get record by no */
    TEST_START("Get record by no");
    {
        LogErrorHistory retrieved;
        memset(&retrieved, 0, sizeof(LogErrorHistory));

        if (LogErrorHistory_Get(manager, record.no, &retrieved) == 0) {
            if (retrieved.error_id == 9001 &&
                strcmp(retrieved.error_msg, "Test error message") == 0) {
                TEST_PASS();
                printf("  Error ID: %d, Msg: %s\n", retrieved.error_id, retrieved.error_msg);
            } else {
                TEST_FAIL("Data mismatch");
            }
        } else {
            TEST_FAIL("Failed to get record");
        }
    }

    /* Test 5: Delete record */
    TEST_START("Delete record");
    if (LogErrorHistory_Delete(manager, record.no) == 0) {
        exists = LogErrorHistory_Exists(manager, record.no);
        if (exists == 0) {
            TEST_PASS();
        } else {
            TEST_FAIL("Record still exists after delete");
        }
    } else {
        TEST_FAIL("Failed to delete record");
    }
}

void test_business_operations(DBConnectionManager* manager) {
    LogErrorHistory record;

    printf("\n--- Business Operations Tests ---\n");

    /* Setup: Create multiple test error logs */
    memset(&record, 0, sizeof(LogErrorHistory));

    record.error_id = 9002;
    strncpy(record.error_msg, "SQL syntax error", sizeof(record.error_msg) - 1);
    strncpy(record.error_query, "SLECT * FROM users", sizeof(record.error_query) - 1);
    strncpy(record.proc_name, "user_login", sizeof(record.proc_name) - 1);
    record.proc_line = 10;
    strncpy(record.query_user, "admin", sizeof(record.query_user) - 1);
    LogErrorHistory_Add(manager, &record);

    record.error_id = 9003;
    strncpy(record.error_msg, "Connection timeout", sizeof(record.error_msg) - 1);
    strncpy(record.error_query, "UPDATE accounts SET balance = 100", sizeof(record.error_query) - 1);
    strncpy(record.proc_name, "update_balance", sizeof(record.proc_name) - 1);
    record.proc_line = 25;
    strncpy(record.query_user, "system", sizeof(record.query_user) - 1);
    LogErrorHistory_Add(manager, &record);

    record.error_id = 9002;
    strncpy(record.error_msg, "Another SQL syntax error", sizeof(record.error_msg) - 1);
    strncpy(record.error_query, "DELETE FORM items", sizeof(record.error_query) - 1);
    strncpy(record.proc_name, "user_login", sizeof(record.proc_name) - 1);
    record.proc_line = 15;
    strncpy(record.query_user, "admin", sizeof(record.query_user) - 1);
    LogErrorHistory_Add(manager, &record);

    /* Test 6: Get recent errors */
    TEST_START("Get recent error logs");
    {
        LogErrorHistory records[10];
        int count = 0;

        if (LogErrorHistory_GetRecent(manager, records, 10, &count) == 0) {
            if (count >= 3) {
                TEST_PASS();
                printf("  Retrieved %d recent errors\n", count);
                for (int i = 0; i < count && i < 3; i++) {
                    printf("    [%d] Error %d: %s\n", i, records[i].error_id, records[i].error_msg);
                }
            } else {
                TEST_FAIL("Expected at least 3 records");
            }
        } else {
            TEST_FAIL("Failed to get recent errors");
        }
    }

    /* Test 7: Get by error ID */
    TEST_START("Get errors by error ID");
    {
        LogErrorHistory records[10];
        int count = 0;

        if (LogErrorHistory_GetByErrorId(manager, 9002, records, 10, &count) == 0) {
            if (count == 2) {
                TEST_PASS();
                printf("  Found %d errors with ID 9002\n", count);
            } else {
                TEST_FAIL("Expected 2 records with error_id 9002");
            }
        } else {
            TEST_FAIL("Failed to get errors by ID");
        }
    }

    /* Test 8: Get by procedure name */
    TEST_START("Get errors by procedure name");
    {
        LogErrorHistory records[10];
        int count = 0;

        if (LogErrorHistory_GetByProcName(manager, "user_login", records, 10, &count) == 0) {
            if (count == 2) {
                TEST_PASS();
                printf("  Found %d errors in user_login\n", count);
            } else {
                TEST_FAIL("Expected 2 records for user_login");
            }
        } else {
            TEST_FAIL("Failed to get errors by proc name");
        }
    }

    /* Test 9: Get by date range */
    TEST_START("Get errors by date range");
    {
        LogErrorHistory records[10];
        int count = 0;

        if (LogErrorHistory_GetByDateRange(manager, "2020-01-01", "2030-12-31",
                                           records, 10, &count) == 0) {
            if (count >= 3) {
                TEST_PASS();
                printf("  Found %d errors in date range\n", count);
            } else {
                TEST_FAIL("Expected at least 3 records");
            }
        } else {
            TEST_FAIL("Failed to get errors by date range");
        }
    }

    /* Test 10: Get total count */
    TEST_START("Get total error count");
    {
        int total_count = 0;

        if (LogErrorHistory_GetCount(manager, &total_count) == 0) {
            if (total_count >= 3) {
                TEST_PASS();
                printf("  Total errors: %d\n", total_count);
            } else {
                TEST_FAIL("Expected at least 3 records");
            }
        } else {
            TEST_FAIL("Failed to get total count");
        }
    }

    /* Test 11: Delete old errors */
    TEST_START("Delete old error logs");
    {
        int count_before = 0;
        int count_after = 0;

        LogErrorHistory_GetCount(manager, &count_before);

        /* Delete errors older than 2010 (should delete nothing from our test data) */
        if (LogErrorHistory_DeleteOlderThan(manager, "2010-01-01") == 0) {
            LogErrorHistory_GetCount(manager, &count_after);
            if (count_after == count_before) {
                TEST_PASS();
                printf("  Count before: %d, after: %d\n", count_before, count_after);
            } else {
                TEST_FAIL("Unexpected deletion");
            }
        } else {
            TEST_FAIL("Failed to delete old errors");
        }
    }
}
