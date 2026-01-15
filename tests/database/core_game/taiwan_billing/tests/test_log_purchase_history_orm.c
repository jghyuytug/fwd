#include "log_purchase_history_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

/* Test counters */
static int test_total = 0;
static int test_passed = 0;

#define TEST_START(name) do { printf("\n[TEST] %s\n", name); test_total++; } while(0)
#define TEST_PASS() do { printf("  PASS\n"); test_passed++; } while(0)
#define TEST_FAIL(msg) do { printf("  FAIL: %s\n", msg); } while(0)

void cleanup_test_data(DBConnectionManager* manager);
void test_crud_operations(DBConnectionManager* manager);
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

    if (DBConnectionManager_Connect(&manager, DB_TYPE_BILLING) < 0) {
        printf("Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("=== Log Purchase History ORM Test Suite ===\n");

    cleanup_test_data(&manager);
    test_crud_operations(&manager);
    test_business_operations(&manager);
    cleanup_test_data(&manager);

    DBConnectionManager_Cleanup(&manager);

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
        "DELETE FROM log_purchase_history WHERE tran_id BETWEEN 1000000001 AND 1000000010");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result);
    DBQueryResult_Free(&result);
}

void test_crud_operations(DBConnectionManager* manager) {
    LogPurchaseHistory record;
    int exists;

    printf("\n--- CRUD Operations Tests ---\n");

    /* Test 1: Add purchase record */
    TEST_START("Add purchase record");
    memset(&record, 0, sizeof(LogPurchaseHistory));
    record.tran_id = 1000000001;
    record.tran_state = 1;
    strncpy(record.account_id, "test_account_001", sizeof(record.account_id) - 1);
    strncpy(record.charac_id, "test_char_001", sizeof(record.charac_id) - 1);
    record.item_id = 12345;
    record.cera = 5000;
    record.befor_cera = 10000;
    record.after_cera = 5000;
    strncpy(record.query_user, "test_user", sizeof(record.query_user) - 1);

    if (LogPurchaseHistory_Add(manager, &record) == 0) {
        TEST_PASS();
    } else {
        TEST_FAIL("Failed to add purchase record");
    }

    /* Test 2: Check existence */
    TEST_START("Check record existence");
    exists = LogPurchaseHistory_Exists(manager, 1000000001);
    if (exists == 1) {
        TEST_PASS();
    } else {
        TEST_FAIL("Record should exist");
    }

    /* Test 3: Get record */
    TEST_START("Get record by tran_id");
    {
        LogPurchaseHistory retrieved;
        memset(&retrieved, 0, sizeof(LogPurchaseHistory));

        if (LogPurchaseHistory_Get(manager, 1000000001, &retrieved) == 0) {
            if (retrieved.tran_id == 1000000001 && retrieved.item_id == 12345) {
                TEST_PASS();
                printf("  Tran ID: %"PRIu64", Item: %u, Cera: %u\n",
                       retrieved.tran_id, retrieved.item_id, retrieved.cera);
            } else {
                TEST_FAIL("Data mismatch");
            }
        } else {
            TEST_FAIL("Failed to get record");
        }
    }

    /* Test 4: Delete record */
    TEST_START("Delete record");
    if (LogPurchaseHistory_Delete(manager, 1000000001) == 0) {
        exists = LogPurchaseHistory_Exists(manager, 1000000001);
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
    LogPurchaseHistory record;

    printf("\n--- Business Operations Tests ---\n");

    /* Setup: Create test records */
    memset(&record, 0, sizeof(LogPurchaseHistory));

    record.tran_id = 1000000002;
    record.tran_state = 1;
    strncpy(record.account_id, "acc_001", sizeof(record.account_id) - 1);
    strncpy(record.charac_id, "char_001", sizeof(record.charac_id) - 1);
    record.item_id = 100;
    record.cera = 1000;
    record.befor_cera = 10000;
    record.after_cera = 9000;
    strncpy(record.query_user, "admin", sizeof(record.query_user) - 1);
    LogPurchaseHistory_Add(manager, &record);

    record.tran_id = 1000000003;
    record.item_id = 200;
    record.cera = 2000;
    record.befor_cera = 9000;
    record.after_cera = 7000;
    LogPurchaseHistory_Add(manager, &record);

    record.tran_id = 1000000004;
    strncpy(record.account_id, "acc_002", sizeof(record.account_id) - 1);
    record.item_id = 100;
    record.cera = 1500;
    LogPurchaseHistory_Add(manager, &record);

    /* Test 5: Get by account */
    TEST_START("Get purchases by account");
    {
        LogPurchaseHistory records[10];
        int count = 0;

        if (LogPurchaseHistory_GetByAccount(manager, "acc_001", records, 10, &count) == 0) {
            if (count == 2) {
                TEST_PASS();
                printf("  Found %d purchases for acc_001\n", count);
            } else {
                TEST_FAIL("Expected 2 records");
            }
        } else {
            TEST_FAIL("Failed to get by account");
        }
    }

    /* Test 6: Get by item */
    TEST_START("Get purchases by item ID");
    {
        LogPurchaseHistory records[10];
        int count = 0;

        if (LogPurchaseHistory_GetByItem(manager, 100, records, 10, &count) == 0) {
            if (count == 2) {
                TEST_PASS();
                printf("  Found %d purchases of item 100\n", count);
            } else {
                TEST_FAIL("Expected 2 records");
            }
        } else {
            TEST_FAIL("Failed to get by item");
        }
    }

    /* Test 7: Get total spent */
    TEST_START("Calculate total cera spent");
    {
        unsigned int total = 0;

        if (LogPurchaseHistory_GetTotalSpent(manager, "acc_001", &total) == 0) {
            if (total == 3000) {
                TEST_PASS();
                printf("  Total spent by acc_001: %u cera\n", total);
            } else {
                TEST_FAIL("Expected 3000 cera");
            }
        } else {
            TEST_FAIL("Failed to calculate total");
        }
    }

    /* Test 8: Get recent records */
    TEST_START("Get recent purchase records");
    {
        LogPurchaseHistory records[10];
        int count = 0;

        if (LogPurchaseHistory_GetRecent(manager, records, 10, &count) == 0) {
            if (count >= 3) {
                TEST_PASS();
                printf("  Retrieved %d recent purchases\n", count);
            } else {
                TEST_FAIL("Expected at least 3 records");
            }
        } else {
            TEST_FAIL("Failed to get recent records");
        }
    }

    /* Test 9: Get total count */
    TEST_START("Get total purchase count");
    {
        int total_count = 0;

        if (LogPurchaseHistory_GetCount(manager, &total_count) == 0) {
            if (total_count >= 3) {
                TEST_PASS();
                printf("  Total purchases: %d\n", total_count);
            } else {
                TEST_FAIL("Expected at least 3 records");
            }
        } else {
            TEST_FAIL("Failed to get total count");
        }
    }
}
