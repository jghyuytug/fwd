#include "log_transaction_history_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* IMPORTANT: NO Chinese characters in test files! */

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

    printf("=== Log Transaction History ORM Test Suite ===\n\n");

    /* Clean up before test */
    cleanup_test_data(&manager);

    /* Run tests */
    test_basic_crud(&manager);
    test_business_operations(&manager);

    /* Clean up after test */
    cleanup_test_data(&manager);

    /* Release resources */
    DBConnectionManager_Cleanup(&manager);
    printf("\n=== All Tests Completed ===\n");
    return 0;
}

void cleanup_test_data(DBConnectionManager* manager) {
    DBQueryResult result;
    char query[512];

    snprintf(query, sizeof(query),
        "DELETE FROM log_transaction_history WHERE tran_id IN (1000000001, 2000000001, 3000000001)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    LogTransactionHistory log;
    int ret;

    printf("[Test 1] Basic CRUD Operations\n");
    printf("--------------------------------\n");

    /* Test 1.1: Add new transaction log */
    printf("1.1 Adding transaction log... ");
    memset(&log, 0, sizeof(LogTransactionHistory));
    log.tran_id = 1000000001ULL;
    log.tran_type = 1;
    strcpy(log.occ_date, "2025-11-18 10:00:00");

    ret = LogTransactionHistory_Add(manager, &log);
    if (ret == 0) {
        printf("OK\n");
    } else {
        printf("FAILED\n");
    }

    /* Test 1.2: Get transaction log */
    printf("1.2 Getting transaction log... ");
    memset(&log, 0, sizeof(LogTransactionHistory));
    ret = LogTransactionHistory_Get(manager, 1000000001ULL, &log);
    if (ret == 0 && log.tran_type == 1) {
        printf("OK (type=%u)\n", log.tran_type);
    } else {
        printf("FAILED\n");
    }

    /* Test 1.3: Check existence */
    printf("1.3 Checking existence... ");
    ret = LogTransactionHistory_Exists(manager, 1000000001ULL);
    if (ret == 1) {
        printf("OK (exists)\n");
    } else {
        printf("FAILED\n");
    }

    /* Test 1.4: Update transaction log */
    printf("1.4 Updating transaction log (type=2)... ");
    log.tran_type = 2;
    strcpy(log.occ_date, "2025-11-18 11:00:00");
    ret = LogTransactionHistory_Update(manager, &log);
    if (ret == 0) {
        printf("OK\n");
    } else {
        printf("FAILED\n");
    }

    /* Test 1.5: Verify update */
    printf("1.5 Verifying update... ");
    memset(&log, 0, sizeof(LogTransactionHistory));
    ret = LogTransactionHistory_Get(manager, 1000000001ULL, &log);
    if (ret == 0 && log.tran_type == 2) {
        printf("OK (type=%u)\n", log.tran_type);
    } else {
        printf("FAILED\n");
    }

    /* Test 1.6: Print info */
    printf("1.6 Printing transaction log info... ");
    LogTransactionHistory_PrintInfo(&log);
    printf("OK\n");

    /* Test 1.7: Delete transaction log */
    printf("1.7 Deleting transaction log... ");
    ret = LogTransactionHistory_Delete(manager, 1000000001ULL);
    if (ret == 0) {
        printf("OK\n");
    } else {
        printf("FAILED\n");
    }

    /* Test 1.8: Verify deletion */
    printf("1.8 Verifying deletion... ");
    ret = LogTransactionHistory_Exists(manager, 1000000001ULL);
    if (ret == 0) {
        printf("OK (not exists)\n");
    } else {
        printf("FAILED\n");
    }

    printf("\n");
}

void test_business_operations(DBConnectionManager* manager) {
    LogTransactionHistory log;
    LogTransactionHistory logs[10];
    int actual_count = 0;
    int ret;

    printf("[Test 2] Business Operations\n");
    printf("-----------------------------\n");

    /* Test 2.1: Add multiple transaction logs */
    printf("2.1 Adding multiple transaction logs... ");

    memset(&log, 0, sizeof(LogTransactionHistory));
    log.tran_id = 1000000001ULL;
    log.tran_type = 1;
    strcpy(log.occ_date, "2025-11-18 09:00:00");
    LogTransactionHistory_Add(manager, &log);

    log.tran_id = 2000000001ULL;
    log.tran_type = 2;
    strcpy(log.occ_date, "2025-11-18 10:00:00");
    LogTransactionHistory_Add(manager, &log);

    log.tran_id = 3000000001ULL;
    log.tran_type = 1;
    strcpy(log.occ_date, "2025-11-18 11:00:00");
    LogTransactionHistory_Add(manager, &log);

    printf("OK (added 3 logs)\n");

    /* Test 2.2: Get all transaction logs */
    printf("2.2 Getting all transaction logs... ");
    ret = LogTransactionHistory_GetAll(manager, logs, 10, &actual_count);
    if (ret == 0 && actual_count == 3) {
        printf("OK (found %d logs)\n", actual_count);
        for (int i = 0; i < actual_count; i++) {
            printf("    [%d] tran_id=%llu, type=%u, date=%s\n",
                   i + 1, logs[i].tran_id, logs[i].tran_type, logs[i].occ_date);
        }
    } else {
        printf("FAILED (expected 3, got %d)\n", actual_count);
    }

    /* Test 2.3: Get by transaction type */
    printf("2.3 Getting by transaction type (type=1)... ");
    ret = LogTransactionHistory_GetByType(manager, 1, logs, 10, &actual_count);
    if (ret == 0 && actual_count == 2) {
        printf("OK (found %d logs)\n", actual_count);
    } else {
        printf("FAILED (expected 2, got %d)\n", actual_count);
    }

    /* Test 2.4: Get by date range */
    printf("2.4 Getting by date range (2025-11-18 00:00 to 23:59)... ");
    ret = LogTransactionHistory_GetByDateRange(manager, "2025-11-18 00:00:00",
                                                 "2025-11-18 23:59:59", logs, 10, &actual_count);
    if (ret == 0 && actual_count == 3) {
        printf("OK (found %d logs)\n", actual_count);
    } else {
        printf("FAILED (expected 3, got %d)\n", actual_count);
    }

    printf("\n");
}
