#include "cash_cera_point_orm.h"
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

    printf("=== Cash Cera Point ORM Test Suite ===\n");

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
    char query[256];

    snprintf(query, sizeof(query),
        "DELETE FROM cash_cera_point WHERE account IN ('test_account_001', 'test_account_002', 'test_account_003')");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result);
    DBQueryResult_Free(&result);
}

void test_crud_operations(DBConnectionManager* manager) {
    CashCeraPoint record;
    int exists;

    printf("\n--- CRUD Operations Tests ---\n");

    /* Test 1: Add new record */
    TEST_START("Add new record");
    memset(&record, 0, sizeof(CashCeraPoint));
    strncpy(record.account, "test_account_001", sizeof(record.account) - 1);
    record.cera_point = 10000;
    /* Let database set timestamps with NOW() */

    if (CashCeraPoint_Add(manager, &record) == 0) {
        TEST_PASS();
    } else {
        TEST_FAIL("Failed to add record");
    }

    /* Test 2: Check existence */
    TEST_START("Check record existence");
    exists = CashCeraPoint_Exists(manager, "test_account_001");
    if (exists == 1) {
        TEST_PASS();
    } else {
        TEST_FAIL("Record should exist");
    }

    /* Test 3: Get record */
    TEST_START("Get record by account");
    memset(&record, 0, sizeof(CashCeraPoint));
    if (CashCeraPoint_Get(manager, "test_account_001", &record) == 0) {
        if (strcmp(record.account, "test_account_001") == 0 && record.cera_point == 10000) {
            TEST_PASS();
            printf("  Retrieved: account=%s, points=%u\n", record.account, record.cera_point);
        } else {
            TEST_FAIL("Data mismatch");
        }
    } else {
        TEST_FAIL("Failed to get record");
    }

    /* Test 4: Update record */
    TEST_START("Update record");
    record.cera_point = 15000;
    if (CashCeraPoint_Update(manager, &record) == 0) {
        CashCeraPoint updated;
        memset(&updated, 0, sizeof(CashCeraPoint));
        if (CashCeraPoint_Get(manager, "test_account_001", &updated) == 0 &&
            updated.cera_point == 15000) {
            TEST_PASS();
            printf("  Updated points: %u\n", updated.cera_point);
        } else {
            TEST_FAIL("Update not reflected");
        }
    } else {
        TEST_FAIL("Failed to update record");
    }

    /* Test 5: Delete record */
    TEST_START("Delete record");
    if (CashCeraPoint_Delete(manager, "test_account_001") == 0) {
        exists = CashCeraPoint_Exists(manager, "test_account_001");
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
    CashCeraPoint record;
    unsigned int balance;
    int result;

    printf("\n--- Business Operations Tests ---\n");

    /* Setup: Create test account */
    memset(&record, 0, sizeof(CashCeraPoint));
    strncpy(record.account, "test_account_002", sizeof(record.account) - 1);
    record.cera_point = 5000;
    CashCeraPoint_Add(manager, &record);

    /* Test 6: Get balance */
    TEST_START("Get balance");
    if (CashCeraPoint_GetBalance(manager, "test_account_002", &balance) == 0) {
        if (balance == 5000) {
            TEST_PASS();
            printf("  Balance: %u\n", balance);
        } else {
            TEST_FAIL("Balance mismatch");
        }
    } else {
        TEST_FAIL("Failed to get balance");
    }

    /* Test 7: Add points */
    TEST_START("Add points");
    if (CashCeraPoint_AddPoints(manager, "test_account_002", 2000) == 0) {
        if (CashCeraPoint_GetBalance(manager, "test_account_002", &balance) == 0 &&
            balance == 7000) {
            TEST_PASS();
            printf("  New balance: %u\n", balance);
        } else {
            TEST_FAIL("Points not added correctly");
        }
    } else {
        TEST_FAIL("Failed to add points");
    }

    /* Test 8: Deduct points (sufficient balance) */
    TEST_START("Deduct points (sufficient balance)");
    if (CashCeraPoint_DeductPoints(manager, "test_account_002", 3000) == 0) {
        if (CashCeraPoint_GetBalance(manager, "test_account_002", &balance) == 0 &&
            balance == 4000) {
            TEST_PASS();
            printf("  New balance: %u\n", balance);
        } else {
            TEST_FAIL("Points not deducted correctly");
        }
    } else {
        TEST_FAIL("Failed to deduct points");
    }

    /* Test 9: Deduct points (insufficient balance) */
    TEST_START("Deduct points (insufficient balance)");
    result = CashCeraPoint_DeductPoints(manager, "test_account_002", 10000);
    if (result < 0) {
        TEST_PASS();
        printf("  Correctly rejected: insufficient balance\n");
    } else {
        TEST_FAIL("Should reject insufficient balance");
    }

    /* Test 10: Get all records */
    TEST_START("Get all records");
    {
        CashCeraPoint records[10];
        int count = 0;

        /* Add another test record */
        memset(&record, 0, sizeof(CashCeraPoint));
        strncpy(record.account, "test_account_003", sizeof(record.account) - 1);
        record.cera_point = 8000;
        CashCeraPoint_Add(manager, &record);

        if (CashCeraPoint_GetAll(manager, records, 10, &count) == 0) {
            if (count >= 2) {
                TEST_PASS();
                printf("  Retrieved %d records\n", count);
                for (int i = 0; i < count && i < 5; i++) {
                    printf("    [%d] %s: %u points\n", i, records[i].account, records[i].cera_point);
                }
            } else {
                TEST_FAIL("Expected at least 2 records");
            }
        } else {
            TEST_FAIL("Failed to get all records");
        }
    }

    /* Test 11: Get by points range */
    TEST_START("Get records by points range");
    {
        CashCeraPoint records[10];
        int count = 0;

        if (CashCeraPoint_GetByPointsRange(manager, 3000, 9000, records, 10, &count) == 0) {
            if (count >= 2) {
                TEST_PASS();
                printf("  Found %d records in range [3000-9000]\n", count);
                for (int i = 0; i < count; i++) {
                    printf("    %s: %u points\n", records[i].account, records[i].cera_point);
                }
            } else {
                TEST_FAIL("Expected at least 2 records in range");
            }
        } else {
            TEST_FAIL("Failed to get records by range");
        }
    }
}
