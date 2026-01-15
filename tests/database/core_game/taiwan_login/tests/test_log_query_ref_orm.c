#include "log_query_ref_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Test statistics */
static int test_total = 0;
static int test_passed = 0;

/* Function prototypes */
void test_basic_crud(DBConnectionManager* manager);
void test_unique_constraint(DBConnectionManager* manager);
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

    printf("=== Log Query Ref ORM Test Suite ===\n\n");

    /* Clean up before test */
    cleanup_test_data(&manager);

    /* Run tests */
    test_basic_crud(&manager);
    test_unique_constraint(&manager);
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

    /* Delete all test records */
    snprintf(query, sizeof(query),
        "DELETE FROM log_query_ref WHERE query_hash LIKE 'testhash%%' OR query_hash LIKE 'uniquehash%%'");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    LogQueryRef record1, record2, fetched;
    int exists;

    printf("TEST 1: Add first query reference (AUTO_INCREMENT)\n");
    test_total++;
    memset(&record1, 0, sizeof(LogQueryRef));
    strncpy(record1.query_hash, "testhash001", sizeof(record1.query_hash) - 1);
    strncpy(record1.query, "SELECT * FROM users WHERE id = 1", sizeof(record1.query) - 1);
    /* Note: q_id is auto_increment, do not set it */

    if (LogQueryRef_Add(manager, &record1) == 0) {
        printf("  PASS\n");
        test_passed++;
    } else {
        printf("  FAIL: Failed to add record\n");
    }

    printf("TEST 2: Add second query reference\n");
    test_total++;
    memset(&record2, 0, sizeof(LogQueryRef));
    strncpy(record2.query_hash, "testhash002", sizeof(record2.query_hash) - 1);
    strncpy(record2.query, "UPDATE users SET name = test WHERE id = 2", sizeof(record2.query) - 1);

    if (LogQueryRef_Add(manager, &record2) == 0) {
        printf("  PASS\n");
        test_passed++;
    } else {
        printf("  FAIL: Failed to add record\n");
    }

    printf("TEST 3: Get query reference by hash (UNIQUE KEY)\n");
    test_total++;
    memset(&fetched, 0, sizeof(LogQueryRef));
    if (LogQueryRef_GetByHash(manager, "testhash001", &fetched) == 0) {
        if (strcmp(fetched.query_hash, "testhash001") == 0 &&
            strcmp(fetched.query, "SELECT * FROM users WHERE id = 1") == 0 &&
            fetched.q_id > 0) {  /* AUTO_INCREMENT should generate q_id > 0 */
            printf("  PASS (q_id=%u)\n", fetched.q_id);
            test_passed++;
        } else {
            printf("  FAIL: Data mismatch\n");
        }
    } else {
        printf("  FAIL: Failed to get record by hash\n");
    }

    printf("TEST 4: Get query reference by q_id (PRIMARY KEY)\n");
    test_total++;
    unsigned short q_id_to_fetch = fetched.q_id;
    memset(&fetched, 0, sizeof(LogQueryRef));
    if (LogQueryRef_Get(manager, q_id_to_fetch, &fetched) == 0) {
        if (fetched.q_id == q_id_to_fetch &&
            strcmp(fetched.query_hash, "testhash001") == 0) {
            printf("  PASS\n");
            test_passed++;
        } else {
            printf("  FAIL: Data mismatch\n");
        }
    } else {
        printf("  FAIL: Failed to get record by q_id\n");
    }

    printf("TEST 5: Check record exists by q_id\n");
    test_total++;
    exists = LogQueryRef_Exists(manager, q_id_to_fetch);
    if (exists == 1) {
        printf("  PASS\n");
        test_passed++;
    } else {
        printf("  FAIL: Record should exist (got %d)\n", exists);
    }

    printf("TEST 6: Check hash exists (UNIQUE KEY)\n");
    test_total++;
    exists = LogQueryRef_HashExists(manager, "testhash001");
    if (exists == 1) {
        printf("  PASS\n");
        test_passed++;
    } else {
        printf("  FAIL: Hash should exist (got %d)\n", exists);
    }

    printf("TEST 7: Update query reference\n");
    test_total++;
    strncpy(fetched.query, "SELECT * FROM users WHERE id = 999 -- UPDATED", sizeof(fetched.query) - 1);

    if (LogQueryRef_Update(manager, &fetched) == 0) {
        memset(&fetched, 0, sizeof(LogQueryRef));
        if (LogQueryRef_Get(manager, q_id_to_fetch, &fetched) == 0) {
            if (strstr(fetched.query, "UPDATED") != NULL) {
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

    printf("TEST 8: Delete query reference by q_id\n");
    test_total++;
    if (LogQueryRef_Delete(manager, q_id_to_fetch) == 0) {
        exists = LogQueryRef_Exists(manager, q_id_to_fetch);
        if (exists == 0) {
            printf("  PASS\n");
            test_passed++;
        } else {
            printf("  FAIL: Record still exists after delete\n");
        }
    } else {
        printf("  FAIL: Failed to delete record\n");
    }

    printf("TEST 9: Delete query reference by hash\n");
    test_total++;
    if (LogQueryRef_DeleteByHash(manager, "testhash002") == 0) {
        exists = LogQueryRef_HashExists(manager, "testhash002");
        if (exists == 0) {
            printf("  PASS\n");
            test_passed++;
        } else {
            printf("  FAIL: Hash still exists after delete\n");
        }
    } else {
        printf("  FAIL: Failed to delete record by hash\n");
    }
}

void test_unique_constraint(DBConnectionManager* manager) {
    LogQueryRef record;

    printf("TEST 10: Add record with unique hash\n");
    test_total++;
    memset(&record, 0, sizeof(LogQueryRef));
    strncpy(record.query_hash, "uniquehash001", sizeof(record.query_hash) - 1);
    strncpy(record.query, "SELECT 1", sizeof(record.query) - 1);

    if (LogQueryRef_Add(manager, &record) == 0) {
        printf("  PASS\n");
        test_passed++;
    } else {
        printf("  FAIL: Failed to add record\n");
    }

    printf("TEST 11: Try to add duplicate hash (should fail)\n");
    test_total++;
    memset(&record, 0, sizeof(LogQueryRef));
    strncpy(record.query_hash, "uniquehash001", sizeof(record.query_hash) - 1);
    strncpy(record.query, "SELECT 2", sizeof(record.query) - 1);

    if (LogQueryRef_Add(manager, &record) < 0) {
        printf("  PASS (duplicate rejected as expected)\n");
        test_passed++;
    } else {
        printf("  FAIL: Duplicate hash should be rejected\n");
    }
}

void test_business_operations(DBConnectionManager* manager) {
    LogQueryRef records[10];
    int actual_count;
    int total_count;

    /* Add test data */
    LogQueryRef test_records[3];
    memset(test_records, 0, sizeof(test_records));

    strncpy(test_records[0].query_hash, "testhash101", sizeof(test_records[0].query_hash) - 1);
    strncpy(test_records[0].query, "SELECT * FROM table1", sizeof(test_records[0].query) - 1);

    strncpy(test_records[1].query_hash, "testhash102", sizeof(test_records[1].query_hash) - 1);
    strncpy(test_records[1].query, "SELECT * FROM table2", sizeof(test_records[1].query) - 1);

    strncpy(test_records[2].query_hash, "testhash103", sizeof(test_records[2].query_hash) - 1);
    strncpy(test_records[2].query, "SELECT * FROM table3", sizeof(test_records[2].query) - 1);

    LogQueryRef_Add(manager, &test_records[0]);
    LogQueryRef_Add(manager, &test_records[1]);
    LogQueryRef_Add(manager, &test_records[2]);

    printf("TEST 12: Get all query references\n");
    test_total++;
    actual_count = 0;
    if (LogQueryRef_GetAll(manager, records, 10, &actual_count) == 0) {
        if (actual_count >= 3) {  /* At least 3 test records */
            printf("  PASS (found %d records)\n", actual_count);
            test_passed++;
        } else {
            printf("  FAIL: Expected at least 3 records, got %d\n", actual_count);
        }
    } else {
        printf("  FAIL: Failed to get records\n");
    }

    printf("TEST 13: Get latest query reference\n");
    test_total++;
    LogQueryRef latest;
    if (LogQueryRef_GetLatest(manager, &latest) == 0) {
        if (strcmp(latest.query_hash, "testhash103") == 0) {
            printf("  PASS (latest hash=%s, q_id=%u)\n", latest.query_hash, latest.q_id);
            test_passed++;
        } else {
            printf("  FAIL: Latest record mismatch\n");
        }
    } else {
        printf("  FAIL: Failed to get latest record\n");
    }

    printf("TEST 14: Count query references\n");
    test_total++;
    total_count = LogQueryRef_Count(manager);
    if (total_count >= 3) {
        printf("  PASS (total count=%d)\n", total_count);
        test_passed++;
    } else {
        printf("  FAIL: Expected at least 3, got %d\n", total_count);
    }

    printf("TEST 15: PrintInfo function\n");
    test_total++;
    if (actual_count > 0) {
        LogQueryRef_PrintInfo(&records[0]);
        printf("  PASS\n");
        test_passed++;
    } else {
        printf("  FAIL: No record to print\n");
    }
}
