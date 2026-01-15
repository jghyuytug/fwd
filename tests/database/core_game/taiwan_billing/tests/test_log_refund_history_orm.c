#include "log_refund_history_orm.h"
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

    printf("=== Log Refund History ORM Test Suite ===\n\n");

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
        "DELETE FROM log_refund_history WHERE account_id IN "
        "('testaccount001', 'testaccount002', 'testaccount003')");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    LogRefundHistory refund;
    int ret;

    printf("[Test 1] Basic CRUD Operations\n");
    printf("--------------------------------\n");

    /* Test 1.1: Add new refund record (composite primary key) */
    printf("1.1 Adding refund record (composite key)... ");
    memset(&refund, 0, sizeof(LogRefundHistory));
    refund.tran_id = 1000000001ULL;
    strcpy(refund.account_id, "testaccount001");
    strcpy(refund.order_tran_id, "ORD2025111800001");
    refund.amount = 5000;
    refund.tran_state = 1;
    strcpy(refund.query_user, "admin001");
    strcpy(refund.occ_date, "2025-11-18 10:00:00");

    ret = LogRefundHistory_Add(manager, &refund);
    if (ret == 0) {
        printf("OK\n");
    } else {
        printf("FAILED\n");
    }

    /* Test 1.2: Get refund record using composite primary key */
    printf("1.2 Getting refund record (composite key)... ");
    memset(&refund, 0, sizeof(LogRefundHistory));
    ret = LogRefundHistory_Get(manager, 1000000001ULL, "testaccount001", &refund);
    if (ret == 0 && refund.amount == 5000) {
        printf("OK (amount=%u)\n", refund.amount);
    } else {
        printf("FAILED\n");
    }

    /* Test 1.3: Check if refund record exists */
    printf("1.3 Checking refund existence... ");
    ret = LogRefundHistory_Exists(manager, 1000000001ULL, "testaccount001");
    if (ret == 1) {
        printf("OK (exists)\n");
    } else {
        printf("FAILED\n");
    }

    /* Test 1.4: Update refund record */
    printf("1.4 Updating refund record (state=2, amount=6000)... ");
    refund.tran_state = 2;
    refund.amount = 6000;
    ret = LogRefundHistory_Update(manager, &refund);
    if (ret == 0) {
        printf("OK\n");
    } else {
        printf("FAILED\n");
    }

    /* Test 1.5: Verify update */
    printf("1.5 Verifying update... ");
    memset(&refund, 0, sizeof(LogRefundHistory));
    ret = LogRefundHistory_Get(manager, 1000000001ULL, "testaccount001", &refund);
    if (ret == 0 && refund.tran_state == 2 && refund.amount == 6000) {
        printf("OK (state=%u, amount=%u)\n", refund.tran_state, refund.amount);
    } else {
        printf("FAILED\n");
    }

    /* Test 1.6: Print refund info */
    printf("1.6 Printing refund info... ");
    LogRefundHistory_PrintInfo(&refund);
    printf("OK\n");

    /* Test 1.7: Delete refund record (composite key) */
    printf("1.7 Deleting refund record... ");
    ret = LogRefundHistory_Delete(manager, 1000000001ULL, "testaccount001");
    if (ret == 0) {
        printf("OK\n");
    } else {
        printf("FAILED\n");
    }

    /* Test 1.8: Verify deletion */
    printf("1.8 Verifying deletion... ");
    ret = LogRefundHistory_Exists(manager, 1000000001ULL, "testaccount001");
    if (ret == 0) {
        printf("OK (not exists)\n");
    } else {
        printf("FAILED\n");
    }

    printf("\n");
}

void test_business_operations(DBConnectionManager* manager) {
    LogRefundHistory refund;
    LogRefundHistory refunds[10];
    int actual_count = 0;
    int ret;

    printf("[Test 2] Business Operations\n");
    printf("-----------------------------\n");

    /* Test 2.1: Add multiple refund records */
    printf("2.1 Adding multiple refund records... ");

    memset(&refund, 0, sizeof(LogRefundHistory));
    refund.tran_id = 2000000001ULL;
    strcpy(refund.account_id, "testaccount001");
    strcpy(refund.order_tran_id, "ORD2025111800101");
    refund.amount = 1000;
    refund.tran_state = 1;
    strcpy(refund.query_user, "admin001");
    strcpy(refund.occ_date, "2025-11-18 09:00:00");
    LogRefundHistory_Add(manager, &refund);

    refund.tran_id = 2000000002ULL;
    strcpy(refund.account_id, "testaccount002");
    strcpy(refund.order_tran_id, "ORD2025111800102");
    refund.amount = 2000;
    refund.tran_state = 2;
    strcpy(refund.occ_date, "2025-11-18 10:00:00");
    LogRefundHistory_Add(manager, &refund);

    refund.tran_id = 2000000003ULL;
    strcpy(refund.account_id, "testaccount003");
    strcpy(refund.order_tran_id, "ORD2025111800103");
    refund.amount = 3000;
    refund.tran_state = 1;
    strcpy(refund.occ_date, "2025-11-18 11:00:00");
    LogRefundHistory_Add(manager, &refund);

    printf("OK (added 3 refunds)\n");

    /* Test 2.2: Get all refund records */
    printf("2.2 Getting all refund records... ");
    ret = LogRefundHistory_GetAll(manager, refunds, 10, &actual_count);
    if (ret == 0 && actual_count == 3) {
        printf("OK (found %d refunds)\n", actual_count);
        for (int i = 0; i < actual_count; i++) {
            printf("    [%d] tran_id=%llu, account=%s, state=%u, amount=%u\n",
                   i + 1, refunds[i].tran_id, refunds[i].account_id,
                   refunds[i].tran_state, refunds[i].amount);
        }
    } else {
        printf("FAILED (expected 3, got %d)\n", actual_count);
    }

    /* Test 2.3: Get by account */
    printf("2.3 Getting by account (testaccount001)... ");
    ret = LogRefundHistory_GetByAccount(manager, "testaccount001", refunds, 10, &actual_count);
    if (ret == 0 && actual_count == 1) {
        printf("OK (found %d refunds)\n", actual_count);
    } else {
        printf("FAILED (expected 1, got %d)\n", actual_count);
    }

    /* Test 2.4: Get by state (state=1) */
    printf("2.4 Getting by state (state=1)... ");
    ret = LogRefundHistory_GetByState(manager, 1, refunds, 10, &actual_count);
    if (ret == 0 && actual_count == 2) {
        printf("OK (found %d refunds)\n", actual_count);
    } else {
        printf("FAILED (expected 2, got %d)\n", actual_count);
    }

    /* Test 2.5: Get by date range */
    printf("2.5 Getting by date range (2025-11-18 00:00 to 23:59)... ");
    ret = LogRefundHistory_GetByDateRange(manager, "2025-11-18 00:00:00",
                                           "2025-11-18 23:59:59", refunds, 10, &actual_count);
    if (ret == 0 && actual_count == 3) {
        printf("OK (found %d refunds)\n", actual_count);
    } else {
        printf("FAILED (expected 3, got %d)\n", actual_count);
    }

    printf("\n");
}
