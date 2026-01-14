#include "log_gift_history_orm.h"
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

    printf("=== Log Gift History ORM Test Suite ===\n\n");

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
        "DELETE FROM log_gift_history WHERE tran_id IN (100001, 100002, 100003, 200001, 200002)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    LogGiftHistory log;
    int ret;

    printf("[Test 1] Basic CRUD Operations\n");
    printf("--------------------------------\n");

    /* Test 1.1: Add new gift log */
    printf("1.1 Adding gift log (tran_id=100001)... ");
    memset(&log, 0, sizeof(LogGiftHistory));
    log.tran_id = 100001;
    log.tran_state = 1;
    strcpy(log.send_account_id, "sender001");
    strcpy(log.send_charac_id, "char001");
    strcpy(log.recv_account_id, "receiver001");
    log.item_id = 12345;
    log.cera = 1000;
    log.send_befor_cera = 5000;
    log.send_after_cera = 4000;
    log.recv_befor_cera = 2000;
    log.recv_after_cera = 3000;
    strcpy(log.query_user, "admin");
    strcpy(log.occ_date, "2025-11-18 10:00:00");

    ret = LogGiftHistory_Add(manager, &log);
    if (ret == 0) {
        printf("OK\n");
    } else {
        printf("FAILED\n");
    }

    /* Test 1.2: Check if log exists */
    printf("1.2 Checking if log exists... ");
    ret = LogGiftHistory_Exists(manager, 100001);
    if (ret == 1) {
        printf("OK (exists)\n");
    } else {
        printf("FAILED (not found)\n");
    }

    /* Test 1.3: Get log */
    printf("1.3 Getting log (tran_id=100001)... ");
    memset(&log, 0, sizeof(LogGiftHistory));
    ret = LogGiftHistory_Get(manager, 100001, &log);
    if (ret == 0 && log.tran_id == 100001) {
        printf("OK\n");
        printf("    ");
        printf("Gift: %s -> %s, item=%u, cera=%u\n",
               log.send_account_id, log.recv_account_id, log.item_id, log.cera);
    } else {
        printf("FAILED\n");
    }

    /* Test 1.4: Update log */
    printf("1.4 Updating log (tran_id=100001)... ");
    log.tran_state = 2;
    log.cera = 1500;
    ret = LogGiftHistory_Update(manager, &log);
    if (ret == 0) {
        printf("OK\n");
    } else {
        printf("FAILED\n");
    }

    /* Test 1.5: Verify update */
    printf("1.5 Verifying update... ");
    memset(&log, 0, sizeof(LogGiftHistory));
    ret = LogGiftHistory_Get(manager, 100001, &log);
    if (ret == 0 && log.tran_state == 2 && log.cera == 1500) {
        printf("OK (state=%u, cera=%u)\n", log.tran_state, log.cera);
    } else {
        printf("FAILED\n");
    }

    /* Test 1.6: Delete log */
    printf("1.6 Deleting log (tran_id=100001)... ");
    ret = LogGiftHistory_Delete(manager, 100001);
    if (ret == 0) {
        printf("OK\n");
    } else {
        printf("FAILED\n");
    }

    /* Test 1.7: Verify deletion */
    printf("1.7 Verifying deletion... ");
    ret = LogGiftHistory_Exists(manager, 100001);
    if (ret == 0) {
        printf("OK (not exists)\n");
    } else {
        printf("FAILED (still exists)\n");
    }

    printf("\n");
}

void test_business_operations(DBConnectionManager* manager) {
    LogGiftHistory logs[10];
    int actual_count = 0;
    int ret;

    printf("[Test 2] Business Operations\n");
    printf("-----------------------------\n");

    /* Test 2.1: Add multiple logs */
    printf("2.1 Adding multiple gift logs... ");
    for (int i = 0; i < 3; i++) {
        LogGiftHistory log;
        memset(&log, 0, sizeof(LogGiftHistory));
        log.tran_id = 200001 + i;
        log.tran_state = i % 2;
        snprintf(log.send_account_id, sizeof(log.send_account_id), "sender%03d", i + 1);
        snprintf(log.send_charac_id, sizeof(log.send_charac_id), "char%03d", i + 1);
        snprintf(log.recv_account_id, sizeof(log.recv_account_id), "receiver%03d", i + 1);
        log.item_id = 10000 + i;
        log.cera = 500 * (i + 1);
        log.send_befor_cera = 3000;
        log.send_after_cera = 3000 - log.cera;
        log.recv_befor_cera = 1000;
        log.recv_after_cera = 1000 + log.cera;
        strcpy(log.query_user, "testuser");
        snprintf(log.occ_date, sizeof(log.occ_date), "2025-11-18 1%d:00:00", i);
        LogGiftHistory_Add(manager, &log);
    }
    printf("OK (added 3 logs)\n");

    /* Test 2.2: Get all logs */
    printf("2.2 Getting all logs... ");
    memset(logs, 0, sizeof(logs));
    ret = LogGiftHistory_GetAll(manager, logs, 10, &actual_count);
    if (ret == 0) {
        printf("OK (found %d logs)\n", actual_count);
        for (int i = 0; i < actual_count && i < 3; i++) {
            printf("    [%d] tran_id=%llu, %s -> %s, cera=%u\n",
                   i + 1, logs[i].tran_id, logs[i].send_account_id,
                   logs[i].recv_account_id, logs[i].cera);
        }
    } else {
        printf("FAILED\n");
    }

    /* Test 2.3: Get by account */
    printf("2.3 Getting logs by account (sender001)... ");
    memset(logs, 0, sizeof(logs));
    ret = LogGiftHistory_GetByAccount(manager, "sender001", logs, 10, &actual_count);
    if (ret == 0) {
        printf("OK (found %d logs)\n", actual_count);
    } else {
        printf("FAILED\n");
    }

    /* Test 2.4: Get by state */
    printf("2.4 Getting logs by state (state=0)... ");
    memset(logs, 0, sizeof(logs));
    ret = LogGiftHistory_GetByState(manager, 0, logs, 10, &actual_count);
    if (ret == 0) {
        printf("OK (found %d logs)\n", actual_count);
    } else {
        printf("FAILED\n");
    }

    /* Test 2.5: Get by date range */
    printf("2.5 Getting logs by date range... ");
    memset(logs, 0, sizeof(logs));
    ret = LogGiftHistory_GetByDateRange(manager, "2025-11-18 00:00:00",
                                         "2025-11-18 23:59:59", logs, 10, &actual_count);
    if (ret == 0) {
        printf("OK (found %d logs)\n", actual_count);
    } else {
        printf("FAILED\n");
    }

    printf("\n");
}
