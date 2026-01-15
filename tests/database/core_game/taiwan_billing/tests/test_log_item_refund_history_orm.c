#include "log_item_refund_history_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

    printf("=== Item Refund History Log ORM Test Suite ===\n");

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
    LogItemRefundHistory log;
    LogItemRefundHistory retrieved;
    unsigned int test_id = 0;

    TEST_START("Basic CRUD Operations");

    /* Prepare test data */
    memset(&log, 0, sizeof(LogItemRefundHistory));
    log.purchase_tran_id = 12345;
    log.recharge_tran_id = 67890;
    strncpy(log.account_id, "test_account_001", sizeof(log.account_id) - 1);
    strncpy(log.occ_date, "2025-11-18 10:00:00", sizeof(log.occ_date) - 1);
    strncpy(log.reason, "Test refund reason", sizeof(log.reason) - 1);
    strncpy(log.admin_id, "admin001", sizeof(log.admin_id) - 1);
    strncpy(log.query_user, "test_user", sizeof(log.query_user) - 1);

    /* Test Add */
    TEST_ASSERT(LogItemRefundHistory_Add(manager, &log) == 0,
                "Add new refund log");

    /* Get the inserted ID */
    DBQueryResult result;
    char query[512];
    char* row[1];

    snprintf(query, sizeof(query),
             "SELECT pf_rel_id FROM log_item_refund_history "
             "WHERE account_id = 'test_account_001' ORDER BY pf_rel_id DESC LIMIT 1");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) == 0) {
        if (DBQueryResult_FetchRow(&result, row) > 0) {
            test_id = row[0] ? (unsigned int)atoi(row[0]) : 0;
        }
        DBQueryResult_Free(&result);
    }

    /* Test Get */
    memset(&retrieved, 0, sizeof(LogItemRefundHistory));
    TEST_ASSERT(LogItemRefundHistory_Get(manager, test_id, &retrieved) == 0,
                "Get refund log");
    TEST_ASSERT(retrieved.purchase_tran_id == 12345, "Verify purchase_tran_id");
    TEST_ASSERT(retrieved.recharge_tran_id == 67890, "Verify recharge_tran_id");
    TEST_ASSERT(strcmp(retrieved.account_id, "test_account_001") == 0,
                "Verify account_id");

    /* Test Exists */
    TEST_ASSERT(LogItemRefundHistory_Exists(manager, test_id) == 1,
                "Check log exists");

    /* Test Delete */
    TEST_ASSERT(LogItemRefundHistory_Delete(manager, test_id) == 0,
                "Delete log record");
    TEST_ASSERT(LogItemRefundHistory_Exists(manager, test_id) == 0,
                "Verify deletion");
}

void test_business_operations(DBConnectionManager* manager) {
    LogItemRefundHistory log1, log2, log3;
    LogItemRefundHistory results[20];
    int count = 0;

    TEST_START("Business Operations");

    /* Add multiple test records */
    memset(&log1, 0, sizeof(LogItemRefundHistory));
    log1.purchase_tran_id = 100001;
    log1.recharge_tran_id = 200001;
    strncpy(log1.account_id, "test_acc_01", sizeof(log1.account_id) - 1);
    strncpy(log1.occ_date, "2025-11-17 10:00:00", sizeof(log1.occ_date) - 1);
    strncpy(log1.reason, "Duplicate item", sizeof(log1.reason) - 1);
    strncpy(log1.admin_id, "gm001", sizeof(log1.admin_id) - 1);
    strncpy(log1.query_user, "user001", sizeof(log1.query_user) - 1);
    LogItemRefundHistory_Add(manager, &log1);

    memset(&log2, 0, sizeof(LogItemRefundHistory));
    log2.purchase_tran_id = 100002;
    log2.recharge_tran_id = 200002;
    strncpy(log2.account_id, "test_acc_01", sizeof(log2.account_id) - 1);
    strncpy(log2.occ_date, "2025-11-18 11:00:00", sizeof(log2.occ_date) - 1);
    strncpy(log2.reason, "Wrong purchase", sizeof(log2.reason) - 1);
    strncpy(log2.admin_id, "gm001", sizeof(log2.admin_id) - 1);
    strncpy(log2.query_user, "user002", sizeof(log2.query_user) - 1);
    LogItemRefundHistory_Add(manager, &log2);

    memset(&log3, 0, sizeof(LogItemRefundHistory));
    log3.purchase_tran_id = 100003;
    log3.recharge_tran_id = 200003;
    strncpy(log3.account_id, "test_acc_02", sizeof(log3.account_id) - 1);
    strncpy(log3.occ_date, "2025-11-18 12:00:00", sizeof(log3.occ_date) - 1);
    strncpy(log3.reason, "System error", sizeof(log3.reason) - 1);
    strncpy(log3.admin_id, "gm002", sizeof(log3.admin_id) - 1);
    strncpy(log3.query_user, "user003", sizeof(log3.query_user) - 1);
    LogItemRefundHistory_Add(manager, &log3);

    /* Test GetByAccountId */
    count = 0;
    memset(results, 0, sizeof(results));
    TEST_ASSERT(LogItemRefundHistory_GetByAccountId(manager, "test_acc_01",
                                                     results, 20, &count) == 0,
                "Get logs by account ID");
    TEST_ASSERT(count == 2, "Verify count for account test_acc_01");

    /* Test GetByPurchaseTranId */
    count = 0;
    memset(results, 0, sizeof(results));
    TEST_ASSERT(LogItemRefundHistory_GetByPurchaseTranId(manager, 100002,
                                                          results, 20, &count) == 0,
                "Get logs by purchase transaction ID");
    TEST_ASSERT(count >= 1, "Verify purchase tran ID query");

    /* Test GetByRechargeTranId */
    count = 0;
    memset(results, 0, sizeof(results));
    TEST_ASSERT(LogItemRefundHistory_GetByRechargeTranId(manager, 200003,
                                                          results, 20, &count) == 0,
                "Get logs by recharge transaction ID");
    TEST_ASSERT(count >= 1, "Verify recharge tran ID query");

    /* Test GetByDateRange */
    count = 0;
    memset(results, 0, sizeof(results));
    TEST_ASSERT(LogItemRefundHistory_GetByDateRange(manager,
                                                     "2025-11-17 00:00:00",
                                                     "2025-11-18 23:59:59",
                                                     results, 20, &count) == 0,
                "Get logs by date range");
    TEST_ASSERT(count >= 3, "Verify date range query");
    printf("  [INFO] Logs in date range: %d\n", count);

    /* Test GetByAdmin */
    count = 0;
    memset(results, 0, sizeof(results));
    TEST_ASSERT(LogItemRefundHistory_GetByAdmin(manager, "gm001",
                                                 results, 20, &count) == 0,
                "Get logs by admin ID");
    TEST_ASSERT(count == 2, "Verify admin query count");

    /* Test GetRecent */
    count = 0;
    memset(results, 0, sizeof(results));
    TEST_ASSERT(LogItemRefundHistory_GetRecent(manager, 10, results, &count) == 0,
                "Get recent logs");
    TEST_ASSERT(count >= 3, "Verify recent logs count");
    printf("  [INFO] Recent logs count: %d\n", count);

    /* Test PrintInfo */
    if (count > 0) {
        printf("\n  [INFO] Sample refund log:\n");
        LogItemRefundHistory_PrintInfo(&results[0]);
    }
}

void cleanup_test_data(DBConnectionManager* manager) {
    DBQueryResult result;
    char query[1024];

    /* Delete test records by account ID pattern */
    snprintf(query, sizeof(query),
        "DELETE FROM log_item_refund_history "
        "WHERE account_id LIKE 'test_%%'");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result);
    DBQueryResult_Free(&result);
}
