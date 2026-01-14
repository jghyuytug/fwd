#include "cash_transaction_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* IMPORTANT: NO Chinese characters in test files! */

/* Test counters */
static int test_total = 0;
static int test_passed = 0;

/* Test helper macros */
#define TEST_START(name) \
    do { \
        printf("\n[TEST] %s\n", name); \
        test_total++; \
    } while(0)

#define TEST_ASSERT(condition, message) \
    do { \
        if (condition) { \
            printf("  [PASS] %s\n", message); \
            test_passed++; \
        } else { \
            printf("  [FAIL] %s\n", message); \
        } \
    } while(0)

/* Forward declarations */
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
    if (DBConnectionManager_Connect(&manager, DB_TYPE_BILLING) < 0) {
        printf("Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("=== Cash Transaction ORM Test Suite ===\n");

    /* Clean up before test */
    cleanup_test_data(&manager);

    /* Run tests */
    test_basic_crud(&manager);
    test_business_operations(&manager);

    /* Clean up after test */
    cleanup_test_data(&manager);

    /* Release resources */
    DBConnectionManager_Cleanup(&manager);

    printf("\n=== Test Results ===\n");
    printf("Total: %d, Passed: %d, Failed: %d\n",
           test_total, test_passed, test_total - test_passed);
    printf("=== All Tests Completed ===\n");

    return (test_total == test_passed) ? 0 : 1;
}

void test_basic_crud(DBConnectionManager* manager) {
    CashTransaction transaction;
    CashTransaction retrieved;
    long long test_id = 0;

    TEST_START("Basic CRUD Operations");

    /* Prepare test data */
    memset(&transaction, 0, sizeof(CashTransaction));
    transaction.dummy = 'A';

    /* Test Add */
    TEST_ASSERT(CashTransaction_Add(manager, &transaction) == 0,
                "Add new transaction record");

    /* Get the inserted ID - query last inserted */
    DBQueryResult result;
    char query[256];
    char* row[1];

    snprintf(query, sizeof(query),
             "SELECT tran_id FROM cash_transaction WHERE dummy = 'A' "
             "ORDER BY tran_id DESC LIMIT 1");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) == 0) {
        if (DBQueryResult_FetchRow(&result, row) > 0) {
            test_id = row[0] ? atoll(row[0]) : 0;
        }
        DBQueryResult_Free(&result);
    }

    /* Test Get */
    memset(&retrieved, 0, sizeof(CashTransaction));
    TEST_ASSERT(CashTransaction_Get(manager, test_id, &retrieved) == 0,
                "Get transaction record");
    TEST_ASSERT(retrieved.tran_id == test_id, "Verify tran_id");
    TEST_ASSERT(retrieved.dummy == 'A', "Verify dummy field");

    /* Test Exists */
    TEST_ASSERT(CashTransaction_Exists(manager, test_id) == 1,
                "Check transaction exists");

    /* Test Delete */
    TEST_ASSERT(CashTransaction_Delete(manager, test_id) == 0,
                "Delete transaction record");
    TEST_ASSERT(CashTransaction_Exists(manager, test_id) == 0,
                "Verify deletion");
}

void test_business_operations(DBConnectionManager* manager) {
    CashTransaction trans1, trans2, trans3;
    CashTransaction results[10];
    long long next_id = 0;
    long long latest_id = 0;
    int count = 0;

    TEST_START("Business Operations");

    /* Add multiple test records */
    memset(&trans1, 0, sizeof(CashTransaction));
    trans1.dummy = 'X';
    CashTransaction_Add(manager, &trans1);

    memset(&trans2, 0, sizeof(CashTransaction));
    trans2.dummy = 'Y';
    CashTransaction_Add(manager, &trans2);

    memset(&trans3, 0, sizeof(CashTransaction));
    trans3.dummy = 'Z';
    CashTransaction_Add(manager, &trans3);

    /* Test GetNextTranId */
    TEST_ASSERT(CashTransaction_GetNextTranId(manager, &next_id) == 0,
                "Get next transaction ID");
    TEST_ASSERT(next_id > 0, "Next ID should be positive");
    printf("  [INFO] Next transaction ID: %lld\n", next_id);

    /* Test GetLatestTranId */
    TEST_ASSERT(CashTransaction_GetLatestTranId(manager, &latest_id) == 0,
                "Get latest transaction ID");
    TEST_ASSERT(latest_id > 0, "Latest ID should be positive");
    TEST_ASSERT(next_id == latest_id + 1, "Next ID should be latest + 1");
    printf("  [INFO] Latest transaction ID: %lld\n", latest_id);

    /* Test GetAll */
    count = 0;
    memset(results, 0, sizeof(results));
    TEST_ASSERT(CashTransaction_GetAll(manager, results, 10, &count) == 0,
                "Get all transactions");
    TEST_ASSERT(count >= 3, "Should have at least 3 test records");
    printf("  [INFO] Total transactions in DB: %d\n", count);

    /* Get the IDs of our test records */
    long long test_ids[3] = {0, 0, 0};
    int test_idx = 0;

    DBQueryResult result;
    char query[256];
    char* row[1];

    snprintf(query, sizeof(query),
             "SELECT tran_id FROM cash_transaction WHERE dummy IN ('X', 'Y', 'Z') "
             "ORDER BY tran_id");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) == 0) {
        while (test_idx < 3 && DBQueryResult_FetchRow(&result, row) > 0) {
            test_ids[test_idx++] = row[0] ? atoll(row[0]) : 0;
        }
        DBQueryResult_Free(&result);
    }

    /* Test GetRange */
    if (test_ids[0] > 0 && test_ids[2] > 0) {
        count = 0;
        memset(results, 0, sizeof(results));
        TEST_ASSERT(CashTransaction_GetRange(manager, test_ids[0], test_ids[2],
                                             results, 10, &count) == 0,
                    "Get transactions in range");
        TEST_ASSERT(count >= 3, "Should get at least 3 records in range");
        printf("  [INFO] Records in range [%lld, %lld]: %d\n",
               test_ids[0], test_ids[2], count);
    }

    /* Test PrintInfo */
    if (count > 0) {
        printf("\n  [INFO] Sample transaction record:\n");
        CashTransaction_PrintInfo(&results[0]);
    }
}

void cleanup_test_data(DBConnectionManager* manager) {
    DBQueryResult result;
    char query[512];

    /* Delete test records by dummy values */
    snprintf(query, sizeof(query),
        "DELETE FROM cash_transaction WHERE dummy IN ('A', 'X', 'Y', 'Z')");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result);
    DBQueryResult_Free(&result);
}
